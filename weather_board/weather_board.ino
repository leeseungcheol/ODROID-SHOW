#include <SPI.h>
#include <Wire.h>
#include "TimerOne.h"
#include <Adafruit_GFX.h>
#include <ODROID_Si1132.h>
#include <ODROID_Si70xx.h>
#include <Adafruit_ILI9340.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

const char version[] = "v1.4";

uint8_t ledPin = 5;
uint8_t pwm = 255;
uint8_t textSize = 2;
uint8_t rotation = 1;

float battery = 0;
float oldBattery;
uint8_t batteryCnt;
uint8_t batteryState;
uint8_t timer;
uint8_t count;

uint16_t foregroundColor, backgroundColor;

#define SEALEVELPRESSURE_HPA        (1024.25)

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(SEALEVELPRESSURE_HPA);
Adafruit_BME280 bme; // I2C
ODROID_Si70xx si7020;
ODROID_Si1132 si1132;

float BMP180Temperature = 0;
float BMP180Pressure = 0;
float BMP180Altitude = 0;

float BME280Temperature = 0;
float BME280Pressure = 0;
float BME280Humidity = 0;
float BME280Altitude = 0;

float Si7020Temperature = 0;
float Si7020Humidity = 0;

float Si1132UVIndex = 0;
uint32_t Si1132Visible = 0;
uint32_t Si1132IR = 0;

uint8_t WB_VERSION = 2;
unsigned char errorState;

void setup()
{
        Serial.begin(500000);
        Serial.println("Welcome to the WEATHER-BOARD");

        // initialize the sensors
        tft.begin();
        si1132.begin();
        
        // Check board version
        if (read8(BME280_ADDRESS, BME280_REGISTER_CHIPID) != 0x60)
                WB_VERSION = 1;

        if (WB_VERSION == 1) {
                bmp.begin();
                sensor_t sensor;
                bmp.getSensor(&sensor);
        } else {
                if(!bme.begin()) {
                }
        }

        // initialize the digital pins
	initPins();

	analogReference(INTERNAL);

        //Timer one setting
        Timer1.initialize(300000);
        Timer1.attachInterrupt(timerCallback);

        tft.setRotation(rotation);
        tft.setTextSize(textSize);
        tft.setCursor(50, 50);
        tft.print("Hello WEATHER-BOARD!");
        tft.setCursor(250, 200);
        tft.print(version);
        delay(1000);
        tft.fillScreen(backgroundColor);

	displayLabel();
}

uint8_t read8(byte _i2caddr, byte reg)
{
	byte value;
	Wire.beginTransmission((uint8_t)_i2caddr);
	Wire.write((uint8_t)reg);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
	value = Wire.read();
	Wire.endTransmission();
	return value;
}

void displayLabel()
{
	tft.setCharCursor(0, 1);
	tft.setTextColor(ILI9340_GREEN, backgroundColor);
	tft.println("Si1132");
	tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
	tft.println("UV Index : ");
	tft.println("Visible :");
	tft.println("IR :");

	if (WB_VERSION == 1) {
		tft.setCharCursor(0, 6);
		tft.setTextColor(ILI9340_GREEN, backgroundColor);
		tft.println("Si7020");
		tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
		tft.println("Temp : ");
		tft.println("Humidity :");
		tft.setCharCursor(0, 10);
		tft.setTextColor(ILI9340_GREEN, backgroundColor);
		tft.println("BMP180");
		tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
		tft.println("Temp : ");
		tft.println("Pressure :");
		tft.println("Altitude :");
	} else {
		tft.setCharCursor(0, 6);
		tft.setTextColor(ILI9340_GREEN, backgroundColor);
		tft.setTextColor(ILI9340_GREEN, backgroundColor);
		tft.println("BME280");
		tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
		tft.println("Temp : ");
		tft.println("Pressure :");
		tft.println("Humidity :");
		tft.println("Altitude :");
	}
	tft.drawRect(240, 10, 70, 30, 870);
}

void initPins()
{
	pinMode(ledPin, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, INPUT);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
        pinMode(A2, INPUT);

	analogWrite(ledPin, pwm);
}

void timerCallback()
{
	readBtn();
	timer++;
}

unsigned char btn0Presses = 0;
unsigned char btn0Releases = 0;
unsigned char btn1Presses = 0;
unsigned char btn1Releases = 0;
unsigned char btn2Presses = 0;
unsigned char btn2Releases = 0;

unsigned char btn0Pushed = 0;
unsigned char btn1Pushed = 0;
unsigned char btn2Pushed = 0;

