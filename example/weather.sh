#!/bin/bash

flag=0
serialPort="/dev/ttyUSB0"

trap "flag=1" SIGINT SIGKILL SIGTERM

./port_open &
subppid=$!

sleep 0.1
echo -ne "\ec\e[2s\e[1r" > $serialPort
sleep 0.1

while true
do
	if [ $flag -ne 0 ] ; then
		echo -ne "\ec\e[2s\e[1r" > $serialPort
		kill $subppid
		exit
	fi
		echo -ne "\e[0;0f\e[32mBMP180  \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[35mTemp : \e[0w *C   \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[35mPressure : \e[1w Pa   \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[35mAltitude : \e[2w meters   \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[32mSi7020  \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[36mTemp : \e[3w *C   \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[36mHumidity : \e[4w %   \n\n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[32mSi1132  \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[37mUV Index : \e[5w   \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[37mVisible : \e[6w lux   \n\r" > $serialPort
		sleep 0.1;
		echo -ne "\e[37mIR : \e[7w lux   \n\r" > $serialPort
		sleep 0.1;
done

