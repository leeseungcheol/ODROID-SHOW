#!/bin/bash

flag=0
trap "flag=1" SIGINT SIGKILL SIGTERM
./port_open &
subppid=$!

DATA[0]="ODROID"
DATA[1]="SHOW"

echo -ne "\e[5s\e[0r" > /dev/ttyUSB0

sleep 0.1
echo -ne "\ec" > /dev/ttyUSB0
sleep 0.1

while true
do
	if [ $flag -ne 0 ] ; then
		kill $subppid
		exit
	fi
	for ((j=1; j<8; j++)); do
		echo -ne "\e[25;100f" > /dev/ttyUSB0
		for ((i=0; i<6; i++)); do
			echo -ne "\e[3"$j"m\e[3"$j"m${DATA[0]:$i:1}" > /dev/ttyUSB0
			sleep 0.02
		done
		echo -ne "\eE\e[55;150f" > /dev/ttyUSB0
		for ((i=0; i<4; i++)); do
			echo -ne "\e[3"$j"m\e[3"$j"m${DATA[1]:$i:1}" > /dev/ttyUSB0
			sleep 0.02
		done
	done
done

