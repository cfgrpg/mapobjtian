#include "mapobjtian.h"

// 官方文档（JS API v4.x）：http://lbs.tianditu.gov.cn/server/js/api.html
// 在线脚本入口通常为：http://api.tianditu.gov.cn/api?v=4.0&tk=YOUR_KEY
MapObjTian::MapObjTian(QObject *parent) : MapObjBase(parent)
{
    // 使用用户提供的密钥
    // 版本号取用与百度类似的versionKey约定："4.0&tk=xxxx"
    versionKey = "4.0&tk=your tk";

    zoomCur = 12;
    zoomMin = 3;
    zoomMax = 18;
}

void MapObjTian::addHead()
{
    QString pathMap = "map_load.js";
    QString pathTools = "tools";

    if (!mapLocal) {
        QString url = "http://api.tianditu.gov.cn/";
        pathMap = url + "api?v=" + versionKey;
    }

    QStringList scripts;
    // 先注入兜底函数与变量，避免外部脚本错误导致后续runJs找不到函数
    scripts << QString("<script type=\"text/javascript\">\n"
                     "var markers = markers || [];\n"
                     "var moves = moves || [];\n"
                     "function deleteOverlay(){ }\n"
                     "function addMarker(){ }\n"
                     "function addMarkerEvent(){ }\n"
                     "</script>");
    scripts << QString("<script type=\"text/javascript\" src=\"%1\"></script>").arg(pathMap);
    // 注入天地图密钥至页面上下文（从 versionKey 提取 tk= 后的值）
    QString tdtKey = versionKey;
    int pos = tdtKey.indexOf("tk=");
    QString onlyKey = (pos >= 0 ? tdtKey.mid(pos + 3) : tdtKey);
    scripts << QString("<script type=\"text/javascript\">var tdtTk='%1';</script>").arg(onlyKey);
    MapObjBase::addHead(scripts);
}

void MapObjTian::addMap()
{
    // 初始化地图
    html << QString("  var map = new T.Map('map', {minZoom:%1, maxZoom:%2});").arg(zoomMin).arg(zoomMax);
    html << QString("  var point = getPoint('%1');").arg(centerPoint);
    html << QString("  map.centerAndZoom(point, %1);").arg(zoomCur);
    html << QString("  map.enableScrollWheelZoom();");

    // 导航控件
    html << QString("  if (%1 & %2) { map.addControl(new T.Control.Zoom()); }")
               .arg(QString::number(mapControl)).arg(QString::number(MapControl_Navigation));

    // 比例尺控件
    html << QString("  if (%1 & %2) { map.addControl(new T.Control.Scale()); }")
               .arg(QString::number(mapControl)).arg(QString::number(MapControl_Scale));

    // 地图类型控件（自定义DOM控制条，中文标签采用Unicode转义防止乱码）
    html << QString("  if (%1 & %2) {")
               .arg(QString::number(mapControl)).arg(QString::number(MapControl_MapType));
    html << QString("    var mt = document.getElementById('tdt_maptype_ctrl');");
    html << QString("    if (!mt) { mt = document.createElement('div'); mt.id='tdt_maptype_ctrl'; document.body.appendChild(mt); }");
    html << QString("    mt.style.position='absolute'; mt.style.top='5px'; mt.style.right='5px'; mt.style.zIndex='9999';");
    html << QString("    mt.style.background='rgba(255,255,255,0.9)'; mt.style.border='1px solid #9db4d3'; mt.style.borderRadius='3px'; mt.style.padding='2px'; mt.style.fontSize='12px'; mt.style.boxShadow='0 1px 3px rgba(0,0,0,0.2)';");
    html << QString("    var st = document.getElementById('tdt_maptype_css');");
    html << QString("    if (!st){ st=document.createElement('style'); st.id='tdt_maptype_css'; st.type='text/css'; document.head.appendChild(st); }");
    html << QString("    st.innerHTML = '#tdt_maptype_ctrl .btn{border:1px solid #9db4d3;background:#fff;padding:2px 8px;margin:0 2px;border-radius:2px;cursor:pointer;color:#333;}' + '#tdt_maptype_ctrl .btn.active{background:#4c8bf5;color:#fff;border-color:#4c8bf5;}' + '#tdt_maptype_ctrl label{color:#333;}';");
    html << QString("    mt.innerHTML=\"<button class=\\'btn active\\' id=\\'tdt_btn_vec\\'>\\u5730\\u56fe</button><button class=\\'btn\\' id=\\'tdt_btn_sat\\'>\\u536b\\u661f</button><label id=\\'tdt_hyb_wrap\\' style=\\'display:none;margin-left:6px;\\'><input type=\\'checkbox\\' id=\\'tdt_chk_hyb\\'/> \\u6df7\\u5408</label>\";");
    html << QString("    var bvec=document.getElementById('tdt_btn_vec'); var bsat=document.getElementById('tdt_btn_sat');");
    html << QString("    var hybWrap=document.getElementById('tdt_hyb_wrap'); var chk=document.getElementById('tdt_chk_hyb');");
    html << QString("    function setActive(vecActive){ if (bvec&&bsat){ if (vecActive){ bvec.classList.add('active'); bsat.classList.remove('active'); } else { bsat.classList.add('active'); bvec.classList.remove('active'); } } }");
    html << QString("    if (bvec) { bvec.onclick=function(){ setActive(true); setMapType(0); if (hybWrap){hybWrap.style.display='none';} if (chk){chk.checked=false;} }; }");
    html << QString("    if (bsat) { bsat.onclick=function(){ setActive(false); if (hybWrap){hybWrap.style.display='inline-block';} var hybrid=(chk&&chk.checked); setMapType(hybrid?2:1); }; }");
    html << QString("    if (chk) { chk.onchange=function(){ setActive(false); setMapType(this.checked?2:1); }; }");
    html << QString("  }");
}

