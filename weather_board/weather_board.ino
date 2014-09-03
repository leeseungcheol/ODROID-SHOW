#include <SPI.h>
#include <Wire.h>
#include <TimerOne.h>
#include <Adafruit_GFX.h>
#include <ODROID_Si1132.h>
#include <ODROID_Si70xx.h>
#include <Adafruit_ILI9340.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

uint8_t ledPin = 5;

uint8_t textSize = 2;
uint8_t rotation = 1;
uint16_t foregroundColor, backgroundColor;
uint32_t x = 0;

const char version[] = "v1.2";

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
Adafruit_BMP085 bmp180;

ODROID_Si70xx si7020;
ODROID_Si1132 si1132;

float UVindex = 0;
float bmp180Temperature = 0;
uint32_t bmp180Pressure = 0;
float bmp180Altitude = 0;

float si7020Temperature = 0;
float si7020Humidity = 0;
float si1132UVIndex = 0;

uint32_t si1132Visible = 0;
uint32_t si1132IR = 0;

void setup() {
        Serial.begin(500000);
        Serial.println("Welcome to the WEATHER-BOARD");

        si1132.begin();
        bmp180.begin();
        bmp.begin();
        tft.begin();

        // initialize the digital pin as an output for LED Backlibht
        pinMode(ledPin, OUTPUT);
        analogWrite(ledPin, 200);

        timer1_setup();
        delay(1500);

        backgroundColor = ILI9340_BLACK;
        tft.fillScreen(backgroundColor);
        foregroundColor = ILI9340_YELLOW;
        tft.setTextColor(foregroundColor, backgroundColor);
        tft.setTextSize(textSize);
        tft.setCursor(0, 0);
        tft.setRotation(rotation);

        tft.setCursor(50, 50);
        tft.print("Hello WEATHER-BOARD!");
        tft.setCursor(250, 200);

        tft.print(version);

        delay(1000);
        tft.fillScreen(backgroundColor);
        tft.setCursor(0, 0);


}

void timer1_setup() {
        cli();
        TCCR1A = 0;
        TCCR1B = 0;
        OCR1A = (16000000 / 1024) -1;
        TCCR1B |= (1 << WGM12);
        TCCR1B |= (1 << CS12) | (1 << CS10);
        TIMSK1 |= (1 << OCIE1A);
        sei();
}

ISR(TIMER1_COMPA_vect) {
        Serial.print("w0");
        Serial.print(bmp180Temperature);
        Serial.print("\ew1");
        Serial.print(bmp180Pressure);
        Serial.print("\ew2");
        Serial.print(bmp180Altitude);
        Serial.print("\ew3");
        Serial.print(si7020Temperature);
        Serial.print("\ew4");
        Serial.print(si7020Humidity);
        Serial.print("\ew5");
        Serial.print(si1132UVIndex);
        Serial.print("\ew6");
        Serial.print(si1132Visible);
        Serial.print("\ew7");
        Serial.print(si1132IR);
        Serial.print("\e"); 
}

void loop(void) {
        sensors_event_t event;
        bmp.getEvent(&event);

        tft.setCursor(0, 0);
        tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.println("BMP180");

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Temp : ");
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
        //bmp180Temperature = bmp180.readTemperature();
        bmp.getTemperature(&bmp180Temperature);
        tft.print(bmp180Temperature);
        tft.println(" *C   ");
        delay(50);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Pressure : ");
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
        //bmp180Pressure = bmp180.readPressure();
        bmp180Pressure = event.pressure;
        tft.print(bmp180Pressure);
        tft.println(" Pa   ");
        delay(50);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Altitude : ");
        tft.setTextColor(ILI9340_CYAN, backgroundColor);
        //bmp180Altitude = bmp180.readAltitude(101500);
        bmp180Altitude = bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, event.pressure, bmp180Temperature);
        tft.print(bmp180Altitude);
        tft.println(" meters     ");
        delay(50);

        tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.println("Si7020");

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Temp : ");
        tft.setTextColor(ILI9340_YELLOW, backgroundColor);
        si7020Temperature = si7020.readTemperature();
        tft.print(si7020Temperature);
        tft.println(" *C   ");
        delay(50);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Humidity : ");
        tft.setTextColor(ILI9340_YELLOW, backgroundColor);
        si7020Humidity = si7020.readHumidity();
        tft.print(si7020Humidity);
        tft.println(" %   \n");
        delay(50);

        tft.setTextColor(ILI9340_GREEN, backgroundColor);
        tft.println("Si1132");

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("UV Index : ");
        tft.setTextColor(ILI9340_WHITE, backgroundColor);
        si1132UVIndex = si1132.readUV()/100.0;
        tft.print(si1132UVIndex);
        tft.println("    ");
        delay(50);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("Visible : ");
        tft.setTextColor(ILI9340_WHITE, backgroundColor);
        si1132Visible = si1132.readVisible();
        tft.print(si1132Visible);
        tft.println(" Lux   ");
        delay(50);

        tft.setTextColor(ILI9340_MAGENTA, backgroundColor);
        tft.print("IR : ");
        tft.setTextColor(ILI9340_WHITE, backgroundColor);
        si1132IR = si1132.readIR();
        tft.print(si1132IR);
        tft.println(" Lux   \n");
        delay(50);
}




