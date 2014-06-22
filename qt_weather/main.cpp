#include <QtGui/QApplication>
#include "weather_board.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WeatherBoard w;
	w.show();

	return a.exec();
}
