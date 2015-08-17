# -*- coding: utf8 -*-
'''
Created on 2012-2-23

@author: sunyifeng
'''

import io
import os

import component.component_base

class comp_guideinfo_caution(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo Caution') 
        
    def _DoCreateTable(self):
        
        self.CreateTable2('caution_tbl')  
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
    
    def _Do(self):
        
        return 0
    
    def _GetOutLinkSeq(self):
        
        # 针对脱出link为空的情形，需要计算获取对应的脱出link序列
        
        self._GetOffLink()
        self._GetOutLinkList()
        self._GetCautionLink()
        
        return 0
    
    def _GetOffLink(self):
        
        self.log.info('Now it is creating temp_caution_off_link_tbl...')
        
        # 作成表单temp_caution_off_link_tbl记录引导点的直接脱出link信息
        # 若机能对脱出link种别没有限定，所得表单temp_caution_off_link_tbl即可满足需求
        
        sqlcmd = """ 
                    drop table if exists temp_caution_off_link_tbl;
                    CREATE TABLE temp_caution_off_link_tbl 
                    as (
                        select distinct a.inlinkid, a.nodeid, b.link_id as outlink, b.link_type as outlink_type, 
                            array[a.inlinkid, b.link_id] as routelinklist,
                            array[a.nodeid] as routenodelist,
                            (case when a.nodeid = b.s_node then b.e_node else b.s_node end) as nxt_node
                        from temp_org_caution_link_tbl a
                        left join link_tbl b
                            on (a.nodeid = b.s_node and b.one_way_code in (1, 2)) or 
                                (a.nodeid = b.e_node and b.one_way_code in (1, 3))
                        where a.outlinkid is null and a.inlinkid != b.link_id
                        order by inlinkid, nodeid, outlink
                    );
                      
                    analyze temp_caution_off_link_tbl;
                """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_off_link_tbl end')
    
    def _GetOutLinkList(self):
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl...')
        
        # 作成表单temp_caution_extend_link_tbl记录要扩展的link信息
        # 脱出link扩展原因：机能限定脱出link不能为inner link
        
        sqlcmd = """ 
                    drop table if exists temp_caution_extend_link_tbl;
                    CREATE TABLE temp_caution_extend_link_tbl 
                    as (
                        select distinct *
                        from temp_caution_off_link_tbl
                        where outlink_type = 4
                        order by inlinkid, nodeid, outlink
                    );
                    
                    ALTER TABLE temp_caution_extend_link_tbl ADD COLUMN id SERIAL;
                      
                    analyze temp_caution_extend_link_tbl;
                """
            
        self.pg.do_big_insert2(sqlcmd)
        
        # 函数mid_get_inner_link_count获取表单temp_caution_extend_link_tbl中脱出link是inner link的记录个数
        # 函数mid_out_link_extend对脱出link是inner link的情况进行link扩展
        
        self.CreateFunction2('mid_get_inner_link_count')
        self.CreateFunction2('mid_out_link_extend')
        
        # 对脱出link是inner link的情形，需要进行扩展
        # 扩展终止条件是脱出link不为inner link
        
        sqlcmd = """ 
                    select mid_out_link_extend();
                """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl end')
        
    def _GetCautionLink(self):
        
        self.log.info('Now it is creating temp_caution_link_tbl...')
        
        # 作成表单temp_caution_link_tbl记录caution对应的路径信息（进入link、引导点、脱出link、脱出时经过link信息）
        
        sqlcmd = """ 
                    drop table if exists temp_caution_link_tbl;
                    CREATE TABLE temp_caution_link_tbl (
                        inlink bigint not null,
                        node bigint not null,
                        outlink bigint not null,
                        passlink_cnt smallint default 0,
                        passlid varchar
                    );
                    
                    insert into temp_caution_link_tbl (
                        inlink, node, outlink, passlink_cnt, passlid
                    )
                    select distinct inlinkid, nodeid, outlinkid, passlink_cnt, passlid
                    from temp_org_caution_link_tbl
                    where outlinkid is not null;
                    
                    insert into temp_caution_link_tbl (
                        inlink, node, outlink, passlink_cnt, passlid
                    )
                    select distinct b.inlink, b.node, b.outlink, b.passlink_cnt, b.passlid
                    from (
                        select distinct inlinkid as inlink, nodeid as node, outlink, 
                            (array_upper(routelinklist, 1)-2)::smallint as passlink_cnt, 
                            (case when array_upper(routelinklist, 1) = 2 then null 
                                else array_to_string(routelinklist[2:array_upper(routelinklist, 1)-1], '|') end)::varchar as passlid
                        from (
                            select distinct inlinkid, nodeid, outlink, routelinklist, routenodelist
                            from temp_caution_off_link_tbl
                            where outlink_type != 4
                            
                            union 
                            
                            select distinct inlinkid, nodeid, outlink, routelinklist, routenodelist
                            from temp_caution_extend_link_tbl
                        ) a
                    ) b
                    left join temp_caution_link_tbl c
                        on b.inlink = c.inlink and 
                            b.node = c.node and 
                            b.outlink = c.outlink and 
                            b.passlid is not distinct from c.passlid
                    where c.inlink is null
                    order by b.inlink, b.node, b.outlink;

                    analyze temp_caution_link_tbl;
                """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_link_tbl end')