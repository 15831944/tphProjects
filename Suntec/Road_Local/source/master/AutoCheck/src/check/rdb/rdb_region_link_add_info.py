# -*- coding: UTF8 -*-
'''
Created on 2013-3-28

@author: hongchenzai
'''

import platform.TestCase

class CCheckRegionAddInfoBase(platform.TestCase.CTestCase):
    def _GetLevels(self):
        levels = []
        sqlcmd = """
            SELECT  tablename, btrim(translate(tablename, 'abcdefghijklmnopqrstxyzuvw_', ''))
             FROM  pg_tables 
             where tablename like 'rdb_region_link_add_info_layer\%_tbl'
             order by tablename;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        while row:
            levels.append(row[1])
            row = self.pg.fetchone()
        return levels
    
class CCheckRegionAddInfoLinkIDValidate(CCheckRegionAddInfoBase):
    '''ͬ'''
    def _do(self): 
        sqlcmd = """
                SELECT count(a.link_id)
                  FROM rdb_region_link_add_info_layer[X]_tbl as a
                  left join rdb_region_link_layer[X]_tbl as b
                  on a.link_id= b.link_id
                  where b.link_id is null;
        """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error('rdb_region_link_add_info_layer[X]_tbl'.replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
       
class CCheckRegionAddInfoStruct(CCheckRegionAddInfoBase):
    '''構造コード'''
    def _do(self):
        
        sqlcmd = """
                select count(region_link_id)
                from (
                select region_link_id, region_struct_code, array_agg(struct_code) as struct_code_array
                  from (
                    SELECT region_link_id, region_struct_code::smallint,
                           link_id_14[seq_num], struct_code
                      from (
                        SELECT link_id as region_link_id, 
                               struct_code as region_struct_code,
                               link_id_14, 
                               generate_series(1, array_upper(link_id_14, 1)) as seq_num
                          FROM rdb_region_link_add_info_layer[X]_tbl
                          left join rdb_region_layer[X]_link_mapping
                          on link_id = region_link_id
                      ) as a
                      left join rdb_link_add_info
                      on link_id_14[seq_num] = link_id
                      where link_id is not null
                  ) as b
                  group by region_link_id, region_struct_code
                ) as c
                where not (region_struct_code = any(struct_code_array))
                      and 
                      not (region_struct_code = 0 and not (1 = any(struct_code_array)));
        """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error("rdb_region_layer[X]_link_mapping: Don't continue or there are circles.".replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
        
class CCheckRegionAddInfoShortcutCode(CCheckRegionAddInfoBase):
    '''抜け道フラグ'''
    def _do(self):
        
        sqlcmd = """
                select count(region_link_id)
                from (
                select region_link_id, region_shortcut_code, array_agg(shortcut_code) as shortcut_code_array
                  from (
                    SELECT region_link_id, region_shortcut_code::smallint,
                           link_id_14[seq_num], shortcut_code
                      from (
                        SELECT link_id as region_link_id, 
                               shortcut_code as region_shortcut_code,
                               link_id_14, 
                               generate_series(1, array_upper(link_id_14, 1)) as seq_num
                          FROM rdb_region_link_add_info_layer[X]_tbl
                          left join rdb_region_layer[X]_link_mapping
                          on link_id = region_link_id
                      ) as a
                      left join rdb_link_add_info
                      on link_id_14[seq_num] = link_id
                      where link_id is not null
                  ) as b
                  group by region_link_id, region_shortcut_code
                ) as c
                where not (region_shortcut_code = any(shortcut_code_array));
            """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error("rdb_region_layer[X]_link_mapping: Don't continue or there are circles.".replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
    
class CCheckRegionAddInfoETCLane(CCheckRegionAddInfoBase):
    '''ETC lane.'''
    def _do(self):
                
        sqlcmd = """
                select count(region_link_id)
                from (
                select region_link_id, region_etc_lane_flag, array_agg(etc_lane_flag) as etc_lane_flag_array
                  from (
                    SELECT region_link_id, region_etc_lane_flag::smallint,
                           link_id_14[seq_num], etc_lane_flag
                      from (
                        SELECT link_id as region_link_id, 
                               etc_lane_flag as region_etc_lane_flag,
                               link_id_14, 
                               generate_series(1, array_upper(link_id_14, 1)) as seq_num
                          FROM rdb_region_link_add_info_layer[X]_tbl
                          left join rdb_region_layer[X]_link_mapping
                          on link_id = region_link_id
                      ) as a
                      left join rdb_link_add_info
                      on link_id_14[seq_num] = link_id
                      where link_id is not null
                  ) as b
                  group by region_link_id, region_etc_lane_flag
                ) as c
                where not (region_etc_lane_flag = any(etc_lane_flag_array))
            """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error("rdb_region_layer[X]_link_mapping: Don't continue or there are circles.".replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
    
class CCheckRegionAddInfoPathExtraFlag(CCheckRegionAddInfoBase):
    def _do(self):
        sqlcmd = """
              SELECT count(*)
              from (
                SELECT link_id as region_link_id, 
                       path_extra_info as region_info_flag,
                       link_id_14, 
                       generate_series(1, array_upper(link_id_14, 1)) as seq_num
                  FROM rdb_region_link_add_info_layer4_tbl
                  left join rdb_region_layer4_link_mapping
                  on link_id = region_link_id
              ) as a
              left join (
                  select m.link_id, m.struct_code, m.shortcut_code, m.etc_lane_flag
                    ,case when n.link_id is null then m.path_extra_info
                        else (n.link_add_info2 << 3) | m.path_extra_info
                     end as path_extra_info
                  from rdb_link_add_info as m
                  left join rdb_link_add_info2 as n
                  on m.link_id = n.link_id
              ) as b
              on link_id_14[seq_num] = link_id
              where region_info_flag <> path_extra_info;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)      
    