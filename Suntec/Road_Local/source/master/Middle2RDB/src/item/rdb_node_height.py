# -*- coding: cp936 -*-

from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_node_height(ItemBase):
    '''
    node height
    '''
        
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Node Height'
                          , 'node_height_tbl'
                          , 'gid'
                          , 'rdb_node_height'
                          , 'gid')
        
    def Do_CreateTable(self):
        
        return self.CreateTable2('rdb_node_height')
    
    def Do(self):
               
        self.pg.execute2("""select count(*) from node_height_tbl;""")
        row = self.pg.fetchone2()
        num  = row[0]
        if num != 0:
            sqlcmd = """
                    --- node with height
                    INSERT INTO rdb_node_height(node_id, height)
                    SELECT b.tile_node_id, a.height
                    FROM node_height_tbl a
                    LEFT JOIN rdb_tile_node b
                    ON a.node_id = b.old_node_id;
                    
                    --- node without height
                    INSERT INTO rdb_node_height(node_id, height)
                    SELECT a.tile_node_id, -32768
                    FROM rdb_tile_node a
                    LEFT JOIN node_height_tbl b
                    ON a.old_node_id = b.node_id
                    where b.node_id is null;                
                    """
            
            rdb_log.log(self.ItemName, 'Now it is inserting rdb_node_height...', 'info')
            self.pg.execute2(sqlcmd)
            self.pg.commit2()

        return 0
    
    def Do_CreatIndex(self):
        
        return self.CreateIndex2('rdb_node_height_node_id_idx')
    
    def _DoContraints(self):
        sqlcmd = """
                ALTER TABLE rdb_node_height DROP CONSTRAINT if exists rdb_node_height_node_id_fkey;
                ALTER TABLE rdb_node_height
                  ADD CONSTRAINT rdb_node_height_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _DoCheckNumber(self):
        'check条目'
        
        sqlcmd = """
            select count(*) 
            from rdb_node_height a
            right join rdb_node b
            on a.node_id = b.node_id
            where a.node_id is null;
            """
        
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            num  = row[0]
            if num != 0:
                rdb_log.log(self.ItemName, 'Number of height is different with number of node.', 'warning')
   