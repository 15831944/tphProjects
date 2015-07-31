# -*- coding: UTF8 -*-
'''
Created on 2013-3-22

@author: hongchenzai
'''

import platform.TestCase

class CCheckRegionRegulationBase(platform.TestCase.CTestCase):
    def _GetLevels(self):
        levels = []
        sqlcmd = """
            SELECT  tablename, btrim(translate(tablename, 'abcdefghijklmnopqrstxyzuvw_', ''))
             FROM  pg_tables 
             where tablename like 'rdb_region_link_regulation_layer\%_tbl'
             order by tablename;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        while row:
            levels.append(row[1])
            row = self.pg.fetchone()
        return levels
    
class CCheckRegionRegulationIDValidate(CCheckRegionRegulationBase):
    '''Regulation ID.'''
    def _do(self):
        sqlcmd = """
                SELECT count(record_no)
                  FROM rdb_region_link_regulation_layer[X]_tbl
                  where regulation_id not in (
                    SELECT regulation_id FROM rdb_link_regulation
                  );
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
       
class CCheckRegionRegulationLinkOnSameLevel(CCheckRegionRegulationBase):
    '''link在同一层。'''
    def _do(self):
        sqlcmd = """
select count(record_no)
  from (
    select record_no, link_array[seq_num]::bigint as link_id
      from (
        select record_no, link_array, generate_series(1,array_upper(link_array,1)) as seq_num, key_string
          from (
            SELECT record_no, regexp_split_to_array(key_string, E'\\,+') as link_array, key_string
              FROM rdb_region_link_regulation_layer[X]_tbl
          ) as a
      ) as b
  ) as c
  left join rdb_region_layer[X]_link_mapping as d
  on link_id = region_link_id
  where d.region_link_id is null;
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if self.pg.execute(sqlcmd.replace('[X]', level)) == -1:
                self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                return False;
            else:
                self.pg.commit()
        return True;
    
    
class CCheckRegionRegulationFirstLink(CCheckRegionRegulationBase):
    '''��һ��link.'''
    def _do(self):
        sqlcmd = """
            SELECT count(record_no)
              FROM rdb_region_link_regulation_layer[X]_tbl
              where first_link_id <> (regexp_split_to_array(key_string, E'\\,+'))[1]::bigint;
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
    
class CCheckRegionRegulationLastLink(CCheckRegionRegulationBase):
    '''���һ��link. '''
    def _do(self):
        sqlcmd = """
                SELECT count(record_no)
                  FROM (
                    SELECT record_no, last_link_id, regexp_split_to_array(key_string, E'\\,+') as link_array
                      FROM rdb_region_link_regulation_layer[X]_tbl
                  ) AS a
                  where last_link_id <> link_array[array_upper(link_array, 1)]::bigint;
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
    
class CCheckRegionRegulationStatus(CCheckRegionRegulationBase):
    '''region link 规制状态. '''
    def _do(self):
        sqlcmd = """
select count(*)
  from (
    -- first link
    SELECT link_array[1]::bigint as link_id
      from (
        SELECT regexp_split_to_array(key_string, E'\\\,+') as link_array
          FROM rdb_region_link_regulation_layer[X]_tbl
      ) as a

    union

    -- last link
    SELECT link_array[array_upper(link_array, 1)]::bigint as link_id
      from (
        SELECT regexp_split_to_array(key_string, E'\\\,+') as link_array
          FROM rdb_region_link_regulation_layer[X]_tbl
      ) as a
  ) as u
  left join (
    -- first link
    SELECT link_array[1]::bigint as first_link
      from (
        SELECT regexp_split_to_array(key_string, E'\\\,+') as link_array
          FROM rdb_region_link_regulation_layer[X]_tbl
      ) as a
  ) as f
  on u.link_id = first_link
  left join (
    -- last link
    SELECT link_array[array_upper(link_array, 1)]::bigint as last_link
      from (
        SELECT regexp_split_to_array(key_string, E'\\\,+') as link_array
          FROM rdb_region_link_regulation_layer[X]_tbl
      ) as a
  ) as l
  on u.link_id = last_link
  left join rdb_region_link_layer[X]_tbl as b
  on u.link_id = b.link_id
  where (first_link is not null and last_link is not null and regulation_exist_state <> 3)
        or (first_link is not null and last_link is null and regulation_exist_state <> 1)
        or (first_link is null and last_link is not null and regulation_exist_state <> 2)
        ;
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    print row2[0]
                    self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                print row2[0]
                self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                return False
            
        return True;
    
class CCheckRegionRegulationCompareLink(CCheckRegionRegulationBase):
    '''region link flag. '''
    def _do(self):
        sqlcmd = """
                select count(*)
                from (
                    select record_no, regulation_type, last_link_dir, 
                          array_to_string(array_agg(link_id_14), ',') as new_key_string
                    from (
                        select record_no, regulation_type, last_link_dir, 
                               array_to_string(link_id_14, ',') as link_id_14, seq_num
                        from (
                            select record_no, regulation_type, last_link_dir, 
                                   link_array[seq_num]::bigint as region_link_id, seq_num
                              from (
                                SELECT record_no, regulation_type, last_link_dir, 
                                       regexp_split_to_array(key_string, E'\\,+') as link_array, 
                                       generate_series(1, link_num) as seq_num
                                  FROM rdb_region_link_regulation_layer[X]_tbl as a
                              ) as b
                          ) as c
                          left join rdb_region_layer[X]_link_mapping as d
                          on c.region_link_id = d.region_link_id
                          order by record_no, regulation_type, last_link_dir, seq_num
                      ) as e
                      group by record_no, regulation_type, last_link_dir
                  ) as f  
                  left join rdb_link_regulation as g
                  on f.record_no = g.record_no
                  where f.regulation_type <> g.regulation_type 
                       or f.last_link_dir <> g.last_link_dir
                       or new_key_string <> key_string;
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_regulation_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
        