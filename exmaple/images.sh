#!/bin/bash

flag=0
serial="/dev/ttyUSB0"

trap "flag=1" SIGINT SIGKILL SIGTERM

./port_open &
subppid=$!

echo -ne "\e[0r" > $serial

while true
do
	if [ $flag -ne 0 ] ; then
		kill $subppid
		exit
	fi
		echo -ne "\e[0r" > $serial
		sleep 0.2
		echo -ne "\e[0;0,240;320i" > $serial
		cat penguin.raw > $serial
		sleep 0.2
		echo -ne "\e[1r" > $serial
		sleep 0.2
		echo -ne "\e[0;0,320;240i" > $serial
		cat butterfly.raw > $serial
		sleep 0.2
		echo -ne "\e[0r" > $serial
		sleep 0.2
		echo -ne "\e[0;0,240;320i" > $serial
		cat woof.raw > $serial
		sleep 0.2
done
