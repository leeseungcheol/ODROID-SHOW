#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "weather_board.h"
#include "ui_weather_board.h"

#define DEVICE "/dev/ttyUSB0"
#define baudrate B500000

WeatherBoard::WeatherBoard(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WeatherBoard)
{
    ui->setupUi(this);

    QPalette* palette = new QPalette();

    palette->setColor(QPalette::WindowText,Qt::red);
    ui->lTemperature_2->setPalette(*palette);

    palette->setColor(QPalette::WindowText,Qt::blue);
    ui->lHumidity_2->setPalette(*palette);

    palette->setColor(QPalette::WindowText,Qt::red);
    ui->lAltitude_2->setPalette(*palette);

    palette->setColor(QPalette::WindowText,Qt::red);
    ui->lPressure_2->setPalette(*palette);

    palette->setColor(QPalette::WindowText,Qt::red);
    ui->lUV_2->setPalette(*palette);

    palette->setColor(QPalette::WindowText,Qt::blue);
    ui->lVisible_2->setPalette(*palette);

    palette->setColor(QPalette::WindowText,Qt::green);
    ui->lIR_2->setPalette(*palette);



    setSerial();
    TempCurve = new QwtPlotCurve("Temperature");
    HumidityCurve = new QwtPlotCurve("Humidity");
    AltitudeCurve = new QwtPlotCurve("Altitude");
    PressureCurve = new QwtPlotCurve("Pressure");
    UVIndexCurve = new QwtPlotCurve("UVIndex");
    VisibleCurve = new QwtPlotCurve("Visible");
    IRCurve = new QwtPlotCurve("IR");
    tempIndex = 0;
    humidityIndex = 0;
    altitudeIndex = 0;
    pressureIndex = 0;
    uvIndexIndex = 0;
    visibleIndex = 0;
    irIndex = 0;

    displayTempHumiPlot();
    displayAltitudePlot();
    displayPressurePlot();
    displayUVAmbientPlot();

    notRsRead = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notRsRead, SIGNAL(activated(int)), this, SLOT(updateData()));

}

WeatherBoard::~WeatherBoard()
{
    ::close(fd);
}

