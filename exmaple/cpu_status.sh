#!/bin/bash

flag=0

trap "flag=1" SIGINT SIGKILL SIGTERM

./port_open &
subppid=$!

serialPort="/dev/ttyUSB0"

function cpu_state {
	cpuFreqM=$(echo "scale=0; " `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` "/1000" | bc)
	cpuTempM=$(echo "scale=1; " `cat /sys/class/thermal/thermal_zone0/temp` "/1000" | bc)
}

sleep 0.1
echo -ne "\ec\e[2s\e[3r" > $serialPort
sleep 0.1

while true
do
	if [ $flag -ne 0 ] ; then
		kill $subppid
		exit
	fi
		echo -ne "\e[H\e[35mTime : \e[36m" > $serialPort
		date +"%T" > $serialPort
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > $serialPort
		sleep 0.1
		mpstat -P 0 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > $serialPort
		sleep 0.1
		echo -ne "\e[31m\eM" > $serialPort
		sleep 0.1
		mpstat -P 0 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > $serialPort
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > $serialPort
		sleep 0.1
		mpstat -P 1 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > $serialPort
		sleep 0.1
		echo -ne "\e[31m\eM" > $serialPort
		sleep 0.1
		mpstat -P 1 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > $serialPort
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > $serialPort
		sleep 0.1
		mpstat -P 2 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > $serialPort
		sleep 0.1
		echo -ne "\e[31m\eM" > $serialPort
		sleep 0.1
		mpstat -P 2 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > $serialPort
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > $serialPort
		sleep 0.1
		mpstat -P 3 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > $serialPort
		sleep 0.1
		echo -ne "\e[31m\eM" > $serialPort
		sleep 0.1
		mpstat -P 3 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > $serialPort
		sleep 0.1
		cpu_state
		echo -ne "\eE\eM" > $serialPort
		sleep 0.1
		echo -ne "\e[33mCPU Freq:   \e[37m"$cpuFreqM"MHz   \eE" > $serialPort
		echo -ne "\e[33mCPU Temp:   \e[37m$cpuTempM\e   " > $serialPort
		sleep 1
done
