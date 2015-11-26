# -*- coding: UTF8 -*-
'''
Created on 2015-11-16

@author: lsq
'''



import component.component_base

class comp_detele_redundance_regulation(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Detele_redundance_regulation')
    
    def _Do(self):
        
        self.log.info('Begin deleting redundance regulation...')
        
        self._prepare()
        self._select_redundance_regulation_by_traffic_flow()
        self._delete_redundance_regulation()
        
        self.log.info('End deleting redundance regulation.')
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
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_traffic_flow_redundance_regulation_id_tbl;
                CREATE TABLE temp_traffic_flow_redundance_regulation_id_tbl
                (
                    regulation_id integer
                );
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateFunction2('mid_get_redundance_regulation_by_traffic_flow')
        self.pg.callproc('mid_get_redundance_regulation_by_traffic_flow')
        self.pg.commit2()
        
        self.log.info(' End selecting redundance regulation by traffic flow.')
        return 0
    
    def _delete_redundance_regulation(self):
        
        # 删除冗余的规制
        self.log.info(' Begin deleting redundance regulation...')
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        sqlcmd = """
                INSERT INTO regulation_relation_tbl
                    (regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope, is_seasonal)
                SELECT regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope, is_seasonal
                FROM regulation_relation_tbl_bak_redundance
                WHERE regulation_id not in (
                    SELECT DISTINCT regulation_id
                    FROM temp_traffic_flow_redundance_regulation_id_tbl
                );
                
                INSERT INTO regulation_item_tbl
                    (regulation_id, linkid, nodeid, seq_num)
                SELECT regulation_id, linkid, nodeid, seq_num
                FROM regulation_item_tbl_bak_redundance
                WHERE regulation_id not in (
                    SELECT DISTINCT regulation_id
                    FROM temp_traffic_flow_redundance_regulation_id_tbl
                );
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info(' End deleting redundance regulation.')
        return 0