void MapObjTian::setEnable()
{
    html << QString("  function setEnable(type, enable) {");
    html << QString("    if (type == 1) {");
    html << QString("      enable ? map.enableDrag() : map.disableDrag();");
    html << QString("    } else if (type == 2) {");
    html << QString("      // keyboard toggle not available in TDT v4");
    html << QString("    } else if (type == 3) {");
    html << QString("      enable ? map.enableScrollWheelZoom() : map.disableScrollWheelZoom();");
    html << QString("    } else if (type == 4) {");
    html << QString("      enable ? map.enableDoubleClickZoom() : map.disableDoubleClickZoom();");
    html << QString("    } else if (type == 5) {");
    html << QString("      // pinch toggle not available");
    html << QString("    } else if (type == 6) {");
    html << QString("      // continuous zoom toggle not available");
    html << QString("    } else if (type == 7) {");
    html << QString("      // inertial dragging toggle not available");
    html << QString("    } else if (type == 10) {");
    html << QString("      initDistanceTool();");
    html << QString("    }");
    html << QString("  }");
}

void MapObjTian::setMapStyle()
{
    // 天地图支持样式切换（以矢量底图样式示意，具体样式标识见官方）
    html << QString("  function setMapStyle(style) {");
    // html << QString("    // 可根据style切换不同底图图层，如normal/black/indigo");
    // html << QString("    // 这里只切换底图类型，不额外加载图层");
    html << QString("  }");
}

