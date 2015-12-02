# -*- coding: cp936 -*-
import time
import math
import psycopg2   

MAX_DEEP = 5
DIR_LEFT_SIDE = 1
DIR_RIGHT_SIDE = 2
A_WGS84 = 6378137.0
E2_WGS84 = 6.69437999013e-3

# 功能类，此类用于换算两点的经纬度与方位、距离
class LatLonPoint(object):
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
        c = 1.0 - (E2_WGS84 * sinlat * sinlat)
        M2SecLon = 3600 / ((math.pi / 180.0) * A_WGS84 * math.cos(ref_lat_radian) / math.sqrt(c))
        M2SecLat = 1.0 / ((math.pi / 648000.0) * A_WGS84 * (1 - E2_WGS84)) * math.sqrt(c * c * c)
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
    
    # 获取本link与linkObj2之间的相连node。
    # 若linkObj2与本link并无相连点，则返回空，并置errMsg。
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
    
    # 获取与linkObj2的连接node以外的另一个node。
    # 若linkObj2与本link并无相连点，则返回空，并置errMsg。
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
            return DIR_LEFT_SIDE, angleA - angleB
        
        if 180 <= angleA - angleB and angleA - angleB < 360:
            return DIR_RIGHT_SIDE, 360 - (angleA - angleB)
        
        if -360 < angleA - angleB and angleA - angleB < -180:
            return DIR_LEFT_SIDE, 360 + (angleA - angleB)
        
        if -180 <= angleA - angleB and angleA - angleB < 0:
            return DIR_RIGHT_SIDE, -(angleA - angleB)
        
    # 获取车辆沿此条link序列行进时最终的交通流变化方向。
    @staticmethod
    def getTrafficAngleByLinkList(errMsg, linkList):
        resultAngle = 0
        for i in range(0, len(linkList) - 1):
            nodeid = linkList[i].getConnectedNodeidWith(errMsg, linkList[i + 1])
            trafficAngle1 = linkList[i].getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
            trafficAngle2 = linkList[i + 1].getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            position, angle = link_object_base.getPositionAndAngleOfA2B(trafficAngle2, trafficAngle1)
            if position == DIR_LEFT_SIDE:
                resultAngle += angle
            else:
                resultAngle -= angle
        
        if resultAngle >= 0:
            return DIR_LEFT_SIDE, resultAngle
        else:
            return DIR_RIGHT_SIDE, -resultAngle

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
        print time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
        self.findOneInOneOutLinks()
        print time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
        return
        
    # 当决策点为非分歧点时，apl不会播报该条诱导数据，该条数据成为冗余数据。
    # 找出这些点，进行推link或者删除记录。
    def findOneInOneOutLinks(self):
        sqlcmd = """
--****************************************************************************************
create or replace function is_lane_info_grouped(laneinfo1 varchar, laneinfo2 varchar)
returns boolean
language plpgsql
as
$$
begin
    if length(laneinfo1) <> length(laneinfo2) then
        return false;
    end if;

    for i in 1..length(laneinfo1) loop
        if substr(laneinfo1, i, 1)='1' and substr(laneinfo2, i, 1)='1' then
            return false;
        end if;
    end loop;
    return true;
end;
$$;
--****************************************************************************************
drop table if exists lane_tbl_test;
select * into lane_tbl_test from lane_tbl;

create index lane_tbl_test_id_idx
on lane_tbl_test
using btree
(id);

create index lane_tbl_test_inlinkid_idx
on lane_tbl_test
using btree
(inlinkid);

create index lane_tbl_test_nodeid_idx
on lane_tbl_test
using btree
(nodeid);

create index lane_tbl_test_outlinkid_idx
on lane_tbl_test
using btree
(outlinkid);
--****************************************************************************************
drop table if exists lane_tbl_link_seqnr;
select t.id, unnest(t.link_list)::bigint as link_id, 
       generate_series(1, array_upper(link_list, 1)) as seqnr
into lane_tbl_link_seqnr
from
(
    select distinct id, 
       array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text] as link_list
    from lane_tbl_test
)as t;
create index lane_tbl_link_seqnr_link_id_idx
on lane_tbl_link_seqnr
using btree
(link_id);
--****************************************************************************************
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
) as t
group by id;

create index lane_tbl_link_list_id_idx
on lane_tbl_link_list
using btree
(id);
--****************************************************************************************
select a.id, a.seqnr_list, a.link_id_list, a.s_node_list, 
    a.e_node_list, a.link_type_list, a.the_geom_text_list,
    b.lanenum, b.laneinfo, b.arrowinfo, b.lanenuml, b.lanenumr,
    b.buslaneinfo, b.exclusive
from 
lane_tbl_link_list as a
left join lane_tbl_test as b
on a.id=b.id;
--****************************************************************************************
"""
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        oFStream = open(r"C:\My\20151116_aus_lane_fix_2_branches\1.txt", "w")
        for row in rows:
            id = row[0]
            seqnr_list = row[1]
            link_list = row[2]
            s_node_list = row[3]
            e_node_list = row[4]
            link_type_list = row[5]
            the_geom_text_list = row[6]
            lanenum = row[7]
            laneinfo = row[8]
            arrowinfo = row[9]
            lanenuml = row[10]
            lanenumr = row[11]
            buslaneinfo = row[12]
            exclusive = row[13]

            errMsg = ['']
            linkObjList = []
            for i in range(0, len(link_list)):
                linkObjList.append(link_object_lane(errMsg, link_list[i], s_node_list[i], e_node_list[i],
                                                    the_geom_text_list[i], link_type_list[i]))
            bHasFork = self.isLinkListHasFork(errMsg, linkObjList)
            if errMsg[0] <> '':
                print """error: id: %s, errMsg: %s""" % (id, errMsg[0])
                continue
            if bHasFork == True:
                pass
            # 整条link序列没有任何分歧路口。
            else:
                # 将整条link序列向前推进直至第一个分歧路口
                self.pushLinkUntilFork(errMsg, linkObjList)
                if errMsg[0] <> '':
                    print errMsg[0]
                    continue
                
                # 推到分歧路口后，使用分歧路口的前一条link为新的inlink
                newInlink = linkObjList[-1]
                nodeid = newInlink.getNonConnectedNodeid(errMsg, linkObjList[-2])
                if errMsg[0] <> '':
                    print errMsg[0]
                    continue
                
                # 基于新的inlink，尝试在分歧路口生成所有可能的行车路线
                possibleLinkLists = []
                self.findPossibleLinkList(errMsg, possibleLinkLists, newInlink, nodeid)
                if errMsg[0] <> '':
                    print errMsg[0]
                
                # 过滤可能的行车路线，只有符合箭头信息且原表中不存在相同诱导记录的信息才收录。
                for onePossibleLinkList in possibleLinkLists:
                    strTemp = """(id: %s),(%s),(%s),(""" % (id, laneinfo, arrowinfo)
                    for oneLink in onePossibleLinkList:
                        strTemp += str(oneLink.link_id) + '|'
                    strTemp = strTemp.rstrip('|')
                    strTemp += """),"""
                    bLinkListFitsArrowInfo = self.isLinklistFitsArrowInfo(errMsg, onePossibleLinkList, arrowinfo)
                    if bLinkListFitsArrowInfo == False or errMsg[0] <> '':
                        strTemp += """(failed: fits arrow info is false)"""
                        oFStream.write(strTemp+'\n')
                        continue
                    
                    bExists = self.isGuideinfoLaneExists(errMsg, onePossibleLinkList, laneinfo)
                    if bExists == True or errMsg[0] <> '':
                        strTemp += """(failed: already exists.)"""
                        oFStream.write(strTemp+'\n')
                        continue

                    # 添加新的车线诱导信息。
                    self.addLaneInfo(errMsg, onePossibleLinkList, lanenum, laneinfo,
                                     arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
                    strTemp += """(succeed)"""
                    oFStream.write(strTemp+'\n')
                sqlcmd = """delete from lane_tbl_test where id=%s""" % id
                self.pg.execute(sqlcmd)
        self.conn.commit()
        oFStream.close()
        return
    
    # 新增一条车线诱导信息。
    def addLaneInfo(self, errMsg, linkList, lanenum, laneinfo, 
                    arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive):
        inlinkObj = linkList[0]
        outlinkObj = linkList[-1]
        passlid = "|".join(str(x) for x in linkList[1:-1])
        nodeid = inlinkObj.getConnectedNodeidWith(errMsg, linkList[1])
        sqlcmd = """
insert into 
lane_tbl_test(id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
        lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
values
(max(id)+1, %s, %s, %s, %s, %s, 
 %s, %s, %s, %s, %s, %s, %s)
""" % (nodeid, inlinkObj.link_id, outlinkObj.link_id, passlid, len(linkList)-2, 
       lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
        self.pg.execute(sqlcmd)
        self.conn.commit()
        return
    
    # 基于inlinkObj和nodeid的可能的行车序列。
    # 如果outlink为inner link，则会尝试向后推link，解决uturn等情况。
    def findPossibleLinkList(self, errMsg, possibleLinkList, inlinkObj, nodeid=None):
        outlinkList = self.getOutlinkList(errMsg, nodeid, inlinkObj)
        if errMsg[0] <> '':
            errMsg[0] = """Failed when push link step2: """ + errMsg[0]
            return
        
        for oneOutlink in outlinkList:
            if oneOutlink.link_type == 0: # 遇到roundabout，此outlink排除。
                continue
            elif oneOutlink.link_type == 4: # inner link，仍继续向前探路直至非inner link。
                self.findPossibleLinkList2(errMsg, possibleLinkList, [inlinkObj, oneOutlink])
            else: # 非inner link，推点结束，找到一条可能的行车路线。
                possibleLinkList.append([inlinkObj, oneOutlink])
                
    
    # findPossibleLinkList的重载版本，传入的是link序列，便于递归。                  
    def findPossibleLinkList2(self, errMsg, possibleLinkList, linkList):
        if len(linkList) > MAX_DEEP:
            return
        if len(linkList) < 2:
            errMsg[0] = """findPossibleLinkList2: length of linkList is less than 2."""
            return
        
        nodeid = linkList[-1].getNonConnectedNodeid(errMsg, linkList[-2])
        outlinkList = self.getOutlinkList(errMsg, nodeid, linkList[-1])
        nodeidList = self.linkListToNodeList(errMsg, linkList)
        for oneOutlink in outlinkList:
            newEndNode = oneOutlink.getNonConnectedNodeid(errMsg, linkList[-1])
            # 加入此条outlink后会形成环，跳过。
            if newEndNode in nodeidList:
                continue
            
            tempList = list(linkList)
            tempList.append(oneOutlink)
            if oneOutlink.link_type <> 4:
                possibleLinkList.append(tempList)
            else:
                self.findPossibleLinkList2(errMsg, possibleLinkList, tempList)

    # 根据link序列生成node序列，node序列长度应为link序列长度+1
    def linkListToNodeList(self, errMsg, linkList):
        if len(linkList) == 0:
            return []
        elif len(linkList) == 1:
            return [linkList[0].s_node, linkList[0].e_node]
        else:
            nodeList = []
            nodeList.append(linkList[0].getNonConnectedNodeid(errMsg, linkList[1]))
            for i in range(0, len(linkList) - 1):
                nodeList.append(linkList[i].getConnectedNodeidWith(errMsg, linkList[i + 1]))
            nodeList.append(linkList[-1].getNonConnectedNodeid(errMsg, linkList[-2]))
            if errMsg[0] <> '':
                return None
            return nodeList

    # 沿着当前交通流，向前推link，直至遇到第一个分歧路口，确定inlink。
    # linkObjList: 此list会被不断更新，添加新的link直至分叉路口。
    # 递归结束后，linkObjList[-1]则为新的inlink。
    def pushLinkUntilFork(self, errMsg, linkObjList):
        nodeid = linkObjList[-1].getNonConnectedNodeid(errMsg, linkObjList[-2])
        if errMsg[0] <> '':
            return

        outlinkList = self.getOutlinkList(errMsg, nodeid, linkObjList[-1])
        connectedLinkList = self.getConnectedLinkList(errMsg, nodeid)
        if errMsg[0] <> '':
            errMsg[0] = """Failed when push link step1: """ + errMsg[0]
            return
        
        if len(outlinkList) == 0:  # 断头路，报错。
            errMsg[0] = """Failed when push link step1: cannot find a valid outlink.\n"""
            errMsg[0] += """inlink: %s and node: %s""" % (linkObjList[-1].link_id, nodeid)
            return
        elif len(outlinkList) == 1 and len(connectedLinkList) == 2:  # outlink数仍是1，继续向前推。
            linkObjList.append(outlinkList[0])
            self.pushLinkUntilFork(errMsg, linkObjList)
            return
        else:  # 找到分岔路。
            return
        return
    
    # 根据交通流变化的角度，判断箭头信息是否符合。
    def isLinklistFitsArrowInfo(self, errMsg, linkList, arrowInfo):
        bestArrowInfo = -1
        position, angle = link_object_base.getTrafficAngleByLinkList(errMsg, linkList)
        angle = angle % 360
        if position == DIR_RIGHT_SIDE:
            if angle <= 30:
                bestArrowInfo = 1  # 1-straight
            elif 30 < angle and angle <= 60:
                bestArrowInfo = 2  # 2-slight right
            elif 60 < angle and angle <= 120:
                bestArrowInfo = 4  # 4-right
            elif 120 < angle and angle <= 150:
                bestArrowInfo = 8  # 8-hard right
            else:
                bestArrowInfo = 8  # 8-hard right
        else:  # position==DIR_LEFT_SIDE:
            if angle <= 30:
                bestArrowInfo = 1  # 1-straight
            elif 30 < angle and angle <= 60:
                bestArrowInfo = 128  # 128-slight left
            elif 60 < angle and angle <= 120:
                bestArrowInfo = 64  # 64-left
            elif 120 < angle and angle <= 150:
                bestArrowInfo = 32  # 8-hard left
            else:
                bestArrowInfo = 32  # 8-hard left
        
        if bestArrowInfo & arrowInfo <> 0:
            return True
        else:
            return False
            
    # 查询lane_tbl表，查询此条link序列在lane_tbl表中是否已存在车线诱导信息。
    # 必须要inlink的此条车线信息已存在，才符合要求。
    # 若只存在inlink的其他车线信息，不影响最终判断。
    def isGuideinfoLaneExists(self, errMsg, linkObjList, whichLane):
        inlinkObj = linkObjList[0]
        outlinkObj = linkObjList[-1]
        nodeid = inlinkObj.getConnectedNodeidWith(errMsg, linkObjList[1])
        if errMsg[0] <> '':
            return
        sqlcmd = """
select count(*) 
from lane_tbl_test where 
inlinkid=%s and 
outlinkid=%s and 
nodeid=%s and 
passlink_cnt=%s and  
is_lane_info_grouped(laneinfo, '%s')=false;
""" % (inlinkObj.link_id, outlinkObj.link_id, nodeid, len(linkObjList) - 2, whichLane)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        return len(rows) > 0
    
    # 求此条link序列中是否包含分歧
    def isLinkListHasFork(self, errMsg, linkObjList):
        for i in range(0, len(linkObjList) - 1):
            inlinkObj = linkObjList[i]
            outlinkObj = linkObjList[i + 1]            
            nodeid = inlinkObj.getConnectedNodeidWith(errMsg, outlinkObj)
            if errMsg[0] <> '':
                return False
            
            outlinkList = self.getOutlinkList(errMsg, nodeid, inlinkObj)
            # 此点可流出的link数大于等于2，判定为分歧，此条数据正常，不再关注。
            if len(outlinkList) >= 2:
                return True
            
            potentialOutlinkList = self.getPotentialOutlinkList(errMsg, nodeid, inlinkObj)
            # 此点可能的流出link数大于等于2，可能为分歧，此条数据正常，不再关注。
            if len(potentialOutlinkList) >= 2:
                return True
            
            # 连接到此点的link数大于2，此条数据正常，不再关注。
            connectedOutlinkList = self.getConnectedLinkList(errMsg, nodeid)
            if len(connectedOutlinkList) > 2:
                return True
        return False
            
            
    # 根据交通流，算出此点的outlink数量小于2。
    # 原因1：在link_tbl中，车辆类型不是小汽车等类型时该link被标记为双向禁止，导致某个点误判为单进单出点。
    # 原因2：在link_tbl中，一些正在建设中的道路被标记为双向禁止，也会导致某个点呗误判为单进单出甚至算路失败。
    # 需要进一步分析道路形态进行判断。
    # 求出该点的所有相连link，假设从node点到这些link都能流通。
    # 若某条outlink的交通流与inlink的交通流不大于120度，则认为这是一条可能可以通行的道路。
    def getPotentialOutlinkList(self, errMsg, nodeid, linkObj):
        inlinkTrafficAngle = linkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
        if errMsg[0] <> '':
            return False
        
        connectedLinkList = self.getConnectedLinkList(errMsg, nodeid)
        potentialOutlinkList = []
        if errMsg[0] <> '':
            return False
          
        for oneLinkObj in connectedLinkList:
            if oneLinkObj.link_id == linkObj.link_id:
                continue
            
            oneTrafficAngle = oneLinkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            position, angle = link_object_base.getPositionAndAngleOfA2B(oneTrafficAngle, inlinkTrafficAngle)
            # 假设node点流入该link并且拐角将小于120度
            if angle < 120:
                potentialOutlinkList.append(oneLinkObj)
        return potentialOutlinkList
    
    # 求此以inlink_id为流入link，nodeid点的outlink列表。
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
        self.pg.execute(sqlcmd, [nodeid, linkObj.link_id, nodeid, linkObj.link_id])
        rows = self.pg.fetchall()
        linkList = []
        for row in rows:
            link_id = row[0]
            s_node = row[1]
            e_node = row[2]
            link_type = row[3]
            the_geom_text = row[4]
            linkList.append(link_object_lane(errMsg, link_id, s_node, e_node, the_geom_text, link_type))
        return linkList
    
    # 获取连接到此点的link列表。
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
            linkList.append(link_object_lane(errMsg, link_id, s_node, e_node, the_geom_text, link_type))
        return linkList
    
if __name__ == '__main__':
    dbIP = '''192.168.10.20'''
    dbNameList = ['C_NIMIF_15SUM_0082_0003']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = spotguide_push_link(dbIP, dbName, userName, password)
            datMaker.do()
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

    
    
    
    
    
    
    
    
    
    
    
    
    
    
