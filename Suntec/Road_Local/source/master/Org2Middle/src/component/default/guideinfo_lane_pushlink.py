#encoding=utf-8
'''
Created on 2015-11-24
@author: tangpinghui
'''
from component.component_base import comp_base
from common.guideinfo_pushlink_common import link_object

# 当决策点为非分歧点时，apl不会播报此条诱导数据，此条数据成为冗余数据。
# 本类将对这样的数据进行处理。
class guideinfo_lane_pushlink(comp_base):
    def __init__(self):
        comp_base.__init__(self, 'Guideinfo_Lane_Pushlink')
    
    def _DoCreateTable(self):
        return 0
    
    def _DoCreateIndex(self):
        return 0
    
    def _DoCreateFunction(self):
        return 0
    
    def _Do(self):
        self.backupLaneTable()
        self.pushLinkOneInOneOut()
        return
    
    # 备份lane_tbl表至lane_tbl_bak_pushlink
    def backupLaneTable(self):
        sqlcmd = """
drop table if exists lane_tbl_bak_pushlink;
select * 
into lane_tbl_bak_pushlink 
from lane_tbl;
"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return
    
    # 冗余数据将被删除。
    # 删除这些数据前根据具体情况判断是否需要生成新的车线诱导记录。
    def pushLinkOneInOneOut(self):
        sqlcmd = """
-- step1: 分拆link序列。
drop table if exists lane_tbl_link_seqnr;
select id, 
       unnest(array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text])::bigint as link_id, 
       generate_series(1, array_upper((array[inlinkid::text] || string_to_array(passlid, '|') || array[outlinkid::text]), 1)) as seqnr
into lane_tbl_link_seqnr
from lane_tbl;

create index lane_tbl_link_seqnr_link_id_idx
on lane_tbl_link_seqnr
using btree
(link_id);

--***************************************************
-- step2: 算出link序列的中间node。
create or replace function get_connected_node(s_node1 bigint, e_node1 bigint, s_node2 bigint, e_node2 bigint)
returns bigint
language plpgsql
as
$$
begin
    if s_node1=s_node2 or s_node1=e_node2 then
        return s_node1;
    elseif e_node1=s_node2 or e_node1=e_node2 then
        return e_node1;
    else
        return -1;
    end if;
end;
$$;

create or replace function generate_node_array(s_node_list bigint[], e_node_list bigint[])
returns bigint[]
language plpgsql
as
$$
declare
    resultArray bigint[];
begin
    for i in 1..array_upper(s_node_list, 1)-1 loop
        resultArray := resultArray || get_connected_node(s_node_list[i], e_node_list[i], s_node_list[i+1], e_node_list[i+1]);
    end loop;
    return resultArray;
end;
$$;

drop table if exists lane_tbl_connection_node;
select id, unnest(generate_node_array(array_agg(s_node), array_agg(e_node))) as connection_node
into lane_tbl_connection_node
from
(
    select a.id, a.seqnr, b.link_id, b.s_node, b.e_node
    from
    lane_tbl_link_seqnr as a
    left join link_tbl as b
    on a.link_id=b.link_id
    order by a.id, a.seqnr
) as t
group by id;

create index lane_tbl_connection_node_id_idx
on lane_tbl_connection_node
using btree
(id);

create index lane_tbl_connection_node_connection_node_idx
on lane_tbl_connection_node
using btree
(connection_node);

--***************************************************
-- step3: 求出每个中间node的连接link数
drop table if exists lane_tbl_connection_node_linkcount;
select a.id, a.connection_node, count(b.link_id) as link_count
into lane_tbl_connection_node_linkcount
from
lane_tbl_connection_node as a
left join link_tbl as b
on a.connection_node = b.s_node or a.connection_node=b.e_node
group by a.id, a.connection_node;

create index lane_tbl_connection_node_linkcount_id_idx
on lane_tbl_connection_node_linkcount
using btree
(id);

--***************************************************
-- step4: 仅取中间node的连接link数都小于等于2的项
select t1.*, t2.lanenum, t2.laneinfo, t2.arrowinfo, t2.lanenuml,
       t2.lanenumr, t2.buslaneinfo, t2.exclusive
