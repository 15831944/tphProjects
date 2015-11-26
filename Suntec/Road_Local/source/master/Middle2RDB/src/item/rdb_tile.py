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
                    drop table if exists temp_newid_candidate;
                    create temp table temp_newid_candidate
                    as
                    select  a.node_id as mid_node_id, 
                            b.node_id as rdb_node_id, 
                            b.tile_id, 
                            b.seq_num
                    from node_tbl as a
                    left join id_fund_node as b
                    on a.feature_key = b.feature_key and a.tile_id = b.tile_id
                    left join park_node_tbl as c
                    on a.node_id = c.park_node_id
                    where b.node_id is not null and 
                        (c.park_node_id is null or (c.park_node_id is not null and 
                        (((b.node_id::bit(32))::integer > (1<<23) and c.park_node_type = 0) or 
                        ((b.node_id::bit(32))::integer < (1<<23) and c.park_node_type in (1,2,3)))));
                    
                    create index temp_newid_candidate_mid_node_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (mid_node_id);
                    
                    create index temp_newid_candidate_rdb_node_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (rdb_node_id);
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select mid_node_id
                        from temp_newid_candidate
                        group by mid_node_id having count(*) > 1
                    ) as b
                    where a.mid_node_id = b.mid_node_id;
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select rdb_node_id
                        from temp_newid_candidate
                        group by rdb_node_id having count(*) > 1
                    ) as b
                    where a.rdb_node_id = b.rdb_node_id;
                    
                    insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                    select rdb_node_id, mid_node_id, tile_id, seq_num
                    from temp_newid_candidate;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_tile_node_tile_node_id_idx')
        self.CreateIndex2('rdb_tile_node_old_node_id_idx')
        
        self.log.info('dispatch node id by feature_key and geometry...')
        sqlcmd = """
                    drop table if exists temp_newid_candidate;
                    create temp table temp_newid_candidate
                    as
                    select  a.node_id as mid_node_id, 
                            c.node_id as rdb_node_id, 
                            c.tile_id, 
                            c.seq_num
                    from node_tbl as a
                    left join rdb_tile_node as b
                    on a.node_id = b.old_node_id
                    left join id_fund_node as c
                    on (a.feature_key = c.feature_key) and st_equals(a.the_geom, c.the_geom) and a.tile_id = c.tile_id
                    left join rdb_tile_node as d
                    on c.node_id = d.tile_node_id
                    left join park_node_tbl as e
                    on a.node_id = e.park_node_id
                    where b.tile_node_id is null and c.node_id is not null and d.tile_node_id is null
                        and (e.park_node_id is null or (e.park_node_id is not null and 
                        (((c.node_id::bit(32))::integer > (1<<23) and e.park_node_type = 0) or 
                        ((c.node_id::bit(32))::integer < (1<<23) and e.park_node_type in (1,2,3)))));
                    
                    create index temp_newid_candidate_mid_node_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (mid_node_id);
                    
                    create index temp_newid_candidate_rdb_node_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (rdb_node_id);
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select mid_node_id
                        from temp_newid_candidate
                        group by mid_node_id having count(*) > 1
                    ) as b
                    where a.mid_node_id = b.mid_node_id;
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select rdb_node_id
                        from temp_newid_candidate
                        group by rdb_node_id having count(*) > 1
                    ) as b
                    where a.rdb_node_id = b.rdb_node_id;
                    
                    insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                    select rdb_node_id, mid_node_id, tile_id, seq_num
                    from temp_newid_candidate
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('dispatch node id by geometry...')
        sqlcmd = """
                    drop table if exists temp_newid_candidate;
                    create temp table temp_newid_candidate
                    as
                    select  a.node_id as mid_node_id, 
                            c.node_id as rdb_node_id, 
                            c.tile_id, 
                            c.seq_num
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
                    
                    create index temp_newid_candidate_mid_node_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (mid_node_id);
                    
                    create index temp_newid_candidate_rdb_node_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (rdb_node_id);
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select mid_node_id
                        from temp_newid_candidate
                        group by mid_node_id having count(*) > 1
                    ) as b
                    where a.mid_node_id = b.mid_node_id;
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select rdb_node_id
                        from temp_newid_candidate
                        group by rdb_node_id having count(*) > 1
                    ) as b
                    where a.rdb_node_id = b.rdb_node_id;
                    
                    insert into rdb_tile_node(tile_node_id, old_node_id, tile_id, seq_num)
                    select rdb_node_id, mid_node_id, tile_id, seq_num
                    from temp_newid_candidate;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('dispatch node id for the left nodes...')
        sqlcmd = """
                drop table if exists temp_id_fund_tile_max_node_id;
                create temp table temp_id_fund_tile_max_node_id
                as
                select tile_id, count(*) as max_node_id
                from id_fund_node
                where (node_id::bit(32))::integer < (1<<23)
                group by tile_id;
                
                create index temp_id_fund_tile_max_node_id_tile_id_idx
                    on temp_id_fund_tile_max_node_id
                    using btree
                    (tile_id);
    
                drop table if exists temp_dispatch_tile_node_array;
                create temp table temp_dispatch_tile_node_array
                as
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
                group by tile_id;
                
                create index temp_dispatch_tile_node_array_tile_id_idx
                    on temp_dispatch_tile_node_array
                    using btree
                    (tile_id);
                
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
                    from temp_dispatch_tile_node_array as ta
                    left join temp_id_fund_tile_max_node_id as tm
                    on ta.tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #park node
        sqlcmd = """
                drop table if exists temp_id_fund_tile_max_node_id;
                create temp table temp_id_fund_tile_max_node_id
                as
                select tile_id, count(*) as max_node_id
                from id_fund_node
                where (node_id::bit(32))::integer >= (1<<23)
                group by tile_id;
                
                create index temp_id_fund_tile_max_node_id_tile_id_idx
                    on temp_id_fund_tile_max_node_id
                    using btree
                    (tile_id);
    
                drop table if exists temp_dispatch_tile_node_array;
                create temp table temp_dispatch_tile_node_array
                as
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
                group by tile_id;
                
                create index temp_dispatch_tile_node_array_tile_id_idx
                    on temp_dispatch_tile_node_array
                    using btree
                    (tile_id);
                
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
                    from temp_dispatch_tile_node_array as ta
                    left join temp_id_fund_tile_max_node_id as tm
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
                    drop table if exists temp_newid_candidate;
                    create temp table temp_newid_candidate
                    as
                    select  a.link_id as mid_link_id, 
                            b.link_id as rdb_link_id, 
                            b.tile_id, 
                            b.seq_num
                    from link_tbl as a
                    left join id_fund_link as b
                    on a.feature_key = b.feature_key and a.tile_id = b.tile_id
                    left join park_link_tbl as c
                    on a.link_id = c.park_link_id
                    where b.link_id is not null and (c.park_link_id is null or
                        (c.park_link_id is not null and (b.link_id::bit(32))::integer > (1<<23) ));
                    
                    create index temp_newid_candidate_mid_link_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (mid_link_id);
                    
                    create index temp_newid_candidate_rdb_link_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (rdb_link_id);
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select mid_link_id
                        from temp_newid_candidate
                        group by mid_link_id having count(*) > 1
                    ) as b
                    where a.mid_link_id = b.mid_link_id;
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select rdb_link_id
                        from temp_newid_candidate
                        group by rdb_link_id having count(*) > 1
                    ) as b
                    where a.rdb_link_id = b.rdb_link_id;
                    
                    insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                    select rdb_link_id, mid_link_id, tile_id, seq_num
                    from temp_newid_candidate;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_tile_link_tile_link_id_idx')
        self.CreateIndex2('rdb_tile_link_old_link_id_idx')
        
        self.log.info('dispatch link id by feature_key and geometry...')
        sqlcmd = """
                    drop table if exists temp_newid_candidate;
                    create temp table temp_newid_candidate
                    as
                    select  a.link_id as mid_link_id, 
                            c.link_id as rdb_link_id, 
                            c.tile_id, 
                            c.seq_num
                    from link_tbl as a
                    left join rdb_tile_link as b
                    on a.link_id = b.old_link_id
                    left join id_fund_link as c
                    on (a.feature_key = c.feature_key) and st_equals(a.the_geom, c.the_geom) and a.tile_id = c.tile_id
                    left join rdb_tile_link as d
                    on c.link_id = d.tile_link_id
                    left join park_link_tbl as e
                    on a.link_id = e.park_link_id
                    where b.tile_link_id is null and c.link_id is not null and d.tile_link_id is null
                        and (e.park_link_id is null or
                            (e.park_link_id is not null and (c.link_id::bit(32))::integer > (1<<23) ));
                    
                    create index temp_newid_candidate_mid_link_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (mid_link_id);
                    
                    create index temp_newid_candidate_rdb_link_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (rdb_link_id);
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select mid_link_id
                        from temp_newid_candidate
                        group by mid_link_id having count(*) > 1
                    ) as b
                    where a.mid_link_id = b.mid_link_id;
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select rdb_link_id
                        from temp_newid_candidate
                        group by rdb_link_id having count(*) > 1
                    ) as b
                    where a.rdb_link_id = b.rdb_link_id;
                    
                    insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                    select rdb_link_id, mid_link_id, tile_id, seq_num
                    from temp_newid_candidate
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('dispatch link id by geometry...')
        sqlcmd = """
                    drop table if exists temp_newid_candidate;
                    create temp table temp_newid_candidate
                    as
                    select  a.link_id as mid_link_id, 
                            c.link_id as rdb_link_id, 
                            c.tile_id, 
                            c.seq_num
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
                            (e.park_link_id is not null and (c.link_id::bit(32))::integer > (1<<23) ));
                    
                    create index temp_newid_candidate_mid_link_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (mid_link_id);
                    
                    create index temp_newid_candidate_rdb_link_id_idx_idx
                        on temp_newid_candidate
                        using btree
                        (rdb_link_id);
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select mid_link_id
                        from temp_newid_candidate
                        group by mid_link_id having count(*) > 1
                    ) as b
                    where a.mid_link_id = b.mid_link_id;
                    
                    delete from temp_newid_candidate as a
                    using
                    (
                        select rdb_link_id
                        from temp_newid_candidate
                        group by rdb_link_id having count(*) > 1
                    ) as b
                    where a.rdb_link_id = b.rdb_link_id;
                    
                    insert into rdb_tile_link(tile_link_id, old_link_id, tile_id, seq_num)
                    select rdb_link_id, mid_link_id, tile_id, seq_num
                    from temp_newid_candidate;
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
                            where display_class not in(30,31)
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
                            where display_class in(30,31)
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
        self.CreateTable2('rtic_id_fund_link')     
        
        # try to get id_fund from base_db...
        if common.rdb_common.GetPara('base_db'):
            # get paras of pg_dump and pg_restore
            base_db_para = common.rdb_common.GetPara('base_db')
            src_host = self.__analyzeDBParas(base_db_para).get('host')
            src_dbname = self.__analyzeDBParas(base_db_para).get('dbname')
            
            current_db_para = common.rdb_common.GetPara('db1')
            dst_host = self.__analyzeDBParas(current_db_para).get('host')
            dst_dbname = self.__analyzeDBParas(current_db_para).get('dbname')
            
            cache_path = os.path.abspath(common.rdb_common.GetPath('cache'))
            backup_file = os.path.join(cache_path, '%s_id_fund.backup' % src_dbname)
            
            paras = {
                     'set src_host=': src_host,
                     'set src_dbname=': src_dbname,
                     'set dst_host=': dst_host,
                     'set dst_dbname=': dst_dbname,
                     'set backup_file=': backup_file,
                     }
            
            # write batch file of pg_dump and pg_restore
            get_id_fund_sample = os.path.join(cache_path, 'get_id_fund.bat.sample')
            get_id_fund_batch = os.path.join(cache_path, 'get_id_fund.bat')
            all_cmd = open(get_id_fund_sample).readlines()
            file = open(get_id_fund_batch, 'w')
            for cmd in all_cmd:
                for (para_name, para_value) in paras.items():
                    if cmd.find(para_name) == 0:
                        cmd = ''.join([para_name, para_value, os.linesep])
                        break
                file.write(cmd)
            file.close()
            
            # execute batch file of pg_dump and pg_restore
            execute_result = os.system(get_id_fund_batch)
            if execute_result != 0:
                raise Exception, 'fail to get id_fund from %s.%s...' % (src_host,src_dbname)
        
        self.CreateIndex2('id_fund_node_node_id_idx')
        self.CreateIndex2('id_fund_node_tile_id_idx')
        self.CreateIndex2('id_fund_node_feature_key_tile_id_idx')
        self.CreateIndex2('id_fund_node_the_geom_idx')
        
        self.CreateIndex2('id_fund_link_link_id_idx')
        self.CreateIndex2('id_fund_link_tile_id_idx')
        self.CreateIndex2('id_fund_link_feature_key_tile_id_idx')
        self.CreateIndex2('id_fund_link_the_geom_idx')
        
        self.log.info("Getting id fund end.")
    
    def __analyzeDBParas(self, db_para_string):
        paras = {}
        for one_para_string in db_para_string.split(' '):
            one_para = one_para_string.split('=')
            para_name = one_para[0]
            para_value = one_para[1].strip("'")
            paras[para_name] = para_value
        return paras
    
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


