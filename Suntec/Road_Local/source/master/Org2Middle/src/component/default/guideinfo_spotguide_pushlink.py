#encoding=utf-8
from component.component_base import comp_base
from common.guideinfo_pushlink_common import link_object
        
class guideinfo_spotguide_pushlink(comp_base):
    def __init__(self):
        comp_base.__init__(self, 'Guideinfo_Spotguide_pushlink')
        
    def _DoCreateTable(self):
        return 0
    
    def _DoCreateIndex(self):
        return 0
    
    def _DoCreateFunction(self):
        return 0
    
    def _Do(self):
        self.backupSpotguideTable()
        self.dealWithInlinkIsInnerLink()
        self.log.info("""Push inlink that is inner link OK.""")
        self.dealWithOutlinkIsInnerLink()
        self.log.info("""Push outlink that is inner link OK.""")
        return
    
    # 备份spotguide_tbl表至spotguide_tbl_bak_pushlink
    def backupSpotguideTable(self):
        sqlcmd = """
drop table if exists spotguide_tbl_bak_pushlink;
select * 
into spotguide_tbl_bak_pushlink
from spotguide_tbl;
"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return
        
    
    # 部分spotguide的inlink是inner link，将它们向后推至非inner link。
    def dealWithInlinkIsInnerLink(self):
        sqlcmd = """
-- 连link_tbl取得link属性，合并成array
select id, array_agg(d.seqnr) as seqnr_list, array_agg(e.link_id) as link_id_list, 
       array_agg(e.s_node) as s_node_list, array_agg(e.e_node) as e_node_list, 
       array_agg(e.link_type) as link_type_list, array_agg(st_astext(e.the_geom)) as the_geom_text_list
from
(
    -- 分拆link序列，以便连link_tbl取link属性
    select c.id, generate_series(1, array_upper(link_list, 1)) as seqnr, unnest(link_list)::bigint as link_id
    from
    (
        -- 求每条spotuguide的link序列
        select distinct a.*, array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text] as link_list
        from
        spotguide_tbl as a
        left join link_tbl as b
        on a.inlinkid=b.link_id
        where b.link_type=4 -- 仅取inlink为inner link的项
    ) as c
    order by id, seqnr
) as d
left join link_tbl as e
on d.link_id=e.link_id
group by d.id
"""
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            _id = row[0]
            seqnr_list = row[1]
            link_list = row[2]
            s_node_list = row[3]
            e_node_list = row[4]
            link_type_list = row[5]
            the_geom_text_list = row[6]

            errMsg = ['']
            linkList = []
            for i in range(0, len(link_list)):
                linkList.append(link_object(errMsg, link_list[i], s_node_list[i], e_node_list[i],
                                                         link_type_list[i], the_geom_text_list[i]))
            # 推导到非inner link。
            possibleLinkList = []
            nodeid = linkList[0].getNotConnectionNodeid(errMsg, linkList[1])
            link_object.findInRoute(errMsg, self.pg, possibleLinkList, linkList[0], nodeid)
            if errMsg[0] <> '':
                return

            minAngle = 180
            bestLinkList = None
            for onePossibleLinkList in possibleLinkList:
                position, angle = link_object.getTrafficAngleByLinkList(errMsg, onePossibleLinkList)
                if angle < minAngle:
                    minAngle = angle
                    bestLinkList = onePossibleLinkList
                    
            if bestLinkList == None:
                self.log.info("""bestLinkList not found.""")
                continue
            
            newLinkList = list(bestLinkList)
            newLinkList.extend(linkList[1:])
            
            self.updateSpotguideTbl(errMsg, id, newLinkList)
            strLog = """id: %s, (%s) --> (%s)""" %\
                    (_id, link_object.getLinkListString(linkList), link_object.getLinkListString(newLinkList))
            self.log.info(strLog)
        self.pg.commit2()
        return

    # 部分spotguide的out是inner link，将它们向前推至非inner link。
    def dealWithOutlinkIsInnerLink(self):
        sqlcmd = """
-- 连link_tbl取得link属性，合并成array
select id, array_agg(d.seqnr) as seqnr_list, array_agg(e.link_id) as link_id_list, 
       array_agg(e.s_node) as s_node_list, array_agg(e.e_node) as e_node_list, 
       array_agg(e.link_type) as link_type_list, array_agg(st_astext(e.the_geom)) as the_geom_text_list
from
(
    -- 分拆link序列，以便连link_tbl取link属性
    select c.id, generate_series(1, array_upper(link_list, 1)) as seqnr, unnest(link_list)::bigint as link_id
    from
    (
        -- 求每条spotuguide的link序列
        select distinct a.*, array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text] as link_list
        from
        spotguide_tbl as a
        left join link_tbl as b
        on a.outlinkid=b.link_id
        where b.link_type=4 -- 仅取outlinkid为inner link的项
    ) as c
    order by id, seqnr
) as d
left join link_tbl as e
on d.link_id=e.link_id
group by d.id
"""
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            _id = row[0]
            seqnr_list = row[1]
            link_list = row[2]
            s_node_list = row[3]
            e_node_list = row[4]
            link_type_list = row[5]
            the_geom_text_list = row[6]

            errMsg = ['']
            linkList = []
            for i in range(0, len(link_list)):
                linkList.append(link_object(errMsg, link_list[i], s_node_list[i], e_node_list[i],
                                                         link_type_list[i], the_geom_text_list[i]))
            # 推导到非inner link。
            possibleLinkList = []
            nodeid = linkList[-1].getNotConnectionNodeid(errMsg, linkList[-2])
            link_object.findOutRoute(errMsg, self.pg, possibleLinkList, linkList[-1], nodeid)
            if errMsg[0] <> '':
                self.log.info(errMsg[0])
                continue

            minAngle = 180
            bestLinkList = None
            for onePossibleLinkList in possibleLinkList:
                position, angle = link_object.getTrafficAngleByLinkList(errMsg, onePossibleLinkList)
                if angle < minAngle:
                    minAngle = angle
                    bestLinkList = onePossibleLinkList
                    
            if bestLinkList == None:
                self.log.info("""bestLinkList not found.""")
                continue
            
            newLinkList = list(linkList)
            newLinkList.extend(bestLinkList[1:])
            
            self.updateSpotguideTbl(errMsg, _id, newLinkList)
            strLog = """id: %s, (%s) --> (%s)""" %\
                    (id, link_object.getLinkListString(linkList), link_object.getLinkListString(newLinkList))
            self.log.info(strLog)
        self.pg.commit2()
        return

    def updateSpotguideTbl(self, errMsg, _id, linkList):
        inlinkid = linkList[0].link_id
        outlinkid = linkList[-1].link_id
        nodeid = linkList[0].getConnectionNodeid(errMsg, linkList[0])
        passlid = link_object.getLinkListString(linkList[1:-1])
        
        strTemp="""
update spotguide_tbl
set nodeid=%s, inlinkid=%s, outlinkid=%s, passlid=%s, passlink_cnt=%s
where id=%s
"""
        sqlcmd = strTemp % (nodeid, inlinkid, outlinkid, passlid, len(linkList)-2, _id)
        self.pg.execute2(sqlcmd)
        return    
    
    
    
    
    
    
    
    
    
    
    
    
    
