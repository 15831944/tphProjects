# -*- coding: UTF8 -*-
'''
Created on 2015-11-16

@author: lsq
'''



import component.component_base

class comp_detele_regulation(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Detele_regulation')
    
    def _Do(self):
        
        self._prepare()
        self._select_redundance_regulation_by_traffic_flow()
        self._delete_traffic_flow_error_regulation()
        #self._delete_overdue_regulation() 

        return 0
    
    def _prepare(self):
        
        self.log.info(' Begin preparing...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS regulation_relation_tbl_bak_redundance;
                CREATE TABLE regulation_relation_tbl_bak_redundance
                AS (
                    SELECT *
                    FROM regulation_relation_tbl
                );

                DROP INDEX IF EXISTS regulation_relation_tbl_bak_redundance_regulation_id_idx;
                CREATE INDEX regulation_relation_tbl_bak_redundance_regulation_id_idx
                    ON regulation_relation_tbl_bak_redundance
                    USING btree
                    (regulation_id);
                
                analyze regulation_relation_tbl_bak_redundance;
                    
                DROP TABLE IF EXISTS regulation_item_tbl_bak_redundance;
                CREATE TABLE regulation_item_tbl_bak_redundance
                AS (
                    SELECT *
                    FROM regulation_item_tbl
                );
                
                DROP INDEX IF EXISTS regulation_item_tbl_bak_redundance_linkid_idx;
                CREATE INDEX regulation_item_tbl_bak_redundance_linkid_idx
                    ON regulation_item_tbl_bak_redundance
                    USING btree
                    (linkid);
                    
                DROP INDEX IF EXISTS regulation_item_tbl_bak_redundance_regulation_id_idx;
                CREATE INDEX regulation_item_tbl_bak_redundance_regulation_id_idx
                    ON regulation_item_tbl_bak_redundance
                    USING btree
                    (regulation_id);
                
                analyze regulation_item_tbl_bak_redundance;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info(' End preparing.')
        return 0
    
    def _select_redundance_regulation_by_traffic_flow(self):
        
        self.log.info(' Begin selecting redundance regulation by traffic flow...')
        
        self.log.info('  Begin preparing data...')
        sqlcmd = """
                DROP TABLE IF EXISTS temp_traffic_flow_redundance_regulation_id_tbl;
                CREATE TABLE temp_traffic_flow_redundance_regulation_id_tbl
                (
                    regulation_id integer
                );
                
                DROP TABLE IF EXISTS regulation_item_tbl_bak_redundance_link;
                CREATE TABLE regulation_item_tbl_bak_redundance_link
                AS (
                    SELECT *
                    FROM regulation_item_tbl_bak_redundance
                    WHERE seq_num != 2
                );
                
                DROP INDEX IF EXISTS regulation_item_tbl_bak_redundance_link_linkid_idx;
                CREATE INDEX regulation_item_tbl_bak_redundance_link_linkid_idx
                    ON regulation_item_tbl_bak_redundance_link
                    USING btree
                    (linkid);
                
                analyze regulation_item_tbl_bak_redundance_link;
                
                DROP TABLE IF EXISTS regulation_item_tbl_bak_redundance_node;
                CREATE TABLE regulation_item_tbl_bak_redundance_node
                AS (
                    SELECT *
                    FROM regulation_item_tbl_bak_redundance
                    WHERE seq_num = 2
                );
                
                DROP INDEX IF EXISTS regulation_item_tbl_bak_redundance_node_regulation_id_idx;
                CREATE INDEX regulation_item_tbl_bak_redundance_node_regulation_id_idx
                    ON regulation_item_tbl_bak_redundance_node
                    USING btree
                    (regulation_id);
                
                analyze regulation_item_tbl_bak_redundance_node;
                
                DROP TABLE IF EXISTS regulation_item_tbl_bak_redundance_finally;
                CREATE TABLE regulation_item_tbl_bak_redundance_finally
                AS (
                    SELECT d.*, e.nodeid
                    FROM (
                        SELECT regulation_id, count(*) as link_num, array_agg(linkid) as link_array, 
                            array_agg(s_node) as s_node_array, array_agg(e_node) as e_node_array, 
                            array_agg(one_way_code) as one_way_array
                        FROM (
                            SELECT a.regulation_id, a.linkid, b.s_node, b.e_node, b.one_way_code
                            FROM regulation_item_tbl_bak_redundance_link a
                            LEFT JOIN link_tbl b
                                ON a.linkid = b.link_id
                            WHERE b.link_id is not null
                            ORDER BY a.regulation_id, a.seq_num
                        ) c
                        GROUP BY regulation_id
                    ) d
                    LEFT JOIN regulation_item_tbl_bak_redundance_node e
                        ON d.regulation_id = e.regulation_id
                );
                
                analyze regulation_item_tbl_bak_redundance_finally;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('  End preparing data.')
        
        self.CreateFunction2('mid_get_redundance_regulation_by_traffic_flow')
        self.pg.callproc('mid_get_redundance_regulation_by_traffic_flow')
        self.pg.commit2()
        
        self.log.info(' End selecting redundance regulation by traffic flow.')
        return 0
    
    def _delete_traffic_flow_error_regulation(self):
        
        # 删除冗余的规制
        self.log.info(' Begin deleting traffic flow error regulation...')
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_traffic_flow_redundance_regulation_id_tbl_regulation_id_idx;
                CREATE INDEX temp_traffic_flow_redundance_regulation_id_tbl_regulation_id_idx
                    ON temp_traffic_flow_redundance_regulation_id_tbl
                    USING btree
                    (regulation_id);

                analyze temp_traffic_flow_redundance_regulation_id_tbl;
                
                INSERT INTO regulation_relation_tbl
                    (regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope, is_seasonal)
                SELECT a.regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope, is_seasonal
                FROM regulation_relation_tbl_bak_redundance a
                LEFT JOIN temp_traffic_flow_redundance_regulation_id_tbl b
                    ON a.regulation_id = b.regulation_id
                WHERE b.regulation_id is null;
                
                INSERT INTO regulation_item_tbl
                    (regulation_id, linkid, nodeid, seq_num)
                SELECT a.regulation_id, linkid, nodeid, seq_num
                FROM regulation_item_tbl_bak_redundance a
                LEFT JOIN temp_traffic_flow_redundance_regulation_id_tbl b
                    ON a.regulation_id = b.regulation_id
                WHERE b.regulation_id is null;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info(' End deleting traffic flow error regulation.')
        return 0
    
    def _delete_overdue_regulation(self):
        
        # 删除过期的规制数据
        self.log.info(' Begin deleting overdue regulation...')
        
        self.log.info('  Begin backup regulation table...')
        sqlcmd = """
                DROP TABLE IF EXISTS condition_regulation_tbl_bak_overdue;
                CREATE TABLE condition_regulation_tbl_bak_overdue
                AS (
                    SELECT *
                    FROM condition_regulation_tbl
                );
                
                DROP TABLE IF EXISTS regulation_relation_tbl_bak_overdue;
                CREATE TABLE regulation_relation_tbl_bak_overdue
                AS (
                    SELECT *
                    FROM regulation_relation_tbl
                );
                
                DROP INDEX IF EXISTS regulation_relation_tbl_bak_overdue_cond_id_idx;
                CREATE INDEX regulation_relation_tbl_bak_overdue_cond_id_idx
                    ON regulation_relation_tbl_bak_overdue
                    USING btree
                    (cond_id);
                
                analyze regulation_relation_tbl_bak_overdue;
                
                DROP TABLE IF EXISTS regulation_item_tbl_bak_overdue;
                CREATE TABLE regulation_item_tbl_bak_overdue
                AS (
                    SELECT *
                    FROM regulation_item_tbl
                );
                
                DROP INDEX IF EXISTS regulation_item_tbl_bak_overdue_regulation_id_idx;
                CREATE INDEX regulation_item_tbl_bak_overdue_regulation_id_idx
                    ON regulation_item_tbl_bak_overdue
                    USING btree
                    (regulation_id);
                
                analyze regulation_item_tbl_bak_overdue;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('  Begin delete overdue regulation...')
        self.CreateTable2('condition_regulation_tbl')
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        self.CreateFunction2('mid_delete_overdue_regulation_condition')
        self.pg.callproc('mid_delete_overdue_regulation_condition')
        self.pg.commit2()
        
        sqlcmd = """
                INSERT INTO regulation_relation_tbl (regulation_id, nodeid, inlinkid, 
                    outlinkid, condtype, cond_id, gatetype, slope, is_seasonal)
                SELECT regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, 
                    gatetype, slope, is_seasonal
                FROM regulation_relation_tbl_bak_overdue
                WHERE cond_id IS NULL OR cond_id IN (
                    SELECT cond_id 
                    FROM condition_regulation_tbl
                );
                
                INSERT INTO regulation_item_tbl (regulation_id, linkid, nodeid, seq_num)
                SELECT regulation_id, linkid, nodeid, seq_num
                FROM regulation_item_tbl_bak_overdue
                WHERE regulation_id IN (
                    SELECT regulation_id
                    FROM regulation_relation_tbl
                );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info(' End deleting overdue regulation.')
        return 0