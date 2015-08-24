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
        
        # ����ѳ�linkΪ�ջ����ѳ�link��inner link�����Σ���Ҫ�����ȡ��Ӧ���ѳ�link����
        # 1������������ѳ�linkû���κ�Ҫ�����º�����������
        # 2�����������Ҫ���ѳ�link����Ϊ�գ�2/3����������
        # 3����������Ҫ���ѳ�link��Ϊ�ն����ѳ�link����Ϊinner link�����º�������������
        
        self.__GetCautionRouteInfo()
        self.__GetExtendLnkLst()
        self.__UpdateCautionRouteInfo()
        self.__UpdateCautionlinktbl()
        
        return 0
    
    def __GetCautionRouteInfo(self):
        
        self.log.info('Now it is creating temp_caution_off_link_tbl...')
        
        # ������temp_caution_routeinfo_tbl��¼caution��Ӧ��·����Ϣ�����ɣ�
        # 1�����ѳ�linkΪ�գ���ȡ��ֱ���ѳ�link������·����Ϣ
        # 2�����ѳ�link��Ϊ�գ�����·����Ϣ
        
        self.CreateFunction2('mid_get_pass_node')
        self.CreateFunction2('mid_get_nxt_node')
        
        sqlcmd = '''
                    drop table if exists temp_caution_routeinfo_tbl;
                    create table temp_caution_routeinfo_tbl
                    as (
                        -- out link is null and calc off link, then get route info
                        select inlinkid, nodeid, link_id as outlinkid, link_type as outlink_type,
                            array[inlinkid, link_id] as routelnklst,
                            array[nodeid] as routenodelst,
                            (case when nodeid = s_node then e_node else s_node end) as nxt_node,
                            data_kind, voice_id, strTTS, image_id
                        from caution_tbl a
                        left join link_tbl b
                            on (nodeid = s_node and one_way_code in (1, 2)) or
                                (nodeid = e_node and one_way_code in (1, 3))
                        where outlinkid is null and inlinkid is distinct from link_id
                        
                        union 
                        
                        -- out link is not null then get route info
                        select inlinkid, nodeid, outlinkid, link_type as outlink_type,
                            (array[inlinkid] || 
                             (case when passlink_cnt = 0 then null else string_to_array(passlid, '|') end)::bigint[] ||
                             array[outlinkid]) as routelnklst,
                             mid_get_pass_node(nodeid, passlink_cnt, passlid) as routenodelst,
                             mid_get_nxt_node(nodeid, outlinkid, passlink_cnt, passlid) as nxt_node,
                             data_kind, voice_id, strTTS, image_id
                        from caution_tbl a
                        left join link_tbl b
                            on outlinkid = link_id
                        where outlinkid is not null
                    );
                    
                    analyze temp_caution_routeinfo_tbl;
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_off_link_tbl end')
        return
    
    def __GetExtendLnkLst(self):
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl...')
        
        # ���ɱ�temp_caution_extend_link_tbl��¼Ҫ��չ��·�������ѳ�linkΪinner link����Ϣ
        # �ѳ�link��չԭ�򣺻����޶��ѳ�link����Ϊinner link
        
        sqlcmd = '''
                    drop table if exists temp_caution_extend_link_tbl;
                    create table temp_caution_extend_link_tbl
                    as (
                        -- initial with outlink is inner link
                        select distinct inlinkid, nodeid, outlinkid, outlink_type,
                            routelnklst, routenodelst, nxt_node
                        from temp_caution_routeinfo_tbl
                        where outlink_type = 4
                    );
                    
                    alter table temp_caution_extend_link_tbl add column id serial;
                    
                    analyze temp_caution_extend_link_tbl;
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        # ����mid_get_inner_link_count��ȡ��temp_caution_extend_link_tbl���ѳ�link��inner link�ļ�¼����
        # ����mid_out_link_extend���ѳ�link��inner link���������link��չ
        
        self.CreateFunction2('mid_get_inner_link_count')
        self.CreateFunction2('mid_out_link_extend')
        
        # ���ѳ�link��inner link�����Σ���Ҫ������չ
        # ��չ��ֹ�������ѳ�link��Ϊinner link
        
        sqlcmd = '''
                    select mid_out_link_extend();
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl end')
        return 
    
    def __UpdateCautionRouteInfo(self):
        
        self.log.info('Now it is updating temp_caution_routeinfo_tbl...')
        
        # ����temp_caution_routeinfo_tbl_bak����caution������·����Ϣ����·����Ϣ���а������ѳ�link��Ϊinner link��
        # ʹ�������Ӿ��ǰ�·�����еĿ����Զ�������
        # �ٴθ��±�temp_caution_routeinfo_tbl����caution������·����Ϣ��������Ŀ�ģ�
        # 1�����±�temp_caution_routeinfo_tbl��caution��Ӧ��·����Ϣ����֤caution��Ӧ���ѳ�link��Ϊinner link
        # 2����temp_caution_routeinfo_tbl��Ϊ����ı����������鲻Ҫ���ѳ�link���ֱ�δ����֮ǰ��temp_caution_routeinfo_tbl \
        #   ����������������������Ҫ���ѳ�link����Ϊinner link������º��temp_caution_routeinfo_tbl����Ҫ��
        sqlcmd = '''
                    drop table if exists temp_caution_routeinfo_tbl_bak;
                    create table temp_caution_routeinfo_tbl_bak
                    as (
                        select *
                        from temp_caution_routeinfo_tbl
                        where outlink_type is distinct from 4  
                        
                        union
                        
                        select a.inlinkid, a.nodeid, b.outlinkid, b.outlink_type, b.routelnklst,
                            b.routenodelst, b.nxt_node, a.data_kind, a.voice_id, a.strTTS, a.image_id
                        from temp_caution_routeinfo_tbl a
                        left join temp_caution_extend_link_tbl b
                            on a.inlinkid = b.inlinkid and a.nodeid = b.nodeid
                        where a.outlink_type = 4
                    );
                    
                    drop table if exists temp_caution_routeinfo_tbl; 
                    create table temp_caution_routeinfo_tbl
                    as (
                        select distinct *
                        from temp_caution_routeinfo_tbl_bak
                        order by inlinkid, nodeid, outlinkid, routelnklst, routenodelst, data_kind
                    );
                    
                    analyze temp_caution_routeinfo_tbl;
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is updating temp_caution_routeinfo_tbl end')
        return
    
    def __UpdateCautionlinktbl(self):
        
        self.log.info('Now it is updating caution_tbl...')
        
        # ��ձ�caution_tbl����
        # ���´�����caution_tbl��������ձ�����
        
        self.CreateTable2('caution_tbl')
        
        # ��temp_caution_routeinfo_tbl�������caution��·����Ϣ����������link�� ����link�Լ��ѳ�link��
        # caution��·����Ϣ�У���һ��link��Ϊ����link�����һ��link��Ϊ�ѳ�link���м��link�м�Ϊ����link
        
        sqlcmd = '''
                    insert into caution_tbl(inlinkid, nodeid, outlinkid, passlid, passlink_cnt,
                        data_kind, voice_id, strTTS, image_id)
                    select inlinkid, nodeid, outlinkid, 
                        (case when array_upper(routelnklst, 1) = 2 then null 
                            else array_to_string(routelnklst[2:array_upper(routelnklst, 1)-1], '|')
                        end)::varchar as passlid,
                        (array_upper(routelnklst, 1)-2)::smallint as passlink_cnt,
                        data_kind, voice_id, strTTS, image_id
                    from temp_caution_routeinfo_tbl;
                    
                    analyze caution_tbl;
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is updating caution_tbl...')
        return