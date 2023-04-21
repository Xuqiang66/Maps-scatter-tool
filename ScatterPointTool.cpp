#include "ScatterPointTool.h"
#include <qthread.h>
#include <qtimer.h>
#include <qfiledialog.h>

ScatterPointTool::ScatterPointTool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	ui.pBtn_stop->setEnabled(false);

	database_name = "";
	csv_file_name = "";
	index_lon = 0;
	index_lat = 0;
	index_time = 0;
	isWithLabel = "";

	gga_lon_mark = 0.0;
	gga_lat_mark = 0.0;
	mark_int = 1;

	loadMap();

	connect(this, SIGNAL(stopScatterPointSignal()), this, SLOT(stopScatterPointSlot()));
}

ScatterPointTool::~ScatterPointTool()
{}

void ScatterPointTool::loadMap()
{
	int ret = 0;
	/**
	*Author: X..
	*Date:	 2022-10-20
	*Description: show map on the ui
	*/
	_myChannel = new myChannel(this);

	web_channel = new QWebChannel(this);
	web_channel->registerObject("qtChannel", _myChannel);
	//ui.webengineview->page()->setBackgroundColor(QColor(8, 21, 47));
	ui.webEngineView->page()->setWebChannel(web_channel);
	//ui.webengineview->load(QUrl("E:/githubCode/QtGaodeDemo/qt_gaode_demo/smap/mymap_ba.html"));
	//ui.webEngineView->load(QUrl("D:/smap/mymap_scatterpoint.html"));
	ui.webEngineView->load(QUrl("qrc:/ScatterPointTool/mymap_scatterpoint.html"));

	QThread::sleep(3);
	ui.label_info->setText(QString::fromLocal8Bit("地图加载成功"));

}

void ScatterPointTool::on_pBtn_start_clicked()
{
	index_lon = ui.lineEdit_lon->text().toInt();
	index_lat = ui.lineEdit_lat->text().toInt();
	index_time = ui.lineEdit_time->text().toInt();

	if ((database_name == "") && (csv_file_name == "")) {
		ui.label_info->setText(QString::fromLocal8Bit("请点击选择文件按钮,选择需要撒点的文件"));
	}
	else if (index_lon == 0 | index_lat == 0 | index_time == 0) {
		ui.label_info->setText(QString::fromLocal8Bit("请输入经纬度以及时间字段索引不能为0"));
	}
	else {
		isWithLabel = ui.comboBox->currentText();
		//qDebug() << "isWithLabel=" << isWithLabel;
		startOtherThread();
		ui.label_info->setText(QString::fromLocal8Bit("开始撒点"));
		ui.pBtn_start->setEnabled(false);
		ui.pBtn_choosefile->setEnabled(false);
		ui.pBtn_stop->setEnabled(true);
	}
	
}

void ScatterPointTool::on_pBtn_stop_clicked()
{
	stopScatterPointSignal();
	ui.label_info->setText(QString::fromLocal8Bit("结束撒点"));
	ui.pBtn_start->setEnabled(true);
	ui.pBtn_choosefile->setEnabled(true);
}

/*输入GNGGA经纬度，点击按钮会自动转换并撒点在地图上*/
void ScatterPointTool::on_pBtn_point_clicked()
{
	double gngga_lon = ui.lineEdit_gngga_lon->text().toDouble();
	double gngga_lat = ui.lineEdit_gngga_lat->text().toDouble();

	double gga_lon = (int)(gngga_lon / 100) + fmod(gngga_lon, 100) / 60;
	double gga_lat = (int)(gngga_lat / 100) + fmod(gngga_lat, 100) / 60;

	if ((gga_lon_mark != gga_lon) && (gga_lat_mark != gga_lat)) {
		QString mark = QString::number(mark_int);
		//addWithoutOneMarkBtnClickEvent(gga_lon, gga_lat);
		addOneMarkBtnClickEvent(gga_lon, gga_lat, mark);

		mark_int++;

		gga_lon_mark = gga_lon;
		gga_lat_mark = gga_lat;
	}

}

void ScatterPointTool::on_pBtn_choosefile_clicked()
{
	ui.label_info->setText(QString::fromLocal8Bit("请选择要撒点的数据库文件"));

	QString filename;
	//QWidget *qwidget = new QWidget();
	//filename = QFileDialog::getOpenFileName(qwidget, "choose file", "", nullptr);
	filename = QFileDialog::getOpenFileName(this, "choose file", "", nullptr);
	//qDebug() << "filename=" << filename;
	database_name = filename;
	csv_file_name = filename;
	QString showselectinfo = QString::fromLocal8Bit("当前选择文件是\n").append(database_name);
	ui.label_info->setText(showselectinfo);
}