void WeatherBoard::setSerial()
{
    fd = -1;

    fd = open(DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    memset(&newtio, 0, sizeof(newtio));

    if (fd < 0)
        printf("/dev/ttyUSB0 open fail!\n");

    tcgetattr(fd, &newtio);

    cfsetispeed(&newtio, baudrate);
    cfsetospeed(&newtio, baudrate);

    newtio.c_cflag |= CS8;
    newtio.c_iflag |= IGNBRK;
    newtio.c_iflag &= ~( BRKINT | ICRNL | IMAXBEL | IXON);
    newtio.c_oflag &= ~( OPOST | ONLCR );
    newtio.c_lflag &= ~( ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK |
                            ECHOCTL | ECHOKE);
    newtio.c_lflag |= NOFLSH;
    newtio.c_cflag &= ~CRTSCTS;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
}



void WeatherBoard::displayTempHumiPlot()
{
    ui->qwtPlotTempHumi->setAxisScale(QwtPlot::yLeft, 0, 100);
    ui->qwtPlotTempHumi->setAxisScale(QwtPlot::xBottom, 0, 100);
    ui->qwtPlotTempHumi->setTitle("Temperature & Humidity");
    ui->qwtPlotTempHumi->setAxisTitle(QwtPlot::xBottom, "sec");
    ui->qwtPlotTempHumi->setCanvasBackground(QBrush(QColor(0, 0, 0)));

    TempCurve->attach(ui->qwtPlotTempHumi);
    TempCurve->setPen(QColor(255, 0, 0));
    HumidityCurve->attach(ui->qwtPlotTempHumi);
    HumidityCurve->setPen(QColor(100, 100, 255));
}
void WeatherBoard::displayAltitudePlot()
{
    ui->qwtPlotAltitude->setAxisScale(QwtPlot::yLeft, 0, 1000);
    ui->qwtPlotAltitude->setAxisScale(QwtPlot::xBottom, 0, 100);
    ui->qwtPlotAltitude->setTitle("Altitude");
    ui->qwtPlotAltitude->setAxisTitle(QwtPlot::xBottom, "sec");
    ui->qwtPlotAltitude->setCanvasBackground(QBrush(QColor(0, 0, 0)));

    AltitudeCurve->attach(ui->qwtPlotAltitude);
    AltitudeCurve->setPen(QColor(255, 0, 0));
}

void WeatherBoard::displayPressurePlot()
{
    ui->qwtPlotPressure->setAxisScale(QwtPlot::yLeft, 0, 150000);
    ui->qwtPlotPressure->setAxisScale(QwtPlot::xBottom, 0, 100);
    ui->qwtPlotPressure->setTitle("Pressure");
    ui->qwtPlotPressure->setAxisTitle(QwtPlot::xBottom, "sec");
    ui->qwtPlotPressure->setCanvasBackground(QBrush(QColor(0, 0, 0)));

    PressureCurve->attach(ui->qwtPlotPressure);
    PressureCurve->setPen(QColor(255, 0, 0));
}

void WeatherBoard::displayUVAmbientPlot()
{
    ui->qwtPlotUVAmbient->setAxisScale(QwtPlot::yLeft, 0, 1000);
    ui->qwtPlotUVAmbient->setAxisScale(QwtPlot::xBottom, 0, 100);
    ui->qwtPlotUVAmbient->setTitle("UV Index & Visible & IR");
    ui->qwtPlotUVAmbient->setAxisTitle(QwtPlot::xBottom, "sec");
    ui->qwtPlotUVAmbient->setCanvasBackground(QBrush(QColor(0, 0, 0)));

    UVIndexCurve->attach(ui->qwtPlotUVAmbient);
    UVIndexCurve->setPen(QColor(255, 0, 0));
    VisibleCurve->attach(ui->qwtPlotUVAmbient);
    VisibleCurve->setPen(QColor(0, 255, 0));
    IRCurve->attach(ui->qwtPlotUVAmbient);
    IRCurve->setPen(QColor(100, 100, 255));
}

void WeatherBoard::updateData()
{
    memset(buf, 0x00, BUFF_SIZE);
    read(fd, readBuf, 1);

    if (readBuf[0] == 'w') {
        int i = 0;
        while (readBuf[0] != '\e') {
            read(fd, readBuf, 1);
            buf[i] = readBuf[0];
            i++;
            if (i > 8)
                break;
        }
        buf[i-1] = '\0';
        switch (buf[0]) {
        case '0':
            //ui->lcdNum_Temp->display(&buf[1]);
            //temperature = ::atof(&buf[1]);
            //drawTempCurve();
            break;
        case '1':
            ui->lcdNum_Pressure->display(&buf[1]);
            pressure = ::atoi(&buf[1]);
            drawPressureCurve();
            break;
        case '2':
            ui->lcdNum_Altitude->display(&buf[1]);
            altitude = ::atof(&buf[1]);
            drawAltitudeCurve();
            break;
        case '3':
            ui->lcdNum_Temp->display(&buf[1]);
            temperature = ::atof(&buf[1]);
            drawTempCurve();
            break;
        case '4':
            ui->lcdNum_Humidity->display(&buf[1]);
            humidity = ::atof(&buf[1]);
            drawHumidityCurve();
            break;
        case '5':
            ui->lcdNum_UV->display(&buf[1]);
            uvIndex = ::atof(&buf[1]);
            drawUVIndexCurve();
            break;
        case '6':
            ui->lcdNum_Visible->display(&buf[1]);
            visible = ::atoi(&buf[1]);
            drawVisibleCurve();
            break;
        case '7':
            ui->lcdNum_IR->display(&buf[1]);
            ir = ::atoi(&buf[1]);
            drawIRCurve();
            break;
        }

      }
}

void WeatherBoard::drawTempCurve()
{
    if (temperature > 0 && temperature < 100) {
        if (tempIndex < 99) {
            yTempData[tempIndex] = temperature;
            xTempData[tempIndex] = tempIndex;
            tempIndex++;
        } else {
            yTempData[99] = temperature;
            for (int i = 1; i <= 99; i++) {
                yTempData[i - 1] = yTempData[i];
            }
        }
    }

    TempCurve->setSamples(xTempData, yTempData, tempIndex);
    ui->qwtPlotTempHumi->replot();
}

void WeatherBoard::drawHumidityCurve()
{
    if (humidity > 0 && humidity < 100) {
        if (humidityIndex < 99) {
            yHumidityData[humidityIndex] = humidity;
            xHumidityData[humidityIndex] = humidityIndex;
            humidityIndex++;
        } else {
            yHumidityData[99] = humidity;
            for (int i = 1; i <= 99; i++) {
                yHumidityData[i - 1] = yHumidityData[i];
            }
        }
    }

    HumidityCurve->setSamples(xHumidityData, yHumidityData, humidityIndex);
    ui->qwtPlotTempHumi->replot();
}

void WeatherBoard::drawAltitudeCurve()
{
    if (altitude > 0 && altitude < 1000) {
        if (altitudeIndex < 99) {
            yAltitudeData[altitudeIndex] = altitude;
            xAltitudeData[altitudeIndex] = altitudeIndex;
            altitudeIndex++;
        } else {
            yAltitudeData[99] = altitude;
            for (int i = 1; i <= 99; i++) {
                yAltitudeData[i - 1] = yAltitudeData[i];
            }
        }
    }

    AltitudeCurve->setSamples(xAltitudeData, yAltitudeData, altitudeIndex);
    ui->qwtPlotAltitude->replot();
}

void WeatherBoard::drawPressureCurve()
{
    if (pressure > 0 && pressure < 150000) {
        if (pressureIndex < 99) {
            yPressureData[pressureIndex] = pressure;
            xPressureData[pressureIndex] = pressureIndex;
            pressureIndex++;
        } else {
            yPressureData[99] = pressure;
            for (int i = 1; i <= 99; i++) {
                yPressureData[i - 1] = yPressureData[i];
            }
        }
    }

    PressureCurve->setSamples(xPressureData, yPressureData, pressureIndex);
    ui->qwtPlotPressure->replot();
}

void WeatherBoard::drawUVIndexCurve()
{
    if (uvIndex > 0 && uvIndex < 1000) {
        if (uvIndexIndex < 99) {
            yUVIndexData[uvIndexIndex] = uvIndex;
            xUVIndexData[uvIndexIndex] = uvIndexIndex;
            uvIndexIndex++;
        } else {
            yUVIndexData[99] = uvIndex;
            for (int i = 1; i <= 99; i++) {
                yUVIndexData[i - 1] = yUVIndexData[i];
            }
        }
    }

    UVIndexCurve->setSamples(xUVIndexData, yUVIndexData, uvIndexIndex);
    ui->qwtPlotUVAmbient->replot();
}

void WeatherBoard::drawVisibleCurve()
{
    if (visible > 0 && visible < 1000) {
        if (visibleIndex < 99) {
            yVisibleData[visibleIndex] = visible;
            xVisibleData[visibleIndex] = visibleIndex;
            visibleIndex++;
        } else {
            yVisibleData[99] = visible;
            for (int i = 1; i <= 99; i++) {
                yVisibleData[i - 1] = yVisibleData[i];
            }
        }
    }

    VisibleCurve->setSamples(xVisibleData, yVisibleData, visibleIndex);
    ui->qwtPlotUVAmbient->replot();
}

void WeatherBoard::drawIRCurve()
{
    if (ir > 0 && ir < 1000) {
        if (irIndex < 99) {
            yIRData[irIndex] = ir;
            xIRData[irIndex] = irIndex;
            irIndex++;
        } else {
            yIRData[99] = ir;
            for (int i = 1; i <= 99; i++) {
                yIRData[i - 1] = yIRData[i];
            }
        }
    }

    IRCurve->setSamples(xIRData, yIRData, irIndex);
    ui->qwtPlotUVAmbient->replot();
}
