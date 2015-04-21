#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define baudrate	B500000

static char serialPort[] = "/dev/ttyUSB0";
static char buffer[64];
char isbusy = 0;
int length = 0;

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

void writeData(int fd, char *str)
{
	write(fd, "\006", 1);
	length = strlen(str) + 48;
	write(fd, &length, 1);
	while (isbusy != '6') {
		read(fd, &isbusy, 1);
		usleep(10000);
	}
	write(fd, str, length - 48);
	isbusy = 0;
}

int main(int argc, char **argv)
{
	int usbdev;

	if (argc == 2) {
		sprintf(serialPort, "%s", argv[1]);
	}

	usbdev = serialSetup();

	writeData(usbdev, "\ec");
	writeData(usbdev, "Hello ODROID-SHOW");
	
	return 0;
}
