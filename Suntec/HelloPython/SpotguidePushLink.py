# -*- coding: cp936 -*-
import math
import psycopg2   

DIR_LEFT_SIDE = 1
DIR_RIGHT_SIDE = 2
A_WGS84 = 6378137.0
E2_WGS84 = 6.69437999013e-3

# �����࣬�������ڻ�������ľ�γ���뷽λ������
class LatLonPoint(object):
    def __init__(self, latitude, longitude):
        self.latitude = latitude*256*3600;    
        self.longitude = longitude*256*3600;
        
    def getPointByAngleDistance(self, angle, distance):
        
        return
     
    def getAngleByLatLon(self, latitude, longitude):
        delta_lat = latitude - self.latitude
        delta_lon = longitude - self.longitude
        ref_lat = (self.latitude + delta_lat / 2.0) / 256.0
        ref_lat_radian = ref_lat / 3600.0 * math.pi / 180.0
        sinlat = math.sin(ref_lat_radian)
        c = 1.0 - (E2_WGS84 * sinlat * sinlat)
        M2SecLon = 3600 / ((math.pi/180.0) * A_WGS84 * math.cos(ref_lat_radian)/math.sqrt(c))
        M2SecLat = 1.0 / ((math.pi/648000.0) * A_WGS84 * (1-E2_WGS84)) * math.sqrt(c*c*c)
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
        
        s = 2 * math.asin(math.sqrt(math.pow(math.sin(a/2),2) +\
                                    math.cos(radLat1)*math.cos(radLat2)*math.pow(math.sin(b/2),2)))
        s = s * A_WGS84;
        s = round(s * 10000) / 10000;
        return s
    
    def getDistanceByPoint(self, latLonPoint2):
        return self.getDistanceByLatLon(latLonPoint2.latitude, latLonPoint2.longitude)

class link_object_base(object):
    def __init__(self, errMsg, link_id, s_node, e_node, the_geom_text):
        self.link_id = link_id
        self.s_node = s_node
        self.e_node = e_node
        self.the_geom_text = the_geom_text
        self.pointlist = self.geomText2Pointlist(errMsg, the_geom_text)
        
    def toString(self):
        strTemp = """link_id: %.0f""" % self.link_id
        strTemp += """, s_node: %.0f""" % self.s_node
        strTemp += """, e_node: %.0f""" % self.e_node
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
    
    # ��ȡ��link��linkObj2֮�������node��
    # ��linkObj2�뱾link���������㣬�򷵻ؿգ�����errMsg��
    def getConnectedNodeidWith(self, errMsg, linkObj2):
        if self.s_node == linkObj2.s_node or self.s_node == linkObj2.e_node:
            return self.s_node
        elif self.e_node == linkObj2.s_node or self.e_node == linkObj2.e_node:
            return self.e_node
        else:
            errMsg[0] = """input in/out links are not connected.\n""" + \
                        """inlink: %s\noutlink: %s""" % \
                        (self.toString(), linkObj2.toString())
            return None
    
    # ��ȡ��linkObj2������node�������һ��node��
    # ��linkObj2�뱾link���������㣬�򷵻ؿգ�����errMsg��
    def getNonConnectedNodeid(self, errMsg, linkObj2):
        if self.s_node == linkObj2.s_node or self.s_node == linkObj2.e_node:
            return self.e_node
        elif self.e_node == linkObj2.s_node or self.e_node == linkObj2.e_node:
            return self.s_node
        else:
            errMsg[0] = """input in/out links are not connected.\n""" + \
                        """inlink: %s\noutlink: %s""" % \
                        (self.toString(), linkObj2.toString())
            return None
    
    # ���ش�link�Ľ�ͨ��������������ļнǡ�
    # nodeid: ��link��ĳ���˵�
    # trafficDir: �ص�ǰlink����˵�/�ص�ǰlink�Ӵ˵��������������'to_this_node'��'from_this_node'��
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
            # û���κ�һ����״����s_node�ľ������5�ף���ʹ��e_nodeΪpoint2
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
            # û���κ�һ����״����e_node�ľ������5�ף���ʹ��s_nodeΪpoint2
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


    # ��angleA����angleB��λ�úͽǶ�
    # ����1��angleA��angleB����/��
    # ����2��angleA��angleB����/�ҵĽǶȣ�ȡֵ��ΧΪ0~180
    @staticmethod
    def getPositionAndAngleOfA2B(angleA, angleB):
        if 0 <= angleA - angleB and angleA - angleB < 180:
            return DIR_LEFT_SIDE, angleA - angleB
        
        if 180 <= angleA - angleB and angleA - angleB < 360:
            return DIR_RIGHT_SIDE, 360 - (angleA - angleB)
        
        if -360 < angleA - angleB and angleA - angleB < -180:
            return DIR_LEFT_SIDE, 360 + (angleA - angleB)
        
        if -180 <= angleA - angleB and angleA - angleB < 0:
            return DIR_RIGHT_SIDE, -(angleA - angleB)