static int callback_in_busy(void *ptr, int count)
{
	//int timeout = *((int *)ptr);
	//QThread::msleep(timeout);
	QThread::msleep(100);
	return 1;
}

static void SqlGetLock(sqlite3 *sqlite_p, int ms)
{
	if (ms > 0)
	{
		sqlite3_busy_handler(sqlite_p, callback_in_busy, (void*)&ms);
	}
	else
	{
		sqlite3_busy_handler(sqlite_p, 0, 0);
	}
}

void ScatterPointTool::stopScatterPointSlot()
{
	m_addMapPoint_timer->stop();
	m_addMapPoint_thread->quit();
}

void ScatterPointTool::startWithReadDatabaseThread()
{
	//database_name = "D://roadAcquisition.db";
	vector<picture_upload_info_t> t_picture_list;

	t_picture_list.clear();
	getFrontPicture(t_picture_list);

	for (int i = 0; i < t_picture_list.size(); i++)
	{
		double lon = QString::fromStdString(t_picture_list[i].longitude).toDouble();
		double lat = QString::fromStdString(t_picture_list[i].latitude).toDouble();
		QString time = QString::fromStdString(t_picture_list[i].time);
		//qDebug() << "time=" << time;

		/*ui.label_lon->setText(QString::number(lon, 'f', 8));
		ui.label_lat->setText(QString::number(lat, 'f', 8));*/

		addOneMarkBtnClickEvent(lon, lat, time);
	}

	if (t_picture_list.size() == 0) {
		stopScatterPointSignal();
		ui.label_info->setText(QString::fromLocal8Bit("结束撒点"));
	}
}

void ScatterPointTool::startWithoutReadDatabaseThread()
{
	//database_name = "D://roadAcquisition.db";
	vector<picture_upload_info_t> t_picture_list;

	t_picture_list.clear();
	getFrontPicture(t_picture_list);

	for (int i = 0; i < t_picture_list.size(); i++)
	{
		double lon = QString::fromStdString(t_picture_list[i].longitude).toDouble();
		double lat = QString::fromStdString(t_picture_list[i].latitude).toDouble();
		//QString time = QString::fromStdString(t_picture_list[i].time);
		//qDebug() << "time=" << time;

		addWithoutOneMarkBtnClickEvent(lon, lat);
	}

	if (t_picture_list.size() == 0) {
		stopScatterPointSignal();
		ui.label_info->setText(QString::fromLocal8Bit("结束撒点"));
	}
}

int ScatterPointTool::updateFrontPicture()
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	const char *zTail;
	string sql = "";
	char *ptr = nullptr;

	if (FileExists(database_name.toStdString()))
	{
		if (open_db(database_name.toStdString(), &db) == 0)
		{
			//SqlGetLock(db, 50);
			
			sql = "update picture set upload_status=0 where upload_status=1";
			exec_db("updateFrontPicture", db, sql, NULL);
				
			ui.label_info->setText(QString::fromLocal8Bit("数据库初始化成功"));
		}
	}
	else
	{
		ui.label_info->setText(QString::fromLocal8Bit("未查找到数据库文件"));
		goto ERRORRETURN;
	}

	close_db(db);
	return 0;
ERRORRETURN:
	return -1;
}

int ScatterPointTool::getFrontPicture(vector<picture_upload_info_t> &p_picture_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	const char *zTail;
	string sql = "";
	char *ptr = nullptr;
	picture_upload_info_t picture_upload_info;

	if (FileExists(database_name.toStdString()))
	{
		if (open_db(database_name.toStdString(), &db) == 0)
		{
			SqlGetLock(db, 50);
			sql = "select * from picture where Continent_ID='0' limit 3;";
			//sql = "select MAX(id) from picture;";
			sqlite3_exec(db, "PRAGMA synchronous = OFF; ", 0, 0, 0);
			exec_db("getFrontPicture", db, "begin;", NULL);
			if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &zTail) == SQLITE_OK)
			{
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					int id = sqlite3_column_int(stmt, 0);
					qDebug() << "id:" << id;
					
					string latitude = (char *)sqlite3_column_text(stmt, index_lat);
					string longitude = (char*)sqlite3_column_text(stmt, index_lon);
					string time = (char*)sqlite3_column_text(stmt, index_time);

					picture_upload_info.time = time;
					picture_upload_info.latitude = latitude;
					picture_upload_info.longitude = longitude;

					p_picture_list.push_back(picture_upload_info);

					sql = "update picture set Continent_ID='1' where id=" + to_string(id);
					exec_db("getFrontPicture", db, sql, NULL);
				}
			}
			else
			{
				exec_db("getFrontPicture", db, "commit;", NULL);
				close_db(db);
				goto ERRORRETURN;
			}
			sqlite3_finalize(stmt);
		}
	}
	else
	{
		ui.label_info->setText(QString::fromLocal8Bit("未查找到数据库文件"));
		goto ERRORRETURN;
	}
	exec_db("getFrontPicture", db, "commit;", NULL);
	close_db(db);
	return 0;