from
(
    select id, array_agg(seqnr) as seqnr_list, array_agg(link_id) as link_id_list, 
           array_agg(s_node) as s_node_list, array_agg(e_node) as e_node_list,
           array_agg(link_type) as link_type_list, array_agg(astext(the_geom)) as the_geom_text_list
    from
    (
        select a.id, b.seqnr, b.link_id, c.s_node, c.e_node, c.the_geom, c.link_type
        from
        (
            select distinct id, array_agg(link_count)
            from lane_tbl_connection_node_linkcount 
            group by id
            having (max(link_count)<=2)
        ) as a
        left join lane_tbl_link_seqnr as b
        on a.id=b.id
        left join link_tbl as c
        on b.link_id=c.link_id
        order by a.id, b.seqnr
    ) as t
    group by id
) as t1
left join lane_tbl as t2
on t1.id=t2.id;
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
                linkObjList.append(link_object(errMsg, link_list[i], s_node_list[i], e_node_list[i],
                                                     link_type_list[i], the_geom_text_list[i]))
            # 将整条link序列向前推进直至第一个分歧路口
            self.pushLinkUntilFork(errMsg, linkObjList)
            if errMsg[0] <> '':
                self.log.info(errMsg[0])
                continue
            
            # 推到分歧路口后，使用分歧路口的前一条link为新的inlink
            newInlink = linkObjList[-1]
            nodeid = newInlink.getNonConnectedNodeid(errMsg, linkObjList[-2])
            if errMsg[0] <> '':
                self.log.info(errMsg[0])
                continue
            
            # 基于新的inlink，尝试在分歧路口生成所有可能的行车路线
            possibleLinkLists = []
            link_object.findOutRoute(errMsg, self.pg, possibleLinkLists, newInlink, nodeid)
            if errMsg[0] <> '':
                self.log.info(errMsg[0])
                continue
            
            # 过滤可能的行车路线，只有符合箭头信息且原表中不存在相同诱导记录的信息才收录。
            for onePossibleLinkList in possibleLinkLists:
                bLinkListFitsArrowInfo = self.isLinklistFitsArrowInfo(errMsg, onePossibleLinkList, arrowinfo)
                if errMsg[0] <> '':
                    self.log.info("""(%s: %s)""" % (_id, errMsg[0]))
                    continue
                    
                if bLinkListFitsArrowInfo == False:
                    continue
                
                bExists = self.isGuideinfoLaneExists(errMsg, onePossibleLinkList, laneinfo)
                if errMsg[0] <> '':
                    self.log.info("""(%s: %s)""" % (_id, errMsg[0]))
                    continue
                
                if bExists == True:
                    continue
                    
                # 当所有条件均允许时，生成新的车线诱导记录。
                self.addLaneInfo(errMsg, onePossibleLinkList, lanenum, laneinfo,
                                 arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
                self.log.info("""generate new lane guideinfo for id: %s, link list: %s""" % \
                              (_id, link_object.getLinkListString(onePossibleLinkList)))
            # 删除此条单进单出的记录
        sqlcmd = """
delete 
from lane_tbl 
where id in 
(
    select distinct id
    from lane_tbl_connection_node_linkcount 
    group by id
    having (max(link_count)<=2)
)
"""
        self.pg.execute(sqlcmd)
        self.conn.commit()
        return
    
    # 沿着当前交通流，向前推link，直至遇到第一个分歧路口，确定inlink。
    # linkObjList: 此list会被不断更新，添加新的link直至分叉路口。
    # 递归结束后，linkObjList[-1]则为新的inlink。
    def pushLinkUntilFork(self, errMsg, linkObjList):
        nodeid = linkObjList[-1].getNonConnectedNodeid(errMsg, linkObjList[-2])
        if errMsg[0] <> '':
            return

        outlinkList = link_object.getOutlinkList(errMsg, self.pg, nodeid, linkObjList[-1])
        connectedLinkList = link_object.getConnectedLinkList(errMsg, self.pg, nodeid)
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
    
    # 新增一条车线诱导信息。
    def addLaneInfo(self, errMsg, linkList, lanenum, laneinfo, 
                    arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive):
        inlinkObj = linkList[0]
        outlinkObj = linkList[-1]
        passlid = "|".join(str(x) for x in linkList[1:-1])
        nodeid = inlinkObj.getConnectedNodeidWith(errMsg, linkList[1])
        sqlcmd = """
insert into 
lane_tbl(id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
        lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
values
(max(id)+1, %s, %s, %s, %s, %s, 
 %s, %s, %s, %s, %s, %s, %s)
""" % (nodeid, inlinkObj.link_id, outlinkObj.link_id, passlid, len(linkList)-2, 
       lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
        self.pg.execute(sqlcmd)
        self.conn.commit()
        return

    # 根据交通流变化的角度，判断箭头信息是否符合。
    def isLinklistFitsArrowInfo(self, errMsg, linkList, arrowInfo):
        bestArrowInfo = -1
        position, angle = link_object.getTrafficAngleByLinkList(errMsg, linkList)
        angle = angle % 360
        if position == link_object.DIR_RIGHT_SIDE:
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
from lane_tbl where 
inlinkid=%s and 
outlinkid=%s and 
nodeid=%s and 
passlink_cnt=%s and  
is_lane_info_grouped(laneinfo, '%s')=false;
""" % (inlinkObj.link_id, outlinkObj.link_id, nodeid, len(linkObjList) - 2, whichLane)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        return len(rows) > 0
            
            

    



    
    
    
    
    
    
    
    
    
    
    
    
    
    
