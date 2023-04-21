#ifndef __MYCHANNEL_H__
#define __MYCHANNEL_H__

#include <QWebChannel>
#include <QJsonObject>

class myChannel :public QObject 
{
	Q_OBJECT
public:
    explicit myChannel(QObject* parent=nullptr);

	void addOneMark(QString lng, QString lat, QString p_time);
	void addWithoutOneMark(QString lng, QString lat);
	void addPolyLine(QString lng, QString lat/*QStringList p_pth*/);

public slots:


signals:
	void addOneMarkSingal(QString p_lngStr, QString p_latStr, QString p_time);
	void addWithoutOneMarkSingal(QString p_lngStr, QString p_latStr);
	void addPolyLineSingal(QString p_lngStr, QString p_latStr/*QStringList p_pthStr*/);

};


#endif
