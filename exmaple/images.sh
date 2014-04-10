#!/bin/bash

flag=0

trap "flag=1" SIGINT SIGKILL SIGTERM

./port_open &
subppid=$!
while true
do
	if [ $flag -ne 0 ] ; then
		kill $subppid
		exit
	fi
		echo -ne "\e[0r" > /dev/ttyUSB0
		sleep 0.2
		echo -ne "\e[0;0,240;320i" > /dev/ttyUSB0
		cat penguin.raw > /dev/ttyUSB0
		sleep 0.2
		echo -ne "\e[1r" > /dev/ttyUSB0
		sleep 0.2
		echo -ne "\e[0;0,320;240i" > /dev/ttyUSB0
		cat butterfly.raw > /dev/ttyUSB0
		sleep 0.2
		echo -ne "\e[0r" > /dev/ttyUSB0
		sleep 0.2
		echo -ne "\e[0;0,240;320i" > /dev/ttyUSB0
		cat woof.raw > /dev/ttyUSB0
		sleep 0.2
done