ERRORRETURN:
	return -1;
}

void ScatterPointTool::startOtherThread()
{
	m_addMapPoint_thread = new QThread();
	m_addMapPoint_timer = new QTimer();
	m_addMapPoint_timer->setTimerType(Qt::PreciseTimer);
	m_addMapPoint_timer->start(100);
	m_addMapPoint_timer->moveToThread(m_addMapPoint_thread);
	if (isWithLabel == "WithLabel") {
		connect(m_addMapPoint_timer, SIGNAL(timeout()), this, SLOT(startWithReadDatabaseThread()), Qt::DirectConnection);//Qt::DirectConnection关键，否则定时器不运行
	}
	else {
		connect(m_addMapPoint_timer, SIGNAL(timeout()), this, SLOT(startWithoutReadDatabaseThread()), Qt::DirectConnection);//Qt::DirectConnection关键，否则定时器不运行
	}
	
	m_addMapPoint_thread->start();



	//m_addMapCsvPoint_thread = new QThread();
	//m_addMapCsvPoint_timer = new QTimer();
	//m_addMapCsvPoint_timer->setTimerType(Qt::PreciseTimer);
	//m_addMapCsvPoint_timer->start();
	//m_addMapCsvPoint_timer->moveToThread(m_addMapCsvPoint_thread);
	//connect(m_addMapCsvPoint_timer, SIGNAL(timeout()), this, SLOT(readCsvFile2Map()), Qt::DirectConnection);//Qt::DirectConnection关键，否则定时器不运行
	//m_addMapCsvPoint_thread->start();

}

void ScatterPointTool::readCsvFile2Map()
{
	if (csv_file_name == "") {
		qDebug() << "csv file not exist.";
		return;
	}
	m_addMapCsvPoint_timer->stop();
	qDebug() << "csv_file_name:" << csv_file_name;
	QFile inFile(csv_file_name);
	QStringList lines;
	if (inFile.open(QIODevice::ReadOnly)) {
		QTextStream stream_text(&inFile);
		while (!stream_text.atEnd()) {
			lines.push_back(stream_text.readLine());//行读取
		}
		qDebug() << "lines.size():" << lines.size();
		for (int j = 20; j < lines.size(); j+=1) {
			QString line = lines.at(j);
			QStringList split = line.split(",");

			rectArr.x1_long = split.at(0).toDouble();
			rectArr.y1_lat = split.at(1).toDouble();
				
			rectInfo.push_back(rectArr);
				
		}
		inFile.close();
	}

	int mark_int1 = 0;
	for (int i = 0; i < rectInfo.size(); i++)
	{
		double lon = rectInfo[i].x1_long;
		double lat = rectInfo[i].y1_lat;
		//addWithoutOneMarkBtnClickEvent(lon, lat);

		QString mark = QString::number(mark_int1);
		addOneMarkBtnClickEvent(lon, lat, mark);

		mark_int1++;

		QThread::msleep(10);

		if (i == rectInfo.size()) {
			ui.label_info->setText(QString::fromLocal8Bit("结束撒点"));
		}
	}
	
	
}

void ScatterPointTool::addOneMarkBtnClickEvent(double lon, double lat, QString p_time)
{
	QString longitude, latitude, time= "";
	POSITION ws84;
	double m_lon = lon;
	double m_lat = lat;
	time = p_time;

	ws84 = wgs84togcj02(m_lon, m_lat);
	longitude = QString::number(ws84.longitude, 'f', 8);
	latitude = QString::number(ws84.latitude, 'f', 8);

	_myChannel->addOneMark(longitude, latitude, time);
	

}

void ScatterPointTool::addWithoutOneMarkBtnClickEvent(double lon, double lat)
{
	QString longitude, latitude= "";
	POSITION ws84;
	double m_lon = lon;
	double m_lat = lat;

	ws84 = wgs84togcj02(m_lon, m_lat);
	longitude = QString::number(ws84.longitude, 'f', 8);
	latitude = QString::number(ws84.latitude, 'f', 8);

	_myChannel->addWithoutOneMark(longitude, latitude);


}