void MapObjTian::setMapType()
{
    // 基础底图切换：0=矢量 1=卫星 2=混合(影像+注记)
    html << QString("  var layVec, layCva, layImg, layCia;");
    html << QString("  function tdSub(x,y,z){ return (x+y+z)%8; }");
    html << QString("  function tdUrl(prefix, layer, x, y, z){ var idx=tdSub(x,y,z); return 'http://t'+idx+'.tianditu.gov.cn/'+prefix+'/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER='+layer+'&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&tk=' + tdtTk + '&TILECOL='+x+'&TILEROW='+y+'&TILEMATRIX='+z; }");
    html << QString("  function ensureBaseLayers(){");
    html << QString("    if (!layVec) { layVec = new T.TileLayer('', {minZoom:%1, maxZoom:%2}); layVec.getTileUrl=function(t){return tdUrl('vec_w','vec',t.x,t.y,t.z);} }").arg(zoomMin).arg(zoomMax);
    html << QString("    if (!layCva) { layCva = new T.TileLayer('', {minZoom:%1, maxZoom:%2}); layCva.getTileUrl=function(t){return tdUrl('cva_w','cva',t.x,t.y,t.z);} }").arg(zoomMin).arg(zoomMax);
    html << QString("    if (!layImg) { layImg = new T.TileLayer('', {minZoom:%1, maxZoom:%2}); layImg.getTileUrl=function(t){return tdUrl('img_w','img',t.x,t.y,t.z);} }").arg(zoomMin).arg(zoomMax);
    html << QString("    if (!layCia) { layCia = new T.TileLayer('', {minZoom:%1, maxZoom:%2}); layCia.getTileUrl=function(t){return tdUrl('cia_w','cia',t.x,t.y,t.z);} }").arg(zoomMin).arg(zoomMax);
    html << QString("  }");
    html << QString("  function clearBaseLayers(){");
    html << QString("    if (layVec) map.removeLayer(layVec);");
    html << QString("    if (layCva) map.removeLayer(layCva);");
    html << QString("    if (layImg) map.removeLayer(layImg);");
    html << QString("    if (layCia) map.removeLayer(layCia);");
    html << QString("  }");
    html << QString("  function setMapType(type) {");
    html << QString("    ensureBaseLayers();");
    html << QString("    clearBaseLayers();");
    html << QString("    if (type == 1) {");
    html << QString("      map.addLayer(layImg); map.addLayer(layCia);");
    html << QString("    } else if (type == 2) {");
    html << QString("      map.addLayer(layImg); map.addLayer(layCva);");
    html << QString("    } else {");
    html << QString("      map.addLayer(layVec); map.addLayer(layCva);");
    html << QString("    }");
    html << QString("  }");
}

void MapObjTian::addConvertor()
{
    // 天地图在线JS未提供统一坐标转换接口，这里保持空实现
}

void MapObjTian::addGeocoder()
{
    // 简要实现（若无需要可留空）
    html << QString("  var geocoder;");
    html << QString("  function initGeocoder() {");
    html << QString("    if (geocoder) {return}");
    html << QString("    if (T.Geocoder) { geocoder = new T.Geocoder(); }");
    html << QString("  }");

    html << QString("  function getPointByAddr(flag, addr) {");
    html << QString("    initGeocoder();");
    html << QString("    if (!geocoder) {return}");
    html << QString("    geocoder.getPoint(addr, function(result){");
    html << QString("      if (result) { receiveData('geocoderresult', flag + '|' + result.lng + ',' + result.lat); }");
    html << QString("    });");
    html << QString("  }");

    html << QString("  function getAddrByPoint(flag, point) {");
    html << QString("    initGeocoder();");
    html << QString("    if (!geocoder) {return}");
    html << QString("    geocoder.getLocation(getPoint(point), function(res){");
    html << QString("      if (res && res.address) { receiveData('geocoderresult', flag + '|' + res.address); }");
    html << QString("    });");
    html << QString("  }");
}

void MapObjTian::addSearch()
{
    html << QString("  var localSearch;");
    html << QString("  function initLocalSearch(){");
    html << QString("    if (localSearch) {return}");
    html << QString("    if (T.LocalSearch) { localSearch = new T.LocalSearch(map); }");
    html << QString("  }");

    // 通用搜索（与百度一致的签名）
    html << QString("  function search(keyword, city){");
    html << QString("    initLocalSearch();");
    html << QString("    if (!localSearch) { return; }");
    html << QString("    localSearch.search(keyword);");
    html << QString("    if (localSearch.setSearchCompleteCallback){");
    html << QString("      localSearch.setSearchCompleteCallback(function(result){");
    html << QString("        var datas = [];");
    html << QString("        if (result && result.getPois){");
    html << QString("          var pois = result.getPois();");
    html << QString("          for (var i=0; i<pois.length; ++i){");
    html << QString("            var p = pois[i]; var pt = p.lnglat || p.lonlat || p.location || p.point; ");
    html << QString("            if (pt){ datas.push((p.name||'') + ';' + (p.address||'') + ';' + getPointString(pt)); }");
    html << QString("          }");
    html << QString("        }");
    html << QString("        if (datas.length>0){ receiveData('searchresult', datas.join('|')); }");
    html << QString("      });");
    html << QString("    }");
    html << QString("  }");

    // 兼容函数占位
    html << QString("  function searchNearby(keyword, center, radius){ search(keyword); }");
    html << QString("  function searchInBounds(keyword, start, end){ search(keyword); }");
}

