#ifndef WEATHERBOARD_H
#define WEATHERBOARD_H

#include <QMainWindow>
#include <QSocketNotifier>
#include <QFile>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <termios.h>

#define BUFF_SIZE 32

namespace Ui {
	class WeatherBoard;
}

class WeatherBoard : public QMainWindow
{
	Q_OBJECT

public:
	explicit WeatherBoard(QWidget *parent = 0);
	~WeatherBoard();
    //void paintEvent(QPaintEvent *);

private:
	Ui::WeatherBoard *ui;
    QSocketNotifier *notRsRead;

    struct termios newtio;
    int fd;
	char buf[BUFF_SIZE];
    char readBuf[BUFF_SIZE];

    double xTempData[100];
    double yTempData[100];
    int tempIndex;

    double xHumidityData[100];
    double yHumidityData[100];
    int humidityIndex;

    double xAltitudeData[100];
    double yAltitudeData[100];
    int altitudeIndex;

    double xPressureData[100];
    double yPressureData[100];
    int pressureIndex;

    double xUVIndexData[100];
    double yUVIndexData[100];
    int uvIndexIndex;

    double xVisibleData[100];
    double yVisibleData[100];
    int visibleIndex;

    double xIRData[100];
    double yIRData[100];
    int irIndex;

    float temperature;
    float humidity;
    float altitude;
    int pressure;
    float uvIndex;
    int visible;
    int ir;

    QwtPlotCurve *TempCurve;
    QwtPlotCurve *HumidityCurve;
    QwtPlotCurve *PressureCurve;
    QwtPlotCurve *AltitudeCurve;
    QwtPlotCurve *UVIndexCurve;
    QwtPlotCurve *VisibleCurve;
    QwtPlotCurve *IRCurve;

    void setSerial(void);

    void displayTempHumiPlot(void);
    void displayAltitudePlot(void);
    void displayPressurePlot(void);
    void displayUVAmbientPlot(void);

    void drawTempCurve(void);
    void drawHumidityCurve(void);
    void drawAltitudeCurve(void);
    void drawPressureCurve(void);
    void drawUVIndexCurve(void);
    void drawVisibleCurve(void);
    void drawIRCurve(void);

private slots:
	void updateData();
};

#endif // WEATHERBOARD_H
