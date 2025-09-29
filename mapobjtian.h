#ifndef MAPOBJTIAN_H
#define MAPOBJTIAN_H

#include "mapobjbase.h"

class MapObjTian : public MapObjBase
{
    Q_OBJECT
public:
    explicit MapObjTian(QObject *parent = 0);

private:
    //添加网页头部
    void addHead();
    //添加地图函数
    void addMap();

    //启用禁用功能
    void setEnable();
    //设置地图样式
    void setMapStyle();
    //设置地图类型
    void setMapType();

    //添加坐标转换
    void addConvertor();
    //添加地址解析
    void addGeocoder();
    //添加本地搜索
    void addSearch();
    //添加路线查询
    void addRoute();

    //添加测距工具
    void addDistanceTool();

    //添加标注点
    void addMarker();
    //添加折线
    void addPolyline();
    //添加多边形
    void addPolygon();
    //添加圆形
    void addCircle();
    //添加覆盖物（可留空或降级提示）
    void addOverlay();
    //添加轨迹移动（最小声明moves避免未定义）
    void addMove();
};

#endif // MAPOBJTIAN_H


