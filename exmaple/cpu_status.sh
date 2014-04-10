#!/bin/bash

flag=0

trap "flag=1" SIGINT SIGKILL SIGTERM

./port_open &
subppid=$!

function cpu_state {
	cpuUsageM=$(top -bn 1 | awk '{print $9}' | tail -n +8 | awk '{s+=$1} END {print s}')
	cpuUsageM=$(top -bn 1 | awk '{print $9}' | tail -n +8 | awk '{s+=$1} END {print s}')
	cpuUsageM=$(top -bn 1 | awk '{print $9}' | tail -n +8 | awk '{s+=$1} END {print s}')
	cpuUsageM=$(top -bn 1 | awk '{print $9}' | tail -n +8 | awk '{s+=$1} END {print s}')
	cpuFreqM=$(echo "scale=0; " `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` "/1000" | bc)
	cpuTempM=$(echo "scale=1; " `cat /sys/class/thermal/thermal_zone0/temp` "/1000" | bc)
}

while true
do
	if [ $flag -ne 0 ] ; then
		kill $subppid
		exit
	fi
		mpstat -P ALL | grep -A10 "all" | grep -v "all" | awk '{print "core"$2" : "$3}'
		#cpu_state
		#echo -ne "\e[H\e[35mTime : " > /dev/ttyUSB0
		#date +"%T" > /dev/ttyUSB0
		#echo -ne "\eE\e[33mCPU Usage:  \e[32m$cpuUsageM%   \eE" > /dev/ttyUSB0
		#echo -ne "\e[33mCPU Freq:   \e[32m"$cpuFreqM"MHz   \eE" > /dev/ttyUSB0
		#echo -ne "\e[33mCPU Temp:   \e[32m$cpuTempM\e   " > /dev/ttyUSB0
		#sleep 1
done