void readBtn()
{
        if (!digitalRead(A1) && (btn2Presses == 0)) {
                btn2Presses = 1;
                btn2Releases = 0;
                btn2Pushed = 1;
                digitalWrite(6, LOW);
        }

        if (digitalRead(A1) && (btn2Releases == 0)) {
                btn2Releases = 1;
                btn2Presses = 0;
                btn2Pushed = 0;
                digitalWrite(6, HIGH);
        }

        if (!digitalRead(7) && (btn0Presses == 0)) {
                btn0Presses = 1;
                btn0Releases = 0;
                btn0Pushed = 1;
                if (pwm > 225)
                        pwm = 255;
                else
                        pwm += 30;
                analogWrite(ledPin, pwm);
                digitalWrite(3, LOW);
        }

        if (digitalRead(7) && (btn0Releases == 0)) {
                btn0Releases = 1;
                btn0Presses = 0;
                btn0Pushed = 0;
                digitalWrite(3, HIGH);
        }

        if (!digitalRead(A0) && (btn1Presses == 0)) {
                btn1Presses = 1;
                btn1Releases = 0;
                btn1Pushed = 1;
                if (pwm < 30)
                        pwm = 0;
                else
                        pwm -= 30;
                analogWrite(ledPin, pwm);
                digitalWrite(4, LOW);
        }

        if (digitalRead(A0) && (btn1Releases == 0)) {
                btn1Releases = 1;
                btn1Presses = 0;
                btn1Pushed = 0;
                digitalWrite(4, HIGH);
        }
}

void sendToHost()
{
	Serial.print("w0");
	Serial.print(battery);
	Serial.print("\ew2");
	Serial.print(Si1132UVIndex);
	Serial.print("\ew3");
	Serial.print(Si1132Visible);
	Serial.print("\ew4");
	Serial.print(Si1132IR);
	Serial.print("\ew5");
	if (WB_VERSION == 1) {
		Serial.print(Si7020Temperature);
		Serial.print("\ew6");
		Serial.print(Si7020Humidity);
		Serial.print("\ew7");
		Serial.print(BMP180Temperature);
		Serial.print("\ew8");
		Serial.print(BMP180Pressure);
		Serial.print("\ew9");
		Serial.print(BMP180Altitude);
	} else {
		Serial.print(BME280Temperature);
		Serial.print("\ew6");
		Serial.print(BME280Pressure);
		Serial.print("\ew7");
		Serial.print(BME280Altitude);
		Serial.print("\ew8");
	}

	Serial.print("\e");
}

void getBMP180()
{
        sensors_event_t event;
        bmp.getEvent(&event);
        
        if (event.pressure) {
                bmp.getTemperature(&BMP180Temperature);
                BMP180Pressure = event.pressure;
                BMP180Altitude = bmp.pressureToAltitude(1025, event.pressure);
        }
}

void getSi1132()
{
        Si1132UVIndex = 0;
        Si1132Visible = 0;
        Si1132IR = 0;
        for (int i = 0; i < 10; i++) {
                Si1132Visible += si1132.readVisible();
                Si1132IR += si1132.readIR();
                Si1132UVIndex += si1132.readUV();
        }
        Si1132UVIndex /= 10;
        Si1132UVIndex /= 100;
        Si1132Visible /= 10;
        Si1132IR /= 10;
}

void getSi7020()
{
        Si7020Temperature = si7020.readTemperature();
        Si7020Humidity = si7020.readHumidity();
}

void getBME280()
{
	BME280Temperature = bme.readTemperature();
	BME280Pressure = (bme.readPressure()/100);
	BME280Humidity = bme.readHumidity();
	BME280Altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
}

void displayBMP180()
{
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
	tft.setCharCursor(7, 11);
        tft.print(BMP180Temperature);
        tft.println(" *C  ");
        delay(20);

	tft.setCharCursor(11, 12);
        tft.print(BMP180Pressure);
        tft.println(" Pa  ");
        delay(20);

	tft.setCharCursor(11, 13);
        tft.print(BMP180Altitude);
        tft.println(" meters   ");
        delay(20);
}

void displaySi7020()
{
        tft.setTextColor(ILI9340_YELLOW, backgroundColor);
	tft.setCharCursor(7, 7);
        tft.print(Si7020Temperature);
        tft.println(" *C  ");
        delay(20);

	tft.setCharCursor(11, 8);
        tft.print(Si7020Humidity);
        tft.println(" %  \n");
        delay(20);
}

void displaySi1132()
{
        tft.setTextColor(ILI9340_RED, backgroundColor);
	tft.setCharCursor(11, 2);
        tft.print(Si1132UVIndex);
        tft.println("  ");
        delay(20);

	tft.setCharCursor(10, 3);
        tft.print(Si1132Visible);
        tft.println(" Lux  ");
        delay(20);

	tft.setCharCursor(5, 4);
        tft.print(Si1132IR);
        tft.println(" Lux  \n");
        delay(20);
}

