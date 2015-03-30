#!/bin/bash

flag=0
serialPort="/dev/ttyUSB0"

trap "flag=1" SIGINT SIGKILL SIGTERM

gcc -o port_open port_open.c
./port_open $serialPort &
subppid=$!

sleep 0.1
echo -ne "\ec\e[0r" > $serialPort
sleep 0.3

while true
do
	if [ $flag -ne 0 ] ; then
		echo -ne "\ec\e[1r" > $serialPort
		kill $subppid
		exit
	fi
	        echo -ne "\e[0r" > $serialPort
	        sleep 0.2
	        echo -ne "\e[0;0,240;320i" > $serialPort
	        cat penguin.raw > $serialPort
	        sleep 0.1
	        echo -ne "\e[1r" > $serialPort
	        sleep 0.2
	        echo -ne "\e[0;0,320;240i" > $serialPort
	        cat butterfly.raw > $serialPort
	        sleep 0.1
	        echo -ne "\e[0r" > $serialPort
	        sleep 0.2
	        echo -ne "\e[0;0,240;320i" > $serialPort
	        cat woof.raw > $serialPort
	        sleep 0.1
		echo -ne "\ec\e[0r" > $serialPort
		sleep 0.3
		echo -ne "\e[40;10,220;200i" > $serialPort
		cat paint.raw > $serialPort
		sleep 0.1
		echo -ne "\ec\e[1r" > $serialPort
		sleep 0.3
		echo -ne "\e[10;10,190;200i" > $serialPort
		cat paint.raw > $serialPort
		sleep 0.1
done
