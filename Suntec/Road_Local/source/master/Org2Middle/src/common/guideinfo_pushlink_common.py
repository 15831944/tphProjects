#encoding=utf-8
import math

# 功能类，此类用于换算两点的经纬度与方位、距离
class LatLonPoint(object):
    A_WGS84 = 6378137.0
    E2_WGS84 = 6.69437999013e-3
    def __init__(self, latitude, longitude):
        self.latitude = latitude * 256 * 3600;    
        self.longitude = longitude * 256 * 3600;
        
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
    
    def getDistanceByPoint(self, latLonPoint2):
        return self.getDistanceByLatLon(latLonPoint2.latitude, latLonPoint2.longitude)

class link_object(object):
    MAX_DEEP = 5
    DIR_LEFT_SIDE = 1
    DIR_RIGHT_SIDE = 2
    def __init__(self, errMsg, link_id, s_node, e_node, link_type, the_geom_text):
        self.link_id = link_id
        self.s_node = s_node
        self.e_node = e_node
        self.link_type = link_type
        self.the_geom_text = the_geom_text
        self.pointlist = self.geomText2Pointlist(errMsg, the_geom_text)
        
    def toString(self):
        strTemp = """link_id: %s""" % self.link_id
        strTemp += """, s_node: %s""" % self.s_node
        strTemp += """, e_node: %s""" % self.e_node
        strTemp += """, link_type: %s""" % self.link_type
        return strTemp
    
    def geomText2Pointlist(self, errMsg, the_geom_text):
        pointlist = []
        try:
            the_geom_text = the_geom_text.replace("""MULTILINESTRING((""", "")
            the_geom_text = the_geom_text.replace("""))""", "")
            the_geom_text = the_geom_text.replace("""LINESTRING(""", "")
            the_geom_text = the_geom_text.replace(""")""", "")
            geomTextSplit = the_geom_text.split(',')
            for oneGeomText in geomTextSplit:
                lat = float(oneGeomText.split(' ')[1])
                lon = float(oneGeomText.split(' ')[0])
                pointlist.append(LatLonPoint(lat, lon))
        except Exception, ex:
            errMsg[0] = ex
            return None
        return pointlist
    
    # 获取本link与linkObj2之间的相连node。
    # 若linkObj2与本link并无相连点，则返回空，并置errMsg。
    def getConnectionNodeid(self, errMsg, linkObj2):
        if self.s_node == linkObj2.s_node or self.s_node == linkObj2.e_node:
            return self.s_node
        elif self.e_node == linkObj2.s_node or self.e_node == linkObj2.e_node:
            return self.e_node
        else:
            errMsg[0] = """input in/out links are not connected.\n""" + \
                        """inlink: %s\noutlink: %s""" % \
                        (self.toString(), linkObj2.toString())
            return None
    
    # 获取与linkObj2的连接node以外的另一个node。
    # 若linkObj2与本link并无相连点，则返回空，并置errMsg。
    def getNotConnectionNodeid(self, errMsg, linkObj2):
        if self.s_node == linkObj2.s_node or self.s_node == linkObj2.e_node:
            return self.e_node
        elif self.e_node == linkObj2.s_node or self.e_node == linkObj2.e_node:
            return self.s_node
        else:
            errMsg[0] = """input in/out links are not connected.\n""" + \
                        """inlink: %s\noutlink: %s""" % \
                        (self.toString(), linkObj2.toString())
            return None
    
    # 求沿此link的交通流向与正东方向的夹角。
    # nodeid: 本link的某个端点
    # trafficDir: 沿当前link流向此点/沿当前link从此点流出，必须等于'to_this_node'或'from_this_node'。
    def getTrafficDirAngleToEast(self, errMsg, nodeid, trafficDir='to_this_node'):
        if trafficDir <> 'to_this_node' and trafficDir <> 'from_this_node':
            errMsg[0] = """invalid argument trafficDir: %s""" % trafficDir
            return None
        
        if nodeid == self.s_node:
            point1 = self.pointlist[0]
            point2 = None
            for i in range(1, len(self.pointlist)):
                if point1.getDistanceByPoint(self.pointlist[i]) > 5:
                    point2 = self.pointlist[i]
                    break
            # 没有任何一个形状点与s_node的距离大于5米，则使用e_node为point2
            if point2 == None:
                point2 = self.pointlist[-1]
            
            if trafficDir == 'to_this_node':
                return point2.getAngleByPoint(point1)
            else:  # trafficDir == 'from_this_node'
                return point1.getAngleByPoint(point2)
            
        elif nodeid == self.e_node:
            point1 = self.pointlist[-1]
            point2 = None
            for i in range(len(self.pointlist) - 2, -1, -1):
                if point1.getDistanceByPoint(self.pointlist[i]) > 5:
                    point2 = self.pointlist[i]
                    break
            # 没有任何一个形状点与e_node的距离大于5米，则使用s_node为point2
            if point2 == None:
                point2 = self.pointlist[0]
            
            if trafficDir == 'to_this_node':
                return point2.getAngleByPoint(point1)
            else:  # trafficDir == 'from_this_node'
                return point1.getAngleByPoint(point2)
        
        else:
            errMsg[0] = """nodeid is not the link's endpoint.\n""" + \
                        """linkObject: %s, nodeid: %s""" % (self.toString(), nodeid)
            return None

    # 求angleA对与angleB的位置和角度
    # 返回1：angleA在angleB的左/右
    # 返回2：angleA在angleB的左/右的角度，取值范围为0~180
    @staticmethod
    def getPositionAndAngleOfA2B(angleA, angleB):
        if 0 <= angleA - angleB and angleA - angleB < 180:
            return link_object.DIR_LEFT_SIDE, angleA - angleB
        
        if 180 <= angleA - angleB and angleA - angleB < 360:
            return link_object.DIR_RIGHT_SIDE, 360 - (angleA - angleB)
        
        if -360 < angleA - angleB and angleA - angleB < -180:
            return link_object.DIR_LEFT_SIDE, 360 + (angleA - angleB)
        
        if -180 <= angleA - angleB and angleA - angleB < 0:
            return link_object.DIR_RIGHT_SIDE, -(angleA - angleB)
        
    # 获取车辆沿此条link序列行进时最终的交通流变化方向。
    @staticmethod
    def getTrafficAngleByLinkList(errMsg, linkList):
        resultAngle = 0
        for i in range(0, len(linkList) - 1):
            nodeid = linkList[i].getConnectionNodeid(errMsg, linkList[i + 1])
            trafficAngle1 = linkList[i].getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
            trafficAngle2 = linkList[i+1].getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            position, angle = link_object.getPositionAndAngleOfA2B(trafficAngle2, trafficAngle1)
            if position == link_object.DIR_LEFT_SIDE:
                resultAngle += angle
            else:
                resultAngle -= angle
        
        if resultAngle >= 0:
            return link_object.DIR_LEFT_SIDE, resultAngle
        else:
            return link_object.DIR_RIGHT_SIDE, -resultAngle
        
    # 求以此link为流出link，node点的所有流入link列表。
    @staticmethod
    def getInlinkList(errMsg, _pg, nodeid, linkObj):
        sqlcmd = """
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where s_node=%s and one_way_code in (1,3) and link_id<>%s
        union
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where e_node=%s and one_way_code in (1,2) and link_id<>%s;
        """
        _pg.execute2(sqlcmd, [nodeid, linkObj.link_id, nodeid, linkObj.link_id])
        rows = _pg.fetchall2()
        linkList = []
        for row in rows:
            link_id = row[0]
            s_node = row[1]
            e_node = row[2]
            link_type = row[3]
            the_geom_text = row[4]
            linkList.append(link_object(errMsg, link_id, s_node, e_node, link_type, the_geom_text))
        return linkList
    
    # 求以此link为流入link，node点的所有流出link列表。
    @staticmethod
    def getOutlinkList(errMsg, _pg, nodeid, linkObj):
        sqlcmd = """
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where s_node=%s and one_way_code in (1,2) and link_id<>%s
        union
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where e_node=%s and one_way_code in (1,3) and link_id<>%s;
        """
        _pg.execute2(sqlcmd, [nodeid, linkObj.link_id, nodeid, linkObj.link_id])
        rows = _pg.fetchall2()
        linkList = []
        for row in rows:
            link_id = row[0]
            s_node = row[1]
            e_node = row[2]
            link_type = row[3]
            the_geom_text = row[4]
            linkList.append(link_object(errMsg, link_id, s_node, e_node, link_type, the_geom_text))
        return linkList
    
    # 获取连接到此node点的link列表。
    @staticmethod
    def getConnectedLinkList(errMsg, _pg, nodeid):
        sqlcmd = """
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where s_node=%s or e_node=%s
        """
        _pg.execute2(sqlcmd, [nodeid, nodeid])
        rows = _pg.fetchall2()
        linkList = []
        for row in rows:
            link_id = row[0]
            s_node = row[1]
            e_node = row[2]
            link_type = row[3]
            the_geom_text = row[4]
            linkList.append(link_object(errMsg, link_id, s_node, e_node, link_type, the_geom_text))
        return linkList
    
    # 根据交通流，算出此点的outlink数量小于2。
    # 原因1：在link_tbl中，车辆类型不是小汽车等类型时该link被标记为双向禁止，导致某个点误判为单进单出点。
    # 原因2：在link_tbl中，一些正在建设中的道路被标记为双向禁止，也会导致某个点呗误判为单进单出甚至算路失败。
    # 需要进一步分析道路形态进行判断。
    # 求出该点的所有相连link，假设从node点到这些link都能流通。
    # 若某条outlink的交通流与inlink的交通流不大于120度，则认为这是一条可能可以通行的道路。
    @staticmethod
    def getPotentialOutlinkList(errMsg, _pg, nodeid, linkObj):
        inlinkTrafficAngle = linkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
        if errMsg[0] <> '':
            return False
        
        connectedLinkList = link_object.getConnectedLinkList(errMsg, _pg, nodeid)
        potentialOutlinkList = []
        if errMsg[0] <> '':
            return False
          
        for oneLinkObj in connectedLinkList:
            if oneLinkObj.link_id == linkObj.link_id:
                continue
            
            oneTrafficAngle = oneLinkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            position, angle = link_object.getPositionAndAngleOfA2B(oneTrafficAngle, inlinkTrafficAngle)
            # 假设此link从node点流入该link并且拐角将小于120度
            if angle < 120:
                potentialOutlinkList.append(oneLinkObj)
        return potentialOutlinkList
    
    # 根据link序列生成node序列，node序列长度应为link序列长度+1
    @staticmethod
    def makeNodeListByLinkList(errMsg, linkList):
        if len(linkList) == 0:
            return []
        elif len(linkList) == 1:
            return [linkList[0].s_node, linkList[0].e_node]
        else:
            nodeList = []
            nodeList.append(linkList[0].getNotConnectionNodeid(errMsg, linkList[1]))
            for i in range(0, len(linkList) - 1):
                nodeList.append(linkList[i].getConnectionNodeid(errMsg, linkList[i+1]))
            nodeList.append(linkList[-1].getNotConnectionNodeid(errMsg, linkList[-2]))
            if errMsg[0] <> '':
                return None
            return nodeList
                
    # 找出所有从此node点流出的行车路线，遇到非inner link及停止。
    # 如果outlink为inner link，则会尝试向前推link，直到非inner link。
    @staticmethod
    def findOutRoute(errMsg, _pg, possibleLinkList, inlinkObj, nodeid=None):
        outlinkList = link_object.getOutlinkList(errMsg, _pg, nodeid, inlinkObj)
        if errMsg[0] <> '':
            errMsg[0] = """Failed when push link step2: """ + errMsg[0]
            return
        
        for oneOutlink in outlinkList:
            if oneOutlink.link_type == 0: # 遇到roundabout，此outlink排除。
                continue
            elif oneOutlink.link_type == 4: # inner link，仍继续向前探路直至非inner link。
                link_object.findOutRoute2(errMsg, _pg, possibleLinkList, [inlinkObj, oneOutlink])
            else: # 非inner link，推点结束，找到一条可能的行车路线。
                possibleLinkList.append([inlinkObj, oneOutlink])
                
    
    # findOutRoute的重载版本，传入的是link序列，便于递归。
    @staticmethod         
    def findOutRoute2(errMsg, _pg, possibleLinkList, linkList):
        if len(linkList) > link_object.MAX_DEEP:
            return
        if len(linkList) < 2:
            errMsg[0] = """findOutRoute2: length of linkList is less than 2."""
            return
        
        nodeid = linkList[-1].getNotConnectionNodeid(errMsg, linkList[-2])
        outlinkList = link_object.getOutlinkList(errMsg, _pg, nodeid, linkList[-1])
        nodeidList = link_object.makeNodeListByLinkList(errMsg, linkList)
        for oneOutlink in outlinkList:
            newEndNode = oneOutlink.getNotConnectionNodeid(errMsg, linkList[-1])
            # 加入此条outlink后会形成环，跳过。
            if newEndNode in nodeidList:
                continue
            
            tempList = list(linkList)
            tempList.append(oneOutlink)
            if oneOutlink.link_type <> 4:
                possibleLinkList.append(tempList)
            else:
                link_object.findOutRoute2(errMsg, _pg, possibleLinkList, tempList)
                
    # 找出所有流入此node点的可能的行车路线，遇到非inner link即停止。
    @staticmethod
    def findInRoute(errMsg, _pg, possibleLinkList, outlinkObj, nodeid=None):
        inlinkList = link_object.getInlinkList(errMsg, _pg, nodeid, outlinkObj)
        if errMsg[0] <> '':
            errMsg[0] = """Failed when push link step2: """ + errMsg[0]
            return
        
        for oneInlink in inlinkList:
            if oneInlink.link_type == 4: # inner link，仍继续向前探路直至非inner link。
                link_object.findInRoute2(errMsg, _pg, possibleLinkList, [oneInlink, outlinkObj])
            else: # 非inner link，推点结束，找到一条可能的行车路线。
                possibleLinkList.append([oneInlink, outlinkObj])
                
    
    # findInRoute的重载版本，传入的是link序列，便于递归。
    @staticmethod         
    def findInRoute2(errMsg, _pg, possibleLinkList, linkList):
        if len(linkList) > link_object.MAX_DEEP:
            return
        if len(linkList) < 2:
            errMsg[0] = """findOutRoute2: length of linkList is less than 2."""
            return
        
        nodeid = linkList[0].getNotConnectionNodeid(errMsg, linkList[1])
        inlinkList = link_object.getInlinkList(errMsg, _pg, nodeid, linkList[0])
        # 避开环路
        nodeidList = link_object.makeNodeListByLinkList(errMsg, linkList)
        for oneInlink in inlinkList:
            newStartNode = oneInlink.getNotConnectionNodeid(errMsg, linkList[0])
            # 加入此条inlink后会形成环，跳过
            if newStartNode in nodeidList:
                continue
            
            tempList = list(linkList)
            tempList.insert(0, oneInlink)
            if oneInlink.link_type <> 4:
                possibleLinkList.append(tempList)
            else:
                link_object.findInRoute2(errMsg, _pg, possibleLinkList, tempList)
    
    # 获取link序列字符串
    @staticmethod
    def getLinkListString(linkList):
        strTemp = '|'.join(str(x.link_id) for x in linkList)
        return strTemp

















