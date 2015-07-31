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
        self.__makeTileNode()
        self.__makeTileLink()
        self.__updateIDFund()
        return 0
    
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
                    where b.node_id is not null
                    ;
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
                    where b.tile_node_id is null and c.node_id is not null and d.tile_node_id is null
                    ;
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
                            where b.tile_node_id is null
                            order by a.tile_id, a.node_id
                        )as c
                        group by tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_node_id
                        from id_fund_node
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
                    where b.link_id is not null
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
                    where b.tile_link_id is null and c.link_id is not null and d.tile_link_id is null
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        self.log.info('dispatch link id for the left links...')
        
        self.CreateFunction2('rdb_merge_link_common_attri')
        self.CreateTable2('temp_link_tbl')
        self.CreateIndex2('temp_link_tbl_link_id_idx')
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
        self.pg.callproc('rdb_make_tile_links')
        self.pg.commit2()
        self.CreateIndex2('temp_link_tbl_newid_tile_id_idx')
        
        #
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
                            order by tile_id, gid
                        )as a
                        group by tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_link_id
                        from id_fund_link
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
    
    def __getIDFund(self):
        self.log.info("Getting id fund...")
        
        if common.rdb_common.GetPath('get_id_fund_batch'):
            self.CreateTable2('id_fund_node')
            self.CreateTable2('id_fund_link')
            os.system(common.rdb_common.GetPath('get_id_fund_batch'))
        else:
            self.CreateTable2('id_fund_node')
            self.CreateIndex2('id_fund_node_node_id_idx')
            self.CreateIndex2('id_fund_node_tile_id_idx')
            self.CreateIndex2('id_fund_node_feature_key_tile_id_idx')
            self.CreateIndex2('id_fund_node_the_geom_idx')
            
            self.CreateTable2('id_fund_link')
            self.CreateIndex2('id_fund_link_link_id_idx')
            self.CreateIndex2('id_fund_link_tile_id_idx')
            self.CreateIndex2('id_fund_link_feature_key_tile_id_idx')
            self.CreateIndex2('id_fund_link_the_geom_idx')
        
        self.log.info("Getting id fund end.")
    
    def __updateIDFund(self):
        self.log.info("Updating id fund...")
        
        sqlcmd = """
                insert into id_fund_node
                            (node_id, tile_id, seq_num, feature_string, feature_key, the_geom, version)
                (
                    select  a.tile_node_id,
                            a.tile_id,
                            a.seq_num, 
                            c.feature_string,
                            c.feature_key,
                            c.the_geom,
                            current_database()
                    from rdb_tile_node as a
                    left join id_fund_node as b
                    on a.tile_node_id = b.node_id
                    left join node_tbl as c
                    on a.old_node_id = c.node_id
                    where b.node_id is null
                    order by a.tile_node_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                insert into id_fund_link
                            (link_id, tile_id, seq_num, feature_string, feature_key, the_geom, version)
                (
                    select  a.tile_link_id,
                            a.tile_id,
                            a.seq_num, 
                            c.feature_string,
                            c.feature_key,
                            c.the_geom,
                            current_database()
                    from rdb_tile_link as a
                    left join id_fund_link as b
                    on a.tile_link_id = b.link_id
                    left join link_tbl as c
                    on a.old_link_id = c.link_id
                    where b.link_id is null
                    order by a.tile_link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info("Updating id fund end.")