void MapObjTian::addRoute()
{
    // 暂无官方JS路线规划，提供占位以避免调用错误
    html << QString("  function initRoute(){ }");
    html << QString("  function searchRoute(type, policy, start, end){ alert('Route search not supported on this map core'); }");
}

void MapObjTian::addDistanceTool()
{
    // 简易测距：记录点击点集合并累计距离（演示用）
    html << QString("  var distanceTool, distancePoints = [];");
    html << QString("  function initDistanceTool() {");
    html << QString("    if (distanceTool) {return}");
    html << QString("    distanceTool = true;");
    html << QString("    map.addEventListener('click', function(e){");
    html << QString("      distancePoints.push(e.lnglat || e.lngLat || e); // 兼容字段");
    html << QString("      if (distancePoints.length > 1) { receiveData('distance', distancePoints.length); }");
    html << QString("    });");
    html << QString("  }");
}

void MapObjTian::addMarker()
{
    // 标注点：声明数组并实现基础添加
    html << QString("  var markers = [];");
    html << QString("  function addMarker(flag, point, name, image, width, height, center, index) {");
    MapObjBase::addMarker();
    html << QString("    var pos = getPoint(point);");
    html << QString("    var marker = new T.Marker(pos);");
    html << QString("    marker.flag = flag;");
    html << QString("    marker.pos = point;");
    html << QString("    if (name) {");
    html << QString("      var label = new T.Label({text:name, position:pos, offset:new T.Point(width, 0)});");
    html << QString("      marker.label = label;");
    html << QString("      map.addOverLay(label);");
    html << QString("    }");
    html << QString("    if (image) {");
    html << QString("      var icon = new T.Icon({iconUrl:image, iconSize:new T.Point(width, height)});");
    html << QString("      marker.setIcon(icon);");
    html << QString("    }");
    html << QString("    map.addOverLay(marker);");
    html << QString("    markers.push(marker);");
    html << QString("    return marker;");
    html << QString("  }");
}

void MapObjTian::addPolyline()
{
    html << QString("  function addPolyline(flag, points, color, weight, opacity, style, arrow) {");
    html << QString("    var pts = getPoints(points);");
    html << QString("    var property = getOverlayProperty(color, weight, opacity, style);");
    html << QString("    var line = new T.Polyline(pts, property);");
    html << QString("    line.flag = flag;");
    html << QString("    map.addOverLay(line);");
    html << QString("    return line;");
    html << QString("  }");
}

void MapObjTian::addPolygon()
{
    html << QString("  function addPolygon(flag, points, color, weight, opacity, style) {");
    html << QString("    var pts = getPoints(points);");
    html << QString("    var property = getOverlayProperty(color, weight, opacity, style);");
    html << QString("    var polygon = new T.Polygon(pts, property);");
    html << QString("    polygon.flag = flag;");
    html << QString("    map.addOverLay(polygon);");
    html << QString("    return polygon;");
    html << QString("  }");
}

void MapObjTian::addCircle()
{
    html << QString("  function addCircle(flag, point, radius, color, weight, opacity, style) {");
    html << QString("    var center = getPoint(point);");
    html << QString("    var property = getOverlayProperty(color, weight, opacity, style);");
    html << QString("    var circle = new T.Circle(center, radius, property);");
    html << QString("    circle.flag = flag;");
    html << QString("    map.addOverLay(circle);");
    html << QString("    return circle;");
    html << QString("  }");
}

void MapObjTian::addOverlay()
{
    html << QString("  function addCurveLine() {");
    html << QString("    alert('Curve not supported on this map core');");
    html << QString("  }");
}

void MapObjTian::addMove()
{
    // 声明moves以避免未定义错误（具体路书功能后续扩展）
    html << QString("  var moves = [];");
}


