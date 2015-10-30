# coding:utf-8
import platform.TestCase

# 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_br数据
class CCheckBrDataUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                            from
                            (
                                select patternno, arrowno, nodeid, inlinkid, outlinkid
                                from org_br 
                                group by patternno, arrowno, nodeid, inlinkid, outlinkid
                                having count(*) > 1
                          )as t
                '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0

# 确认org_br表不为空
class CCheckBrCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_br;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

# 确认org_br表中没有“type”为6的项。
class CCheckBrType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_br where type='6'
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)<=0
     
# 确认org_br表里的每组inlinkid，outlinkid都相连
class CCheckAllInlinkOutlinkPasslidConnected(platform.TestCase.CTestCase):
    
    # 确认每组inlinkid，outlinkid都相连
    def _do(self):
        
        # 创建临时表temp_spotguide_bl，里面仅列出所有与spotguide有关的link信息。
        # 优化：建立临时表，查询link两端节点时使用，提高查询速度
        sqlcmd = '''
drop table if exists temp_mesh_border_nodes;
select *
into temp_mesh_border_nodes
from org_n where id<>adjoin_nid and adjoin_nid<>'0';

drop index if exists temp_mesh_border_nodes_id_idx;
create index temp_mesh_border_nodes_id_idx
on temp_mesh_border_nodes
using btree
(id);

drop table if exists temp_spotguide_org_data_check;
select a.linkid, 
    case when c.id is null then b.snodeid
    when c.id<c.adjoin_nid then c.id
    else c.adjoin_nid end as snodeid,
    case when d.id is null then b.enodeid
    when d.id<d.adjoin_nid then d.id
    else d.adjoin_nid end as enodeid
into temp_spotguide_org_data_check
from 
(   select inlinkid as linkid from org_br
    union
    select outlinkid as linkid from org_br
    union
    select unnest(string_to_array(passlid, '|')) as linkid from org_br where passlid<>''
    union 
    select unnest(string_to_array(passlid2, '|')) as linkid from org_br where passlid2<>''
) as a
left join org_r as b
on a.linkid=b.id
left join temp_mesh_border_nodes as c
on b.snodeid=c.id
left join temp_mesh_border_nodes as d
on b.enodeid=d.id;

drop index if exists temp_spotguide_org_data_check_linkid_idx;
create index temp_spotguide_org_data_check_linkid_idx
on temp_spotguide_org_data_check
using btree
(linkid);
''' 
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        
        sqlcmd = '''
                    select nodeid, inlinkid, outlinkid, 
                            case when passlid2<>'' then passlid||'|'||passlid2
                            else passlid end as passlid 
                    from org_br;
                '''
        rows = self.pg.get_batch_data(sqlcmd)
        for row in rows:
            #nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            passlid = row[3]
            
            linkList = [inlinkid, outlinkid]
            if passlid<>'':
                linkList[1:0] = passlid.split('|')
                
            for i in range(0, len(linkList)-1):
                if self._GetNodeBetweenLinks(linkList[i], linkList[i+1], "temp_spotguide_org_data_check") is None:
                    return False
        return True
    
    # 通过查询link与node的信息确定两条link是否相交。
    # 如果相交，返回连接点，否则返回空
    def _GetNodeBetweenLinks(self, linkid1, linkid2, tempLinkTbl):
        node_sqlcmd = '''
                        SELECT snodeid, enodeid FROM %s WHERE linkid='%s';
                    '''
        self.pg.execute(node_sqlcmd % (tempLinkTbl, linkid1))
        inres_row = self.pg.fetchone()
        self.pg.execute(node_sqlcmd % (tempLinkTbl, linkid2))
        outres_row = self.pg.fetchone()
    
        if inres_row is None or outres_row is None:
            return None
        elif inres_row[0] in (outres_row[0], outres_row[1]):
            return inres_row[0]
        elif inres_row[1] in (outres_row[0], outres_row[1]):
            return inres_row[1]
        else:
            return None
        
# 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_dm数据
class CCheckDmDataUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                            from
                            (
                                select patternno, arrowno, nodeid, inlinkid, outlinkid
                                from org_dm 
                                group by patternno, arrowno, nodeid, inlinkid, outlinkid
                                having count(*) > 1
                          )as t
                '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0

class CCheckDmCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_dm;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
