//经度转换
double ScatterPointTool::translate_lon(double lon, double lat)
{
	double ret = 300.0 + lon + 2.0*lat + 0.1*lon*lon + 0.1*lon*lat + 0.1*sqrt(abs(lon));
	ret += (20.0 * sin(6.0*lon*PI) + 20.0*sin(2.0*lon*PI)) *2.0 / 3.0;
	ret += (20.0 * sin(lon*PI) + 40.0*sin(lon / 3.0 *PI)) *2.0 / 3.0;
	ret += (150 * sin(lon / 12.0 *PI) + 300.0*sin(lon / 30.0 * PI)) *2.0 / 3.0;
	return ret;
}

//纬度转换
double ScatterPointTool::translate_lat(double lon, double lat)
{
	double ret = -100 + 2.0*lon + 3.0*lat + 0.2*lat*lat + 0.1*lon*lat + 0.2*sqrt((abs(lon)));
	ret += (20.0 *sin(6.0*lon*PI) + 20 * sin(2.0*lon*PI)) *2.0 / 3.0;
	ret += (20.0 *sin(lat*PI) + 40.0*sin(lat / 3.0*PI)) *2.0 / 3.0;
	ret += (160.0*sin(lat / 12.0*PI) + 320.0*sin(lat / 30.0 *PI)) *2.0 / 3.0;
	return ret;
}


POSITION ScatterPointTool::wgs84togcj02(double wgs_lon, double wgs_lat)
{
	double dlat = translate_lat(wgs_lon - 105.0, wgs_lat - 35.0);
	double dlon = translate_lon(wgs_lon - 105.0, wgs_lat - 35.0);
	double radlat = wgs_lat / 180.0 * PI;
	double magic = sin(radlat);
	magic = 1 - ee * magic*magic;
	double squrtmagic = sqrt(magic);
	dlon = (dlon *180.0) / (a / squrtmagic * cos(radlat)*PI);
	dlat = (dlat *180.0) / ((a*(1 - ee)) / (magic * squrtmagic)*PI);
	gcj_pos.longitude = wgs_lon + dlon;
	gcj_pos.latitude = wgs_lat + dlat;
	return gcj_pos;
}

bool ScatterPointTool::FileExists(const string p_path) {
	if (p_path.length() <= 0) {
		return true;
	}
	fstream _file;
	_file.open(p_path, ios::in);
	if (_file) {
		_file.close();
		return true;
	}
	return false;
}

int ScatterPointTool::open_db(const string p_dbname, sqlite3 **db)
{
	int rc;
	string print_str = "";
	const char *errmsg_ptr = nullptr;

	//rc = sqlite3_open(p_dbname.c_str(), db);
	rc = sqlite3_open_v2(p_dbname.c_str(), db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL);
	if (rc != SQLITE_OK) {
		//数据库打开失败，打印错误日志
		print_str = p_dbname;
		print_str.append(" open error");
		errmsg_ptr = sqlite3_errmsg(*db);
		if (errmsg_ptr) {
			print_str.append(errmsg_ptr);
		}


		return -1;
	}
	else {
		return 0;
	}
	return -1;
}

static int callback(void *data, int argc, char **argv, char **azColName) {
	int i;
	if (data == NULL)
	{
		return 0;
	}
	//sprintf_s((char *)data, 100, "%s", "sdferoptgergm");
	//fprintf(stderr, "%s: ", (const char*)data);
	for (i = 0; i < argc; i++) {
		sprintf_s((char *)data, 1024, "%s%s:%s;", (char *)data, azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int ScatterPointTool::exec_db(string p_commont, sqlite3 * p_db, const string p_sql, char* p_data)
{
	int rc;
	char *zErrMsg = NULL;
	string print_str = "";
	if (!p_db) {
		print_str = "";
		print_str.append("database is NULL");
		return -1;
	}
	rc = sqlite3_exec(p_db, p_sql.c_str(), callback, p_data, &zErrMsg);

	if (rc != SQLITE_OK) {
		print_str = p_commont;
		print_str.append("SQL exec error: ");
		print_str.append(rc + " ");
		if (zErrMsg) {
			print_str.append(zErrMsg);
		}

		sqlite3_free(zErrMsg);
		return -1;
	}
	else {
		;
	}

	return 0;
}

void ScatterPointTool::close_db(sqlite3 * p_db)
{
	sqlite3_close(p_db);
}