class link_object_lane(link_object_base):
    def __init__(self, errMsg, link_id, s_node, e_node, the_geom_text, link_type):
        link_object_base.__init__(self, errMsg, link_id, s_node, e_node, the_geom_text)
        self.link_type = link_type
        
    def toString(self):
        strTemp = link_object_base.toString(self)
        strTemp += """, link_type: %s""" % self.link_type
        return strTemp

class spotguide_push_link(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
        self.focusLineList = []
    
    def do(self):
        self.findOneInOneOutLinks()
        return
        
    # �����ߵ�Ϊ�Ƿ����ʱ��apl���Ქ�������յ����ݣ��������ݳ�Ϊ�������ݡ�
    # �ҳ���Щ�㣬�����Ƶ����ɾ����¼��
    def findOneInOneOutLinks(self):
        sqlcmd = """
drop table if exists lane_tbl_link_seqnr;
select t.id, unnest(t.link_list)::bigint as link_id, 
       generate_series(1, array_upper(link_list, 1)) as seqnr
into lane_tbl_link_seqnr
from
(
    select distinct id, 
       array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text] as link_list
    from lane_tbl
)as t;
create index lane_tbl_link_seqnr_link_id_idx
on lane_tbl_link_seqnr
using btree
(link_id);

drop table if exists lane_tbl_link_list;
select id, array_agg(seqnr) as seqnr_list, 
    array_agg(link_id) as link_id_list,
    array_agg(s_node) as s_node_list,
    array_agg(e_node) as e_node_list,
    array_agg(link_type) as link_type_list,
    array_agg(st_astext(the_geom)) as the_geom_text_list
into lane_tbl_link_list
from
(
    select a.id, a.seqnr, b.link_id, b.s_node, b.e_node, b.link_type, b.the_geom
    from
    lane_tbl_link_seqnr as a
    left join link_tbl as b
    on a.link_id=b.link_id
    order by a.id, a.seqnr
group by id;
) as t

create index lane_tbl_link_list_id_idx
on lane_tbl_link_list
using btree
(id);

select a.id, a.seqnr_list, a.link_id_list, a.s_node_list, 
    a.e_node_list, a.link_type_list, a.the_geom_text_list
from 
lane_tbl_link_list as a
left join lane_tbl as b
on a.id=b.id;
"""
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            id = row[0]
            link_list = row[1]
            seqnr_list = row[2]
            s_node_list = row[3]
            e_node_list = row[4]
            link_type_list = row[5]
            the_geom_text_list = row[6]
            name_list = row[7]

            errMsg = ['']
            linkObjList = []
            for i in range(0, len(link_list)):
                linkObjList.append(link_object_lane(errMsg, link_list[i], s_node_list[i], e_node_list[i], 
                                                    link_type_list[i], the_geom_text_list[i]))
            bHasFork = self.isLinkListHasFork(errMsg, linkObjList)
            if errMsg[0] <> '':
                print """error: id: %s, errMsg: %s""" % (id, errMsg[0])
                continue
            
            if bHasFork == False:
                whichLane = '010'
                arrowInfo = 64
                resultList = []
                self.pushLink(errMsg, resultList, linkObjList, whichLane, arrowInfo)
        return
    
    # ��link
    def pushLink(self, errMsg, resultList, linkObjList, whichLane, arrowInfo):
        nodeid = linkObjList[-1].getNonConnectedNodeid(errMsg, linkObjList[-2])
        if errMsg[0] <> '':
            return
        
        newInlink = linkObjList[-1]
        outlinkList = self.getOutlinkList(errMsg, nodeid, newInlink)
        if errMsg[0] <> '':
            return
        
        if len(outlinkList) == 0:
            errMsg[0] = """cannot find a valid outlink, push link failed.\n"""
            errMsg[0] += """inlink: %s and node: %s""" % (newInlink, nodeid)
            return
        elif len(outlinkList) == 1: # find only one outlink, so keep on pushing the link.
            linkObjList.append(outlinkList[0])
            self.pushLink(errMsg, resultList, linkObjList, whichLane, arrowInfo)
            return
        else: # now meet a fork, new inlink is found.
            inlinkTrafficAngle = newInlink.getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
            for oneOutlink in outlinkList:
                if oneOutlink.link_type == 3: # this outlink is a inner link
                # is there exists an old lane's guide information?
                bHasOldInfo = self.isGuideinfoLaneExists(errMsg, [newInlink, oneOutlink], whichLane)
                if errMsg[0] <> '':
                    return
                if bHasOldInfo == True:
                    errMsg[0] = """a lane guideinfo for link list: %s-->%s, on inlink's lane: %s already exists.""" %\
                    (newInlink.link_id, oneOutlink.link_id, whichLane)
                    return
                outlinkTrafficAngle = oneOutlink.getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
                position, angle = link_object_base.getPositionAndAngleOfA2B(outlinkTrafficAngle, inlinkTrafficAngle)
                
        return
    
    # ���ݽ�ͨ���仯�ĽǶȣ��жϼ�ͷ��Ϣ�Ƿ���ϡ�
    def isArrowInfoFitsAngle(self, errMsg, position, angle, arrowInfo):
        if position<>DIR_LEFT_SIDE and position<>DIR_RIGHT_SIDE:
            errMsg[0] = """invalid argument 'position': %s""" % position
            return
        bestArrowInfo = -1
        if position==DIR_RIGHT_SIDE:
            if angle <= 30:
                bestArrowInfo = 1 # 1-straight
            elif 30<angle and angle<=60:
                bestArrowInfo = 2 # 2-slight right
            elif 60<angle and angle<=120:
                bestArrowInfo = 4 # 4-right
            elif 120<angle and angle<=150:
                bestArrowInfo = 8 # 8-hard right
            else:
                
        else: # position==DIR_LEFT_SIDE:
            if angle <= 30:
                bestArrowInfo = 1 # 1-straight
            elif 30<angle and angle<=60:
                bestArrowInfo = 128 # 128-slight left
            elif 60<angle and angle<=120:
                bestArrowInfo = 64 # 64-left
            elif 120<angle and angle<=150:
                bestArrowInfo = 32 # 8-hard left
            
        
            
    
    # ��ѯlane_tbl����ѯ����link������lane_tbl�����Ƿ��Ѵ��ڳ����յ���Ϣ��
    def isGuideinfoLaneExists(self, errMsg, linkObjList, whichLane):
        inlinkObj = linkObjList[0]
        outlinkObj = linkObjList[-1]
        nodeid = inlinkObj.getConnectedNodeidWith(errMsg, outlinkObj)
        if errMsg[0]<>'':
            return
        sqlcmd = """
select count(*) 
from lane_tbl where 
inlinkid=%s and 
outlinkid=%s and 
nodeid=%s and 
passlink_count=0 and  
laneinfo&whichLane<>0;
"""
# todo
        return
    
    # �����link�������Ƿ��������
    def isLinkListHasFork(self, errMsg, linkObjList):
        for i in range(0, len(linkObjList)-1):
            inlinkObj = linkObjList[i]
            outlinkObj = linkObjList[i+1]            
            nodeid = inlinkObj.getConnectedNodeidWith(errMsg, outlinkObj)
            if errMsg[0] <> '':
                return False
            
            outlinkList = self.getOutlinkList(errMsg, nodeid, inlinkObj)
            # �˵��������link�����ڵ���2���ж�Ϊ���磬�����������������ٹ�ע��
            if len(outlinkList) >= 2:
                return True
            
            potentialOutlinkList = self.getPotentialOutlinkList(errMsg, nodeid, inlinkObj)
            # �˵���ܵ�����link�����ڵ���2������Ϊ���磬�����������������ٹ�ע��
            if len(potentialOutlinkList) >= 2:
                return True
            
            # ���ӵ��˵��link������2�������������������ٹ�ע��
            connectedOutlinkList = self.getConnectedLinkList(errMsg, nodeid)
            if len(connectedOutlinkList) > 2:
                return True
            return False
            
            
    # ���ݽ�ͨ��������˵��outlink����С��2��
    # ԭ��1����link_tbl�У��������Ͳ���С����������ʱ��link�����Ϊ˫���ֹ������ĳ��������Ϊ���������㡣
    # ԭ��2����link_tbl�У�һЩ���ڽ����еĵ�·�����Ϊ˫���ֹ��Ҳ�ᵼ��ĳ����������Ϊ��������������·ʧ�ܡ�
    # ��Ҫ��һ��������·��̬�����жϡ�
    # ����õ����������link�������node�㵽��Щlink������ͨ��
    # ��ĳ��outlink�Ľ�ͨ����inlink�Ľ�ͨ��������120�ȣ�����Ϊ����һ�����ܿ���ͨ�еĵ�·��
    def getPotentialOutlinkList(self, errMsg, nodeid, linkObj):
        inlinkTrafficAngle = linkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
        if errMsg[0] <> '':
            return False
        
        connectedLinkList = self.getConnectedLinkList(errMsg, nodeid)
        potentialOutlinkList = []
        if errMsg[0] <> '':
            return False
          
        for oneLinkObj in connectedLinkList:
            if oneLinkObj.id == linkObj.id:
                continue
            
            oneTrafficAngle = oneLinkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            position, angle = self.getPositionAndAngle(oneTrafficAngle, inlinkTrafficAngle)
            # ����node�������link���ҹսǽ�С��120��
            if angle<120:
                potentialOutlinkList.append(oneLinkObj)
        return potentialOutlinkList
    
    # �����inlink_idΪ����link��nodeid���outlink�б�
    def getOutlinkList(self, errMsg, nodeid, linkObj):
        sqlcmd = """
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where s_node=%s and one_way_code in (1,2) and link_id<>%s
        union
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where e_node=%s and one_way_code in (1,3) and link_id<>%s;
        """
        self.pg.execute(sqlcmd, [nodeid, linkObj.id, nodeid, linkObj.id])
        rows = self.pg.fetchall()
        linkList = []
        for row in rows:
            link_id = row[0]
            s_node = row[1]
            e_node = row[2]
            link_type = row[3]
            the_geom_text = row[4]
            linkList.append(link_object_lane(errMsg, link_id, s_node, e_node, link_type, the_geom_text))
        return linkList
    
    # ��ȡ���ӵ��˵��link�б�
    def getConnectedLinkList(self, errMsg, nodeid):
        sqlcmd = """
        select link_id, s_node, e_node, link_type, st_astext(the_geom) as the_geom_text_list
        from link_tbl 
        where s_node=%s or e_node=%s
        """
        self.pg.execute(sqlcmd, [nodeid, nodeid])
        rows = self.pg.fetchall()
        linkList = []
        for row in rows:
            link_id = row[0]
            s_node = row[1]
            e_node = row[2]
            link_type = row[3]
            the_geom_text = row[4]
            linkList.append(link_object_lane(errMsg, link_id, s_node, e_node, link_type, the_geom_text))
        return linkList
    
if __name__ == '__main__':
    dbIP = '''192.168.10.16'''
    dbNameList = ['AUS_SENSIS_201503_0082_0007_103']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = spotguide_push_link(dbIP, dbName, userName, password)
            datMaker.do()
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

    
    
    
    
    
    
    
    
    
    
    
    
    
    
