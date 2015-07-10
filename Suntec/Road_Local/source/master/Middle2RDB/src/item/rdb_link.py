# -*- coding: cp936 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common
import codecs
from item import rdb_link_sequence


class rdb_link(ItemBase):
    '''
    rdb_link类
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Link'
                          , 'link_tbl'
                          , 'gid'
                          , 'rdb_link'
                          , 'gid')

    def Do_CreateTable(self):
        return self.CreateTable2('rdb_link')

    def Do_CreateFunction(self):
        'Create Function'
        if self.CreateFunction2('rdb_cnv_road_disp_class') == -1:
            return -1
        # if self.CreateFunction2('rdb_cnv_road_type') == -1:
        #    return -1
        # if self.CreateFunction2('rdb_cnv_link_type') == -1:
        #    return -1
        # if self.CreateFunction2('rdb_cnv_link_toll') == -1:
        #    return -1
        # if self.CreateFunction2('rdb_cnv_road_function_code') == -1:
        #    return -1
        if self.CreateFunction2('rdb_cnv_road_direction') == -1:
            return -1
        if self.CreateFunction2('rdb_conn_linkid') == -1:
            return -1
        if self.CreateFunction2('rdb_auto_simplify_geom') == -1:
            return -1
        if self.CreateFunction2('rdb_cnv_length') == -1:
            return -1

        return 0

    def Do(self):
        self.__make_rdb_name()
        self.__make_rdb_link()        
        self.__make_sequence()
        return 0 
    
    def __make_rdb_name(self):    
        rdb_log.log('rdb_name', 'Start make rdb_name.', 'info')
        self.CreateTable2('temp_rdb_name')
        
        sqlcmd = """
                insert into temp_rdb_name
                     (
                        select   unnest(array_agg(link_id)) as link_id,
                                row_number() over(order by road_name ) as name_id,                        
                                road_name 
                        from  link_tbl
                        where road_name is not null
                        group by road_name
                       -- union                        
                        --select    link_id,
                        --          0 as name_id,                        
                           --       road_name 
                        --from      link_tbl
                       -- where road_name is null
                        order by name_id
                       ) 
                 """                                    
        self.pg.execute2(sqlcmd)
        self.pg.commit2()        
        self.CreateIndex2('temp_rdb_name_link_id_idx')        
                   
        self.CreateTable2('rdb_name')       
        sqlcmd = """
                    insert into rdb_name
                    (
                    select distinct name_id, name
                    from temp_rdb_name
                    order by name_id
                    )
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # create index
#        self.CreateIndex2('temp_rdb_name_name_idx')
        self.CreateIndex2('rdb_name_name_id_idx')
        rdb_log.log('rdb_name', 'End make rdb_name', 'info')        
        return 0
    
    def __make_rdb_link(self):
        sqlcmd = """
           INSERT INTO rdb_link(
                link_id, link_id_t, iso_country_code, display_class, start_node_id, start_node_id_t, end_node_id, end_node_id_t,
                road_type, one_way, function_code, link_length, road_name, road_name_id, road_number,
                lane_id, toll, regulation_flag, tilt_flag, speed_regulation_flag,
                link_add_info_flag, fazm, tazm, fnlink, fnlink_t, tnlink, tnlink_t,
                link_type, extend_flag, fazm_path, tazm_path, shield_flag, bypass_flag, matching_flag, 
                highcost_flag, the_geom, the_geom_900913)
                (
                 SELECT link_id, link_id_t, iso_country_code, display_class, s_node_id, start_node_id_t, end_node_id, end_node_id_t,
                      road_type,one_way_code, function_class, link_length, road_name, 
                      (case when road_name_id is null then 0 
                      else road_name_id end) as road_name_id, 
                      road_number, lane_id,toll,
                      regulation_flag, false, speed_regulation_flag, link_add_info_flag,
                      fazm, tazm, fnlink, FN.tile_id, tnlink, TN.tile_id, link_type,
                      extend_flag, fazm_path, tazm_path, shield_flag, bypass_flag, matching_flag, highcost_flag,
                      the_geom, the_geom_900913
                 from (
                     SELECT  tile_link_id as link_id                        -- link_id
                           , rdb_tile_link.tile_id as  link_id_t           -- link_id_t
                           , road_name_id
                           , b.iso_country_code as iso_country_code        -- iso_country_code  
                           , display_class                                -- dipslay class
                           , s_node_id                                 -- start_node_id
                           , start_node_id_t                           -- start_tile_id
                           , end_node_id                                   -- end_node_id
                           , end_node_id_t                                 -- end_tile_id
                           , road_type                                     -- road type
                           , one_way_code                                  -- one_way
                           , function_class                                -- function_code
                           , b.length  as link_length                      -- link_length
                           , (case when ln.lane_id is null then -1 else ln.lane_id end)    -- lane_id
                           , b.toll                                        -- toll
                           , (d.link_id is not null) as  regulation_flag   -- regulation_flag
                           , speed_regulation_flag  -- speed_regulation_flag
                           , (e.link_id is not null) as  link_add_info_flag-- link_add_info_flag
                           , fazm                                                   -- fazm
                           , tazm                                                   -- tazm
                           , rdb_conn_linkid(tile_link_id, start_conn_links) fnlink  -- fnlink
                           , rdb_conn_linkid(tile_link_id, end_conn_links) tnlink    -- tnlink
                           , link_type as link_type                                 -- link_type
                           , extend_flag as extend_flag
                           , road_name
                           , road_number
                           , fazm as fazm_path
                           , tazm as tazm_path
                           , (road_number is not null) as shield_flag
                           , bypass_flag 
                           , matching_flag
                           , highcost_flag
                           , ST_LineMerge(the_geom) as the_geom
                           , st_transform(ST_LineMerge(the_geom),3857) as the_geom_900913
                      FROM (
                             SELECT       link_tbl.link_id as old_link_id
                                          , temp_rdb_name.name_id as road_name_id
                                          , iso_country_code
                                          , display_class
                                          , link_type
                                          , road_type
                                          , (case when one_way_code = 4 then 0 else one_way_code end) as one_way_code
                                          , toll
                                          , s_c.node_id as s_node_id
                                          , s_c.tile_id as start_node_id_t
                                          , e_c.node_id as end_node_id
                                          , e_c.tile_id as end_node_id_t
                                          , function_class
                                          , rdb_cnv_length(length) as length
                                          , ownership
                                          , not (speed_limit_s2e = 0 and speed_limit_e2s = 0) and not (speed_limit_s2e is null and speed_limit_e2s is null) as speed_regulation_flag
                                          , s_c.branches as start_conn_links
                                          , e_c.branches as end_conn_links
                                          , elevated
                                          , extend_flag
                                          , bypass_flag
                                          , matching_flag
                                          , highcost_flag 
                                          , road_name
                                          , road_number
                                          , fazm
                                          , tazm
                                          , the_geom
                                     FROM link_tbl
                                     left join temp_rdb_name
                                     on temp_rdb_name.link_id = link_tbl.link_id
                                     -- Get all connected links of a start node
                                     LEFT JOIN (
                                             -- Get all connectend links of a node
                                             SELECT node_id, tn.tile_id, tn.old_node_id, branches
                                              FROM  rdb_tile_node as tn
                                              LEFT JOIN rdb_node
                                              ON rdb_node.node_id = tn.tile_node_id
                                        ) as s_c
                                     ON s_node = s_c.old_node_id
                                     -- Get all connected links of a end node
                                     LEFT JOIN (
                                             -- Get all connectend links of a node
                                             SELECT node_id, tn.tile_id, tn.old_node_id, branches
                                              FROM rdb_tile_node as tn
                                              LEFT JOIN rdb_node
                                              ON rdb_node.node_id = tn.tile_node_id
                                        ) as e_c
                                     ON e_node = e_c.old_node_id
                            ) as b
                            left join rdb_tile_link
                            ON rdb_tile_link.old_link_id = b.old_link_id
                            LEFT JOIN temp_rdb_linklane_info as ln
                            ON ln.old_link_id = b.old_link_id
                            LEFT JOIN (
                                        SELECT distinct unnest(string_to_array(key_string, ',')::bigint[]) as link_id
                                        FROM rdb_link_regulation
                            ) as d
                            ON rdb_tile_link.tile_link_id = d.link_id
                            LEFT JOIN (
                                     SELECT distinct(link_id) as link_id
                                             from  rdb_link_add_info
                            ) as e
                            ON rdb_tile_link.tile_link_id = e.link_id
                            ) as A
                            LEFT JOIN rdb_tile_link as FN
                            ON fnlink = FN.tile_link_id
                            left join rdb_tile_link as TN
                            ON tnlink = TN.tile_link_id
                            ORDER BY link_id
                );
                """

        rdb_log.log(self.ItemName, 'Now it is inserting to rdb_link...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        self.CreateIndex2('rdb_link_link_id_idx')
        self.CreateIndex2('rdb_link_the_geom_idx')
        self.CreateIndex2('rdb_link_start_node_id_idx')
        self.CreateIndex2('rdb_link_end_node_id_idx')
        self.CreateIndex2('rdb_link_link_id_t_idx') 
        return 0  
    
    def __make_sequence(self):
        
        rdb_log.log(self.ItemName, 'Now it is inserting to rdb_link_sequence...', 'info')
        pg_sequence = rdb_link_sequence.rdb_linksequence_info(self,'rdb_link',
                                                              'rdb_node',                                                              
                                                              'temp_link_sequence_dir',
                                                              'rdb_link_sequence',                                                             
                                                              'rdb_link_regulation')
        if pg_sequence._CreateTable() == -1:
            return -1      
        if pg_sequence._Get_road_sequence() == -1:
            return -1
        
        return 0    

    def Do_CreatIndex(self):
        '创建相关表索引.'
#        self.CreateIndex2('rdb_link_link_id_idx')
#        self.CreateIndex2('rdb_link_the_geom_idx')
#        self.CreateIndex2('rdb_link_start_node_id_idx')
#        self.CreateIndex2('rdb_link_end_node_id_idx')
#        self.CreateIndex2('rdb_link_link_id_t_idx')
        return 0

    def _DoCheckValues(self):
        'check字段值'
        sqlcmd = """
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_toll;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_toll CHECK (toll = ANY (ARRAY[0, 1, 2, 3]));

                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_display_class;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_display_class CHECK (display_class = ANY (ARRAY[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,30,31]));

                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_road_type;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_road_type CHECK (road_type = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]));

                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_link_type;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_link_type CHECK (link_type = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]));

                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_function_code;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_function_code CHECK (function_code = ANY (ARRAY[1, 2, 3, 4, 5]));
                """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0

    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                -- Create FOREIGN KEY
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_end_node_id_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_end_node_id_fkey FOREIGN KEY (end_node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_lane_id_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_lane_id_fkey FOREIGN KEY (lane_id)
                      REFERENCES rdb_linklane_info (lane_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_start_node_id_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_start_node_id_fkey FOREIGN KEY (start_node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_fnlink_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_fnlink_fkey FOREIGN KEY (fnlink)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_tnlink_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_tnlink_fkey FOREIGN KEY (tnlink)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                --ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_road_name_id_fkey;
                --ALTER TABLE rdb_link
                --  ADD CONSTRAINT rdb_link_road_name_id_fkey FOREIGN KEY (road_name_id)
                --     REFERENCES rdb_name_dictionary (name_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        #sqlcmd = self.pg.ReplaceDictionary(sqlcmd)

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0

class rdb_link_simplify_axf_china(ItemBase):
    def __init__(self):
        ItemBase.__init__(self, 'Link_Simplify')

    def Do(self):
        # backup
        rdb_log.log(self.ItemName, 'backup rdb_link...', 'info')
        sqlcmd = """
                    drop table if exists temp_simplify_rdb_link_backup;
                    create table temp_simplify_rdb_link_backup
                    as
                    select * from rdb_link;
                    create index temp_simplify_rdb_link_backup_link_id_idx
                        on temp_simplify_rdb_link_backup
                        using btree
                        (link_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # simplify 900913
        rdb_log.log(self.ItemName, 'simplify 900913...', 'info')
        sqlcmd = """
                    drop table if exists temp_simplify_rdb_link_with_simple_geom_900913;
                    create table temp_simplify_rdb_link_with_simple_geom_900913
                    as
                    select link_id, ST_SimplifyPreserveTopology(the_geom_900913,1) as the_geom_900913
                    from temp_simplify_rdb_link_backup;
                    create index temp_simplify_rdb_link_with_simple_geom_900913_link_id_idx
                        on temp_simplify_rdb_link_with_simple_geom_900913
                        using btree
                        (link_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # change 4326
        rdb_log.log(self.ItemName, 'simplify 4326...', 'info')
        sqlcmd = """
                    drop table if exists temp_simplify_rdb_link_with_simple_geom_4326;
                    create table temp_simplify_rdb_link_with_simple_geom_4326
                    as
                    (
                        select  a.link_id,
                                st_setpoint(st_setpoint(b.the_geom, 0, st_startpoint(a.the_geom)),
                                            st_numpoints(b.the_geom)-1,
                                            st_endpoint(a.the_geom)
                                )as the_geom
                        from temp_simplify_rdb_link_backup as a
                        left join
                        (
                            select link_id, st_transform(the_geom_900913,4326) as the_geom
                            from temp_simplify_rdb_link_with_simple_geom_900913
                        )as b
                        on a.link_id = b.link_id
                    );
                    create index temp_simplify_rdb_link_with_simple_geom_4326_link_id_idx
                        on temp_simplify_rdb_link_with_simple_geom_4326
                        using btree
                        (link_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # update rdb_link
        rdb_log.log(self.ItemName, 'update rdb_link...', 'info')
        self.CreateTable2('rdb_link')
        sqlcmd = """
                INSERT INTO rdb_link(
                    link_id, link_id_t, iso_country_code, display_class, start_node_id, start_node_id_t, 
                    end_node_id, end_node_id_t, road_type, one_way, function_code, 
                    link_length, road_name,  road_name_id, road_number, lane_id, toll, regulation_flag, 
                    tilt_flag, speed_regulation_flag, link_add_info_flag, fazm, tazm, 
                    fnlink, fnlink_t, tnlink, tnlink_t, link_type, extend_flag,  
                    bypass_flag, matching_flag, highcost_flag, fazm_path, tazm_path, 
                    shield_flag, the_geom, the_geom_900913)
                (
                select  a.link_id, link_id_t, display_class, start_node_id, start_node_id_t, 
                        end_node_id, end_node_id_t, road_type, one_way, function_code, 
                        link_length, road_name, road_name_id, road_number, lane_id, toll, regulation_flag, 
                        tilt_flag, speed_regulation_flag, link_add_info_flag, fazm, tazm, 
                        fnlink, fnlink_t, tnlink, tnlink_t, link_type, extend_flag,
                        bypass_flag, matching_flag, highcost_flag, fazm_path, tazm_path, 
                        shield_flag, b.the_geom, c.the_geom_900913
                from temp_simplify_rdb_link_backup as a
                left join temp_simplify_rdb_link_with_simple_geom_4326 as b
                on a.link_id = b.link_id
                left join temp_simplify_rdb_link_with_simple_geom_900913 as c
                on a.link_id = c.link_id
                order by a.link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # simplify link shape points when it's count over 1024
        sqlcmd = """
            update rdb_link as A
            set the_geom = B.new_the_geom
            from
            (
                select link_id, rdb_auto_simplify_geom(the_geom) as new_the_geom
                from rdb_link where st_npoints(the_geom) > 1023
            ) as B
            where A.link_id = B.link_id;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('rdb_link_link_id_idx')
        self.CreateIndex2('rdb_link_the_geom_idx')
        self.CreateIndex2('rdb_link_start_node_id_idx')
        self.CreateIndex2('rdb_link_end_node_id_idx')
        self.CreateIndex2('rdb_link_link_id_t_idx')

    def _DoCheckLogic(self):
        sqlcmd = """
                    select count(*)
                    from rdb_link as a
                    left join rdb_node as b
                    on a.start_node_id = b.node_id
                    left join rdb_node as c
                    on a.end_node_id = c.node_id
                    where  not st_equals(st_startpoint(a.the_geom), b.the_geom)
                        or not st_equals(st_endpoint(a.the_geom), c.the_geom);
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] > 0:
            raise Exception, 'simplified geom error.'

class rdb_shape_optimize(ItemBase):
    def __init__(self):
        ItemBase.__init__(self, 'Shape_Optimize')

    def Do(self):
        self._prepare()
        self._simplify()
        self._transform_4096()
        self._optimizeNode()
        self._updateShape()
    
    def _prepare(self):
        self.CreateFunction2('lonlat2pixel_tile')
        self.CreateFunction2('lonlat2pixel')
        self.CreateFunction2('tile_bbox')
        self.CreateFunction2('world2lonlat')
        self.CreateFunction2('lonlat2world')
        self.CreateFunction2('pixel2world')
        
        self.log.info('backup rdb_link/rdb_node...')
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_link_backup;
                    create table temp_optimize_rdb_link_backup
                    as
                    select * from rdb_link;
                    create index temp_optimize_rdb_link_backup_link_id_idx
                        on temp_optimize_rdb_link_backup
                        using btree
                        (link_id);
                    create index temp_optimize_rdb_link_backup_start_node_id_idx
                        on temp_optimize_rdb_link_backup
                        using btree
                        (start_node_id);
                    create index temp_optimize_rdb_link_backup_end_node_id_idx
                        on temp_optimize_rdb_link_backup
                        using btree
                        (end_node_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_node_backup;
                    create table temp_optimize_rdb_node_backup
                    as
                    select * from rdb_node;
                    create index temp_optimize_rdb_node_backup_link_id_idx
                        on temp_optimize_rdb_node_backup
                        using btree
                        (node_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _simplify(self):
        # simplify 900913
        self.log.info('simplify 900913...')
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_link_simplify_900913;
                    create table temp_optimize_rdb_link_simplify_900913
                    as
                    select link_id, ST_SimplifyPreserveTopology(the_geom_900913,1) as the_geom_900913
                    from temp_optimize_rdb_link_backup;
                    create index temp_optimize_rdb_link_simplify_900913_link_id_idx
                        on temp_optimize_rdb_link_simplify_900913
                        using btree
                        (link_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # simplify link shape points when its count is over 1024...
        self.log.info('simplify link shape points when its count is over 1024...')
        self.CreateFunction2('rdb_auto_simplify_geom_900913')
        sqlcmd = """
            update temp_optimize_rdb_link_simplify_900913 as A
            set the_geom_900913 = B.the_geom_900913
            from
            (
                select link_id, rdb_auto_simplify_geom(the_geom_900913) as the_geom_900913
                from temp_optimize_rdb_link_simplify_900913 
                where st_npoints(the_geom_900913) > 1023
            ) as B
            where A.link_id = B.link_id;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # change 4326
        self.log.info('change 4326 from simplify 900913...')
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_link_simplify_4326;
                    create table temp_optimize_rdb_link_simplify_4326
                    as
                    (
                        select  a.gid,
                                a.link_id,
                                a.link_id_t,
                                a.start_node_id,
                                a.end_node_id,
                                st_setpoint(st_setpoint(b.the_geom, 0, st_startpoint(a.the_geom)),
                                            st_numpoints(b.the_geom)-1,
                                            st_endpoint(a.the_geom)
                                )as the_geom
                        from temp_optimize_rdb_link_backup as a
                        left join
                        (
                            select link_id, st_transform(the_geom_900913,4326) as the_geom, the_geom_900913
                            from temp_optimize_rdb_link_simplify_900913
                        )as b
                        on a.link_id = b.link_id
                    );
                    create index temp_optimize_rdb_link_simplify_4326_gid_idx
                        on temp_optimize_rdb_link_simplify_4326
                        using btree
                        (gid);
                    create index temp_optimize_rdb_link_simplify_4326_link_id_idx
                        on temp_optimize_rdb_link_simplify_4326
                        using btree
                        (link_id);
                    create index temp_optimize_rdb_link_simplify_4326_start_node_id_idx
                        on temp_optimize_rdb_link_simplify_4326
                        using btree
                        (start_node_id);
                    create index temp_optimize_rdb_link_simplify_4326_end_node_id_idx
                        on temp_optimize_rdb_link_simplify_4326
                        using btree
                        (end_node_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _transform_4096(self):
        self.log.info('transform 4096...')
        
        self.CreateFunction2('rdb_move_point_pixel')
        self.CreateFunction2('rdb_transform_4326_to_4096')
        
        # link
        self.log.info('transform 4096 for link...')
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_link_simplify_4096;
                    create table temp_optimize_rdb_link_simplify_4096
                    (
                        link_id bigint,
                        the_geom geometry
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_optimize_rdb_link_simplify_4096
                    (
                        select  link_id,
                                rdb_transform_4326_to_4096(the_geom) as the_geom
                        from temp_optimize_rdb_link_simplify_4326
                        where gid >= %s and gid <= %s
                    );
                """
        (min_gid, max_gid) = self.pg.getMinMaxValue('temp_optimize_rdb_link_simplify_4326', 'gid')
        self.pg.multi_execute(sqlcmd, min_gid, max_gid, 4, 100000, self.log)
        
        sqlcmd = """
                    create index temp_optimize_rdb_link_simplify_4096_link_id_idx
                        on temp_optimize_rdb_link_simplify_4096
                        using btree
                        (link_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # node
        self.log.info('transform 4096 for node...')
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_node_simplify_4096;
                    create table temp_optimize_rdb_node_simplify_4096
                    (
                        node_id bigint,
                        the_geom geometry
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_optimize_rdb_node_simplify_4096
                    (
                        select  node_id,
                                rdb_transform_4326_to_4096(the_geom) as the_geom
                        from temp_optimize_rdb_node_backup
                        where gid >= %s and gid <= %s
                    );
                """
        (min_gid, max_gid) = self.pg.getMinMaxValue('temp_optimize_rdb_node_backup', 'gid')
        self.pg.multi_execute(sqlcmd, min_gid, max_gid, 4, 100000, self.log)
        
        sqlcmd = """
                    create index temp_optimize_rdb_node_simplify_4096_node_id_idx
                        on temp_optimize_rdb_node_simplify_4096
                        using btree
                        (node_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def _optimizeNode(self):
        self.log.info('Optimize node shape...')
        self.__findShortLink()
        self.__calculateNewNodePosition()
        self.__moveNode()
    
    def __findShortLink(self):
        self.log.info('Optimize node shape: find short link...')
        
        # find short link
        self.CreateTable2('temp_optimize_shortlink')
        self.CreateIndex2('temp_optimize_shortlink_link_id_idx')
        self.CreateIndex2('temp_optimize_shortlink_start_node_id_idx')
        self.CreateIndex2('temp_optimize_shortlink_end_node_id_idx')
        
        # find close short link, and set them as a group
        self.CreateTable2('temp_optimize_related_nodes')
        self.CreateFunction2('rdb_optimize_find_related_nodes')
        self.pg.callproc('rdb_optimize_find_related_nodes')
        self.CreateIndex2('temp_optimize_related_nodes_node_id_idx')
        
        # find group info
        self.CreateFunction2('rdb_get_angle_for_geom')
        self.CreateFunction2('rdb_get_angle_for_4096')
        self.CreateFunction2('rdb_get_angle_shape_point')
        self.CreateFunction2('rdb_get_node_index_array')
        self.CreateTable2('temp_optimize_node_group')
        self.CreateIndex2('temp_optimize_node_group_group_id_idx')
    
    def __calculateNewNodePosition(self):
        self.log.info('Optimize node shape: calculate new position for vertex of short link...')
        
        self.CreateFunction2('rdb_get_average')
        self.CreateFunction2('rdb_get_maximum')
        self.CreateFunction2('rdb_optimize_get_try_array')
        self.CreateFunction2('rdb_optimize_check_try_array')
        self.CreateFunction2('rdb_calc_move_cost')
        self.CreateFunction2('rdb_optimize_node_to_best_position')
        
        self.CreateTable2('temp_optimize_try_all')
        self.CreateIndex2('temp_optimize_try_all_node_num_idx')
        self.CreateIndex2('temp_optimize_try_all_try_index_idx')
        
        self.CreateTable2('temp_optimize_node_new_position')
        self.pg.callproc('rdb_optimize_node_to_best_position')
        self.CreateIndex2('temp_optimize_node_new_position_node_id_idx')
        self.pg.commit2()
    
    def __moveNode(self):
        self.log.info('Optimize node shape: move node...')
        
        sqlcmd = """
                    drop table if exists temp_optimize_rdb_node_4096;
                    create table temp_optimize_rdb_node_4096
                    as
                    select  a.node_id, 
                            (case when b.node_id is not null then b.the_geom else a.the_geom end) as the_geom_4096
                    from temp_optimize_rdb_node_simplify_4096 as a
                    left join temp_optimize_node_new_position as b
                    on a.node_id = b.node_id;
                    create index temp_optimize_rdb_node_4096_node_id_idx
                        on temp_optimize_rdb_node_4096
                        using btree
                        (node_id);
                    
                    drop table if exists temp_optimize_rdb_link_4096;
                    create table temp_optimize_rdb_link_4096
                    as
                    select  link_id, the_geom_4096, 
                            rdb_cnv_length(ST_Length_Spheroid(the_geom_4096,'SPHEROID("WGS_84", 6378137, 298.257223563)')) as link_length
                    from
                    (
                        select  a.link_id, 
                                (
                                case 
                                when b.node_id is null and c.node_id is null then a.the_geom
                                when c.node_id is null then st_setpoint(a.the_geom, 0, b.the_geom)
                                when b.node_id is null then st_setpoint(a.the_geom, st_numpoints(a.the_geom)-1, c.the_geom)
                                else st_setpoint(st_setpoint(a.the_geom, 0, b.the_geom), st_numpoints(a.the_geom)-1, c.the_geom)
                                end
                                )as the_geom_4096
                        from temp_optimize_rdb_link_backup as t
                        left join temp_optimize_rdb_link_simplify_4096 as a
                        on t.link_id = a.link_id
                        left join temp_optimize_node_new_position as b
                        on t.start_node_id = b.node_id
                        left join temp_optimize_node_new_position as c
                        on t.end_node_id = c.node_id
                    )as t;
                    create index temp_optimize_rdb_link_4096_link_id_idx
                        on temp_optimize_rdb_link_4096
                        using btree
                        (link_id);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def _updateShape(self):
        # update rdb_link
        self.log.info('update rdb_link...')
        
        self.CreateTable2('rdb_link')
        self.pg.execute2("SELECT AddGeometryColumn('','rdb_link','the_geom_4096','4326','LINESTRING',2);")
        self.pg.commit2()
        
        sqlcmd = """
                INSERT INTO rdb_link(
                    link_id, link_id_t, iso_country_code, display_class, start_node_id, start_node_id_t, 
                    end_node_id, end_node_id_t, road_type, one_way, function_code, 
                    link_length, road_name, road_name_id, road_number, lane_id, toll, regulation_flag, 
                    tilt_flag, speed_regulation_flag, link_add_info_flag, fazm, tazm, 
                    fnlink, fnlink_t, tnlink, tnlink_t, link_type, extend_flag, 
                    bypass_flag, matching_flag, highcost_flag, fazm_path, tazm_path, shield_flag,
                    the_geom, the_geom_900913, the_geom_4096)
                (
                select  a.link_id, a.link_id_t, a.iso_country_code, display_class, a.start_node_id, start_node_id_t, a.end_node_id, end_node_id_t,
                        road_type, one_way, function_code, 
                        d.link_length, road_name, road_name_id, road_number, lane_id, toll, regulation_flag, 
                        tilt_flag, speed_regulation_flag, link_add_info_flag, fazm, tazm, 
                        fnlink, fnlink_t, tnlink, tnlink_t, link_type, extend_flag, 
                        bypass_flag, matching_flag, highcost_flag, fazm_path, tazm_path, shield_flag,
                        b.the_geom, c.the_geom_900913, d.the_geom_4096
                from temp_optimize_rdb_link_backup as a
                left join temp_optimize_rdb_link_simplify_4326 as b
                on a.link_id = b.link_id
                left join temp_optimize_rdb_link_simplify_900913 as c
                on a.link_id = c.link_id
                left join temp_optimize_rdb_link_4096 as d
                on a.link_id = d.link_id
                order by a.link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_link_link_id_idx')
        self.CreateIndex2('rdb_link_the_geom_idx')
        #self.CreateIndex2('rdb_link_the_geom_4096_idx')
        self.CreateIndex2('rdb_link_start_node_id_idx')
        self.CreateIndex2('rdb_link_end_node_id_idx')
        self.CreateIndex2('rdb_link_link_id_t_idx')
        
        
        # update rdb_node
        self.log.info('update rdb_node...')
        
        self.CreateTable2('rdb_node')
        self.pg.execute2("SELECT AddGeometryColumn('','rdb_node','the_geom_4096','4326','POINT',2);")
        self.pg.commit2()
        
        sqlcmd = """
                INSERT INTO rdb_node
                select  a.*, b.the_geom_4096
                from temp_optimize_rdb_node_backup as a
                left join temp_optimize_rdb_node_4096 as b
                on a.node_id = b.node_id
                order by a.node_id
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_node_node_id_idx')
        self.CreateIndex2('rdb_node_the_geom_idx')
        #self.CreateIndex2('rdb_node_the_geom_4096_idx')
        self.CreateIndex2('rdb_node_node_id_t_idx')

    def _DoCheckLogic(self):
        self.log.info('check node lonlat...')
        sqlcmd = """
                    select count(*)
                    from rdb_link as a
                    left join rdb_node as b
                    on a.start_node_id = b.node_id
                    left join rdb_node as c
                    on a.end_node_id = c.node_id
                    where  not st_equals(st_startpoint(a.the_geom), b.the_geom)
                        or not st_equals(st_endpoint(a.the_geom), c.the_geom);
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] > 0:
            raise Exception, 'simplified geom error.'
        
        self.log.info('check short link...')
        sqlcmd = """
                    select count(*)
                    from rdb_link
                    where link_length < 5 and st_equals(st_startpoint(the_geom_4096), st_endpoint(the_geom_4096));
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] > 0:
            raise Exception, 'short link geom error.'

class rdb_link_wellshape_handle(ItemBase):
    '''
    classdocs
    '''
    @staticmethod
    def instance():
        proj_mapping = {
            ('jpn'):                rdb_link_wellshape_handle(),
            ('jdb'):                rdb_link_wellshape_handle(),
            ('axf'):                rdb_link_wellshape_handle(),
            ('ta'):                 rdb_link_wellshape_handle(),
            ('rdf'):                rdb_link_wellshape_handle_rdf(),
            ('nostra'):             rdb_link_wellshape_handle(),
            ('mmi'):                rdb_link_wellshape_handle(),
            ('msm'):                rdb_link_wellshape_handle(),
            ('ni'):                 rdb_link_wellshape_handle(),
        }
        return rdb_common.getItem(proj_mapping)
    
    def __init__(self, item_name='Wellshape_Handle_Dummy'):
        '''
        Constructor
        '''
        ItemBase.__init__(self, item_name)

#handle with wellshape innerlink of rdf data
class rdb_link_wellshape_handle_rdf(rdb_link_wellshape_handle):
    def __init__(self):
        rdb_link_wellshape_handle.__init__(self, 'Wellshape_Handle')

    def Do(self):
        # 得到“井”字路口与快捷道相交的innerlink
        self._get_wellshape_innerlink()
        self._change_rdb_link_linktype()
        return 0

    def _get_wellshape_innerlink(self):
        self.CreateTable2('wellshape_start_end_node_rdb_tbl')
        self.CreateTable2('wellshape_innerlink_rdb_tbl')
        file = codecs.open("../sql_script/wellshap_handler.sql", encoding = 'utf-8')
        sqlcmd = file.read()
        file.close()
        sqlcmd = self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _change_rdb_link_linktype(self):
        sql_cmd = '''
            UPDATE rdb_link
            SET link_type=2
            WHERE link_id in (
                SELECT innerlink::bigint
                FROM wellshape_innerlink_rdb_tbl
            );
        '''
        self.pg.execute2(sql_cmd)
        self.pg.commit2()
        return 0
