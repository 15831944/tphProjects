# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import os
from base.rdb_ItemBase import ItemBase
from common import rdb_log
import common.rdb_common

class rdb_tile(ItemBase):
    '''
    Tile
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Tile')
    
    def Do(self):
        self.__getIDFund()
        self.__makeTileList()
        self.__makeTileNode()
        self.__makeTileLink()
        self.__updateIDFund()
        return 0
    
    def __makeTileList(self):
        self.log.info('make tile list table...')
        sqlcmd = """
                drop table if exists temp_tile_list;
                create table temp_tile_list 
                as
                select  tile_id, 
                        (tile_id >> 14) & 16383 as tx, tile_id & 16383 as ty, 14 as tz
                from
                (
                    select tile_id from node_tbl
                    union
                    select tile_id from link_tbl
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def __makeTileNode(self):
        self.log.info('Start make tile nodes...')
        
        #
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_tile_id_idx')
        self.CreateIndex2('node_tbl_feature_key_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        
        #
        self.CreateTable2('rdb_tile_node')
        
        self.log.info('dispatch node id by feature_key...')
        sqlcmd = """
                    insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                    select b.node_id, a.node_id, b.tile_id, b.seq_num
                    from node_tbl as a
                    left join id_fund_node as b
                    on a.feature_key = b.feature_key and a.tile_id = b.tile_id
                    left join park_node_tbl as c
                    on a.node_id = c.park_node_id
                    where b.node_id is not null and 
                        (c.park_node_id is null or (c.park_node_id is not null and 
                        (((b.node_id::bit(32))::integer > (1<<23) and c.park_node_type = 0) or 
                        ((b.node_id::bit(32))::integer < (1<<23) and c.park_node_type in (1,2,3)))));
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_tile_node_tile_node_id_idx')
        self.CreateIndex2('rdb_tile_node_old_node_id_idx')
        
        self.log.info('dispatch node id by geometry...')
        sqlcmd = """
                    insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                    select c.node_id, a.node_id, c.tile_id, c.seq_num
                    from node_tbl as a
                    left join rdb_tile_node as b
                    on a.node_id = b.old_node_id
                    left join id_fund_node as c
                    on st_equals(a.the_geom, c.the_geom) and a.tile_id = c.tile_id
                    left join rdb_tile_node as d
                    on c.node_id = d.tile_node_id
                    left join park_node_tbl as e
                    on a.node_id = e.park_node_id                   
                    where b.tile_node_id is null and c.node_id is not null and d.tile_node_id is null
                        and (e.park_node_id is null or (e.park_node_id is not null and 
                        (((c.node_id::bit(32))::integer > (1<<23) and e.park_node_type = 0) or 
                        ((c.node_id::bit(32))::integer < (1<<23) and e.park_node_type in (1,2,3)))));
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('dispatch node id for the left nodes...')
        sqlcmd = """
                insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                select  ((tile_id::bigint << 32) | (begin_node_id + seq_num)) as tile_node_id, 
                        node_array[seq_num] as old_node_id, 
                        tile_id,
                        (begin_node_id + seq_num) as seq_num
                from
                (
                    select  ta.tile_id, node_array, node_count, 
                            (case when tm.max_node_id is null then 0 else tm.max_node_id end) as begin_node_id,
                            generate_series(1, node_count) as seq_num
                    from
                    (
                        select tile_id, array_agg(node_id) as node_array, count(node_id) as node_count
                        from
                        (
                            select a.tile_id, a.node_id
                            from node_tbl as a
                            left join rdb_tile_node as b
                            on a.node_id = b.old_node_id
                            where b.tile_node_id is null and (a.kind is null or a.kind not in('100'))
                            order by a.tile_id, a.node_id
                        )as c
                        group by tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_node_id
                        from id_fund_node
                        where (node_id::bit(32))::integer < (1<<23)
                        group by tile_id
                    )as tm
                    on ta.tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #park node
        sqlcmd = """
                insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                select  ((tile_id::bigint << 32) | (begin_node_id + seq_num)) as tile_node_id, 
                        node_array[seq_num] as old_node_id, 
                        tile_id,
                        (begin_node_id + seq_num) as seq_num
                from
                (
                    select  ta.tile_id, node_array, node_count, 
                            (case when tm.max_node_id is null then 1<<23 else tm.max_node_id end) as begin_node_id,
                            generate_series(1, node_count) as seq_num
                    from
                    (
                        select tile_id, array_agg(node_id) as node_array, count(node_id) as node_count
                        from
                        (
                            select a.tile_id, a.node_id
                            from node_tbl as a
                            left join rdb_tile_node as b
                            on a.node_id = b.old_node_id
                            where b.tile_node_id is null and a.kind = '100'
                            order by a.tile_id, a.node_id
                        )as c
                        group by tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_node_id
                        from id_fund_node
                        where (node_id::bit(32))::integer >= (1<<23)
                        group by tile_id
                    )as tm
                    on ta.tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        self.log.info('End make tile nodes.')
    
    def __makeTileLink(self):
        rdb_log.log(self.ItemName, 'Start make tile links...', 'info')
        
        # link_tbl
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_tile_id_idx')
        self.CreateIndex2('link_tbl_feature_key_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        
        #
        self.CreateTable2('rdb_tile_link')
        
        self.log.info('dispatch link id by feature_key...')
        sqlcmd = """
                    insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                    select b.link_id, a.link_id, b.tile_id, b.seq_num
                    from link_tbl as a
                    left join id_fund_link as b
                    on a.feature_key = b.feature_key and a.tile_id = b.tile_id
                    left join park_link_tbl as c
                    on a.link_id = c.park_link_id
                    where b.link_id is not null and (c.park_link_id is null or
                        (c.park_link_id is not null and (b.link_id::bit(32))::integer > (1<<23) ))
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_tile_link_tile_link_id_idx')
        self.CreateIndex2('rdb_tile_link_old_link_id_idx')
        
        self.log.info('dispatch link id by geometry...')
        sqlcmd = """
                    insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                    select c.link_id, a.link_id, c.tile_id, c.seq_num
                    from link_tbl as a
                    left join rdb_tile_link as b
                    on a.link_id = b.old_link_id
                    left join id_fund_link as c
                    on st_equals(a.the_geom, c.the_geom) and a.tile_id = c.tile_id
                    left join rdb_tile_link as d
                    on c.link_id = d.tile_link_id
                    left join park_link_tbl as e
                    on a.link_id = e.park_link_id
                    where b.tile_link_id is null and c.link_id is not null and d.tile_link_id is null
                        and (e.park_link_id is null or
                            (e.park_link_id is not null and (c.link_id::bit(32))::integer > (1<<23) ))
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        self.log.info('search continuous links for the left links...')
        
        self.CreateFunction2('rdb_merge_link_common_attri')
        self.CreateTable2('temp_link_tbl')
        self.CreateTable2('temp_link_tbl_tile_id_idx')
        #self.CreateIndex2('temp_link_tbl_link_id_idx')
        self.CreateIndex2('temp_link_tbl_tile_id_common_idx')
        self.CreateIndex2('temp_link_tbl_s_node_idx')
        self.CreateIndex2('temp_link_tbl_e_node_idx')
        self.CreateIndex2('temp_link_tbl_common_attri_idx')
        
        #
        self.CreateTable2('temp_link_tbl_newid')
        self.CreateTable2('temp_link_tbl_walked')
        self.CreateIndex2('temp_link_tbl_walked_link_id')
        
        self.CreateFunction2('rdb_make_tile_links_in_one_direction')
        self.CreateFunction2('rdb_make_tile_links')
        sqlcmd = """select rdb_make_tile_links(%s)"""
        self.pg.multi_execute_in_tile(sqlcmd, 'temp_tile_list', 4, self.log)
        self.CreateIndex2('temp_link_tbl_newid_tile_id_idx')
        
        #
        self.log.info('dispatch link id for the left links...')
        sqlcmd = """
                drop index rdb_tile_link_tile_link_id_idx;
                drop index rdb_tile_link_old_link_id_idx;
                insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                select  ((tile_id::bigint << 32) | (begin_link_id + seq_num)) as tile_link_id, 
                        link_array[seq_num] as old_link_id, 
                        tile_id, 
                        (begin_link_id + seq_num) as seq_num
                from
                (
                    select  ta.tile_id, link_array, link_count, 
                            (case when tm.max_link_id is null then 0 else max_link_id end) as begin_link_id,
                            generate_series(1, link_count) as seq_num
                    from
                    (
                        select tile_id, array_agg(link_id) as link_array, count(link_id) as link_count
                        from
                        (
                            select gid, link_id, tile_id
                            from temp_link_tbl_newid
                            where display_class not in(20,21)
                            order by tile_id, gid
                        )as a
                        group by tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_link_id
                        from id_fund_link
                        where (link_id::bit(32))::integer < (1<<23)
                        group by tile_id
                    )as tm
                    on ta.tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_link_tbl_newid_park')
        self.CreateFunction2('rdb_make_tile_links_park')
        self.pg.callproc('rdb_make_tile_links_park')
        self.pg.commit2()
        #park
        sqlcmd = """
                insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                select  ((tile_id::bigint << 32) | (begin_link_id + seq_num)) as tile_link_id, 
                        link_array[seq_num] as old_link_id, 
                        tile_id, 
                        (begin_link_id + seq_num) as seq_num
                from
                (
                    select  ta.tile_id, link_array, link_count, 
                            (case when tm.max_link_id is null then 1<<23 else max_link_id end) as begin_link_id,
                            generate_series(1, link_count) as seq_num
                    from
                    (
                        select tile_id, array_agg(link_id) as link_array, count(link_id) as link_count
                        from
                        (
                            select gid, link_id, tile_id
                            from temp_link_tbl_newid
                            where display_class in(20,21)
                            order by tile_id, gid
                        )as a
                        group by tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_link_id
                        from id_fund_link
                        where (link_id::bit(32))::integer >= (1<<23)
                        group by tile_id
                    )as tm
                    on ta.tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('rdb_tile_link_tile_link_id_idx')
        self.CreateIndex2('rdb_tile_link_old_link_id_idx')
        
        rdb_log.log(self.ItemName, 'End make tile links.', 'info')
        return 0
    
    def __getIDFund(self):
        self.log.info("Getting id fund...")
        
        self.CreateTable2('id_fund_node')
        self.CreateTable2('id_fund_link')
        
        if common.rdb_common.GetPath('get_id_fund_batch'):
            os.system(common.rdb_common.GetPath('get_id_fund_batch'))
        
        self.CreateIndex2('id_fund_node_node_id_idx')
        self.CreateIndex2('id_fund_node_tile_id_idx')
        self.CreateIndex2('id_fund_node_feature_key_tile_id_idx')
        self.CreateIndex2('id_fund_node_the_geom_idx')
        
        self.CreateIndex2('id_fund_link_link_id_idx')
        self.CreateIndex2('id_fund_link_tile_id_idx')
        self.CreateIndex2('id_fund_link_feature_key_tile_id_idx')
        self.CreateIndex2('id_fund_link_the_geom_idx')
        
        self.log.info("Getting id fund end.")
    
    def __updateIDFund(self):
        self.log.info("Updating id fund...")
        
        sqlcmd = """   
                drop table if exists temp_id_fund_node;
                create temp table temp_id_fund_node
                as
                (
                    select  a.tile_node_id as node_id,
                            a.tile_id,
                            a.seq_num, 
                            c.feature_string,
                            c.feature_key,
                            c.the_geom,
                            current_database() as version
                    from rdb_tile_node as a
                    left join id_fund_node as b
                    on a.tile_node_id = b.node_id
                    left join node_tbl as c
                    on a.old_node_id = c.node_id
                    where b.node_id is null
                );
                
                drop index id_fund_node_node_id_idx;
                drop index id_fund_node_tile_id_idx;
                drop index id_fund_node_feature_key_tile_id_idx;
                drop index id_fund_node_the_geom_idx;
                analyze id_fund_node;
                insert into id_fund_node
                            (node_id, tile_id, seq_num, feature_string, feature_key, the_geom, version)
                (
                    select node_id, tile_id, seq_num, feature_string, feature_key, the_geom, version
                    from temp_id_fund_node
                    order by node_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """   
                drop table if exists temp_id_fund_link;
                create temp table temp_id_fund_link
                as
                (
                    select  a.tile_link_id as link_id,
                            a.tile_id,
                            a.seq_num, 
                            c.feature_string,
                            c.feature_key,
                            c.the_geom,
                            current_database() as version
                    from rdb_tile_link as a
                    left join id_fund_link as b
                    on a.tile_link_id = b.link_id
                    left join link_tbl as c
                    on a.old_link_id = c.link_id
                    where b.link_id is null
                );
                
                drop index id_fund_link_link_id_idx;
                drop index id_fund_link_tile_id_idx;
                drop index id_fund_link_feature_key_tile_id_idx;
                drop index id_fund_link_the_geom_idx;
                analyze id_fund_link;
                insert into id_fund_link
                            (link_id, tile_id, seq_num, feature_string, feature_key, the_geom, version)
                (
                    select  link_id, tile_id, seq_num, feature_string, feature_key, the_geom, version
                    from temp_id_fund_link
                    order by link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info("Updating id fund end.")


