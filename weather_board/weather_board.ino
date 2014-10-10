#include <SPI.h>
#include <Wire.h>
#include "TimerOne.h"
#include <Adafruit_GFX.h>
#include <ODROID_Si1132.h>
#include <ODROID_Si70xx.h>
#include <Adafruit_ILI9340.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_Sensor.h>

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

const char version[] = "v1.2";

uint8_t ledPin = 5;
uint8_t pwm = 255;
uint8_t textSize = 2;
uint8_t rotation = 1;
uint16_t foregroundColor, backgroundColor;

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
ODROID_Si70xx si7020;
ODROID_Si1132 si1132;

float BMP180Temperature = 0;
float BMP180Pressure = 0;
float BMP180Altitude = 0;

float Si7020Temperature = 0;
float Si7020Humidity = 0;
float Si1132UVIndex = 0;

uint32_t Si1132Visible = 0;
uint32_t Si1132IR = 0;
uint32_t UVindex = 0;

void setup() {
        Serial.begin(500000);
        Serial.println("Welcome to the WEATHER-BOARD");

        // initialize the sensors
        si1132.begin();
        bmp.begin();
        tft.begin();
        
        sensor_t sensor;
        bmp.getSensor(&sensor);

        // initialize the digital pin as an output for LED Backlibht
        pinMode(ledPin, OUTPUT);
        analogWrite(ledPin, pwm);

        delay(1500);

        tft.setRotation(rotation);
        tft.setTextSize(textSize);
        tft.setCursor(50, 50);
        tft.print("Hello WEATHER-BOARD!");
        tft.setCursor(250, 200);
        tft.print(version);

        delay(1000);
        tft.fillScreen(backgroundColor);
        tft.setCursor(0, 0);
        
        //Timer one setting
        Timer1.initialize(200000);
        Timer1.attachInterrupt(timerCallback);
}

void timerCallback() {
        sendSensors();
}

void sendSensors()
{
        Serial.print("w0");
        Serial.print(BMP180Temperature);
        Serial.print("\ew1");
        Serial.print(BMP180Pressure);
        Serial.print("\ew2");
        Serial.print(BMP180Altitude);
        Serial.print("\ew3");
        Serial.print(Si7020Temperature);
        Serial.print("\ew4");
        Serial.print(Si7020Humidity);
        Serial.print("\ew5");
        Serial.print(Si1132UVIndex);
        Serial.print("\ew6");
        Serial.print(Si1132Visible);
        Serial.print("\ew7");
        Serial.print(Si1132IR);
        Serial.print("\e"); 
}

void getBMP180()
{
        sensors_event_t event;
        bmp.getEvent(&event);
        
        if (event.pressure) {
                bmp.getTemperature(&BMP180Temperature);
                BMP180Pressure = event.pressure;
                BMP180Altitude = bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, event.pressure, BMP180Temperature);
        }
}

void getSi1132()
{
        Si1132UVIndex = si1132.readUV()/100.0;
        Si1132Visible = si1132.readVisible();
        Si1132IR = si1132.readIR();
}

void getSi7020()
{
        Si7020Temperature = si7020.readTemperature();
        Si7020Humidity = si7020.readHumidity();
}

void displayBMP180()
{
        tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.println("BMP180");
        
        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Temp : ");
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
        tft.print(BMP180Temperature);
        tft.println(" *C   ");
        delay(20);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Pressure : ");
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
        tft.print(BMP180Pressure);
        tft.println(" Pa   ");
        delay(20);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Altitude : ");
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
        tft.print(BMP180Altitude);
        tft.println(" meters     ");
        delay(200);
}

void displaySi7020()
{
        tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.println("Si7020");
        
        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Temp : ");
        tft.setTextColor(ILI9340_YELLOW, backgroundColor);
        tft.print(Si7020Temperature);
        tft.println(" *C   ");
        delay(20);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Humidity : ");
        tft.setTextColor(ILI9340_YELLOW, backgroundColor);
        tft.print(Si7020Humidity);
        tft.println(" %   \n");
        delay(20);
}

void displaySi1132()
{
        tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.println("Si1132");
        
        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("UV Index : ");
        tft.setTextColor(ILI9340_WHITE, backgroundColor);
        tft.print(Si1132UVIndex);
        tft.println("    ");
        delay(20);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Visible : ");
        tft.setTextColor(ILI9340_WHITE, backgroundColor);
        tft.print(Si1132Visible);
        tft.println(" Lux   ");
        delay(20);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("IR : ");
        tft.setTextColor(ILI9340_WHITE, backgroundColor);
        tft.print(Si1132IR);
        tft.println(" Lux   \n");
        delay(20);
}

void loop(void) {
        tft.setCursor(0, 0);
        getBMP180();
        displayBMP180();
        getSi7020();
        displaySi7020();
        getSi1132();
        displaySi1132();        
}
