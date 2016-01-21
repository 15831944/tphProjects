# -*- coding: cp936 -*-
'''
Created on 2011-12-21

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_cond_speed(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Regulation Speed'
                          , 'link_tbl'
                          , 'gid'
                          , 'rdb_cond_speed'
                          , 'gid'
                          , False)
        
    def Do(self):
        
        if self.CreateTable2('rdb_cond_speed') == -1:
            return -1
        
        if self.CreateFunction2('rdb_getregulationspeed') == -1:
            return -1
        
        sqlcmd = """
            select gid
              from link_tbl
              where speed_limit_s2e is null limit 1;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row is not None:
            return 0
        
        # 求最小和最大id
        sqlcmd = """
            SELECT min(gid), max(gid)
              FROM link_tbl;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd = """
                    insert into rdb_cond_speed(link_id, link_id_t, pos_cond_speed, neg_cond_speed, pos_cond_speed_source, neg_cond_speed_source, unit)
                    (
                    select B.tile_link_id, 
                            b.tile_id, 
                            A.speed_limit_s2e,
                            A.speed_limit_e2s, 
                            A.speed_source_s2e & 7,
                            A.speed_source_e2s & 7,
                            case when A.speed_source_s2e != 0 then A.speed_source_s2e >> 3
                                else A.speed_source_e2s >> 3
                            end
                     from link_tbl as A
                     left join rdb_tile_link as B 
                     on A.link_id = B.old_link_id
                     WHERE not (speed_limit_s2e = 0 and speed_limit_e2s = 0) 
                         and not (speed_limit_s2e is null and speed_limit_e2s is null)
                         and gid >= %s and gid <= %s
                    );
                """
        
        self._call_child_thread(min_id, max_id, sqlcmd, 4, 1500000)
        
        self.CreateIndex2('rdb_cond_speed_link_id_idx')
        self.CreateIndex2('rdb_cond_speed_link_id_t_idx')
                
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_cond_speed DROP CONSTRAINT if exists rdb_cond_speed_link_id_fkey;       
                ALTER TABLE rdb_cond_speed
                  ADD CONSTRAINT rdb_cond_speed_link_id_fkey FOREIGN KEY (link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0

    def _DoCheckNumber(self):
        'check条目'
        
        sqlcmd = """
            select 
                (SELECT  count(link_id)
                  FROM link_tbl
                  where not (speed_limit_s2e = 0 and speed_limit_e2s = 0) 
                         and not (speed_limit_s2e is null and speed_limit_e2s is null)
                ) as src_num,
                (SELECT count(link_id) FROM rdb_cond_speed
                ) as desc_num;
            """
        
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            src_num  = row[0]
            desc_num = row[1]
            if src_num != desc_num:
                rdb_log.log(self.ItemName, 'Number of [speed limit]: ' + str(src_num) + ','\
                                           'But Number of rdb_cond_speed: ' + str(desc_num) + '.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Number of [speed limit] and rdb_cond_speed: ' + str(src_num) + '.', 'info')
        else:
            rdb_log.log(self.ItemName, "Can't find Number of [speed limit]  or  rdb_cond_speed .", 'warning')
        return 0