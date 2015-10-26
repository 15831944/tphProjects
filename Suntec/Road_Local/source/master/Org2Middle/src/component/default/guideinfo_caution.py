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
        '创建表.'  
        
        self.CreateTable2('caution_tbl')
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
    
    def _Do(self):
        
        self._make_caution_from_origin()
        
        # 与机能组确认，caution对应的脱出link可以为空，因此对脱出link不做特殊处理，下面屏蔽
        #self._alter_caution_outlinkinfo()
        
        return 0
    
    def _DoCreateIndex(self):
        '创建相关表索引.'
        
        return 0
    
    def _make_caution_from_origin(self):
        
        # 从不同协议对应的原始表单中抽取caution信息
        # 抽取的caution对应的路径信息可能不完善，将根据机能组要求进行完善
        
        return 0
    
    def _alter_caution_outlinkinfo(self):
        
        # 针对脱出link为空或者脱出link是inner link的情形，需要计算获取对应的脱出link序列
        # 1、若机能组对脱出link没有任何要求，以下函数均可屏蔽
        # 2、若机能组仅要求脱出link不能为空，2/3函数可屏蔽
        # 3、若机能组要求脱出link不为空而且脱出link不能为inner link，以下函数均不可屏蔽
        
        self._make_caution_routeinfo()
        self._update_caution_outlinkinfo()
        self._update_caution_routeinfo()
        self._update_caution_tbl_again()
        
        return 0    
    
    def _make_caution_routeinfo(self):
        
        self.log.info('Now it is creating temp_caution_off_link_tbl...')
        
        # 创建表单temp_caution_routeinfo_tbl记录caution对应的路径信息，作成：
        # 1、若脱出link为空，获取其直接脱出link，作成路径信息
        # 2、若脱出link不为空，作成路径信息
        
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
        
        return 0
    
    def _update_caution_outlinkinfo(self):
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl...')
        
        # 作成表单temp_caution_extend_link_tbl记录要扩展的路径（即脱出link为inner link）信息
        # 脱出link扩展原因：机能限定脱出link不能为inner link
        
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
        
        # 函数mid_get_inner_link_count获取表单temp_caution_extend_link_tbl中脱出link是inner link的记录个数
        # 函数mid_out_link_extend对脱出link是inner link的情况进行link扩展
        
        self.CreateFunction2('mid_get_inner_link_count')
        self.CreateFunction2('mid_out_link_extend')
        
        # 对脱出link是inner link的情形，需要进行扩展
        # 扩展终止条件是脱出link不为inner link
        
        sqlcmd = '''
                    select mid_out_link_extend();
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is creating temp_caution_extend_link_tbl end')
        
        return 0
    
    def _update_caution_routeinfo(self):
        
        self.log.info('Now it is updating temp_caution_routeinfo_tbl...')
        
        # 创建temp_caution_routeinfo_tbl_bak保存caution的完整路径信息（该路径信息中中包含的脱出link不为inner link）
        # 使用左连接就是把路径所有的可能性都包含了
        # 再次更新表单temp_caution_routeinfo_tbl保存caution完整的路径信息，这样的目的：
        # 1、更新表单temp_caution_routeinfo_tbl中caution对应的路径信息，保证caution对应的脱出link不为inner link
        # 2、表单temp_caution_routeinfo_tbl作为对外的表单，若机能组不要求脱出link的种别，未更新之前的temp_caution_routeinfo_tbl \
        #   即可满足条件；若机能组要求脱出link不能为inner link，则更新后的temp_caution_routeinfo_tbl满足要求
        
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
        
        return 0
    
    def _update_caution_tbl_again(self):
        
        self.log.info('Now it is updating caution_tbl...')
        
        # 再次更新表单caution_tbl数据
        # 由于对caution对应的路径信息有更新，需要对原有caution_tbl进行再次更新（将原有caution_tbl表单删除，重新插入更新后的caution信息）
        
        self.CreateTable2('caution_tbl')
        
        # 表单temp_caution_routeinfo_tbl保存的是caution的路径信息（包含进入link， 经过link以及脱出link）
        # caution的路径信息中，第一条link即为进入link，最后一条link即为脱出link，中间的link列即为经过link
        
        sqlcmd = '''
                    insert into caution_tbl(inlinkid, nodeid, outlinkid, passlid, passlink_cnt,
                        data_kind, voice_id, strTTS, image_id)
                    select inlinkid, nodeid, outlinkid, 
                        (case when array_upper(routelnklst, 1) = 2 then null 
                            else array_to_string(routelnklst[2:array_upper(routelnklst, 1)-1], '|')
                        end)::varchar as passlid,
                        (array_upper(routelnklst, 1)-2)::smallint as passlink_cnt,
                        data_kind, voice_id, strTTS, image_id
                    from temp_caution_routeinfo_tbl
                    order by inlinkid, nodeid, outlinkid, data_kind;
                    
                    analyze caution_tbl;
                '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('Now it is updating caution_tbl...')
        
        return 0