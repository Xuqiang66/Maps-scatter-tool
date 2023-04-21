# Maps-scatter-tool
Read database get gps point and show it on Gaode map

编译环境：VS2017+Qt5.9.1

资源文件（高德地图文件）：mymap_scatterpoint.html

软件功能：读取数据库，表名picture（代码中写死了，未根据数据库自行获取表名），界面上输入经度、纬度以及采集时间的字段索引值，选取数据库文件路径，点击开始进行撒点。

注：数据库中经纬度数据默认为wgs84坐标，所以软件做了个84转高德的坐标系转换
