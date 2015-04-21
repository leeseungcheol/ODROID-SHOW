#!/bin/bash

flag=0
serialPort="/dev/ttyUSB0"

trap programExit SIGINT SIGKILL SIGTERM

gcc -o port_open port_open.c
./port_open $serialPort &
subppid=$!
sleep 0.1

function programExit()
{
	kill $subppid
	exit
}

declare -i length
isbusy=0

writeData () {
	echo -ne "\006" > $serialPort
	str=${1//\\}
	length=$((${#str} + 48))
	echo -ne $(printf \\$(printf '%03o' $length)) > $serialPort
	while [ "$isbusy" != 6 ]
	do
		isbusy=$(head -c 1 $serialPort)
	done
	echo -ne "$1" > $serialPort
	isbusy=0
}

writeData "\ec"
writeData "Hello ODROID-SHOW"

programExit
