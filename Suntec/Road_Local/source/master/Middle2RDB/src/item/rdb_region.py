# -*- coding: cp936 -*-
'''
Created on 2012-6-6

@author: liuxinxing
'''
import os
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common
from item import rdb_link_sequence

class rdb_region(ItemBase):
    '''
    classdocs
    '''
    @staticmethod
    def instance():
        proj_mapping = {
            ('default'):            rdb_region(),
            ('jpn'):                rdb_region_ipc_japen(),
            ('jdb'):                rdb_region_ipc_japen(),
            ('ni'):                 rdb_region_ni_china(),
            ('axf'):                rdb_region_axf_china(),
            ('ta'):                 rdb_region_ta_vietnam(),
            ('ta','eu'):            rdb_region_ta_europe(),
            ('ta','vie'):           rdb_region_ta_vietnam(),
            ('ta','aus'):           rdb_region_ta_aus(),
            ('rdf','hkg'):          rdb_region_rdf_ap(),
            ('rdf','sgp'):          rdb_region_rdf_ap(),
            ('rdf','me8'):          rdb_region_rdf_ap(),
			('rdf','mea'):          rdb_region_rdf_ap(),
            ('rdf','ase'):          rdb_region_rdf_sea(),
            ('rdf','na'):           rdb_region_rdf_north_america(),
            ('rdf','bra'):          rdb_region_rdf_brazil(),
            ('rdf','arg'):          rdb_region_rdf_argentina(),
            ('nostra'):             rdb_region_nostra_thailand(),
            ('mmi'):                rdb_region_mmi_india(),
            ('msm'):                rdb_region_malsing_mys(),
            ('zenrin','twn'):       rdb_region_taiwan(),
        }
        return rdb_common.getItem(proj_mapping)
    
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Region')
        
    def Do(self):
        self._reset()
        self._hierarchy()
        self._makeRegionLevel8()
        self._makeRegionLevel6()
        self._makeRegionLevel4()
        return 0
    
    def _reset(self):
        self.CreateTable2('rdb_link_abs')
        self.CreateTable2('temp_region_node_upgrade')
        self._resetLinkFunctionClass()
    
    def _hierarchy(self):
        pass
        
    def _resetLinkFunctionClass(self):
        """
        # add some slow links into region.
        # it will promote the connectivity of region network.
        """
        pass
    
    def _makeRegionLevel4(self):
        self._makeOriginalRegionOnLevel4()
        self._createTempRegion("temp_region_orglink_level4", "temp_region_orgnode_level4")
        self._deleteSALink(4)
        self._deleteIsolatedLink()
        self._mergeRegionLink(4)
        self._makeTargetRegionDataLevel4()
    
    def _makeRegionLevel6(self):
        self._makeOriginalRegionOnLevel6()
        self._createTempRegion("temp_region_orglink_level6", "temp_region_orgnode_level6")
        self._deleteSALink(6)
        self._deleteUTurnLink()
        self._deleteIsolatedIC()
        self._deleteIsolatedLink()
        self._mergeRegionLink(6)
        self._makeTargetRegionDataLevel6()
    
    def _makeRegionLevel8(self):
        pass
    
    def _makeOriginalRegionOnLevelX(self, 
                                    layer=4, level=10, 
                                    rdb_link='rdb_link', attr_filter={'function_code':[1,2,3]},
                                    additional_links='temp_hierarchy_links_layerx'):
        self.log.info('Make original region level%s...' % str(layer))
        
        #
        rdb_link_filter = 'True'
        if attr_filter:
            filter_list = []
            for (key,value) in attr_filter.items():
                filter_list.append('%s = ANY(ARRAY%s)' % (str(key), str(value)))
            rdb_link_filter = 'and '.join(filter_list)
        
        union_additional_links = ''
        if additional_links:
            union_additional_links = """
                                    union
                                    select link_id
                                    from [additional_links]
                                    """
            union_additional_links = union_additional_links.replace('[additional_links]', additional_links)
        
        #
        self.CreateFunction2('get_new_tile_id_by_zlevel')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level[X];
                    create table temp_region_orglink_level[X]
                    as
                    (
                        select  b.*, 
                                get_new_tile_id_by_zlevel(link_id_t,[level]) as region_tile_id, [X] as region_level
                        from 
                        (
                            select link_id
                            from [rdb_link]
                            where [rdb_link_filter]
                            [union_additional_links]
                        )as a
                        left join rdb_link as b
                        on a.link_id = b.link_id
                        where one_way != 0 and road_type not in (7,8,9,14)
                    );
                    
                    CREATE INDEX temp_region_orglink_level[X]_link_id_idx
                        on temp_region_orglink_level[X]
                        using btree
                        (link_id);
                    
                    CREATE INDEX temp_region_orglink_level[X]_start_node_id_idx
                        on temp_region_orglink_level[X]
                        using btree
                        (start_node_id);
                    
                    CREATE INDEX temp_region_orglink_level[X]_end_node_id_idx
                        on temp_region_orglink_level[X]
                        using btree
                        (end_node_id);
                    
                    analyze temp_region_orglink_level[X];
                """
        sqlcmd = sqlcmd.replace('[rdb_link]', str(rdb_link))
        sqlcmd = sqlcmd.replace('[rdb_link_filter]', str(rdb_link_filter))
        sqlcmd = sqlcmd.replace('[union_additional_links]', str(union_additional_links))
        sqlcmd = sqlcmd.replace('[level]', str(level))
        sqlcmd = sqlcmd.replace('[X]', str(layer))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level[X];
                    create table temp_region_orgnode_level[X]
                    as
                    (
                        select c.*, get_new_tile_id_by_zlevel(node_id_t,[level]) as region_tile_id
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level[X]
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level[X]
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                    
                    CREATE INDEX temp_region_orgnode_level[X]_node_id_idx
                        on temp_region_orgnode_level[X]
                        using btree
                        (node_id);
                    
                    analyze temp_region_orgnode_level[X];
                """
        sqlcmd = sqlcmd.replace('[level]', str(level))
        sqlcmd = sqlcmd.replace('[X]', str(layer))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.log.info('Make original region level%s end.' % str(layer))
    
    def _makeOriginalRegionOnLevel4(self, zlevel=10, fc_array=[1,2,3]):
        rdb_log.log('Region', 'Make original region level4...', 'info')
        
        #
        self.CreateFunction2('get_new_tile_id_by_zlevel')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level4;
                    create table temp_region_orglink_level4
                    as
                    (
                        select *, get_new_tile_id_by_zlevel(link_id_t,%s) as region_tile_id, 4 as region_level
                        from rdb_link 
                        where function_code = ANY(ARRAY%s) and one_way != 0 and road_type not in (7,8,9,14)
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orglink_level4_link_id_idx')
        self.CreateIndex2('temp_region_orglink_level4_start_node_id_idx')
        self.CreateIndex2('temp_region_orglink_level4_end_node_id_idx')
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level4;
                    create table temp_region_orgnode_level4
                    as
                    (
                        select c.*, get_new_tile_id_by_zlevel(node_id_t,%s) as region_tile_id
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level4
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level4
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orgnode_level4_node_id_idx')
        
        rdb_log.log('Region', 'Make original region level4 end.', 'info')
    
    def _makeOriginalRegionOnLevel6(self, zlevel=6, fc_array=[1,2]):
        rdb_log.log('Region', 'Make original region level6...', 'info')
        
        #
        self.CreateFunction2('get_new_tile_id_by_zlevel')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level6;
                    create table temp_region_orglink_level6
                    as
                    (
                        select *, get_new_tile_id_by_zlevel(link_id_t,%s) as region_tile_id, 6 as region_level
                        from rdb_link 
                        where function_code = ANY(ARRAY%s) and one_way != 0 and road_type not in (7,8,9,14)
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orglink_level6_link_id_idx')
        self.CreateIndex2('temp_region_orglink_level6_start_node_id_idx')
        self.CreateIndex2('temp_region_orglink_level6_end_node_id_idx')
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level6;
                    create table temp_region_orgnode_level6
                    as
                    (
                        select c.*, get_new_tile_id_by_zlevel(node_id_t,%s) as region_tile_id
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level6
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level6
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orgnode_level6_node_id_idx')
        
        rdb_log.log('Region', 'Make original region level6 end.', 'info')
    
    def _deleteIsolatedIC_backup(self):
        rdb_log.log('Region', 'Delete isolated IC from original region links...', 'info')
        
        self.CreateTable2('temp_region_delete_ic')
        self.CreateIndex2('temp_region_delete_ic_link_id_idx')
        
        self.CreateFunction2('rdb_delete_isolated_ic_from_region')
        self.pg.callproc('rdb_delete_isolated_ic_from_region')
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_links as a
                    using temp_region_delete_ic as b
                    where a.link_id = b.link_id and a.region_level >= a.max_level;
                    analyze temp_region_links;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_nodes as a
                    using 
                    (
                        select a.node_id
                        from temp_region_nodes as a
                        left join temp_region_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                        where b.link_id is null
                    ) as b
                    where a.node_id = b.node_id;
                    analyze temp_region_nodes;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'Delete isolated IC from original region links end.', 'info')
    
    def _deleteIsolatedIC(self):
        self.log.info('Delete isolated ic links...')
        rdb_link_network='temp_region_links'
        self._deleteUnnecessaryRamp(ramp='ic', rdb_link_network=rdb_link_network)
    
    def _deleteIsolatedLink(self):
        rdb_log.log('Region', 'Delete isolated link from original region links...', 'info')
        
        self.CreateTable2('temp_region_links_to_search')
        self.CreateIndex2('temp_region_links_to_search_link_id_idx')
        self.CreateIndex2('temp_region_links_to_search_start_node_id_idx')
        self.CreateIndex2('temp_region_links_to_search_end_node_id_idx')
        self.CreateTable2('temp_region_walked_link')
        self.CreateIndex2('temp_region_walked_link_link_id_idx')
        
        self.CreateFunction2('rdb_find_isolated_link_from_region')
        self.pg.callproc('rdb_find_isolated_link_from_region')
        self.pg.commit2()
        
        sqlcmd = """
                    -- delete the island if the number of its links is less than 1000
                    delete from temp_region_links as a
                    using 
                    (
                        select island_id, unnest(array_agg(link_id)) as link_id
                        from temp_region_walked_link
                        group by island_id having count(*) < 1000
                    )as b
                    where a.link_id = b.link_id and a.region_level >= a.max_level;
                    analyze temp_region_links;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_nodes as a
                    using 
                    (   
                        select a.node_id
                        from temp_region_nodes as a
                        left join temp_region_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                        where b.link_id is null
                    ) as b
                    where a.node_id = b.node_id;
                    analyze temp_region_nodes;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'Delete isolated link from original region links end.', 'info')
    
    def _deleteSALink_backup(self):
        # drop sa link in region, except sa_ramp(sa link connnects ramp and highway)
        rdb_log.log('Region', 'Delete sa link from original region links...', 'info')
        
#        # find sa ramp
#        self.CreateTable2('temp_region_links_sa_ramp')
#        self.CreateFunction2('rdb_find_sa_ramp_from_region')
#        self.pg.callproc('rdb_find_sa_ramp_from_region')
#        self.pg.commit2()
#        self.CreateIndex2('temp_region_links_sa_ramp_link_id_idx')

        # find sa path
        self.CreateTable2('temp_region_links_sa_path')
        self.CreateFunction2('rdb_region_find_sa_path')
        self.pg.callproc('rdb_region_find_sa_path')
        self.pg.commit2()
        self.CreateIndex2('temp_region_links_sa_path_ftnode_idx')
        
        # for sa_paths with the same fnode and tnode,
        # delete dummy sa_paths whose cost exceeds 10% of lowest cost
        sqlcmd = """
                drop table if exists temp_region_links_sa_path_cost;
                create table temp_region_links_sa_path_cost
                as
                select fnode, tnode, min(cost) as cost
                from temp_region_links_sa_path
                group by fnode, tnode;
                
                create index temp_region_links_sa_path_cost_ftnode_index
                    on temp_region_links_sa_path_cost
                    using btree
                    (fnode, tnode);
                
                delete from temp_region_links_sa_path as a
                using temp_region_links_sa_path_cost as b
                where a.fnode = b.fnode and a.tnode = b.tnode and a.cost > b.cost * 1.1;
                analyze temp_region_links_sa_path;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        # if sa_path is necessary, keep its links in region
        self.CreateTable2('temp_region_links_sa_ramp')
        self.CreateFunction2('rdb_region_check_sa_path_necessary')
        sqlcmd = """
                insert into temp_region_links_sa_ramp
                select distinct unnest(link_array)
                from temp_region_links_sa_path
                where rdb_region_check_sa_path_necessary(fnode, tnode, cost)
                ;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_links_sa_ramp_link_id_idx')
        
        # delete ordinary sa link
        sqlcmd = """
                delete from temp_region_links as a
                using 
                (
                    select a.link_id
                    from temp_region_links as a
                    left join temp_region_links_sa_ramp as b
                    on a.link_id = b.link_id
                    where (a.link_type = 7) and b.link_id is null
                )as b
                where a.link_id = b.link_id and a.region_level >= a.max_level;
                analyze temp_region_links;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_nodes as a
                    using 
                    (   
                        select a.node_id
                        from temp_region_nodes as a
                        left join temp_region_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                        where b.link_id is null
                    ) as b
                    where a.node_id = b.node_id;
                    analyze temp_region_nodes;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'Delete sa link from original region links end.', 'info')
    
    def _deleteSALink(self, layer):
        self.log.info('Delete unnecessary SAPA links...')
        if layer == 4:
            rdb_link_network='rdb_link'
        else:
            rdb_link_network='temp_region_links'
        self._deleteUnnecessaryRamp(ramp='sapa', rdb_link_network=rdb_link_network)
    
    def _deleteUnnecessaryRamp(self, ramp='sapa', rdb_link_network='rdb_link'):
        # drop unnecessary ramp(sapa/ic) link in region, 
        # only keep important ramp(which connect different highways, or in-out highways in layer 4.)
        import rdb_region_algorithm
        graph = rdb_region_algorithm.CGraph_PG()
        graph.prepareData(rdb_link=rdb_link_network)
        
        if ramp == 'sapa':
            link_types = [7]
            is_legal_path=graph._is_legal_path_of_sa
            is_leaf=graph._is_leaf_of_sa
            is_illegal_path=graph._is_legal_path_of_no_sa
        else:#if ramp == 'ic':
            link_types = [5]
            is_legal_path=graph._is_legal_path_of_ramp
            is_leaf=graph._is_leaf_of_ramp
            is_illegal_path=graph._is_legal_path_of_no_ramp
        try:
            # find important ramp links
            important_ramp_links = set()
            sqlcmd = """
                    -- for all entry point of sapa area
                    select distinct a.node_id
                    from
                    (
                        select start_node_id as node_id
                        from temp_region_links
                        where link_type = ANY(ARRAY[link_types]) and one_way in (1,2)
                        union
                        select end_node_id as node_id
                        from temp_region_links
                        where link_type = ANY(ARRAY[link_types]) and one_way in (1,3)
                    )as a
                    inner join [rdb_link] as b
                    on  (not (b.link_type = ANY(ARRAY[link_types])))
                        and 
                        (
                        (a.node_id = b.start_node_id and b.one_way in (1,3))
                        or 
                        (a.node_id = b.end_node_id and b.one_way in (1,2))
                        )
                    """
            sqlcmd = sqlcmd.replace('[rdb_link]', rdb_link_network)
            sqlcmd = sqlcmd.replace('[link_types]', str(link_types))
            ramp_node_list = self.pg.get_batch_data2(sqlcmd)
            for sapa_node_rec in ramp_node_list:
                root_node = sapa_node_rec[0]
                #print 'root_node =', root_node
                all_paths = graph.searchMinSpanningTree(root_node, forward=True,
                                                       is_legal_path=is_legal_path,
                                                       is_leaf=is_leaf,
                                                       stop_at_leaf=False,
                                                       get_link_cost=rdb_region_algorithm.CLinkCost.getCost2,
                                                       max_single_path_num=1,
                                                       log=self.log)
                for (to_node,paths) in all_paths.items():
                    for (cost,path) in paths:
                        #print '        to_node =', to_node, '    cost =', cost
                        if graph.searchDijkstraPaths(root_node, to_node, forward=True,
                                                     is_legal_path=is_illegal_path,
                                                     max_cost=cost*1.1,
                                                     max_path_num=1,
                                                     get_link_cost=rdb_region_algorithm.CLinkCost.getCost2
                                                     ):
                            continue
                        else:
                            important_ramp_links.update(path)
        finally:
            graph.clearData()
        
        self.CreateTable2('temp_region_links_important_ramp')
        import common.cache_file
        temp_file = common.cache_file.open('temp_region_links_important_ramp')
        for linkid in important_ramp_links:
            temp_file.write('%s\n' % str(linkid))
        temp_file.seek(0)
        self.pg.copy_from2(temp_file, 'temp_region_links_important_ramp')
        self.pg.commit2()
        common.cache_file.close(temp_file,True)
        self.CreateIndex2('temp_region_links_important_ramp_link_id_idx')
        
        # delete ordinary ramp link
        sqlcmd = """
                delete from temp_region_links as a
                using 
                (
                    select a.link_id
                    from temp_region_links as a
                    left join temp_region_links_important_ramp as b
                    on a.link_id = b.link_id
                    where (a.link_type = ANY(ARRAY[link_types])) and b.link_id is null
                )as b
                where a.link_id = b.link_id and a.region_level >= a.max_level;
                analyze temp_region_links;
                """
        sqlcmd = sqlcmd.replace('[link_types]', str(link_types))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_nodes as a
                    using 
                    (   
                        select a.node_id
                        from temp_region_nodes as a
                        left join temp_region_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                        where b.link_id is null
                    ) as b
                    where a.node_id = b.node_id;
                    analyze temp_region_nodes;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def _deleteUTurnLink(self):
        # drop u-turn link in region level6
        rdb_log.log('Region', 'Delete uturn link from original region links...', 'info')
        
        self.CreateFunction2('rdb_detect_parallel_angle')
        self.CreateFunction2('rdb_is_uturn_link')
        sqlcmd = """
                drop table if exists temp_region_links_uturn;
                create temp table temp_region_links_uturn
                as
                select link_id 
                from temp_region_links
                where (link_type in (4,8,13) or uturn = 1) and rdb_is_uturn_link(link_id);
                create index temp_region_links_uturn_link_id_idx
                    on temp_region_links_uturn
                    using btree
                    (link_id);
                
                delete from temp_region_links as a
                using temp_region_links_uturn as b
                where a.link_id = b.link_id and a.region_level >= a.max_level;
                analyze temp_region_links;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_nodes as a
                    using 
                    (   
                        select a.node_id
                        from temp_region_nodes as a
                        left join temp_region_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                        where b.link_id is null
                    ) as b
                    where a.node_id = b.node_id;
                    analyze temp_region_nodes;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'Delete uturn link from original region links end.', 'info')
    
    def _mergeRegionLink(self, layer):
        # input table:    "temp_region_links", "temp_region_nodes"
        # output table:   "temp_region_merge_links", "temp_region_merge_nodes", "temp_region_merge_regulation"
        self._prepareSuspectLinkNode(layer)
        self._searchLinkrow(layer)
        self._mergeLinkrow()
        self._dispatchNewID()
    
    def _createTempRegion(self, region_links_table = "temp_region_orglink_level6", region_nodes_table = "temp_region_orgnode_level6"):
        rdb_log.log('Region', 'Create temp region layer...', 'info')
        
        sqlcmd = """
                drop table if exists temp_region_links;
                create table temp_region_links
                as
                SELECT  a.*, 
                        (extend_flag >> 1) & 1 as pdm_flag, 
                        (case when b.path_extra_info is null then 0 else (b.path_extra_info >> 1) & 1 end) as uturn,
                        (case when c.max_level is null then a.region_level else c.max_level end) as max_level,
                        (case when c.abs_link_id is null then 0 else c.abs_link_id end) as abs_link_id
                FROM %s as a
                left join rdb_link_add_info as b
                on a.link_id = b.link_id
                left join rdb_link_abs as c
                on a.link_id = c.link_id;
                
                drop table if exists temp_region_nodes;
                create table temp_region_nodes
                as
                
                SELECT distinct 
                       b.gid, node_id, b.region_tile_id, node_id_t, b.extend_flag, 
                       link_num, branches, b.the_geom, b.the_geom_900913
                FROM %s as a
                left join %s as b
                on b.node_id in (a.start_node_id, a.end_node_id);
                """ % (region_links_table, region_links_table, region_nodes_table)
        self.pg.execute2(sqlcmd)
        
        self.CreateIndex2('temp_region_links_link_id_idx')
        self.CreateIndex2('temp_region_links_start_node_id_idx')
        self.CreateIndex2('temp_region_links_end_node_id_idx')
        self.CreateIndex2('temp_region_nodes_node_id_idx')
        
        rdb_log.log('Region', 'Create temp region layer end.', 'info')
    
    def _prepareSuspectLinkNode(self, layer):
        rdb_log.log('Region', 'Prepare suspect of merging link and node...', 'info')
        
        self.CreateTable2('temp_region_merge_node_keep')
        self.CreateTable2('temp_region_merge_link_keep')
        if layer == 6 and rdb_common.getProjName().lower() == 'ni':
            sqlcmd = """
                    drop table if exists temp_region_merge_node_keep2;
                    alter table temp_region_merge_node_keep rename to temp_region_merge_node_keep2;
                    create table temp_region_merge_node_keep
                    as
                    select node_id
                    from temp_region_merge_node_keep2
                    union
                    select node_id
                    from
                    (
                        select a.node_id, b.link_id
                        from rdb_node as a
                        inner join temp_hierarchy_links_layer4 as b
                        on a.node_id in (b.start_node_id, end_node_id) and b.link_type not in (4,8,9)
                    )as t
                    group by node_id having count(*) > 2;
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        
        self.CreateIndex2('temp_region_merge_node_keep_node_id_idx')
        self.CreateIndex2('temp_region_merge_link_keep_link_id_idx')
        
        self.CreateTable2('temp_region_merge_node_suspect')
        self.CreateIndex2('temp_region_merge_node_suspect_node_id_idx')
        
        self.CreateTable2('temp_region_merge_link_suspect')
        self.CreateIndex2('temp_region_merge_link_suspect_start_node_id_idx')
        self.CreateIndex2('temp_region_merge_link_suspect_end_node_id_idx')
        
        self.pg.commit2()
        
        rdb_log.log('Region', 'Prepare suspect of merging link and node end.', 'info')
    
    def _searchLinkrow(self, layer):
        rdb_log.log('Region', 'Search linkrow for merging link...', 'info')
        
        self.CreateTable2('temp_region_merge_link_walked')
        self.CreateTable2('temp_region_merge_linkrow')
        
        self.CreateIndex2('temp_region_merge_linkrow_start_node_id_idx')
        self.CreateIndex2('temp_region_merge_linkrow_end_node_id_idx')
        self.CreateIndex2('temp_region_merge_linkrow_start_node_id_end_node_id_idx')
        
        self.CreateFunction2('rdb_search_region_linkrow_in_one_direction')
        self.CreateFunction2('rdb_search_region_linkrow')
        
        self.pg.callproc('rdb_search_region_linkrow')
        self.pg.commit2()
        
        self.CreateFunction2('rdb_split_region_linkrow')
        self.pg.execute2('select rdb_split_region_linkrow(%s)' % str(layer))
        self.pg.commit2()

        rdb_log.log('Region', 'Search linkrow for merging link end.', 'info')
    
    def _mergeLinkrow(self):
        rdb_log.log('Region', 'Merge link...', 'info')
        
        #
        self.CreateFunction2('rdb_get_average_attr')
        
        #
        self.CreateTable2('temp_region_merge_link_mapping')
        self.CreateIndex2('temp_region_merge_link_mapping_link_id_idx')
        self.CreateTable2('temp_region_merge_joinlink')
        self.CreateIndex2('temp_region_merge_joinlink_proxy_link_id_idx')
        
        #
        self.CreateTable2('temp_region_merge_links_oldid')
        self.CreateIndex2('temp_region_merge_links_oldid_link_id_idx')
        self.CreateIndex2('temp_region_merge_links_oldid_start_node_id_idx')
        self.CreateIndex2('temp_region_merge_links_oldid_end_node_id_idx')
        self.CreateIndex2('temp_region_merge_links_oldid_the_geom_idx')
        
        self.CreateTable2('temp_region_merge_nodes_oldid')
        self.CreateIndex2('temp_region_merge_nodes_oldid_node_id_idx')
        self.CreateIndex2('temp_region_merge_nodes_oldid_the_geom_idx')
        
        self.pg.commit2()
        
        rdb_log.log('Region', 'Merge link end.', 'info')
    
    def _dispatchNewID(self):
        rdb_log.log('Region', 'Dispatch new region linkid...', 'info')
        
        #
        #self.CreateTable2('temp_region_merge_linkid_mapping')
        #self.CreateTable2('temp_region_merge_nodeid_mapping')
        #self.CreateIndex2('temp_region_merge_linkid_mapping_link_id_idx')
        #self.CreateIndex2('temp_region_merge_linkid_mapping_old_link_id_idx')
        #self.CreateIndex2('temp_region_merge_nodeid_mapping_node_id_idx')
        #self.CreateIndex2('temp_region_merge_nodeid_mapping_old_node_id_idx')
        self.__dispatchNewIDByIDFund()
        self.__dispatchAbsLinkID()
        
        #
        self.CreateFunction2('rdb_region_map_linkrow')
        self.CreateTable2('temp_region_merge_links')
        self.CreateTable2('temp_region_merge_nodes')
        self.CreateTable2('temp_region_merge_regulation')
        self.CreateIndex2('temp_region_merge_links_link_id_idx')
        self.CreateIndex2('temp_region_merge_nodes_node_id_idx')
        self.CreateIndex2('temp_region_merge_regulation_record_no_idx')
        
        rdb_log.log('Region', 'Dispatch new region linkid end.', 'info')
    
    def __dispatchNewIDByIDFund(self):
        # dispatch new id for link
        self.CreateTable2('temp_region_merge_linkid_mapping')
        sqlcmd = """
                    insert into temp_region_merge_linkid_mapping(region_tile_id, seq_num, link_id, old_link_id)
                    select b.tile_id, b.seq_num, b.link_id, a.link_id
                    from temp_region_merge_links_oldid as a
                    left join id_fund_link as b
                    on a.feature_key = b.feature_key and a.region_tile_id = b.tile_id
                    where b.link_id is not null
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_merge_linkid_mapping_link_id_idx')
        self.CreateIndex2('temp_region_merge_linkid_mapping_old_link_id_idx')
        
        sqlcmd = """
                insert into temp_region_merge_linkid_mapping(region_tile_id, seq_num, link_id, old_link_id)
                select  region_tile_id, 
                        (begin_link_id + seq_num) as seq_num, 
                        ((region_tile_id::bigint << 32) | (begin_link_id + seq_num)) as tile_link_id, 
                        link_array[seq_num] as old_link_id
                from
                (
                    select  ta.region_tile_id, link_array, link_count, 
                            (case when tm.max_link_id is null then 0 else max_link_id end) as begin_link_id,
                            generate_series(1, link_count) as seq_num
                    from
                    (
                        select region_tile_id, array_agg(link_id) as link_array, count(link_id) as link_count
                        from
                        (
                            select a.region_tile_id, a.link_id
                            from temp_region_merge_links_oldid as a
                            left join temp_region_merge_linkid_mapping as b
                            on a.link_id = b.old_link_id
                            where b.old_link_id is null
                            order by a.region_tile_id, a.link_id
                        )as a
                        group by region_tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_link_id
                        from id_fund_link
                        group by tile_id
                    )as tm
                    on ta.region_tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # dispatch new id for node
        self.CreateTable2('temp_region_merge_nodeid_mapping')
        sqlcmd = """
                    insert into temp_region_merge_nodeid_mapping(region_tile_id, seq_num, node_id, old_node_id)
                    select b.tile_id, b.seq_num, b.node_id, a.node_id
                    from temp_region_merge_nodes_oldid as a
                    left join id_fund_node as b
                    on a.feature_key = b.feature_key and a.region_tile_id = b.tile_id
                    where b.node_id is not null
                    ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_merge_nodeid_mapping_node_id_idx')
        self.CreateIndex2('temp_region_merge_nodeid_mapping_old_node_id_idx')
        
        sqlcmd = """
                insert into temp_region_merge_nodeid_mapping(region_tile_id, seq_num, node_id, old_node_id)
                select  region_tile_id, 
                        (begin_node_id + seq_num) as seq_num, 
                        ((region_tile_id::bigint << 32) | (begin_node_id + seq_num)) as tile_node_id, 
                        node_array[seq_num] as old_node_id
                from
                (
                    select  ta.region_tile_id, node_array, node_count, 
                            (case when tm.max_node_id is null then 0 else max_node_id end) as begin_node_id,
                            generate_series(1, node_count) as seq_num
                    from
                    (
                        select region_tile_id, array_agg(node_id) as node_array, count(node_id) as node_count
                        from
                        (
                            select a.region_tile_id, a.node_id
                            from temp_region_merge_nodes_oldid as a
                            left join temp_region_merge_nodeid_mapping as b
                            on a.node_id = b.old_node_id
                            where b.old_node_id is null
                            order by a.region_tile_id, a.node_id
                        )as a
                        group by region_tile_id
                    )as ta
                    left join
                    (
                        select tile_id, count(*) as max_node_id
                        from id_fund_node
                        group by tile_id
                    )as tm
                    on ta.region_tile_id = tm.tile_id
                )as t
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update id fund
        sqlcmd = """
                insert into id_fund_link
                            (link_id, tile_id, seq_num, feature_string, feature_key, the_geom, version)
                (
                    select  a.link_id,
                            a.region_tile_id,
                            a.seq_num, 
                            c.feature_string,
                            c.feature_key,
                            c.the_geom,
                            current_database()
                    from temp_region_merge_linkid_mapping as a
                    left join id_fund_link as b
                    on a.link_id = b.link_id
                    left join temp_region_merge_links_oldid as c
                    on a.old_link_id = c.link_id
                    where b.link_id is null
                    order by a.link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
        sqlcmd = """
                insert into id_fund_node
                            (node_id, tile_id, seq_num, feature_string, feature_key, the_geom, version)
                (
                    select  a.node_id,
                            a.region_tile_id,
                            a.seq_num, 
                            c.feature_string,
                            c.feature_key,
                            c.the_geom,
                            current_database()
                    from temp_region_merge_nodeid_mapping as a
                    left join id_fund_node as b
                    on a.node_id = b.node_id
                    left join temp_region_merge_nodes_oldid as c
                    on a.old_node_id = c.node_id
                    where b.node_id is null
                    order by a.node_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __dispatchAbsLinkID(self):
        # dispatch abs-link-id for region link
        self.CreateTable2('temp_region_merge_abslinkid_mapping')
        self.CreateFunction2('rdb_dispatch_region_abslinkid')
        self.pg.callproc('rdb_dispatch_region_abslinkid')
        self.pg.commit2()
        self.CreateIndex2('temp_region_merge_abslinkid_mapping_old_link_id_idx')
        
        # dispatch abs-link-id for base link
        sqlcmd = """
            insert into rdb_link_abs(link_id, abs_link_id, max_level)
            select  low_level_linkids[seq] as link_id,
                    (abs_link_id + seq - 1) as abs_link_id,
                    region_level
            from
            (
                select  a.region_level, a.low_level_linkids, b.abs_link_id, b.abs_link_diff,
                        generate_series(1, b.abs_link_diff + 1) as seq
                from temp_region_merge_links_oldid as a
                inner join temp_region_merge_abslinkid_mapping as b
                on a.link_id = b.old_link_id and b.bNewDispatch is true
            )as t;
            analyze rdb_link_abs;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            insert into temp_region_node_upgrade(node_id)
            select a.node_id
            from temp_region_merge_nodes_oldid as a
            left join temp_region_node_upgrade as b
            on a.node_id = b.node_id
            where b.node_id is null;
            analyze temp_region_node_upgrade;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def _deal_with_layerX_link_lane(self, target_table_name): 
        rdb_log.log('Region', 'start to make link lane', 'info')
        self.CreateFunction2('rdb_calc_lane_dir')
        
        self.CreateTable2('temp_merge_link_lane_info')
        
        sqlcmd = """
         insert into temp_merge_link_lane_info
         (disobey_flag, lane_dir, lane_up_down_distingush, ops_lane_number,
                ops_width, neg_lane_number, neg_width, relavent_links)
         (
                select disobey_flag, lane_dir, lane_up_down_distingush, ops_lane_number,
                ops_width, neg_lane_number, neg_width, array_agg(link_id) from
                (
                    select link_id, disobey_flag, rdb_calc_lane_dir(one_way) as lane_dir,
                    one_way in (2,3) as lane_up_down_distingush,
                    ops_lane_number,
                    ops_width,
                    neg_lane_number,
                    neg_width 
                    from temp_region_merge_links
                ) as a
                group by disobey_flag, lane_dir, lane_up_down_distingush, ops_lane_number,
                ops_width, neg_lane_number, neg_width
                order by disobey_flag, lane_dir, lane_up_down_distingush, ops_lane_number,
                ops_width, neg_lane_number, neg_width
         ); 
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_merge_link_lane_info_lane_id_idx')
        
        #self.CreateTable2('rdb_region_link_lane_info_layer4_tbl')
        sqlcmd = """
        drop table if exists %s;
        create table %s
        as
        (
            select lane_id, disobey_flag, lane_dir, lane_up_down_distingush, ops_lane_number,
                ops_width, neg_lane_number, neg_width
             from temp_merge_link_lane_info
        );
        """ %(target_table_name, target_table_name)
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_merge_link_lane_info_unnest')
        self.CreateIndex2('temp_merge_link_lane_info_unnest_link_id_idx')
        
        rdb_log.log('Region', 'end to make link lane', 'info')
    
    def _MakeRegionLink(self, target_table_name, temp_LayerX_link_with_regulation_status, region_link_addinfo_table, region_cond_speed_table):
        rdb_log.log('Region', 'start to make region link table', 'info')
        sqlcmd = """
        drop table if exists %s CASCADE;
        create table %s
        as
        (
            select a.link_id, region_tile_id as link_id_t, start_node_id,end_node_id, road_type, pdm_flag, 
            one_way, function_code, link_length, link_type, road_name_id, road_name, 
            toll, bypass_flag, highcost_flag,
            (case when b.regulation_exist_state is null then 0 else b.regulation_exist_state end) as regulation_exist_state, 
            fazm_path, tazm_path, 
            (d.link_id is not null) as link_add_info_flag, 
            (e.link_id is not null) as speed_regulation_flag, 
            c.lane_id, 
            (rdb_calc_length_by_unit(link_length::int))[2] as link_length_modify,
            (rdb_calc_length_by_unit(link_length::int))[1] as link_length_unit,
            (case when a.abs_link_diff >= 0 then a.abs_link_id else a.abs_link_id + a.abs_link_diff end) as abs_link_id,
            abs(a.abs_link_diff) as abs_link_diff,
            (case when a.abs_link_diff >= 0 then true else false end) as abs_link_dir,
            a.the_geom
            from temp_region_merge_links as a
            left join %s as b
            on a.link_id = b.link_id
            left join temp_merge_link_lane_info_unnest as c
            on a.link_id = c.link_id
            left join %s as d
            on a.link_id = d.link_id
            left join %s as e
            on a.link_id = e.link_id
        );
        """ %(target_table_name, target_table_name, temp_LayerX_link_with_regulation_status, region_link_addinfo_table, region_cond_speed_table)
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'end to make region link table', 'info')
        
    def _MakeLinkMappingBtnLayer0AndLayerX(self, target_table_name):
        rdb_log.log('Region', 'start to make region link mapping', 'info')
        sqlcmd = """
        drop table if exists %s CASCADE;
        create table %s
        as
        (
            select link_id as region_link_id, low_level_linkids as link_id_14, low_level_linkdirs as link_dir_14 
            from temp_region_merge_links
        )
        """ %(target_table_name, target_table_name)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'end to make region link mapping', 'info')
    
    def _MakeNodeMappingBtnLayer0AndLayerX(self, target_table_name):
        rdb_log.log('Region', 'start to make region node mapping', 'info')
        sqlcmd = """
        drop table if exists %s CASCADE;
        create table %s
        as
        (
            select node_id as region_node_id, low_level_node_id as node_id_14 from temp_region_merge_nodes
        )
        """ %(target_table_name, target_table_name)
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'end to make region node mapping', 'info')
        
    def _MakeRegionLinkRegulationTbl(self, target_table_name):
        rdb_log.log('Region', 'start to make link regulation table', 'info')
        sqlcmd = """
        drop table if exists %s;
        create table %s
        as
        (
            select record_no, regulation_id, regulation_type, is_seasonal, first_link_id, first_link_id_t,
            last_link_id, last_link_dir, last_link_id_t, link_num, key_string
            from temp_region_merge_regulation
            order by record_no
        );
        """ %(target_table_name, target_table_name)
        
        self.pg.execute2(sqlcmd)
        
        self.pg.commit2()
        
        rdb_log.log('Region', 'end to make link regulation table', 'info')
    
    def _makeRegionXLightAndTollnode(self, rdb_region_layerX_link_mapping, target_layerX_light_tollnode_tbl,
                                     rdb_region_layerX_link, rdb_region_layerX_node):
        rdb_log.log('Region','Make region with light and tollnode...', 'info')
        
        sqlcmd = """
        drop table if exists %s CASCADE;
        
        with expand_region_by_linkid_14 as
        (
            select region_link_id, a.link_id_14, b.start_node_id , b.end_node_id, b.one_way
            from
            (
                select region_link_id , unnest(link_id_14) as link_id_14
                from %s
            ) as a
            left join rdb_link as b
            on a.link_id_14 = b.link_id
        )
        select z.region_link_id, (light_cnt - u.node_light_cnt) as light_cnt, toll_cnt
        into %s
        from
        (
            select region_link_id, sum(light_flag) as light_cnt, sum(toll_flag) as toll_cnt
            from
            (
                select region_link_id, ref_node_id, ((extend_flag >> 9) & 1) as light_flag, ((extend_flag >> 10) & 1) as toll_flag
                from
                (
                    select region_link_id, start_node_id as ref_node_id
                    from expand_region_by_linkid_14
                    where one_way in (1,3)
                    union
                    select region_link_id, end_node_id as ref_node_id
                    from expand_region_by_linkid_14
                    where one_way in (1,2)
                ) as m
                left join rdb_node as n
                on m.ref_node_id = n.node_id
            ) as t
            group by region_link_id
        ) as z
        left join
        (
            select region_link_id, sum(node_light_flag) as node_light_cnt
            from
            (
                select region_link_id, ref_node_id , 
                    case when j.signal_flag = true then 1 else 0 end as node_light_flag
                from
                (
                    select link_id as region_link_id, start_node_id as ref_node_id
                    from %s
                    where one_way in (1,3)
                    union
                    select link_id as region_link_id, end_node_id as ref_node_id
                    from %s
                    where one_way in (1,2) 
                ) as w
                left join %s as j
                on w.ref_node_id = j.node_id 
            ) as y 
            group by region_link_id 
        ) as u
        on z.region_link_id = u.region_link_id;
        
        create index %s_region_link_id_idx
            on %s
            using btree
            (region_link_id);
        """ %(target_layerX_light_tollnode_tbl, rdb_region_layerX_link_mapping, target_layerX_light_tollnode_tbl,
              rdb_region_layerX_link, rdb_region_layerX_link, 
              rdb_region_layerX_node,
              target_layerX_light_tollnode_tbl, target_layerX_light_tollnode_tbl)
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region','Make region with light and tollnode end', 'info')
        
    def _makeregion_sequence_sql(self, str_level):
        sqlcmd = """
                drop table if exists temp_rdb_region_layer[level]_link_mapping_seq;
                create table temp_rdb_region_layer[level]_link_mapping_seq
                as
                (
                    select unnest(link_id_14) as link_id, region_link_id
                    from rdb_region_layer[level]_link_mapping
                );
                
                CREATE INDEX temp_rdb_region_layer[level]_link_mapping_seq_link_id_idx
                  ON temp_rdb_region_layer[level]_link_mapping_seq
                  USING btree
                  (link_id);
                 
                insert into rdb_region_link_layer[level]_sequence
                select a.link_id, 0,
                    (case when b.region_link_id is null then -1 else b.region_link_id end), 
                    (case when c.region_link_id is null then -1 else c.region_link_id end)
                from
                (
                    select a.link_id, 
                        (case when s_l.start_node_id = a.s_node then s_seq.s_link_id else s_seq.e_link_id end) as s_sequence,
                        (case when e_l.start_node_id = a.e_node then e_seq.s_link_id else e_seq.e_link_id end) as e_sequence
                    from
                    (
                        select a.link_id, b.link_id_14[1] as s_link, b.link_id_14[array_upper(b.link_id_14,1)] as e_link, 
                            s_node.node_id_14 as s_node, e_node.node_id_14 as e_node
                        from rdb_region_link_layer[level]_tbl as a
                        left join rdb_region_layer[level]_link_mapping as b
                        on a.link_id = b.region_link_id
                        left join rdb_region_layer[level]_node_mapping as s_node
                        on a.start_node_id = s_node.region_node_id
                        left join rdb_region_layer[level]_node_mapping as e_node
                        on a.end_node_id = e_node.region_node_id
                    )a
                    left join rdb_link as s_l
                    on a.s_link = s_l.link_id
                    left join rdb_link as e_l
                    on a.e_link = e_l.link_id
                    left join rdb_link_sequence as s_seq
                    on a.s_link = s_seq.link_id
                    left join rdb_link_sequence as e_seq
                    on a.e_link = e_seq.link_id
                )a
                left join temp_rdb_region_layer[level]_link_mapping_seq as b
                on a.s_sequence = b.link_id
                left join temp_rdb_region_layer[level]_link_mapping_seq as c
                on a.e_sequence = c.link_id;
                """
        return sqlcmd.replace('[level]', str_level)
    
    def _makeRegionLevel4linksequence(self):
        rdb_log.log('Region', 'Make original region link_sequence...', 'info')

        if not self.pg.IsExistTable('rdb_region_link_layer4_tbl'):
            return 0
        
        self.CreateTable2('rdb_region_link_layer4_sequence')
        self.pg.execute2(self._makeregion_sequence_sql('4'))
        self.pg.commit2()
        self.CreateIndex2('rdb_region_link_layer4_sequence_link_id_idx')
                
#        pg_sequence = rdb_link_sequence.rdb_linksequence_info(self,'rdb_region_link_layer4_tbl',
#                                                              'rdb_region_node_layer4_tbl',
#                                                              'temp_region_link_layer4_sequence_dir',
#                                                              'rdb_region_link_layer4_sequence',
#                                                              'rdb_region_link_regulation_layer4_tbl')
#        if pg_sequence._CreateTable() == -1:
#            return -1
#        
#        if pg_sequence._Get_road_sequence() == -1:
#            return -1

        return 0 
    
    def _makeRegionLevel6linksequence(self):
        rdb_log.log('Region', 'Make original region link_sequence...', 'info')

        if not self.pg.IsExistTable('rdb_region_link_layer6_tbl'):
            return 0
        
        self.CreateTable2('rdb_region_link_layer6_sequence')
        self.pg.execute2(self._makeregion_sequence_sql('6'))
        self.pg.commit2()
        self.CreateIndex2('rdb_region_link_layer6_sequence_link_id_idx')
                
#        pg_sequence = rdb_link_sequence.rdb_linksequence_info(self,'rdb_region_link_layer6_tbl',
#                                                              'rdb_region_node_layer6_tbl',
#                                                              'temp_region_link_layer6_sequence_dir',
#                                                              'rdb_region_link_layer6_sequence',
#                                                              'rdb_region_link_regulation_layer6_tbl')
#        if pg_sequence._CreateTable() == -1:
#            return -1  
#           
#        if pg_sequence._Get_road_sequence() == -1:
#            return -1

        return 0
    
    def _makeRegionLevel8linksequence(self):
        rdb_log.log('Region', 'Make original region link_sequence...', 'info')
        
        if not self.pg.IsExistTable('rdb_region_link_layer8_tbl'):
            return 0
        
        self.CreateTable2('rdb_region_link_layer8_sequence')
        self.pg.execute2(self._makeregion_sequence_sql('8'))
        self.pg.commit2()
        self.CreateIndex2('rdb_region_link_layer8_sequence_link_id_idx')
        
#        pg_sequence = rdb_link_sequence.rdb_linksequence_info(self,'rdb_region_link_layer8_tbl',
#                                                              'rdb_region_node_layer8_tbl',
#                                                              'temp_region_link_layer8_sequence_dir',
#                                                              'rdb_region_link_layer8_sequence',
#                                                              'rdb_region_link_regulation_layer8_tbl')
#        if pg_sequence._CreateTable() == -1:
#            return -1
#          
#        if pg_sequence._Get_road_sequence() == -1:
#            return -1

        return 0 
    
    def _makeTargetRegionDataLevel6(self):
        # input table:   "temp_region_merge_links", "temp_region_merge_nodes", "temp_region_merge_regulation"
        rdb_log.log('Region', 'start to create region level 6 target table', 'info')
        
        self.CreateFunction2('rdb_split_tileid')
        self.CreateFunction2('rdb_split_to_tileid')
        
        self.CreateTable2('temp_region_layerx_link_with_regulation_status')
        self.CreateIndex2('temp_region_layerx_link_with_regulation_status_linkid_idx')
        
        # step three: create regulation table 
        #self.CreateTable2('rdb_region_link_regulation_layer4_tbl')
        self._MakeRegionLinkRegulationTbl('rdb_region_link_regulation_layer6_tbl')
        self.CreateIndex2('rdb_region_link_regulation_layer6_tbl_first_link_id_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer6_tbl_first_link_id_t_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer6_tbl_last_link_id_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer6_tbl_last_link_id_t_idx')
        
        # make link lane 
        self._deal_with_layerX_link_lane('rdb_region_link_lane_info_layer6_tbl')
        self.CreateIndex2('rdb_region_link_lane_info_layer6_tbl_lane_id_idx')
        
        # make link add info
        self.CreateTable2('rdb_region_link_add_info_layer6_tbl')
        self.CreateIndex2('rdb_region_link_add_info_layer6_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_link_add_info_layer6_tbl_link_id_t_idx')
        self.pg.commit2()
        
        # make cond speed
        self.CreateTable2('rdb_region_cond_speed_layer6_tbl')
        self.CreateIndex2('rdb_region_cond_speed_layer6_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_cond_speed_layer6_tbl_link_id_t_idx')
        self.pg.commit2()
        
        # step one: create link table
        self.CreateFunction2('rdb_calc_length_by_unit')
        #self.CreateTable2('rdb_region_link_layer4_tbl')
        self._MakeRegionLink('rdb_region_link_layer6_tbl', 
                              'temp_region_layerx_link_with_regulation_status', 
                              'rdb_region_link_add_info_layer6_tbl',
                              'rdb_region_cond_speed_layer6_tbl')
        self.CreateIndex2('rdb_region_link_layer6_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_link_layer6_tbl_link_id_t_idx')
        
        # create high level node branch node info
        self.CreateFunction2('rdb_get_specify_link_idx')
        self.CreateTable2('temp_region_node_with_branch_link_info')
        self.CreateTable2('temp_region_node_layerx_boundary_flag')
        self.pg.commit2()
        
        # step four: make link mapping table between layer 4 and layer0
        #self.CreateTable2('rdb_region_layer4_link_mapping_tbl')
        self._MakeLinkMappingBtnLayer0AndLayerX('rdb_region_layer6_link_mapping')
        
        # step five: make node mapping table between layer 4 and layer 0
        #self.CreateTable2('rdb_region_layer4_node_mapping_tbl')
        self._MakeNodeMappingBtnLayer0AndLayerX('rdb_region_layer6_node_mapping')
        self.CreateIndex2('rdb_region_layer6_node_mapping_node_id_14_idx')
        
        # step two: create node table, with boundary_flag and branch links
        self.CreateFunction2('rdb_integer_2_octal_bytea')
        self.CreateTable2('rdb_region_node_layer6_tbl')
        #self._MakeRegionNodeTbl('rdb_region_node_layer4_tbl')
        self.pg.commit2()
        self.CreateIndex2('rdb_region_node_layer6_tbl_node_id_idx')
        self.CreateIndex2('rdb_region_node_layer6_tbl_node_id_t_idx')
        
        self._makeRegionXLightAndTollnode('rdb_region_layer6_link_mapping', 'rdb_region_layer6_light_tollnode_tbl',
                                          'rdb_region_link_layer6_tbl','rdb_region_node_layer6_tbl')
        self._makeRegionLevel6linksequence()
        
        rdb_log.log('Region', 'end to create region level 6 target table', 'info')
        
    def _makeTargetRegionDataLevel4(self):
        # input table:   "temp_region_merge_links", "temp_region_merge_nodes", "temp_region_merge_regulation"
        rdb_log.log('Region', 'start to create region level 4 target table', 'info')
        
        self.CreateFunction2('rdb_split_tileid')
        self.CreateFunction2('rdb_split_to_tileid')
        
        self.CreateTable2('temp_region_layerx_link_with_regulation_status')
        self.CreateIndex2('temp_region_layerx_link_with_regulation_status_linkid_idx')
        
        self._MakeRegionLinkRegulationTbl('rdb_region_link_regulation_layer4_tbl')
        self.CreateIndex2('rdb_region_link_regulation_layer4_tbl_first_link_id_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer4_tbl_first_link_id_t_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer4_tbl_last_link_id_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer4_tbl_last_link_id_t_idx')
        
        # make link lane 
        self._deal_with_layerX_link_lane('rdb_region_link_lane_info_layer4_tbl')
        self.CreateIndex2('rdb_region_link_lane_info_layer4_tbl_lane_id_idx')
        
        # make link add info
        self.CreateTable2('rdb_region_link_add_info_layer4_tbl')
        self.CreateIndex2('rdb_region_link_add_info_layer4_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_link_add_info_layer4_tbl_link_id_t_idx')
        self.pg.commit2()
        
        # make cond speed
        self.CreateTable2('rdb_region_cond_speed_layer4_tbl')
        self.CreateIndex2('rdb_region_cond_speed_layer4_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_cond_speed_layer4_tbl_link_id_t_idx')
        self.pg.commit2()
        
        # step one: create link table
        self.CreateFunction2('rdb_calc_length_by_unit')
        self._MakeRegionLink('rdb_region_link_layer4_tbl', 
                              'temp_region_layerx_link_with_regulation_status', 
                              'rdb_region_link_add_info_layer4_tbl',
                              'rdb_region_cond_speed_layer4_tbl')
        self.CreateIndex2('rdb_region_link_layer4_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_link_layer4_tbl_link_id_t_idx')
        
        # create high level node branch node info
        self.CreateFunction2('rdb_get_specify_link_idx')
        self.CreateTable2('temp_region_node_with_branch_link_info')
        self.CreateTable2('temp_region_node_layerx_boundary_flag')
        self.pg.commit2()
        

        self._MakeLinkMappingBtnLayer0AndLayerX('rdb_region_layer4_link_mapping')
        self._MakeNodeMappingBtnLayer0AndLayerX('rdb_region_layer4_node_mapping')
        self.CreateIndex2('rdb_region_layer4_node_mapping_node_id_14_idx')
        
        # step two: create node table, with boundary_flag and branch links
        self.CreateFunction2('rdb_integer_2_octal_bytea')
        self.CreateTable2('rdb_region_node_layer4_tbl')
        self.pg.commit2()
        self.CreateIndex2('rdb_region_node_layer4_tbl_node_id_idx')
        self.CreateIndex2('rdb_region_node_layer4_tbl_node_id_t_idx')
        
        self._makeRegionXLightAndTollnode('rdb_region_layer4_link_mapping', 'rdb_region_layer4_light_tollnode_tbl',
                                          'rdb_region_link_layer4_tbl','rdb_region_node_layer4_tbl')
        self._makeRegionLevel4linksequence()
        
        rdb_log.log('Region', 'end to create region level 4 target table', 'info')


class rdb_region_ipc_japen(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 10, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevel6(self, 2, [1,2])

class rdb_region_axf_china(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 6, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self, zlevel=2, fc_array=[1,2]):
        rdb_log.log('Region', 'Make original region level6...', 'info')
        
        #
        self.CreateFunction2('get_new_tile_id_in_china_level6')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level6;
                    create table temp_region_orglink_level6
                    as
                    (
                        select *, get_new_tile_id_in_china_level6(link_id_t,%s) as region_tile_id, 6 as region_level
                        from rdb_link 
                        where function_code = ANY(ARRAY%s) and one_way != 0 and road_type not in (7,8,9,14)
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orglink_level6_link_id_idx')
        self.CreateIndex2('temp_region_orglink_level6_start_node_id_idx')
        self.CreateIndex2('temp_region_orglink_level6_end_node_id_idx')
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level6;
                    create table temp_region_orgnode_level6
                    as
                    (
                        select c.*, get_new_tile_id_in_china_level6(node_id_t,%s) as region_tile_id
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level6
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level6
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orgnode_level6_node_id_idx')
        
        rdb_log.log('Region', 'Make original region level6 end.', 'info')

class rdb_region_ni_china(rdb_region_axf_china):
    def _hierarchy(self):
        import rdb_region_algorithm
        objHierarchy = rdb_region_algorithm.CHierarchy()
        
        objHierarchy.make(org_link_table='rdb_link',
                          org_link_filter='one_way != 0 and road_type not in (7,8,9,14)',
                          target_link_table='temp_hierarchy_links_layer3',
                          base_level=12,
                          max_level=14,
                          mesh_road_limit=600)
        
        objHierarchy.make(org_link_table='temp_hierarchy_links_layer3',
                          org_link_filter='true',
                          target_link_table='temp_hierarchy_links_layer4',
                          base_level=9,
                          max_level=11,
                          mesh_road_limit=800,
                          overwrap=-1)
        
#        objHierarchy.make(org_link_table='temp_hierarchy_links_layer4',
#                          org_link_filter='true',
#                          target_link_table='temp_hierarchy_links_layer6',
#                          base_level=6,
#                          max_level=8,
#                          mesh_road_limit=1000,
#                          overwrap=-1)
#        
#        objHierarchy.make(org_link_table='temp_hierarchy_links_layer6',
#                          org_link_filter='true',
#                          target_link_table='temp_hierarchy_links_layer8',
#                          base_level=4,
#                          max_level=6,
#                          mesh_road_limit=1200,
#                          overwrap=-1)
    
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevelX(self,
                                               layer=4,
                                               level=10,
                                               rdb_link='rdb_link', 
                                               attr_filter={'function_code':[1,2,3]},
                                               additional_links='temp_hierarchy_links_layer4')
    
class rdb_region_ta_europe(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 10, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevel6(self, 6, [1])

class rdb_region_ta_vietnam(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 6, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevel6(self, 2, [1])

class rdb_region_rdf_ap(rdb_region):
    def _makeOriginalRegionOnLevel4(self, zlevel=6, fc_array=[1,2,3,4]):
        rdb_region._makeOriginalRegionOnLevel4(self, zlevel, fc_array)
        self._addFerryIntoRegion(zlevel, fc_array)
    
    def _addFerryIntoRegion(self, zlevel=6, fc_array=[1,2,3,4]):
        # find ferry link and dock
        self.log.info('find ferry link and dock...')
        sqlcmd = """
                drop table if exists temp_region_promote_ferry;
                create table temp_region_promote_ferry
                as
                select * from rdb_link 
                where road_type = 10 and one_way != 0 and not (function_code = ANY(ARRAY%s))
                ;
                
                create index temp_region_promote_ferry_link_id_idx
                    on temp_region_promote_ferry
                    using btree
                    (link_id);
                """ % str(fc_array)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                drop table if exists temp_region_rdb_node_dock;
                create table temp_region_rdb_node_dock
                as
                select distinct a.node_id
                from
                (
                    select start_node_id as node_id from temp_region_promote_ferry
                    union
                    select end_node_id as node_id from temp_region_promote_ferry
                )as a
                inner join rdb_link as b
                on a.node_id in (b.start_node_id, b.end_node_id) and b.road_type != 10
                ;
                
                create index temp_region_rdb_node_dock_node_id_idx
                    on temp_region_rdb_node_dock
                    using btree
                    (node_id);
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
    
        #
        self.CreateTable2('temp_region_promote_path')
        
        self.log.info('routing from dock to dock...')
        sqlcmd = "select node_id from temp_region_rdb_node_dock"
        self.pg.execute(sqlcmd)
        node_rec_list = self.pg.fetchall2()
        for [node_id] in node_rec_list:
            import rdb_region_algorithm
            graph = rdb_region_algorithm.CGraph_PG()
            graph.prepareData()
            forward_paths = graph.searchFromDockToDock(node_id, True)
            backward_paths = graph.searchFromDockToDock(node_id, False)
            #print 'node_id =', node_id, 'paths =', paths
            graph.clearData()
            
            for (to_node_id, path) in forward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
            for (to_node_id, path) in backward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
        self.pg.commit2()
        
        self.log.info('routing from dock to region...')
        sqlcmd = "select node_id from temp_region_rdb_node_dock"
        self.pg.execute(sqlcmd)
        node_rec_list = self.pg.fetchall2()
        for [node_id] in node_rec_list:
            import rdb_region_algorithm
            graph = rdb_region_algorithm.CGraph_PG()
            graph.prepareData()
            forward_paths = graph.searchFromDockToRegion(node_id, True)
            backward_paths = graph.searchFromDockToRegion(node_id, False)
            #print 'node_id =', node_id, 'paths =', paths
            graph.clearData()
            
            for (to_node_id, path) in forward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
            for (to_node_id, path) in backward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
        self.pg.commit2()
        
        # add ferry and related roads into region
        self.log.info('add ferry and related roads into region...')
        self.CreateTable2('temp_region_promote_link')
        sqlcmd = """
                insert into temp_region_promote_link
                select distinct link_id
                from
                (
                    select unnest(link_array) as link_id
                    from temp_region_promote_path
                    union
                    select link_id
                    from temp_region_promote_ferry
                )as t
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orglink_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.link_id_t,%s) as region_tile_id 
                        from temp_region_promote_link as a
                        left join temp_region_orglink_level4 as b
                        on a.link_id = b.link_id
                        left join rdb_link as c
                        on a.link_id = c.link_id
                        where b.link_id is null
                    );
                """ % (str(zlevel))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orgnode_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.node_id_t,%s) as region_tile_id 
                        from
                        (
                            select unnest(ARRAY[b.start_node_id, b.end_node_id]) as node_id
                            from temp_region_promote_link as a
                            left join rdb_link as b
                            on a.link_id = b.link_id
                        )as a
                        left join temp_region_orgnode_level4 as b
                        on a.node_id = b.node_id
                        left join rdb_node as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
    
    def _makeOriginalRegionOnLevel6(self, zlevel=2, fc_array=[1,2,3]):
        rdb_region._makeOriginalRegionOnLevel6(self, zlevel, fc_array)
    
    def _resetLinkFunctionClass(self):
        """
        # add some slow links into region.
        # it will promote the connectivity of region network.
        """
        pass
#        rdb_log.log('Region', 'Reset link function class begin...', 'info')
#        
#        self.CreateTable2('temp_region_promote_link_fc')
#        self.CreateFunction2('rdb_promote_link_fc_by_same_name')
#        
#        rdb_log.log('Region', 'Reset link function class fc=1 ...', 'info')
#        self.pg.execute('select rdb_promote_link_fc_by_same_name(1);')
#        self.pg.commit2()
#        
#        rdb_log.log('Region', 'Reset link function class fc=2 ...', 'info')
#        self.pg.execute('select rdb_promote_link_fc_by_same_name(2);')
#        self.pg.commit2()
#        
#        rdb_log.log('Region', 'Reset link function class fc=3 ...', 'info')
#        self.pg.execute('select rdb_promote_link_fc_by_same_name(3);')
#        self.pg.commit2()
#        
#        rdb_log.log('Region', 'Reset link function class end.', 'info')

class rdb_region_rdf_sea(rdb_region):
    def _hierarchy(self):
        import rdb_region_algorithm
        objHierarchy = rdb_region_algorithm.CHierarchy()
        
        objHierarchy.make(org_link_table='rdb_link', 
                          org_link_filter='one_way != 0 and road_type not in (7,8,9,14)',
                          target_link_table='temp_hierarchy_links_layer2', 
                          base_level=12,
                          max_level=14,
                          mesh_road_limit=600)
        
        objHierarchy.make(org_link_table='temp_hierarchy_links_layer2',
                          org_link_filter='true',
                          target_link_table='temp_hierarchy_links_layer3',
                          base_level=10,
                          max_level=13,
                          mesh_road_limit=800,
                          overwrap=1)
        
        objHierarchy.make(org_link_table='temp_hierarchy_links_layer3',
                          org_link_filter='true',
                          target_link_table='temp_hierarchy_links_layer4',
                          base_level=9,
                          max_level=11,
                          mesh_road_limit=800,
                          overwrap=-1)
        
        objHierarchy.make(org_link_table='temp_hierarchy_links_layer4',
                          org_link_filter='true',
                          target_link_table='temp_hierarchy_links_layer6',
                          base_level=6,
                          max_level=8,
                          mesh_road_limit=1000,
                          overwrap=-1)
    
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevelX(self, 
                                               layer=4, 
                                               level=10, 
                                               rdb_link='rdb_link', 
                                               attr_filter={'function_code':[1,2,3,4]},
                                               additional_links='temp_hierarchy_links_layer6')
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevelX(self, 
                                               layer=6, 
                                               level=6, 
                                               rdb_link='rdb_link', 
                                               attr_filter={'function_code':[1,2,3]},
                                               additional_links='temp_hierarchy_links_layer6')

class rdb_region_taiwan(rdb_region_rdf_ap):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region_rdf_ap._makeOriginalRegionOnLevel4(self, 8, [1,2,3,4])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region_rdf_ap._makeOriginalRegionOnLevel6(self, 2, [1,2])

class rdb_region_three_layers(rdb_region):
    def _makeOriginalRegionOnLevel4(self, zlevel=10, fc_array=[1,2,3]):
        rdb_region._makeOriginalRegionOnLevel4(self, zlevel, fc_array)
    
    def _makeOriginalRegionOnLevel6(self, zlevel=6, fc_array=[1,2]):
        rdb_region._makeOriginalRegionOnLevel6(self, zlevel, fc_array)
    
    def _makeOriginalRegionOnLevel8(self, zlevel=2, fc_array=[1]):
        rdb_log.log('Region', 'Make original region level8...', 'info')
        
        #
        self.CreateFunction2('get_new_tile_id_by_zlevel')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level8;
                    create table temp_region_orglink_level8
                    as
                    (
                        select *, get_new_tile_id_by_zlevel(link_id_t,%s) as region_tile_id, 8 as region_level
                        from rdb_link 
                        where function_code = ANY(ARRAY%s) and one_way != 0 and road_type not in (7,8,9,14)
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orglink_level8_link_id_idx')
        self.CreateIndex2('temp_region_orglink_level8_start_node_id_idx')
        self.CreateIndex2('temp_region_orglink_level8_end_node_id_idx')
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level8;
                    create table temp_region_orgnode_level8
                    as
                    (
                        select c.*, get_new_tile_id_by_zlevel(node_id_t,%s) as region_tile_id, 8 as region_level
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level8
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level8
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orgnode_level8_node_id_idx')
        
        rdb_log.log('Region', 'Make original region level8 end.', 'info')
    
    def _makeRegionLevel8(self):
        self._makeOriginalRegionOnLevel8()
        self._createTempRegion("temp_region_orglink_level8", "temp_region_orgnode_level8")
        self._deleteSALink(8)
        self._deleteUTurnLink()
        self._deleteIsolatedIC()
        self._deleteIsolatedLink()
        self._mergeRegionLink(8)
        self._makeTargetRegionDataLevel8()
    
    def _makeTargetRegionDataLevel8(self):
        # input table:   "temp_region_merge_links", "temp_region_merge_nodes", "temp_region_merge_regulation"
        rdb_log.log('Region', 'start to create region level 8 target table', 'info')
        
        self.CreateFunction2('rdb_split_tileid')
        self.CreateFunction2('rdb_split_to_tileid')
        
        self.CreateTable2('temp_region_layerx_link_with_regulation_status')
        self.CreateIndex2('temp_region_layerx_link_with_regulation_status_linkid_idx')
        
        # step three: create regulation table 
        #self.CreateTable2('rdb_region_link_regulation_layer4_tbl')
        self._MakeRegionLinkRegulationTbl('rdb_region_link_regulation_layer8_tbl')
        self.CreateIndex2('rdb_region_link_regulation_layer8_tbl_first_link_id_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer8_tbl_first_link_id_t_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer8_tbl_last_link_id_idx')
        self.CreateIndex2('rdb_region_link_regulation_layer8_tbl_last_link_id_t_idx')
        
        # make link lane 
        self._deal_with_layerX_link_lane('rdb_region_link_lane_info_layer8_tbl')
        self.CreateIndex2('rdb_region_link_lane_info_layer8_tbl_lane_id_idx')
        
        # make link add info
        self.CreateTable2('rdb_region_link_add_info_layer8_tbl')
        self.CreateIndex2('rdb_region_link_add_info_layer8_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_link_add_info_layer8_tbl_link_id_t_idx')
        self.pg.commit2()
        
        # make cond speed
        self.CreateTable2('rdb_region_cond_speed_layer8_tbl')
        self.CreateIndex2('rdb_region_cond_speed_layer8_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_cond_speed_layer8_tbl_link_id_t_idx')
        self.pg.commit2()
        
        # step one: create link table
        self.CreateFunction2('rdb_calc_length_by_unit')
        #self.CreateTable2('rdb_region_link_layer4_tbl')
        self._MakeRegionLink('rdb_region_link_layer8_tbl', 
                              'temp_region_layerx_link_with_regulation_status', 
                              'rdb_region_link_add_info_layer8_tbl',
                              'rdb_region_cond_speed_layer8_tbl')
        self.CreateIndex2('rdb_region_link_layer8_tbl_link_id_idx')
        self.CreateIndex2('rdb_region_link_layer8_tbl_link_id_t_idx')
        
        # create high level node branch node info
        self.CreateFunction2('rdb_get_specify_link_idx')
        self.CreateTable2('temp_region_node_with_branch_link_info')
        self.CreateTable2('temp_region_node_layerx_boundary_flag')
        self.pg.commit2()
        
        # step four: make link mapping table between layer 4 and layer0
        #self.CreateTable2('rdb_region_layer4_link_mapping_tbl')
        self._MakeLinkMappingBtnLayer0AndLayerX('rdb_region_layer8_link_mapping')
        
        # step five: make node mapping table between layer 4 and layer 0
        #self.CreateTable2('rdb_region_layer4_node_mapping_tbl')
        self._MakeNodeMappingBtnLayer0AndLayerX('rdb_region_layer8_node_mapping')
        self.CreateIndex2('rdb_region_layer8_node_mapping_node_id_14_idx')
        
        # step two: create node table, with boundary_flag and branch links
        self.CreateFunction2('rdb_integer_2_octal_bytea')
        self.CreateTable2('rdb_region_node_layer8_tbl')
        #self._MakeRegionNodeTbl('rdb_region_node_layer4_tbl')
        self.pg.commit2()
        self.CreateIndex2('rdb_region_node_layer8_tbl_node_id_idx')
        self.CreateIndex2('rdb_region_node_layer8_tbl_node_id_t_idx')
        
        self._makeRegionXLightAndTollnode('rdb_region_layer8_link_mapping', 'rdb_region_layer8_light_tollnode_tbl',
                                          'rdb_region_link_layer8_tbl','rdb_region_node_layer8_tbl')
        self._makeRegionLevel8linksequence()
        
        rdb_log.log('Region', 'end to create region level 8 target table', 'info')
    
    def _makeTargetRegionDataLevel6(self):
        rdb_region._makeTargetRegionDataLevel6(self)
        
        # update upgrade_node_id
        self.log.info('updating upgrade_node_id of rdb_region_node_layer6_tbl...')
        sqlcmd = """
                update rdb_region_node_layer6_tbl as a set upgrade_node_id = b.layer8_node_id
                from
                (
                    select m.region_node_id as layer6_node_id, n.region_node_id as layer8_node_id
                    from rdb_region_layer6_node_mapping as m
                    left join rdb_region_layer8_node_mapping as n
                    on m.node_id_14= n.node_id_14
                )as b
                where a.node_id = b.layer6_node_id;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

class rdb_region_rdf_north_america(rdb_region_three_layers):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel4(self, 10, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel6(self, 6, [1,2])
    
    def _makeOriginalRegionOnLevel8(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel8(self, 2, [1])

class rdb_region_rdf_argentina(rdb_region_three_layers):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel4(self, 10, [1,2,3,4,5])
        self._deleteSomeUrbanRoad()
    
    def _deleteSomeUrbanRoad(self):
        self.log.info('delete some urban road...')
        sqlcmd = """
                delete from temp_region_orglink_level4 as a
                using 
                (
                    select link_id
                    from rdb_link_add_info
                    where (path_extra_info & (1::smallint << 3) != 0) --urban road
                )as b
                where (function_code = 5) and (a.link_id = b.link_id);
                analyze temp_region_orglink_level4;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                delete from temp_region_orgnode_level4 as a
                using 
                (
                    select a.node_id
                    from temp_region_orgnode_level4 as a
                    left join temp_region_orglink_level4 as b
                    on a.node_id in (b.start_node_id, b.end_node_id)
                    where b.link_id is null
                )as b
                where a.node_id = b.node_id;
                analyze temp_region_orgnode_level4;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel6(self, 6, [1,2,3,4])
    
    def _makeOriginalRegionOnLevel8(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel8(self, 2, [1])

class rdb_region_ta_aus(rdb_region_three_layers):
    def _hierarchy(self):
        import rdb_region_algorithm
        objHierarchy = rdb_region_algorithm.CHierarchy()
        
        objHierarchy.make(org_link_table='rdb_link', 
                          org_link_filter='one_way != 0 and road_type not in (7,8,9,14)',
                          target_link_table='temp_hierarchy_links_layer3', 
                          base_level=12,
                          max_level=14,
                          mesh_road_limit=600)
        
        objHierarchy.make(org_link_table='temp_hierarchy_links_layer3', 
                          org_link_filter='one_way != 0 and road_type not in (7,8,9,14)',
                          target_link_table='temp_hierarchy_links_layer4', 
                          base_level=9,
                          max_level=11,
                          mesh_road_limit=800,
                          overwrap=-1)
        
        objHierarchy.make(org_link_table='rdb_link', 
                          org_link_filter='function_code in (1,2,3)',
                          target_link_table='temp_hierarchy_links_layer6', 
                          base_level=8,
                          max_level=10,
                          mesh_road_limit=1000)
    
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevelX(self, 
                                               layer=4, 
                                               level=10, 
                                               rdb_link='rdb_link', 
                                               attr_filter={'function_code':[1,2,3]},
                                               additional_links='temp_hierarchy_links_layer4')
    
    def _makeOriginalRegionOnLevel6(self):
        #rdb_region_three_layers._makeOriginalRegionOnLevel6(self, 6, [1,2,3])
        rdb_region._makeOriginalRegionOnLevelX(self, 
                                               layer=6, 
                                               level=6, 
                                               rdb_link='rdb_link', 
                                               attr_filter={'function_code':[1,2]},
                                               additional_links='temp_hierarchy_links_layer6')
    
    def _makeOriginalRegionOnLevel8(self):
        rdb_region_three_layers._makeOriginalRegionOnLevel8(self, 2, [1])

class rdb_region_rdf_brazil(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 8, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevel6(self, 4, [1])

class rdb_region_nostra_thailand(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 6, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevel6(self, 2, [1,2])
    
    def _deleteUTurnLink(self):
        # drop u-turn link in region level6
        rdb_log.log('Region', 'Delete uturn link from original region links...', 'info')
        
        self.CreateFunction2('rdb_detect_parallel_angle')
        self.CreateFunction2('rdb_is_uturn_link_for_thailand')
        sqlcmd = """
                drop table if exists temp_region_links_uturn;
                create temp table temp_region_links_uturn
                as
                select link_id 
                from temp_region_links
                where (link_type in (4,8,13) or uturn = 1) and rdb_is_uturn_link_for_thailand(link_id, link_length);
                create index temp_region_links_uturn_link_id_idx
                    on temp_region_links_uturn
                    using btree
                    (link_id);
                
                delete from temp_region_links as a
                using temp_region_links_uturn as b
                where a.link_id = b.link_id and a.region_level >= a.max_level;
                analyze temp_region_links;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    delete from temp_region_nodes as a
                    using 
                    (   
                        select a.node_id
                        from temp_region_nodes as a
                        left join temp_region_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                        where b.link_id is null
                    ) as b
                    where a.node_id = b.node_id;
                    analyze temp_region_nodes;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('Region', 'Delete uturn link from original region links end.', 'info')

class rdb_region_mmi_india(rdb_region):
    def _makeOriginalRegionOnLevel4(self):
        rdb_region._makeOriginalRegionOnLevel4(self, 6, [1,2,3])
    
    def _makeOriginalRegionOnLevel6(self):
        rdb_region._makeOriginalRegionOnLevel6(self, 2, [1,2])
        
class rdb_region_malsing_mys(rdb_region):
    def _makeOriginalRegionOnLevel4(self, zlevel=6, fc_array=[1,2,3]):     
        rdb_log.log('Region', 'Make original region level4...', 'info')

        self.CreateFunction2('get_new_tile_id_by_zlevel')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level4;
                    create table temp_region_orglink_level4
                    as
                    (
                        select *, get_new_tile_id_by_zlevel(link_id_t,%s) as region_tile_id 
                        from rdb_link 
                        where function_code = ANY(ARRAY%s) and one_way != 0 and road_type not in (7,8,9,14)                  
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
               
        self.CreateIndex2('temp_region_orglink_level4_link_id_idx')
        self.CreateIndex2('temp_region_orglink_level4_start_node_id_idx')
        self.CreateIndex2('temp_region_orglink_level4_end_node_id_idx')
        
        self._addLowRoadForRegion4And6(zlevel)
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level4;
                    create table temp_region_orgnode_level4
                    as
                    (
                        select c.*, get_new_tile_id_by_zlevel(node_id_t,%s) as region_tile_id 
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level4
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level4
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orgnode_level4_node_id_idx')
        
        rdb_log.log('Region', 'Make original region level4 end.', 'info')
        
        self._addFerryIntoRegion(zlevel, fc_array)
        self._addLowFCHighwayIntoRegion(zlevel, fc_array)
        self._addConnectorIntoRegion(zlevel, fc_array)
        
    def _addFerryIntoRegion(self, zlevel=6, fc_array=[1,2,3]):
        # find ferry link and dock
        self.log.info('find ferry link and dock...')
        sqlcmd = """
                drop table if exists temp_region_promote_ferry;
                create table temp_region_promote_ferry
                as
                select * from rdb_link 
                where road_type = 10 and one_way != 0 and not (function_code = ANY(ARRAY%s))
                ;
                
                create index temp_region_promote_ferry_link_id_idx
                    on temp_region_promote_ferry
                    using btree
                    (link_id);
                """ % str(fc_array)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                drop table if exists temp_region_rdb_node_dock;
                create table temp_region_rdb_node_dock
                as
                select distinct a.node_id
                from
                (
                    select start_node_id as node_id from temp_region_promote_ferry
                    union
                    select end_node_id as node_id from temp_region_promote_ferry
                )as a
                inner join rdb_link as b
                on a.node_id in (b.start_node_id, b.end_node_id) and b.road_type != 10
                ;
                
                create index temp_region_rdb_node_dock_node_id_idx
                    on temp_region_rdb_node_dock
                    using btree
                    (node_id);
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
    
        #
        self.CreateTable2('temp_region_promote_path')
        
        self.log.info('routing from dock to dock...')
        sqlcmd = "select node_id from temp_region_rdb_node_dock"
        self.pg.execute(sqlcmd)
        node_rec_list = self.pg.fetchall2()
        for [node_id] in node_rec_list:
            import rdb_region_algorithm
            graph = rdb_region_algorithm.CGraph_PG()
            graph.prepareData()
            forward_paths = graph.searchFromDockToDock(node_id, True)
            backward_paths = graph.searchFromDockToDock(node_id, False)
            #print 'node_id =', node_id, 'paths =', paths
            graph.clearData()
            
            for (to_node_id, path) in forward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
            for (to_node_id, path) in backward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
        self.pg.commit2()
        
        self.log.info('routing from dock to region...')
        sqlcmd = "select node_id from temp_region_rdb_node_dock"
        self.pg.execute(sqlcmd)
        node_rec_list = self.pg.fetchall2()
        for [node_id] in node_rec_list:
            import rdb_region_algorithm
            graph = rdb_region_algorithm.CGraph_PG()
            graph.prepareData()
            forward_paths = graph.searchFromDockToRegion(node_id, True)
            backward_paths = graph.searchFromDockToRegion(node_id, False)
            #print 'node_id =', node_id, 'paths =', paths
            graph.clearData()
            
            for (to_node_id, path) in forward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
            for (to_node_id, path) in backward_paths:
                self.pg.insert(
                               'temp_region_promote_path',
                               ('from_node_id', 'to_node_id', 'link_array'),
                               (node_id, to_node_id, path)
                               )
        self.pg.commit2()
        
        # add ferry and related roads into region
        self.log.info('add ferry and related roads into region...')
        self.CreateTable2('temp_region_promote_link')
        sqlcmd = """
                insert into temp_region_promote_link
                select distinct link_id
                from
                (
                    select unnest(link_array) as link_id
                    from temp_region_promote_path
                    union
                    select link_id
                    from temp_region_promote_ferry
                )as t
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orglink_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.link_id_t,%s) as region_tile_id 
                        from temp_region_promote_link as a
                        left join temp_region_orglink_level4 as b
                        on a.link_id = b.link_id
                        left join rdb_link as c
                        on a.link_id = c.link_id
                        where b.link_id is null
                    );
                """ % (str(zlevel))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orgnode_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.node_id_t,%s) as region_tile_id 
                        from
                        (
                            select unnest(ARRAY[b.start_node_id, b.end_node_id]) as node_id
                            from temp_region_promote_link as a
                            left join rdb_link as b
                            on a.link_id = b.link_id
                        )as a
                        left join temp_region_orgnode_level4 as b
                        on a.node_id = b.node_id
                        left join rdb_node as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
    
    def _addLowFCHighwayIntoRegion(self, zlevel=6, fc_array=[1,2,3]):
        
        self.log.info('add low FC highway into region...')
        sqlcmd = """
                    insert into temp_region_orglink_level4
                    (
                        select a.*, get_new_tile_id_by_zlevel(a.link_id_t,%s) as region_tile_id 
                        from rdb_link as a
                        left join temp_region_orglink_level4 as b
                        on a.link_id = b.link_id
                        where (a.road_type in (0,2) and not a.function_code = ANY(ARRAY%s)
                            and a.one_way <> 0)
                        and b.link_id is null
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orgnode_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.node_id_t,%s) as region_tile_id 
                        from
                        (
                            select unnest(ARRAY[start_node_id, end_node_id]) as node_id
                            from rdb_link
                            where road_type in (0,2) and not function_code = ANY(ARRAY%s)
                                and one_way <> 0
                        )as a
                        left join temp_region_orgnode_level4 as b
                        on a.node_id = b.node_id
                        left join rdb_node as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()

    def _addConnectorIntoRegion(self, zlevel=6, fc_array=[1,2,3]):
        
        self.log.info('add Connector link into region...')
        
        self.CreateIndex2('rdb_link_road_type_idx')
        self.CreateIndex2('rdb_link_one_way_idx')
        
        self.CreateFunction2('rdb_find_promote_link_connector')       
        self.CreateTable2('temp_region_promote_connector_walked')
        self.pg.execute('select rdb_find_promote_link_connector()')
        
        sqlcmd = """
                    insert into temp_region_orglink_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.link_id_t,%s) as region_tile_id 
                        from temp_region_promote_connector_walked as a
                        left join temp_region_orglink_level4 as b
                        on a.link_id = b.link_id
                        left join rdb_link c
                        on a.link_id = c.link_id
                        where c.one_way in (2,3)
                        and b.link_id is null
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orgnode_level4
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.node_id_t,%s) as region_tile_id 
                        from
                        (
                            select unnest(ARRAY[start_node_id, end_node_id]) as node_id
                            from (
                                select n.* from temp_region_promote_connector_walked m
                                left join rdb_link n
                                on m.link_id = n.link_id
                                where n.one_way in (2,3)
                            ) l
                        )as a
                        left join temp_region_orgnode_level4 as b
                        on a.node_id = b.node_id
                        left join rdb_node as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
                             
    def _makeOriginalRegionOnLevel6(self, zlevel=2, fc_array=[1,2]):
        #rdb_region._makeOriginalRegionOnLevel6(self, 2, [1,2])
        rdb_log.log('Region', 'Make original region level6...', 'info')
        
        #
        self.CreateFunction2('get_new_tile_id_by_zlevel')
        sqlcmd = """
                    drop table if exists temp_region_orglink_level6;
                    create table temp_region_orglink_level6
                    as
                    (
                        select *, get_new_tile_id_by_zlevel(link_id_t,%s) as region_tile_id 
                        from rdb_link 
                        where function_code = ANY(ARRAY%s) and one_way != 0 and road_type not in (7,8,9,14)
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_region_orglink_level6_link_id_idx')
        self.CreateIndex2('temp_region_orglink_level6_start_node_id_idx')
        self.CreateIndex2('temp_region_orglink_level6_end_node_id_idx')
        
        self._addLowRoadForRegion4And6(zlevel)
        
        #
        sqlcmd = """
                    drop table if exists temp_region_orgnode_level6;
                    create table temp_region_orgnode_level6
                    as
                    (
                        select c.*, get_new_tile_id_by_zlevel(node_id_t,%s) as region_tile_id 
                        from
                        (
                            select distinct node_id
                            from
                            (
                                select start_node_id as node_id
                                from temp_region_orglink_level6
                                union
                                select end_node_id as node_id
                                from temp_region_orglink_level6
                            )as a
                        )as b
                        left join rdb_node as c
                        on b.node_id = c.node_id
                    );
                """ % str(zlevel)
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_region_orgnode_level6_node_id_idx')
        
        rdb_log.log('Region', 'Make original region level6 end.', 'info')
        
        self._addLowFCRoundaboutIntoRegion(zlevel, fc_array)
        self._addLowFCHighwayIntoRegion6(zlevel, fc_array)
        
    def _addLowFCRoundaboutIntoRegion(self, zlevel=2, fc_array=[1,2]):
        self.log.info('add low FC roundabout into region...')
        sqlcmd = """
                    insert into temp_region_orglink_level6
                    (
                        select a.*, get_new_tile_id_by_zlevel(a.link_id_t,%s) as region_tile_id 
                        from rdb_link as a
                        left join temp_region_orglink_level6 as b
                        on a.link_id = b.link_id
                        where (a.link_type in (0) and not a.function_code = ANY(ARRAY%s))
                        and a.one_way <> 0 and b.link_id is null
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orgnode_level6
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.node_id_t,%s) as region_tile_id 
                        from
                        (
                            select unnest(ARRAY[start_node_id, end_node_id]) as node_id
                            from rdb_link
                            where link_type in (0) and not function_code = ANY(ARRAY%s)
                                and one_way <> 0
                        )as a
                        left join temp_region_orgnode_level6 as b
                        on a.node_id = b.node_id
                        left join rdb_node as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
    
    def _addLowFCHighwayIntoRegion6(self, zlevel=2, fc_array=[1,2]):
        
        self.log.info('add low FC highway into region...')
        sqlcmd = """
                    insert into temp_region_orglink_level6
                    (
                        select a.*, get_new_tile_id_by_zlevel(a.link_id_t,%s) as region_tile_id 
                        from rdb_link as a
                        left join temp_region_orglink_level6 as b
                        on a.link_id = b.link_id
                        where (a.road_type in (0,2) and not a.function_code = ANY(ARRAY%s)
                            and a.one_way <> 0)
                        and b.link_id is null
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                    insert into temp_region_orgnode_level6
                    (
                        select c.*, get_new_tile_id_by_zlevel(c.node_id_t,%s) as region_tile_id 
                        from
                        (
                            select unnest(ARRAY[start_node_id, end_node_id]) as node_id
                            from rdb_link
                            where road_type in (0,2) and not function_code = ANY(ARRAY%s)
                                and one_way <> 0
                        )as a
                        left join temp_region_orgnode_level6 as b
                        on a.node_id = b.node_id
                        left join rdb_node as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """ % (str(zlevel), str(fc_array))
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
    def _addLowRoadForRegion4And6(self,zlevel = 0):
        rdb_log.log('Region', 'start add low road for region level%s'%str(zlevel), 'info')
        
        #create add link
        if zlevel == 6:
            str_region_tbl = 'temp_region_orglink_level4'
            sqlcmd = '''
                    drop table if exists temp_org_add_region;
                    create table temp_org_add_region
                    as
                    (                      
                        select d.*,get_new_tile_id_by_zlevel(d.link_id_t,%s) as region_tile_id
                        from org_processed_line as a
                        left join temp_topo_link as b
                        on a.folder = b.folder and a.link_id = b.link_id
                        left join temp_link_org2rdb as c
                        on c.org_link_id = b.new_link_id
                        left join rdb_link as d
                        on d.link_id = c.target_link_id
                        left join %s as e
                        on d.link_id = e.link_id
                        where ((a.grmn_type = 'COLLECTOR' and a.route_lvl = 2 and a.spd_limit >= 60) or a.route_lvl >= 3)
                        and d.one_way != 0 and d.road_type not in(7,8,9,14) and e.link_id is null                   
                    );
                '''%(str(zlevel),str_region_tbl)
        elif zlevel == 2:
            str_region_tbl = 'temp_region_orglink_level6'
            sqlcmd = '''
                    drop table if exists temp_org_add_region;
                    create table temp_org_add_region
                    as
                    (                      
                        select d.*,get_new_tile_id_by_zlevel(d.link_id_t,%s) as region_tile_id
                        from org_processed_line as a
                        left join temp_topo_link as b
                        on a.folder = b.folder and a.link_id = b.link_id
                        left join temp_link_org2rdb as c
                        on c.org_link_id = b.new_link_id
                        left join rdb_link as d
                        on d.link_id = c.target_link_id
                        left join %s as e
                        on d.link_id = e.link_id
                        where a.route_lvl >= 3 and (a.folder != 'SG' or (a.spd_limit >= 60 and a.type in (5,8,9,11) and a.folder = 'SG')) 
                        and a.folder != 'Brunei' and d.one_way != 0 and d.road_type not in (7,8,9,14) and e.link_id is null
                    );
                '''%(str(zlevel),str_region_tbl)
        else:
            rdb_log.log('Region', 'level%s is error'%str(zlevel), 'error')
         
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_org_add_region_link_id_idx')
        self.CreateIndex2('temp_org_add_region_start_node_id_idx')
        self.CreateIndex2('temp_org_add_region_end_node_id_idx')

        #node 
        sqlcmd = '''
            drop table if exists temp_node_connect_link_num;
            create table temp_node_connect_link_num
            as
            (
                select node_id,count(*) as num
                from
                (
                    select link_id,unnest(node_array) as node_id
                    from
                    (
                        select link_id,array[start_node_id,end_node_id] as node_array
                        from %s
                        union
                        select link_id,array[start_node_id,end_node_id] as node_array
                        from temp_org_add_region
                    )temp
                )temp1
                group by node_id
            );
            '''%str_region_tbl
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_node_connect_link_num_node_id_idx')
        self.CreateIndex2('temp_node_connect_link_num_num_idx')
        
        #delete shortlink 
        self.CreateFunction2('rdb_delete_shortlink_for_region')
        self.pg.callproc('rdb_delete_shortlink_for_region')
        self.pg.commit2()
        
        sqlcmd = '''
            insert into %s
            select * from temp_org_add_region;
            '''%str_region_tbl
        
        self.pg.execute(sqlcmd)
        self.pg.commit2()
            
        rdb_log.log('Region', 'end add low road for region level%s'%str(zlevel), 'info')
        
    