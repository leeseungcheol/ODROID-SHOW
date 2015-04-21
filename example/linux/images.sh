#!/bin/bash

flag=0
serialPort="/dev/ttyUSB0"

trap programExit SIGINT SIGKILL SIGTERM

gcc -o port_open port_open.c
./port_open $serialPort &
subppid=$!
sleep 0.1

function programExit() {
	kill $subppid
	exit
}

declare -i length
isbusy=0

writeData ()
{
	echo -ne "\006" > $serialPort
	str=${1//\\}
	length=$((${#str} + 48))
	echo -ne $(printf \\$(printf '%03o' $length)) > $serialPort
	while [ "$isbusy" != 6 ]
	do
		isbusy=$(head -c 1 $serialPort)
	done
	echo -ne "$1" >$serialPort
	isbusy=0
}

writeData "\ec\e[0r"

while true
do
	writeData "\e[0r"
	writeData "\e[0;0,240;320i"
	cat penguin.raw > $serialPort
	writeData "\e[1r"
	writeData "\e[0;0,320;240i"
	cat butterfly.raw > $serialPort
	writeData "\e[0r"
	writeData "\e[0;0,240;320i"
	cat woof.raw > $serialPort
	writeData "\ec\e[0r"
	writeData "\e[40;10,220;200i"
	cat paint.raw > $serialPort
	writeData "\ec\e[1r"
	writeData "\e[10;10,190;200i"
	cat paint.raw > $serialPort
done
