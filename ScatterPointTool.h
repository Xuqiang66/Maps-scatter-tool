#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ScatterPointTool.h"
#include "myChannel.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <QWebChannel>
#include <QWebEngineView>
#include <QtWebEngineWidgets/QWebEnginePage>
#include "sqlite3.h"

using namespace std;
constexpr auto PI = 3.1415926535897932384626;

typedef struct _POSITION
{
	double longitude;
	double latitude;
}POSITION;

typedef struct {
	int sql_id;
	string dm_data_company_id;
	string road_type;
	string pictue_dir;
	string pictues_name;
	string time;
	string latitude;
	string longitude;
	string speed;
	string angle;
	string altitude;
	string distance;
}picture_upload_info_t;

typedef struct {
	double x1_long;
	double y1_lat;
}rect_info_t;

class ScatterPointTool : public QMainWindow
{
    Q_OBJECT

public:
    ScatterPointTool(QWidget *parent = nullptr);
    ~ScatterPointTool();

public slots:
	void startWithReadDatabaseThread();
	void startWithoutReadDatabaseThread();
	void on_pBtn_start_clicked();
	void on_pBtn_stop_clicked();
	void stopScatterPointSlot();
	void on_pBtn_choosefile_clicked();
	void on_pBtn_point_clicked();
	void readCsvFile2Map();

signals:
	void stopScatterPointSignal();

private:
	void loadMap();
	void startOtherThread();
	double translate_lon(double lon, double lat);
	double translate_lat(double lon, double lat);
	POSITION wgs84togcj02(double wgs_lon, double wgs_lat);
	int getFrontPicture(vector<picture_upload_info_t> &p_picture_list);
	int updateFrontPicture();
	void addOneMarkBtnClickEvent(double lon, double lat, QString p_time);
	void addWithoutOneMarkBtnClickEvent(double lon, double lat);

	bool FileExists(const string p_path);
	int open_db(const string p_dbname, sqlite3 **db);
	int exec_db(string p_commont, sqlite3 * p_db, const string p_sql, char* p_data);
	void close_db(sqlite3 * p_db);

private:
    Ui::ScatterPointToolClass ui;

	POSITION gcj_pos;
	double a = 6378245.0;
	double ee = 0.00669342162296594323;

	myChannel * _myChannel;
	QWebChannel* web_channel;

	QThread *m_addMapPoint_thread = nullptr;
	QTimer *m_addMapPoint_timer = nullptr;

	QThread *m_addMapCsvPoint_thread = nullptr;
	QTimer *m_addMapCsvPoint_timer = nullptr;

	QString database_name;
	QString csv_file_name;
	int index_lon;
	int index_lat;
	int index_time;
	QString isWithLabel;

	/*点击按钮，GNGGA经纬度转换并撒点在地图上*/
	double gga_lon_mark;
	double gga_lat_mark;
	int mark_int;

	rect_info_t rectArr;
	vector<rect_info_t> rectInfo;
};