void displayBME280()
{
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
	tft.setCharCursor(7, 7);
        tft.print(BME280Temperature);
        tft.println(" *C  ");
        delay(20);

	tft.setCharCursor(11, 8);
        tft.print(BME280Pressure);
        tft.println(" hPa  ");
        delay(20);

	tft.setCharCursor(11, 9);
        tft.print(BME280Humidity);
        tft.println(" %   ");
        delay(20);

        tft.setCharCursor(11, 10);
        tft.print(BME280Altitude);
        tft.println(" m   ");
        delay(20);
}

void errorCheck()
{
        if ((read8(BME280_ADDRESS, BME280_REGISTER_CHIPID) != 0x60) &&
                        (read8(BMP085_ADDRESS, BMP085_REGISTER_CHIPID) != 0x55)) {
                if (!errorState) {
                        tft.fillScreen(backgroundColor);
			displayLabel();
                }
                errorState = 1;
        } else if (errorState) {
                if (read8(BME280_ADDRESS, BME280_REGISTER_CHIPID) != 0x60) {
                        WB_VERSION = 1;
                } else {
                        WB_VERSION = 2;
                }
                si1132.begin();
                if (WB_VERSION == 1) {
                        bmp.begin();
                        sensor_t sensor;
                        bmp.getSensor(&sensor);
                } else {
                        if(!bme.begin()) {
                        }
                }
                tft.fillScreen(backgroundColor);
		displayLabel();
                errorState = 0;
        }
        if (errorState) {
                BMP180Temperature = 0;
                BMP180Pressure = 0;
                BMP180Altitude = 0;

                Si7020Temperature = 0;
                Si7020Humidity = 0;

                BME280Temperature = 0;
                BME280Pressure = 0;
                BME280Humidity = 0;
                BME280Altitude = 0;

                Si1132UVIndex = 0;
                Si1132Visible = 0;
                Si1132IR = 0;
        }
}

void readBattery()
{
        battery = analogRead(A2)*1.094/1024/3.9*15.9;

        if (abs(oldBattery - battery) > 0.5 || battery < 2.1)
                batteryCnt++;
        oldBattery = battery;

        if (timer > 10) {
                if (batteryCnt > 2) {
                        timer = 0;
                        batteryCnt = 0;
                        batteryState = 1;
                } else {
                        batteryState = 0;
                }
        }

        if (batteryState)
                battery = 0;

	tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.setCharCursor(21, 3);
        tft.print(battery);
        if (battery > 3.95) {
                        tft.fillRect(244, 13, 14, 24, 10000);
                        tft.fillRect(260, 13, 14, 24, 10000);
                        tft.fillRect(276, 13, 14, 24, 10000);
                        tft.fillRect(292, 13, 14, 24, 10000);
        } else if (battery > 3.75 && battery <= 3.95) {
                        tft.fillRect(244, 13, 14, 24, 10000);
                        tft.fillRect(260, 13, 14, 24, 10000);
                        tft.fillRect(276, 13, 14, 24, 10000);
                        tft.fillRect(292, 13, 14, 24, 0);
        } else if (battery > 3.65 && battery <= 3.75) {
                        tft.fillRect(244, 13, 14, 24, 10000);
                        tft.fillRect(260, 13, 14, 24, 10000);
                        tft.fillRect(276, 13, 14, 24, 0);
                        tft.fillRect(292, 13, 14, 24, 0);
        } else if (battery > 3.5 && battery <= 3.65) {
                        tft.fillRect(244, 13, 14, 24, 10000);
                        tft.fillRect(260, 13, 14, 24, 0);
                        tft.fillRect(276, 13, 14, 24, 0);
                        tft.fillRect(292, 13, 14, 24, 0);
        } else if (battery <= 3.5) {
                        tft.fillRect(244, 13, 14, 24, 0);
                        tft.fillRect(260, 13, 14, 24, 0);
                        tft.fillRect(276, 13, 14, 24, 0);
                        tft.fillRect(292, 13, 14, 24, 0);
        }
}

void loop(void)
{
	displaySi1132();
	if (WB_VERSION == 1) {
		displayBMP180();
                displaySi7020();
        } else {
                displayBME280();
        }

        if (!errorState) {
                getSi1132();
                if (WB_VERSION == 1) {
                        getBMP180();
                        getSi7020();
                } else {
                        getBME280();
                }

		sendToHost();
        }

        if (count > 5) {
                errorCheck();
                count = 0;
        }
        readBattery();

        count++;
}
