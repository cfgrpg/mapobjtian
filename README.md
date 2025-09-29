# mapobjtian
QT天地图，购买的video_system代码默认只支持百度地图，由于现在公司使用最低5w起，作者的框架已经支持了百度地图 ，但是天地图需要另外付费，项目的中的文件是仿照百度地图的类写的天地图的类

使用办法，
1、your tk替换成自己申请的tk
MapObjTian::MapObjTian(QObject *parent) : MapObjBase(parent)
{
    // 使用用户提供的密钥
    // 版本号取用与百度类似的versionKey约定："4.0&tk=xxxx"
    versionKey = "4.0&tk=your tk";

2、video_system\video_system.pro 中增加天地图的支持
DEFINES += tianx

3、将类文件下载到video_system\core_map\mapobj目录下面


http://www.qtcdev.com/video_system 

国内站点：https://gitee.com/feiyangqingyun

国际站点：https://github.com/feiyangqingyun
