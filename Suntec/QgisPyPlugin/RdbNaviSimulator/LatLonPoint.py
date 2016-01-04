#encoding=utf-8
import math
import ctypes

# 功能类，此类用于换算两点的经纬度与方位、距离
class LatLonPoint(object):
    A_WGS84 = 6378137.0
    E2_WGS84 = 6.69437999013e-3
    def __init__(self, latitude, longitude):
        self.latitude = latitude * 256 * 3600;    
        self.longitude = longitude * 256 * 3600;
    
    def getLat(self):
        return self.latitude / 256 / 3600
    
    def getLon(self):
        return self.longitude / 256 / 3600
    
    def getPointByAngleDistance(self, angle, distance):
        return
     
    def getAngleByLatLon(self, latitude, longitude):
        delta_lat = latitude - self.latitude
        delta_lon = longitude - self.longitude
        ref_lat = (self.latitude + delta_lat / 2.0) / 256.0
        ref_lat_radian = ref_lat / 3600.0 * math.pi / 180.0
        sinlat = math.sin(ref_lat_radian)
        c = 1.0 - (LatLonPoint.E2_WGS84 * sinlat * sinlat)
        M2SecLon = 3600 / ((math.pi / 180.0) * LatLonPoint.A_WGS84 * math.cos(ref_lat_radian) / math.sqrt(c))
        M2SecLat = 1.0 / ((math.pi / 648000.0) * LatLonPoint.A_WGS84 * (1 - LatLonPoint.E2_WGS84)) * math.sqrt(c * c * c)
        real_delta_lat = delta_lat / M2SecLat / 256.0
        real_delta_lon = delta_lon / M2SecLon / 256.0
        real_dir = math.atan2(real_delta_lat, real_delta_lon) * 180.0 / math.pi
        return real_dir
    
    def getAngleByPoint(self, latLonPoint2):
        return self.getAngleByLatLon(latLonPoint2.latitude, latLonPoint2.longitude)
    
    def getDistanceByLatLon(self, latitude, longitude):
        radLat1 = latitude * math.pi / 180
        radLat2 = self.latitude * math.pi / 180
        a = radLat1 - radLat2
        b = longitude * math.pi / 180 - self.longitude * math.pi / 180
        
        s = 2 * math.asin(math.sqrt(math.pow(math.sin(a / 2), 2) + \
                                    math.cos(radLat1) * math.cos(radLat2) * math.pow(math.sin(b / 2), 2)))
        s = s * LatLonPoint.A_WGS84;
        s = round(s * 10000) / 10000;
        return s
    
    # 通过经纬度点获取tileid。
    # tile_z：划分tile的幅度，默认值14：将地球的墨卡托投影沿x轴和y轴切为2^14 * 2^14块。
    @staticmethod
    def getTileId(errMsg, latLonPoint, tile_z=14):
        if tile_z > 14:
            errMsg[0] = "tile_z exceed 14"  
            return None
    
        lon = latLonPoint.getLon()
        lat = latLonPoint.getLat()
        x = lon / 360.0 + 0.5
        y = math.log(math.tan(math.pi / 4.0 + math.radians(lat) / 2.0))
        y = 0.5 - y / 2.0 / math.pi
                                        
        tile_x = int(math.floor(x * (1<<tile_z)))
        tile_y = int(math.floor(y * (1<<tile_z)))
                                                
        tile_id = (tile_z<<28) | (tile_x<<14) | tile_y
        return ctypes.c_int32(tile_id).value  
    
    # 获取两个经纬度点为矩形顶点的所有tile id。
    @staticmethod
    def getTileIdList(errMsg, latLonPoint1, latLonPoint2, tile_z=14, offset=0):
        if tile_z > 14:
            errMsg[0] = "tile_z: %s exceed 14" % tile_z  
            return None
        
        if offset > 2**14:
            errMsg[0] = "unavailable offset: %s" % offset
            return None
        
        tileId1 = LatLonPoint.getTileId(errMsg, latLonPoint1, tile_z)
        tileId2 = LatLonPoint.getTileId(errMsg, latLonPoint2, tile_z)
        resultList = LatLonPoint.getTileIdListByTileId(errMsg, tileId1, tileId2, tile_z, offset)
        if errMsg[0] <> '':
            return None
        return resultList
    
    # 获取两个tile为矩形顶点的所有tile id。
    # tile_z：分割程度
    # offset：向外扩张幅度。
    @staticmethod
    def getTileIdListByTileId(errMsg, tileId1, tileId2, offset=0):
        tile_z = tileId1>>28;
        if tile_z <> tileId2>>28:
            errMsg[0] = "tile_z of two tileIDs are not equal"
            return None

        if offset > 2**14:
            errMsg[0] = "unavailable offset: %s" % offset
            return None
        
        tile_x1 = (tileId1>>14) & (2**14-1);
        tile_y1 = (tileId1) & (2**14-1);
        tile_x2 = (tileId2>>14) & (2**14-1);
        tile_y2 = (tileId2) & (2**14-1);
        
        tile_x_min = min(max(0, tile_x1-offset), max(0, tile_x2-offset))
        tile_x_max = max(min(tile_x1+offset, 2**14), min(tile_x2+offset, 2**14))
        tile_y_min = min(max(0, tile_y1-offset), max(0, tile_y2-offset))
        tile_y_max = max(min(tile_y1+offset, 2**14), min(tile_y2+offset, 2**14))
        
        resultList = []
        for tile_x in range(tile_x_min, tile_x_max):
            for tile_y in range(tile_y_min, tile_y_max):
                tile_id = ((tile_z<<28) | (tile_x<<14) | tile_y)
                resultList.append(ctypes.c_int32(tile_id).value)
        
        return resultList

    # 获取两个node所在的tile为矩形顶点的所有tile id。
    # tile_z：分割程度
    # offset：向外扩张幅度。
    @staticmethod
    def getTileIdListByNodeId(errMsg, nodeId1, nodeId2, offset=0):
        tileId1 = nodeId1>>32
        tileId2 = nodeId2>>32
        return LatLonPoint.getTileIdListByTileId(errMsg, tileId1, tileId2, offset)

























