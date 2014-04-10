#!/bin/bash

flag=0

trap "flag=1" SIGINT SIGKILL SIGTERM

./port_open &
subppid=$!

function cpu_state {
	cpuFreqM=$(echo "scale=0; " `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` "/1000" | bc)
	cpuTempM=$(echo "scale=1; " `cat /sys/class/thermal/thermal_zone0/temp` "/1000" | bc)
}

sleep 0.1
echo -ne "\ec\e[2s\e[3r" > /dev/ttyUSB0
sleep 0.1

while true
do
	if [ $flag -ne 0 ] ; then
		kill $subppid
		exit
	fi
		#@echo -ne "\e[H" > /dev/ttyUSB0
		#echo -ne "\eE\e[35mTime : " > /dev/ttyUSB0
		echo -ne "\e[H\e[35mTime : \e[36m" > /dev/ttyUSB0
		date +"%T" > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 0 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\e[31m\eM" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 0 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 1 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\e[31m\eM" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 1 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 2 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\e[31m\eM" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 2 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\eE\eM\e[32m" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 3 | grep -A1 "usr" | grep -v "usr" | awk '{print "core"$3" : "}' > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\e[31m\eM" > /dev/ttyUSB0
		sleep 0.1
		mpstat -P 3 | grep -A1 "usr" | grep -v "usr" | awk '{print ""$4"%   "}' > /dev/ttyUSB0
		sleep 0.1
		cpu_state
		echo -ne "\eE\eM" > /dev/ttyUSB0
		sleep 0.1
		echo -ne "\e[33mCPU Freq:   \e[37m"$cpuFreqM"MHz   \eE" > /dev/ttyUSB0
		echo -ne "\e[33mCPU Temp:   \e[37m$cpuTempM\e   " > /dev/ttyUSB0
		sleep 1
done
