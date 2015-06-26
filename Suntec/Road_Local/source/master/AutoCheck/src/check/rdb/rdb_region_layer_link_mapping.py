# -*- coding: UTF8 -*-
'''
Created on 2013-3-28

@author: hongchenzai
'''

import platform.TestCase

class CCheckRegionLinkMappingBase(platform.TestCase.CTestCase):
    def _GetLevels(self):
        levels = []
        sqlcmd = """
            SELECT  tablename, btrim(translate(tablename, 'abcdefghijklmnopqrstxyzuvw_', ''))
             FROM  pg_tables 
             where tablename like 'rdb_region_layer\%_link_mapping'
             order by tablename;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        while row:
            levels.append(row[1])
            row = self.pg.fetchone()
        return levels
    
class CCheckRegionLinkMappingRepeatLink(CCheckRegionLinkMappingBase):
    '''同一条14层link，被重复使用。'''
    def _do(self): 
        sqlcmd = """
                select count(link_id)
                  from (
                        select link_id
                    from (
                        select region_link_id, link_id_14[seq_num] as link_id
                          from (
                            SELECT region_link_id, link_id_14, 
                                   generate_series(1, array_upper(link_id_14, 1)) as seq_num
                              FROM rdb_region_layer[X]_link_mapping
                          ) as a
                      ) as b
                      group by link_id
                      having count(link_id) > 1
                  ) as c;
                """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error('rdb_region_layer[X]_link_mapping'.replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
       
class CCheckRegionLinkMappingContinuous(CCheckRegionLinkMappingBase):
    '''同一条region link对应的14层link是否是连续的；又或者形成环'''
    def _do(self):
        self.pg.CreateFunction_ByName('rdb_check_mapping_link_continuous')
        
        sqlcmd = """
            select count(region_link_id)
             from (
                SELECT region_link_id, link_id_14, 
                      rdb_check_mapping_link_continuous(link_id_14) as flag
                  FROM rdb_region_layer[X]_link_mapping
              ) as a
              where flag = false;
        """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error("rdb_region_layer[X]_link_mapping: Don't continue or there are circles.".replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
        