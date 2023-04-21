#include "myChannel.h"

myChannel::myChannel(QObject *parent)
    :QObject(parent)
{

}

void myChannel::addOneMark(QString lng, QString lat, QString p_time)
{
	emit addOneMarkSingal(lng, lat, p_time);
}

void myChannel::addWithoutOneMark(QString lng, QString lat)
{
	emit addWithoutOneMarkSingal(lng, lat);
}

void myChannel::addPolyLine(QString lng, QString lat/*QStringList p_pth*/)
{
	emit addPolyLineSingal(lng, lat/*p_pth*/);
}
