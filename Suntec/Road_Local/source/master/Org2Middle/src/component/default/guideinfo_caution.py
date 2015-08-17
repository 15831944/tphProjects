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
        
        # ����ѳ�linkΪ�յ����Σ���Ҫ�����ȡ��Ӧ���ѳ�link����
        
        self._GetOffLink()
        self._GetOutLinkList()
        self._GetCautionLink()
        
        return 0
    
    def _GetOffLink(self):
        
        self.log.info('Now it is creating temp_caution_off_link_tbl...')
        
        # ���ɱ�temp_caution_off_link_tbl��¼�������ֱ���ѳ�link��Ϣ
        # �����ܶ��ѳ�link�ֱ�û���޶������ñ�temp_caution_off_link_tbl������������
        
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
        
        # ���ɱ�temp_caution_extend_link_tbl��¼Ҫ��չ��link��Ϣ
        # �ѳ�link��չԭ�򣺻����޶��ѳ�link����Ϊinner link
        
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
        
        # ����mid_get_inner_link_count��ȡ��temp_caution_extend_link_tbl���ѳ�link��inner link�ļ�¼����
        # ����mid_out_link_extend���ѳ�link��inner link���������link��չ
        
        self.CreateFunction2('mid_get_inner_link_count')
        self.CreateFunction2('mid_out_link_extend')
        
        # ���ѳ�link��inner link�����Σ���Ҫ������չ
        # ��չ��ֹ�������ѳ�link��Ϊinner link
        
        sqlcmd = """ 
                    select mid_out_link_extend();
                """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl end')
        
    def _GetCautionLink(self):
        
        self.log.info('Now it is creating temp_caution_link_tbl...')
        
        # ���ɱ�temp_caution_link_tbl��¼caution��Ӧ��·����Ϣ������link�������㡢�ѳ�link���ѳ�ʱ����link��Ϣ��
        
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