#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>

#include <math.h>

#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define baudrate	B500000

#define String_startsWith(s, match) (strstr((s), (match)) == (s))


static const char procstat[] = "/proc/stat";
static const char serialPort[] = "/dev/ttyUSB0";
static const char mountPath[] = "/";

static const int GB = 1024*1024*1024; // Giga byte
static char buffer[256];

struct CPUData_ {
	unsigned long long int totalTime;
	unsigned long long int userTime;

	unsigned long long int totalPeriod;
	unsigned long long int userPeriod;
};

struct diskData_ {
	unsigned long long disk_size;
	unsigned long long used;
	unsigned long long free;
};

int serialSetup(void)
{
	int usbdev;
	struct termios options;

	usbdev = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);

	if (usbdev == -1)
		perror("open_port : Unable to open:");

	tcgetattr(usbdev, &options);

	cfsetispeed(&options, baudrate);
	cfsetospeed(&options, baudrate);

	options.c_cflag |= CS8;
	options.c_iflag |= IGNBRK;
	options.c_iflag &= ~( BRKINT | ICRNL | IMAXBEL | IXON);
	options.c_oflag &= ~( OPOST | ONLCR );
	options.c_lflag &= ~( ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK |
							ECHOCTL | ECHOKE);
	options.c_lflag |= NOFLSH;
	options.c_cflag &= ~CRTSCTS;

	tcsetattr(usbdev, TCSANOW, &options);

	return usbdev;
}

void diskSpace(int usbdev)
{
	struct statvfs sfs;
	struct diskData_ diskData;
	if (!statvfs(mountPath, &sfs)) {
		diskData.disk_size = (long long)sfs.f_blocks * sfs.f_bsize;
		diskData.free = (long long)sfs.f_bfree * sfs.f_bsize;
		diskData.used = (long long)diskData.disk_size - diskData.free;
	}

	sprintf(buffer, "\e[37mMount Path : \e[36m%s\n\r", mountPath);
	usleep(100000);
	write(usbdev, buffer, strlen(buffer) + 1);

	sprintf(buffer, "\e[33mdisk_size : \e[32m%.2lfGB\n\r",
						(double)diskData.disk_size/GB);
	write(usbdev, buffer, strlen(buffer) + 1);
	usleep(100000);

	sprintf(buffer, "\e[35mused : \e[32m%.2lfGB\n\r",
						(double)diskData.used/GB);
	write(usbdev, buffer, strlen(buffer) + 1);
	usleep(100000);

	sprintf(buffer, "\e[36mfree : \e[32m%.2lfGB",
						(double)diskData.free/GB);
	write(usbdev, buffer, strlen(buffer) + 1);
	usleep(100000);

}

void systemInfo(int fd)
{
	time_t t;
	struct utsname uts;

	time(&t);
	sprintf(buffer, "\e[35m%s\r", ctime(&t));
	write(fd, buffer, strlen(buffer) + 1);
	usleep(300000);
	uname(&uts);
	sprintf(buffer, "\e[37mOSname:\e[36m%s\n\r", uts.sysname);
	write(fd, buffer, strlen(buffer) + 1);
	sprintf(buffer, "\e[37mVersion:\e[36m%s\n\r", uts.release);
	write(fd, buffer, strlen(buffer) + 1);
	usleep(300000);
	sprintf(buffer, "\e[37mMachine:\e[36m%s\n\r", uts.machine);
	write(fd, buffer, strlen(buffer) + 1);
}

int cpuCount(int cpus)
{
	FILE *file = fopen(procstat, "r");

	do {
		cpus++;
		fgets(buffer, 255, file);
	} while (String_startsWith(buffer, "cpu"));

	fclose(file);

	return cpus;
}

struct CPUData_* cpuUsageInit(struct CPUData_ *cpuData, int cpus)
{
	int i;

	cpuData = (struct CPUData_ *) malloc(cpus * sizeof(struct CPUData_));

	for (i = 0; i < cpus; i++) {
		cpuData[i].totalTime = 1;
		cpuData[i].userTime = 0;
		cpuData[i].totalPeriod = 1;
		cpuData[i].userPeriod = 0;
	}

	return cpuData;
}

void cpuUsageDisplay(int usbdev, struct CPUData_ *cpuData, int cpus)
{
	unsigned long long int usertime, nicetime, systemtime, systemalltime,
				idlealltime, idletime, totaltime, virtalltime;
	unsigned long long int ioWait, irq, softIrq, steal, guest;
	struct utsname uts;
	double total = 0;
	FILE *file;
	int cpuid;
	int i;
	time_t t;

	file = fopen(procstat, "r");
	ioWait = irq = softIrq = steal = guest = 0;

	for (i = 0; i < cpus; i++) {
		fgets(buffer, 255, file);
		usleep(100000);
		if (i == 0) {
			sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
					&usertime, &nicetime, &systemtime, &idletime,
					&ioWait, &irq, &softIrq, &steal, &guest);
		} else {
			sscanf(buffer, "cpu%d %llu %llu %llu %llu %llu %llu %llu %llu %llu",
					&cpuid, &usertime, &nicetime, &systemtime, &idletime,
					&ioWait, &irq, &softIrq, &steal, &guest);
			assert(cpuid == i - 1);
		}

		idlealltime = idletime + ioWait;
		systemalltime = systemtime + irq + softIrq;
		virtalltime = steal + guest;
		totaltime = usertime + nicetime + systemalltime +
			idlealltime + virtalltime;

		assert(usertime >= cpuData[i].userTime);
		assert(totaltime >= cpuData[i].totalTime);

		cpuData[i].userPeriod = usertime - cpuData[i].userTime;
		cpuData[i].totalPeriod = totaltime - cpuData[i].totalTime;

		cpuData[i].totalTime = totaltime;
		cpuData[i].userTime = usertime;

		total = (double)cpuData[i].totalPeriod;

		if ((i != 0) && (i%2 == 1)) {	
			sprintf(buffer, "\e[33mcpu%d :\e[32m%4.1f%% ",
					i, cpuData[i].userPeriod/total*100.0);
			write(usbdev, buffer, strlen(buffer) + 1);
		} else if ((i != 0) && (i%2 == 0)) {
			sprintf(buffer, "\e[33mcpu%d :\e[32m%4.1f%%  \n\r",
					i, cpuData[i].userPeriod/total*100.0);
			write(usbdev, buffer, strlen(buffer) + 1);
		}
	}

	fclose(file);
}

int main(void)
{
	struct CPUData_ *cpuData;
	int usbdev;
	int cpus = -1;

	usbdev = serialSetup();

	sprintf(buffer, "\ec\e[2s\e[1r");
	write(usbdev, buffer, strlen(buffer) + 1);
	usleep(100000);
	cpus = cpuCount(cpus);
	cpuData =  cpuUsageInit(cpuData, cpus);

	while (1) {
		sprintf(buffer, "\e[H");
		write(usbdev, buffer, strlen(buffer) + 1);
		systemInfo(usbdev);
		usleep(100000);

		cpuUsageDisplay(usbdev, cpuData, cpus);
		diskSpace(usbdev);
	}
	
	return 0;
}
