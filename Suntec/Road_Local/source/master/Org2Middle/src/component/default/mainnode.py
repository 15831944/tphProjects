# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: liuxinxing
'''
import component.component_base

class comp_mainnode(component.component_base.comp_base):
    '''
    Link Merge
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'MainNode')        
        
    def _Do(self):
        self.__backupTableList()
        self.__findMainNode()
        self.__searchAndUpdateRegulation()
#        self.__searchAndUpdateGuide()
        return 0
    
    def __backupTableList(self):
        self.log.info('making table list backup begin...')
        
        #
        self.CreateTable2('regulation_relation_tbl_bak_mainnode')
        self.CreateTable2('regulation_item_tbl_bak_mainnode')
        self.CreateIndex2('regulation_relation_tbl_bak_mainnode_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_bak_mainnode_regulation_id_idx')
        
        #
#        guide_table_list = [
#                            'spotguide_tbl',
#                            'signpost_tbl',
#                            'signpost_uc_tbl',
#                            'lane_tbl',
#                            'force_guide_tbl',
#                            'towardname_tbl',
#                            'hook_turn_tbl',
#                            ]
#        
#        for the_table in guide_table_list:
#            sqlcmd = """
#                        drop table if exists [the_table]_bak_mainnode;
#                        create table [the_table]_bak_mainnode
#                        as
#                        select * from [the_table];
#                    """
#            the_sql_cmd = sqlcmd.replace('[the_table]', the_table)
#            self.pg.execute2(the_sql_cmd)
#            self.pg.commit2()
        
        self.log.info('making table list backup end.')
    
    def __findMainNode(self):
        self.log.info('Finding mainnode...')
        
        #
        self.CreateTable2('temp_mainnode_sublink')
        self.CreateTable2('temp_mainnode_subnode')
        self.CreateIndex2('temp_mainnode_sublink_sublink_idx')
        self.CreateIndex2('temp_mainnode_subnode_subnode_idx')
        
        self.CreateFunction2('mid_find_mainnode')
        self.pg.callproc('mid_find_mainnode')
        self.pg.commit2()
        
        #
        self.CreateFunction2('mid_find_inner_path')
        self.CreateFunction2('mid_get_inner_path_array')
        
        self.log.info('Finding mainnode end.')
    
    def __searchAndUpdateRegulation(self):
        self.log.info('Searching and updating regulation...')
        
        #
        self.CreateTable2('temp_mainnode_regulation_new_linkrow')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_mainnode_regulation_new_linkrow_bak;
                CREATE TABLE temp_mainnode_regulation_new_linkrow_bak
                AS (
                    SELECT *
                    FROM temp_mainnode_regulation_new_linkrow
                );
                
                DROP INDEX IF EXISTS temp_mainnode_regulation_new_linkrow_bak_nodeid_idx;
                CREATE INDEX temp_mainnode_regulation_new_linkrow_bak_nodeid_idx
                    ON temp_mainnode_regulation_new_linkrow_bak
                    USING btree
                    (nodeid, inner_link_array);
                
                analyze temp_mainnode_regulation_new_linkrow_bak;
                
                --- first delete the inner path existed in table regulation_item_tbl
                DROP TABLE IF EXISTS temp_mainnode_regulation_new_linkrow;
                CREATE TABLE temp_mainnode_regulation_new_linkrow 
                AS (
                    SELECT a.*
                    FROM temp_mainnode_regulation_new_linkrow_bak a
                    LEFT JOIN (
                        SELECT d.regulation_id, d.link_array, e.nodeid
                        FROM (
                            SELECT regulation_id, array_agg(linkid) as link_array
                            FROM (
                                SELECT *
                                FROM regulation_item_tbl
                                WHERE seq_num != 2
                                ORDER BY regulation_id, seq_num
                            ) c
                            GROUP BY regulation_id HAVING COUNT(*) >= 2
                        ) d
                        LEFT JOIN regulation_item_tbl e
                            ON d.regulation_id = e.regulation_id and e.seq_num = 2
                    ) b
                        ON a.nodeid = b.nodeid and (a.inner_link_array IS NOT DISTINCT FROM b.link_array)
                    WHERE b.regulation_id IS NULL
                );
                
                analyze temp_mainnode_regulation_new_linkrow;
                
                --- second delete multi regulation link list who have same inlinkid/outlinkid/nodeid
                --- mapping the same inner path
                DROP TABLE IF EXISTS temp_mainnode_update_regulation;
                CREATE TABLE temp_mainnode_update_regulation
                AS (
                    SELECT nodeid, inlinkid, outlinkid, inner_link_array, 
                        regulation_id_list[1] as regulation_id
                    FROM (
                        SELECT nodeid, inlinkid, outlinkid, inner_link_array, 
                            array_agg(regulation_id) as regulation_id_list
                        FROM (
                            SELECT regulation_id, nodeid, link_array[1] as inlinkid, 
                                link_array[link_num] as outlinkid, inner_link_array
                            FROM temp_mainnode_regulation_new_linkrow
                            ORDER BY inner_link_array, regulation_id
                        ) a
                        GROUP BY nodeid, inlinkid, outlinkid, inner_link_array
                    ) b
                );
                
                DROP INDEX IF EXISTS temp_mainnode_update_regulation_regulation_id_idx;
                CREATE INDEX temp_mainnode_update_regulation_regulation_id_idx
                    ON temp_mainnode_update_regulation
                    USING btree
                    (regulation_id);
                
                analyze temp_mainnode_update_regulation;
                --- Now the inner path is unique in table temp_mainnode_update_regulation and regulation_item_tbl
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateFunction2('mid_convert_mainnode_regulation_linkrow')
        self.pg.callproc('mid_convert_mainnode_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('Searching and updating regulation end.')
    
    def __searchAndUpdateGuide(self):
        self.log.info('Searching and updating guide at mainnode...')
        
        sqlcmd = """
                drop table if exists temp_mainnode_new_linkrow_[the_guide_tbl] CASCADE;
                create table temp_mainnode_new_linkrow_[the_guide_tbl]
                as
                (
                    select    gid, nodeid, link_num, link_array, 
                        array_upper(inner_link_array,1) as inner_link_num, inner_link_array
                    from
                    (
                        select    gid, nodeid, link_num, link_array,
                            string_to_array(unnest(inner_path_array), ',')::bigint[] as inner_link_array
                        from
                        (
                            select    gid, link_num, link_array, nodeid, 
                                mid_get_inner_path_array(link_array, nodeid) as inner_path_array
                            from
                            (
                                select    gid, 
                                    nodeid,
                                    (passlink_cnt + 2) as link_num,
                                    (
                                    case 
                                    when passlink_cnt = 0 then ARRAY[inlinkid, outlinkid]
                                    else ARRAY[inlinkid] || cast(regexp_split_to_array(passlid, E'\\\\|+') as bigint[]) || ARRAY[outlinkid]
                                    end
                                    )as link_array
                                from [the_guide_tbl]
                            )as a
                        )as t1
                    )as t2
                    where (link_array is distinct from inner_link_array) and (array_upper(inner_link_array,1) < array_upper(link_array,1))
                );
                
                CREATE INDEX temp_mainnode_new_linkrow_[the_guide_tbl]_gid_idx
                    ON temp_mainnode_new_linkrow_[the_guide_tbl]
                    USING btree
                    (gid);
                
                drop table if exists temp_mainnode_update_linkrow_[the_guide_tbl] CASCADE;
                create table temp_mainnode_update_linkrow_[the_guide_tbl]
                as
                (
                    select  x.*, (row_number() over (order by gid, passlid_new)) as gid_new
                    from
                    (
                        select  b.*, 
                            (case when inner_link_num > 2 then (inner_link_num - 2) else 0 end) as passlink_cnt_new,
                            array_to_string(inner_link_array[2:inner_link_num-1], '|') as passlid_new
                        from temp_mainnode_new_linkrow_[the_guide_tbl] as a
                        left join [the_guide_tbl] as b
                        on a.gid = b.gid
                    )as x
                    order by gid, passlid_new
                );
                
                update temp_mainnode_update_linkrow_[the_guide_tbl] set passlink_cnt = passlink_cnt_new;
                update temp_mainnode_update_linkrow_[the_guide_tbl] set passlid = passlid_new;
                update temp_mainnode_update_linkrow_[the_guide_tbl] set gid = gid_new + max_gid
                from
                (
                    select max(gid) as max_gid from [the_guide_tbl]
                )as b;
                
                alter table temp_mainnode_update_linkrow_[the_guide_tbl] drop column gid_new;
                alter table temp_mainnode_update_linkrow_[the_guide_tbl] drop column passlink_cnt_new;
                alter table temp_mainnode_update_linkrow_[the_guide_tbl] drop column passlid_new;
                insert into [the_guide_tbl]
                select * from temp_mainnode_update_linkrow_[the_guide_tbl] order by gid;
                """
        
        guide_table_list = [
                            'spotguide_tbl',
                            'signpost_tbl',
                            'signpost_uc_tbl',
                            'lane_tbl',
                            'force_guide_tbl',
                            'towardname_tbl',
                            'hook_turn_tbl'
                            ]
        
        for the_guide_table in guide_table_list:
            self.log.info('Searching and updating guide at mainnode for %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()
        
        self.log.info('Searching and updating guide at mainnode end.')
        
    