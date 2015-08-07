# -*- coding: UTF8 -*-
'''
Created on 2014-10-9

@author: zhaojie
'''

import io
import os

import component.component_base

class comp_guideinfo_caution_rdf(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo Caution') 
        
    def _DoCreateTable(self):
        
        self.CreateTable2('caution_tbl')       
#        self.CreateTable2('temp_link_in_adminline')
#        self.CreateTable2('temp_admin_line')
#        self.CreateTable2('temp_link_caution')
 
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_get_connected_node')
        self.CreateFunction2('mid_get_caution_passlid')
        self.CreateFunction2('mid_get_caution_in_link')
        self.CreateFunction2('mid_get_caution_out_link')
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        self.CreateIndex2('caution_tbl_inlinkid_idx')    
        self.CreateIndex2('caution_tbl_nodeid_idx')
        self.CreateIndex2('caution_tbl_outlinkid_idx')
        return 0
      
    def _Do(self):
        
        #jude country UC
        # 疑问：县境案内仅在如下区域（北美）作成：美国。加拿大、波多黎各、处女群岛（美国）、墨西哥
        sqlcmd = '''
                SELECT count(*)
                FROM rdf_country
                where iso_country_code in ('USA','CAN','PRI','VIR','MEX');
                '''
              
        row = self.__GetRows(sqlcmd)
        if row[0][0] == 0:
            return 0
        
        #self.__Get_intersection_Link()
        self.__Get_link_in_adminline()
        #self.__Get_caution_link()
        self.__Get_temp_caution()
        
        self.__insert_into_temp_caution_order1()
        self.__Get_caution_link_order0()
        self.__Get_mid_admin_image_code()
        self.__insert_into_caution_tbl()
        
        return 0
    
    def __Get_link_in_adminline(self):
        
        self.log.info('start get link in adminline!')
        
        # 作成表单temp_rdf_nav_link_admin记录link与省级行政界的对照关系（link两边的行政界可能不同）
        
        sqlcmd = '''
                drop table if exists temp_rdf_nav_link_admin;
                create table temp_rdf_nav_link_admin
                as
                (
                    select a.link_id, b.one_way_code, ref_node_id, nonref_node_id, \
                        c.order1_id as left_admin, d.order1_id as right_admin
                    from temp_rdf_nav_link as a
                    left join link_tbl as b
                        on a.link_id = b.link_id
                    left join rdf_admin_hierarchy as c
                        on a.left_admin_place_id = c.admin_place_id
                    left join rdf_admin_hierarchy as d
                        on a.right_admin_place_id = d.admin_place_id
                )
                
                drop index if exists temp_rdf_nav_link_admin_left_admin_idx;
                create index temp_rdf_nav_link_admin_left_admin_idx
                    on temp_rdf_nav_link_admin
                    using btree
                    (left_admin);
                
                drop index if exists temp_rdf_nav_link_admin_right_admin_idx;
                create index temp_rdf_nav_link_admin_right_admin_idx
                    on temp_rdf_nav_link_admin
                    using btree
                    (right_admin);
                
                drop index if exists temp_rdf_nav_link_admin_link_id_idx;
                create index temp_rdf_nav_link_admin_link_id_idx
                    on temp_rdf_nav_link_admin
                    using btree
                    (link_id);
                
                drop index if exists temp_rdf_nav_link_admin_ref_node_id_idx;
                create index temp_rdf_nav_link_admin_ref_node_id_idx
                    on temp_rdf_nav_link_admin
                    using btree
                    (ref_node_id);
                
                drop index if exists temp_rdf_nav_link_admin_nonref_node_id_idx;
                create index temp_rdf_nav_link_admin_nonref_node_id_idx
                    on temp_rdf_nav_link_admin
                    using btree
                    (nonref_node_id);

                analyze temp_rdf_nav_link_admin;
            '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        # 作成表单temp_rdf_nav_node_admin记录跨界（省级行政界）node信息，作成：
        # 1、若link本身跨界，则link始终端对应node都得收录
        # 2、若node同时关联多条link，但link分属不同省级行政区，亦收录
        
        sqlcmd = '''
                drop table if exists temp_rdf_nav_node_admin;
                create table temp_rdf_nav_node_admin
                as
                (
                    select node_id
                    from
                    (
                        select a.node_id, array_agg(distinct b.left_admin) as admin_array
                        from temp_rdf_nav_node as a
                        left join temp_rdf_nav_link_admin as b
                            on a.node_id = b.ref_node_id or a.node_id = b.nonref_node_id
                        where left_admin = right_admin
                        group by a.node_id
                    )temp
                    where array_upper(admin_array,1) > 1
                    
                    union
                    
                    select unnest(array[ref_node_id,nonref_node_id]) as node_id
                    from temp_rdf_nav_link_admin
                    where left_admin <> right_admin
                );
                
                analyze temp_rdf_nav_node_admin;
            '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        # 作成表单temp_rdf_nav_link_admin_need记录跨界（省级行政界）link信息
            
        sqlcmd = '''
                drop table if exists temp_rdf_nav_link_admin_need;
                create table temp_rdf_nav_link_admin_need
                as
                (
                    select a.*
                    from temp_rdf_nav_link_admin as a
                    join temp_rdf_nav_node_admin as b
                    on b.node_id = a.ref_node_id or b.node_id = a.nonref_node_id
                );
                
                drop index if exists temp_rdf_nav_link_admin_need_link_id_idx;
                create index temp_rdf_nav_link_admin_need_link_id_idx
                    on temp_rdf_nav_link_admin_need
                    using btree
                    (link_id);
                    
                analyze temp_rdf_nav_link_admin_need;
            '''
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('end get link in adminline!')
        
    def __Get_temp_caution(self):
        
        self.log.info('start get temp_caution!')
        
        # 作成表单temp_caution_tbl_link记录跨界link的路径信息（进入link、跨界node、脱出link、脱出序列），作成：
        # 1、在表单temp_rdf_nav_link_admin_need中找到可以作为进入link的边界（省级行政界）【进入link不可与行政界重合】
        # 2、通过进入link、跨界node计算可能的脱出路线，即可得到跨界link的路径信息
        
        self.CreateFunction2('mid_get_outlink_for_admin')
        sqlcmd = '''
                drop table if exists temp_caution_tbl_link;
                create table temp_caution_tbl_link
                as (
                    select distinct inlinkid, nodeid, link_array[array_upper(link_array,1)] as outlinkid, 
                        link_array[1:array_upper(link_array,1)-1] as passlid
                    from
                    (
                        select inlinkid, nodeid, (regexp_split_to_array(link_lid,E'\\\|+'))::bigint[] as link_array
                        from
                        (
                            select b.link_id as inlinkid, a.node_id as nodeid, 
                                unnest(mid_get_outlink_for_admin(b.link_id, a.node_id, b.left_admin)) as link_lid
                            from temp_rdf_nav_node_admin as a
                            join temp_rdf_nav_link_admin_need as b
                            on (a.node_id = b.ref_node_id and b.one_way_code in (1,3))
                                or (a.node_id = b.nonref_node_id and b.one_way_code in (1,2)) 
                            where b.left_admin = b.right_admin
                        )temp
                        where link_lid is not null
                    )temp1
                );
                
                analyze temp_caution_tbl_link;
            '''
        
        self.pg.do_big_insert2(sqlcmd)
            
        self.log.info('end get temp_caution!')
    
    def __Get_intersection_Link(self):
        self.log.info('start get intersect link!')
        

        sqlcmd = '''
            insert into temp_admin_line(ad_code, the_geom)
            (
                SELECT ad_code,ST_ExteriorRing(the_geom) as the_geom
                from
                (
                    SELECT ad_code, ST_GeometryN(the_geom,generate_series(1,ST_NumGeometries(the_geom))) as the_geom
                    FROM mid_admin_zone
                    where ad_order = 1
                )temp
            );
            '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.CreateIndex2('temp_admin_line_the_geom_idx')
        

        sqlcmd = '''
            insert into temp_link_caution(link_id, s_node, e_node, one_way_code, the_geom)
            (
                select link_id, a.s_node, a.e_node, one_way_code, a.the_geom
                from link_tbl as a
                join temp_admin_line as b
                on ST_Intersects(a.the_geom, b.the_geom)
                where road_type in (0,1) and link_type in(1,2)
            );
            '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        self.log.info('end get intersect link!')
        return 0
    
    def __Get_caution_link(self):
        self.log.info('start get order1 caution link!')
        

        self.CreateTable2('temp_node_in_admin')
        self.CreateIndex2('temp_node_in_admin_node_id_idx')
        

        sqlcmd = '''
            insert into temp_caution_tbl_link(inlinkid, nodeid, outlinkid, passlid)
            (
                select distinct inlink_lid[1] as inlinkid,mid_get_connected_node(inlink_lid[1],
                     (case when array_upper(inlink_lid,1) > 1 then inlink_lid[2]
                         when passlid is null then outlink_lid[1]
                         else passlid[1] end)) as nodeid,
                    outlink_lid[array_upper(outlink_lid,1)] as outlinkid,
                    mid_get_caution_passlid(passlid,inlink_lid,outlink_lid)
                from
                (
                    select (regexp_split_to_array(unnest(inlinkid_array),E'\\\|+'))::bigint[] as inlink_lid,
                        (regexp_split_to_array(unnest(outlinkid_array),E'\\\|+'))::bigint[] as outlink_lid,
                        passlid
                    from        
                    (
                        select (case when b.node_id is null then array[link_id::varchar]
                                else mid_get_caution_in_link(link_id,s_node) end) as inlinkid_array,
                               (case when b.node_id is not null and c.node_id is null then array[link_id::varchar]
                                else mid_get_caution_out_link(link_id,e_node) end) as outlinkid_array,
                               (case when b.node_id is not null and c.node_id is not null then array[link_id]
                                else null end) as passlid               
                        from temp_link_caution as a
                        left join temp_node_in_admin as b
                        on a.s_node = b.node_id
                        left join temp_node_in_admin as c
                        on a.e_node = c.node_id
                        where one_way_code in (1,2)
                        
                        union
                        
                        select (case when b.node_id is null then array[link_id::varchar]
                                else mid_get_caution_in_link(link_id,e_node) end) as inlinkid_array,
                               (case when b.node_id is not null and c.node_id is null then array[link_id::varchar]
                                else mid_get_caution_out_link(link_id,s_node) end) as outlinkid_array,
                               (case when b.node_id is not null and c.node_id is not null then array[link_id]
                                else null end) as passlid               
                        from temp_link_caution as a
                        left join temp_node_in_admin as b
                        on a.e_node = b.node_id
                        left join temp_node_in_admin as c
                        on a.s_node = c.node_id
                        where one_way_code in (1,3)
                    )temp
                    where inlinkid_array is not null and outlinkid_array is not null
                )temp1
            );
            '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info('end get order1 caution link!')
        return 0
    
    def __Get_caution_link_order0(self):
        
        self.log.info('start get order0 caution link!')
        
        # 作成表单temp_caution_tbl_order0记录县境案内信息（跨越国家边界）
        sqlcmd = '''
            drop table if exists temp_caution_tbl_order0;
            create table temp_caution_tbl_order0
            as (
                select inlinkid, nodeid, outlinkid, passlid,
                    passlink_cnt,data_kind,b.order0_id,c.order0_id
                from temp_caution_tbl_order1 as a
                left join mid_admin_zone as b
                    on a.first_code = b.ad_code
                left join mid_admin_zone as c
                    on a.end_code = c.ad_code
                where b.order0_id <> c.order0_id  
            );
            
            analyze temp_caution_tbl_order0;
            '''
        
        self.pg.do_big_insert2(sqlcmd)
            
        self.log.info('end get order0 caution link!')
        return 0
    
    def __insert_into_temp_caution_order1(self):
        
        self.log.info('start insert into temp_caution tbl!')
        
#        sqlcmd = '''
#            insert into temp_caution_tbl_order1(inlinkid, nodeid, outlinkid, passlid,passlink_cnt,data_kind,first_code,end_code)
#            (
#                select distinct inlinkid, nodeid, outlinkid, passlid_str, passlink_cnt,4::smallint,first_code,end_code
#                from
#                (
#                    select inlinkid, nodeid, outlinkid, passlid_str, passlink_cnt,d1.ad_code as first_code,e1.ad_code as end_code
#                    from
#                    (
#                        select inlinkid, nodeid, outlinkid, passlid_str, passlink_cnt,
#                            (case when b.s_node = nodeid then b.e_node else b.s_node end) as first_node,
#                            (case when c.s_node = end_node_connect then c.e_node else c.s_node end) as end_node
#                        from
#                        (
#                            select inlinkid, nodeid, outlinkid, array_to_string(passlid,'|') as passlid_str,
#                                (case when passlid is null then 0 else array_upper(passlid,1) end) as passlink_cnt,
#                                (case when passlid is null then nodeid 
#                                    else mid_get_connected_node(passlid[array_upper(passlid,1)],outlinkid) end) as end_node_connect
#                            from temp_caution_tbl_link
#                        )a
#                        left join link_tbl as b
#                        on a.inlinkid = b.link_id
#                        left join link_tbl as c
#                        on a.outlinkid = c.link_id
#                    )temp              
#                    left join node_tbl as d
#                    on temp.first_node = d.node_id
#                    left join node_tbl as e
#                    on temp.end_node = e.node_id
#                    left join mid_admin_zone as d1
#                    on d1.ad_order = 1 and ST_Contains(d1.the_geom,d.the_geom)
#                    left join mid_admin_zone as e1
#                    on e1.ad_order = 1 and ST_Contains(e1.the_geom,e.the_geom)
#                )temp1
#                where first_code <> end_code  
#            );
#            '''
        
        # 作成表单temp_caution_tbl_order1记录县境案内信息（跨越省级边界）
        
        sqlcmd = '''
                drop table if exists temp_caution_tbl_order1;
                create table temp_caution_tbl_order1
                as (
                    select inlinkid, nodeid, outlinkid, passlid::varchar as passlid, 
                        (case when passlid is null then 0 else array_upper(passlid,1) end)::smallint as passlink_cnt, 
                        4::smallint as data_kind, b.left_admin as first_code, c.left_admin as end_code
                    from temp_caution_tbl_link as a
                    left join temp_rdf_nav_link_admin_need as b
                        on a.inlinkid = b.link_id and (b.left_admin = b.right_admin)
                    left join temp_rdf_nav_link_admin_need as c
                        on a.outlinkid = c.link_id and (c.left_admin = c.right_admin)
                    where b.link_id is not null and c.link_id is not null
                );
                
                analyze temp_caution_tbl_order1;
            '''
            
        self.pg.do_big_insert2(sqlcmd)
            
        self.log.info('end insert into temp_caution tbl!')
        
        return 0
    
    def __Get_mid_admin_image_code(self):
        
        self.log.info('start insert into mid_admin_image_code!')
        
        # 作成表单mid_admin_image_code记录县境案内对应的图片信息（暂时没有用到）
        
        sqlcmd = '''
            drop table if exists mid_admin_image_code;
            create table mid_admin_image_code
            as (
                select first_code::bigint as incode, end_code::bigint as outcode, null::bigint as image_id
                from temp_caution_tbl_order0
                
                union
                
                select first_code::bigint as incode, end_code::bigint as outcode, null::bigint as image_id
                from temp_caution_tbl_order1 
            );
            
            analyze mid_admin_image_code;
            '''
        
        self.pg.do_big_insert2(sqlcmd)
            
        self.log.info('end insert into mid_admin_image_code!')
        
        return 0
    
    def __insert_into_caution_tbl(self):
        
        self.log.info('start insert into caution tbl!')
        
        # 更新表单caution_tbl
        # 将县境案内（跨国、跨省案内）更新到表单caution_tbl
        
        sqlcmd = '''
            insert into caution_tbl(inlinkid, nodeid, outlinkid, passlid,passlink_cnt,data_kind,strtts)
            (
                select inlinkid, nodeid, outlinkid, passlid,passlink_cnt,data_kind,b.ad_name
                from temp_caution_tbl_order0 a
                left join mid_admin_zone b
                    on a.end_code = b.ad_code
                
                union
                
                select a.inlinkid, a.nodeid, a.outlinkid, a.passlid,a.passlink_cnt,a.data_kind,c.ad_name
                from temp_caution_tbl_order1 a
                left join temp_caution_tbl_order0 b
                    on 
                        a.inlinkid = b.inlinkid and a.nodeid = b.nodeid and 
                        a.outlinkid = b.outlinkid and a.passlid = b.passlid
                left join mid_admin_zone c
                    on a.end_code = c.ad_code
                where b.inlinkid is null                                        
            );
            '''  
          
        self.pg.do_big_insert2(sqlcmd)
            
        self.log.info('end insert into caution tbl!')
        
        return 0
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
    
    def _DoCheckValues(self):
        self.log.info('start CheckValues!')
        sqlcmd = '''    
                select strtts
                from caution_tbl as a
                left join mid_admin_zone as b
                on a.strtts = b.ad_name and (b.ad_order in (0,1))
                where b.ad_code is null and a.data_kind = 4;
             '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            for row in rows:              
                self.log.error('caution_tbl voice_id is not found in admin %s',row[0])
            return -1
            
        self.log.info('end CheckValues!')
        return 0
    
    def _DoCheckLogic(self):
        self.log.info('start CheckLogic!')
        
        sqlcmd = '''    
                select d.ad_code,end_code
                from
                (
                    select (case when s_node = nodeid then e_node else s_node end) as start_id,voice_id as end_code
                    from caution_tbl as a
                    left join link_tbl as b
                    on a.inlinkid = b.link_id
                    where a.data_kind = 4
                )temp
                left join node_tbl as c
                on start_id = c.node_id
                left join mid_admin_zone as d
                on d.ad_order = 1 and ST_Contains(d.the_geom,c.the_geom)
                where ad_code = end_code
                
                union
                
                select d.ad_code,end_code
                from
                (
                    select (case when s_node = nodeid then e_node else s_node end) as start_id,voice_id as end_code
                    from caution_tbl as a
                    left join link_tbl as b
                    on a.inlinkid = b.link_id
                    where a.data_kind = 4
                )temp
                left join node_tbl as c
                on start_id = c.node_id
                left join mid_admin_zone as d
                on d.ad_order = 0 and ST_Contains(d.the_geom,c.the_geom)
                where ad_code = end_code;
                
             '''
#        rows = self.__GetRows(sqlcmd)
#        if rows:
#            for row in rows:               
#                self.log.error('caution_tbl voice_id is not found in admin %s and %s',row[0],row[1])
#            return -1
#        
#        self.log.info('end CheckLogic!')
        return 0
    

