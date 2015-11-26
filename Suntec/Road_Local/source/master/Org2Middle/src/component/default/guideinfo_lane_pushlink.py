# -*- coding: UTF8 -*-
'''
Created on 2015-11-24

@author: tangpinghui
'''

import math
import component.component_base

# -*- coding: cp936 -*-
import math
import psycopg2
# link_tbl表每个行集构成的对象。     

A_WGS84 = 6378137.0
E2_WGS84 = 6.69437999013e-3
# 功能类，此类用于换算两点的经纬度与方位、距离
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

class link_object(object):
    def __init__(self, linkid, sNodeid, eNodeid, the_geom_text, name):
        self.id = linkid
        self.sNodeid = sNodeid
        self.eNodeid = eNodeid
        self.the_geom_text = the_geom_text
        self.pointlist = self.geomText2Pointlist(the_geom_text)
        self.name = name
        
    def toString(self):
        return """id: %.0f, sNodeid: %.0f, eNodeid: %.0f, name: %s""" % \
               (self.id, self.sNodeid, self.eNodeid, self.name)
    
    def geomText2Pointlist(self, the_geom_text):
        pointlist = []
        the_geom_text = the_geom_text.replace("""MULTILINESTRING((""", "")
        the_geom_text = the_geom_text.replace("""))""", "")
        the_geom_text = the_geom_text.replace("""LINESTRING(""", "")
        the_geom_text = the_geom_text.replace(""")""", "")
        geomTextSplit = the_geom_text.split(',')
        for oneGeomText in geomTextSplit:
            lat = float(oneGeomText.split(' ')[1])
            lon = float(oneGeomText.split(' ')[0])
            pointlist.append(LatLonPoint(lat, lon))
        return pointlist
    
    # 获取本link与linkObj2之间的相连node。
    # 若linkObj2与本link并无相连点，则返回空，并置errMsg。
    def getConnectedNodeidWith(self, errMsg, linkObj2):
        if self.sNodeid == linkObj2.sNodeid or self.sNodeid == linkObj2.eNodeid:
            return self.sNodeid
        elif self.eNodeid == linkObj2.sNodeid or self.eNodeid == linkObj2.eNodeid:
            return self.eNodeid
        else:
            errMsg[0] = """input in/out links are not connected.\n""" + \
                        """inlink: %s\noutlink: %s""" % \
                        (self.toString(), linkObj2.toString())
            return None
    
    # 获取与linkObj2的连接node以外的另一个node。
    # 若linkObj2与本link并无相连点，则返回空，并置errMsg。
    def getNonConnectedNodeid(self, errMsg, linkObj2):
        if self.sNodeid == linkObj2.sNodeid or self.sNodeid == linkObj2.eNodeid:
            return self.eNodeid
        elif self.eNodeid == linkObj2.sNodeid or self.eNodeid == linkObj2.eNodeid:
            return self.sNodeid
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
        
        if nodeid == self.sNodeid:
            point1 = self.pointlist[0]
            point2 = None
            for i in range(1, len(self.pointlist)):
                if point1.getDistanceByPoint(self.pointlist[i]) > 5:
                    point2 = self.pointlist[i]
                    break
            # 没有任何一个形状点与sNodeid的距离大于5米，则使用eNodeid为point2
            if point2 == None:
                point2 = self.pointlist[-1]
            
            if trafficDir == 'to_this_node':
                return point2.getAngleByPoint(point1)
            else:  # trafficDir == 'from_this_node'
                return point1.getAngleByPoint(point2)
            
        elif nodeid == self.eNodeid:
            point1 = self.pointlist[-1]
            point2 = None
            for i in range(len(self.pointlist) - 2, -1, -1):
                if point1.getDistanceByPoint(self.pointlist[i]) > 5:
                    point2 = self.pointlist[i]
                    break
            # 没有任何一个形状点与eNodeid的距离大于5米，则使用sNodeid为point2
            if point2 == None:
                point2 = self.pointlist[0]
            
            if trafficDir == 'to_this_node':
                return point2.getAngleByPoint(point1)
            else:  # trafficDir == 'from_this_node'
                return point1.getAngleByPoint(point2)
        
        else:
            errMsg[0] = """nodeid is not the link's endpoint.\n""" + \
                        """link: %s, nodeid: %s""" % (self.toString(), nodeid)
            return None

# 此类用于对lane_tbl中的记录进行推link操作。
# step1: 挑选出lane_tbl中单进单出的记录
# step2: 若这些记录的箭头是复合箭头，认为沿着这条单进单出的路线向前不远处必定可以找到一个分歧点
# step3: 将复合箭头分解，并匹配至分歧点的各个outlink，生成新的车线记录
class comp_guideinfo_lane_pushlink(component.component_base.comp_base):
    def __init__(self):
        component.component_base.comp_base.__init__(self, 'Guideinfo_Lane_Pushlink')
        
    def _DoCreateTable(self):
        return 0
    
    def _DoCreateIndex(self):
        return 0
    
    def _Do(self):
        self.log.info("""spotguide push link begin...""")
        self.findOneInOneOutLinks()
        self.log.info("""spotguide push link end.""")
        return
    
    # spotguide机能要求决策点必须是分歧点。
    # 当inlink后的第一个节点不是分歧点的时候应该推点，推至分歧点上。
    # 此函数过滤spotguide_tbl表，将决策点非分歧点的spotguide记录过滤出来。
    def findOneInOneOutLinks_spotguide(self):
        sqlcmd = """
select id, array_agg(link_id), array_agg(seqnr), array_agg(s_node), array_agg(e_node), 
    array_agg(st_astext(the_geom)) as the_geom_text_list,
    array_agg(road_name) as name_list
from
(
    select a.id, a.patternno, a.arrowno, a.link_id, a.seqnr, 
           b.s_node, b.e_node, b.the_geom, b.road_name
    from
    (
        select t.id, t.patternno, t.arrowno, unnest(t.link_list)::bigint as link_id, 
               generate_series(1, array_upper(link_list, 1)) as seqnr
        from
        (
            select distinct id, patternno, arrowno
                   array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text] as link_list
            from spotguide_tbl
            where type<>12
        )as t
    ) as a
    left join link_tbl as b
    on a.link_id=b.link_id
    order by a.id, a.seqnr
) as t
group by id
"""
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        oneInOneOutList = []
        for row in rows:
            id = row[0]
            link_list = row[1]
            seqnr_list = row[2]
            s_node_list = row[3]
            e_node_list = row[4]
            the_geom_text_list = row[5]
            name_list = row[6]

            inlinkObj = link_object(link_list[0], s_node_list[0], e_node_list[0], 
                                               the_geom_text_list[0], name_list[0])
            outlinkObj = link_object(link_list[1], s_node_list[1], e_node_list[1], 
                                               the_geom_text_list[1], name_list[1])
            errMsg = ['']
            nodeid = inlinkObj.getConnectedNodeidWith(errMsg, outlinkObj)
            if errMsg[0] <> '':
                self.log.error(errMsg[0])
                continue
            
            outlinkList = self.getOutlinkList(errMsg, nodeid, inlinkObj)
            if errMsg[0] <> '':
                self.log.error(errMsg[0])
                continue
                
            # 以inlinkObj为流入link，nodeid为目标节点求出流出link的数目小于2，此点为非分歧点。
            if len(outlinkList) < 2:
                linkObjList = []
                for i in range(0, len(link_list)):
                    linkObjList.append(link_object(link_list[i], s_node_list[i], e_node_list[i], 
                                                   the_geom_text_list[i], name_list[i]))
                #self.pushForwardLink(errMsg, linkObjList, laneInfoList, maxDeep)
        return
    
    # 根据link序列进行推link操作，直至推导到第一个分歧点。
    # linkList: link序列
    # maxDeep: 当推进的link数超过此值仍未遇到分歧时，返回错误。
    def pushForwardLink(self, errMsg, linkObjList, laneInfoList, maxDeep=10):
        if len(linkObjList<2):
            errMsg[0] = 'link strand count must be at least 2, but now it\'s %s' %len(linkObjList)
            return
        
        if len(linkObjList>=10):
            errMsg[0] = 'link count exceed %s, but did not reach a junction.' % maxDeep
            return
            
        linkObj1 = linkObjList[-2]
        linkObj2 = linkObjList[-1]
        connNode = linkObj1.getConnectedNodeidWith(errMsg, linkObj2)
        if errMsg[0] <> '':
            return
        
        restNodeid = None
        if linkObj2.sNode == connNode:
            restNodeid = linkObj2.eNode
        else:
            restNodeid = linkObj2.sNode
            
        outlinkList = self.getOutlinkList(errMsg, restNodeid, linkObj2)
        if errMsg[0] <> '':
            return
        
        if len(outlinkList) == 1:
            linkObjList.append(outlinkList)
            return self.pushForwardLink(errMsg, linkObjList, laneInfoList, maxDeep)
        else:
            bestOutlink = None
            for oneOutlink in outlinkList:
                
                bestOutlink = oneOutlink
                
            linkObjList.append(bestOutlink)
            return
    
    
    # 求此以inlinkid为流入link，nodeid点的outlink数量。
    def getOutlinkList(self, errMsg, nodeid, linkObj):
        sqlcmd = """
        select link_id, s_node, e_node, st_astext(the_geom) as the_geom_text_list, road_name
        from link_tbl 
        where s_node=%s and one_way_code in (1,2) and link_id<>%s
        union
        select link_id, s_node, e_node, st_astext(the_geom) as the_geom_text_list, road_name
        from link_tbl 
        where e_node=%s and one_way_code in (1,3) and link_id<>%s;
        """
        self.pg.execute2(sqlcmd, [nodeid, linkObj.id, nodeid, linkObj.id])
        rows = self.pg.fetchall2()
        linkList = []
        for row in rows:
            linkid = row[0]
            sNodeid = row[1]
            eNodeid = row[2]
            the_geom_text = row[3]
            name = row[4]
            linkList.append(link_object(linkid, sNodeid, eNodeid, the_geom_text, name))
            
        if len(linkList) <= 0:
            errMsg[0] = 'can\'t find outlink.\n link: %s.\n nodeid: %s' % (linkObj.toString(), nodeid)
            return None
        return linkList
























