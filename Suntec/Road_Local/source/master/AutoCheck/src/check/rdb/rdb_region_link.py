# -*- coding: UTF8 -*-
'''
Created on 2013-3-21

@author: hongchenzai
'''

import platform.TestCase

class CCheckRegionBase(platform.TestCase.CTestCase):
    def _GetLevels(self):
        levels = []
        sqlcmd = """
            SELECT  tablename, btrim(translate(tablename, 'abcdefghijklmnopqrstxyzuvw_', ''))
             FROM  pg_tables 
             where tablename like 'rdb_region_link_layer\%_tbl'
             order by tablename;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        while row:
            levels.append(row[1])
            row = self.pg.fetchone()
        #print levels
        return levels
    
class CCheckRegionLinkStartNodeIdValidate(CCheckRegionBase):
    '''Start Node'''
    def _do(self):
        sqlcmd = """
                SELECT count(link_id)
                  FROM rdb_region_link_layer[X]_tbl
                  left join rdb_region_node_layer[X]_tbl
                  on start_node_id = node_id
                  where node_id is null;
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
    
class CCheckRegionLinkEndNodeIdValidate(CCheckRegionBase):
    '''End Node'''
    def _do(self):
        sqlcmd = """
                SELECT count(link_id)
                  FROM rdb_region_link_layer[X]_tbl
                  left join rdb_region_node_layer[X]_tbl
                  on start_node_id = node_id
                  where node_id is null;
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
    
class CCheckRegionLinkRoadTypeValidate(CCheckRegionBase):
    '''Road Type'''
    def _do(self):
        sqlcmd = """
            ALTER TABLE rdb_region_link_layer[X]_tbl DROP CONSTRAINT if exists check_road_type;
            ALTER TABLE rdb_region_link_layer[X]_tbl
              ADD CONSTRAINT check_road_type CHECK (road_type = [CONDITION]); 
        """
        
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if level == '6':   # tile 2
                condition = 'ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13])'
            else:              # tile 10
                condition = 'ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13])'
            sqlcmd2 = sqlcmd.replace('[CONDITION]', condition)
            if self.pg.execute(sqlcmd2.replace('[X]', level)) == -1:
                self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False;
            else:
                self.pg.commit()
        return True;
    
class CCheckRegionLinkOneWayValidate(CCheckRegionBase):
    '''One Way'''
    def _do(self):
        sqlcmd = """
            ALTER TABLE rdb_region_link_layer[X]_tbl DROP CONSTRAINT if exists check_one_way;
            ALTER TABLE rdb_region_link_layer[X]_tbl
              ADD CONSTRAINT check_one_way CHECK (
                 one_way = ANY (ARRAY[0, 1, 2, 3]));     
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if self.pg.execute(sqlcmd.replace('[X]', level)) == -1:
                self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False;
            else:
                self.pg.commit()
        return True;

class CCheckRegionLinkNameIDValidate(CCheckRegionBase):
    '''Name ID'''
    def _do(self):
        sqlcmd = """
                  select count(*)
                    from (
                        select region_link_id, road_name_id, link_id_14[seq_num] as link_id
                        from (
                            SELECT link_id as region_link_id, road_name_id, link_id_14, 
                                   generate_series(1, array_upper(link_id_14, 1)) as seq_num
                              FROM rdb_region_link_layer[X]_tbl as a
                              left join rdb_region_layer[X]_link_mapping
                              on a.link_id = region_link_id
                        ) as a
                    ) as b
                    left join rdb_link as c
                    on b.link_id = c.link_id
                    where b.road_name_id <> c.road_name_id;
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;

        
class CCheckRegionLinkLaneIDValidate(CCheckRegionBase):
    '''Lane ID'''
    def _do(self):
        sqlcmd = """
            SELECT count(link_id)
              FROM rdb_region_link_layer[X]_tbl as a
              left join rdb_region_link_lane_info_layer[X]_tbl as b
              on a.lane_id = b.lane_id
              where b.lane_id is null
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;

class CCheckRegionLinkTollValidate(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
            ALTER TABLE rdb_region_link_layer[X]_tbl DROP CONSTRAINT if exists check_toll;
            ALTER TABLE rdb_region_link_layer[X]_tbl
              ADD CONSTRAINT check_toll CHECK (toll = ANY (ARRAY[0, 1, 2, 3]));
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if self.pg.execute(sqlcmd.replace('[X]', level)) == -1:
                self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False;
            else:
                self.pg.commit()
        return True;
    
class CCheckRegionLinkLinkTypeValidate(CCheckRegionBase):
    '''Link Type'''
    def _do(self):
        sqlcmd = """
            ALTER TABLE rdb_region_link_layer[X]_tbl DROP CONSTRAINT if exists check_link_type;
            ALTER TABLE rdb_region_link_layer[X]_tbl
              ADD CONSTRAINT check_link_type CHECK (link_type = [CONDITION]); 
        """
        
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if level == '6':   # tile 2
                condition = 'ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])'
            else:              # tile 10
                condition = 'ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])'
            sqlcmd2 = sqlcmd.replace('[CONDITION]', condition)
            if self.pg.execute(sqlcmd2.replace('[X]', level)) == -1:
                self.logger.error('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False;
            else:
                self.pg.commit()
            
        ### 检查link_type值    
        # 1. sublinks的linktype存在不一样的情况下：
        # 如果sublink是SA的话，提升到整个合并后的Link,
        # 如果sublink是上下线分离的话，提升到合并后的link.
        # 其他情况设置成1（Main road (no separation betweentwo-way traffic) link）
        # 2. 如果sublink的linktype都一致的话，取其值。
        sqlcmd = """
        select count(region_link_id)
          from (
            select region_link_id, array_agg(link_type) as type_array
              from (
                select distinct region_link_id, link_type
                  from (
                    SELECT region_link_id, link_id_14,
                           generate_series(1, array_upper(link_id_14, 1)) as seq_num
                      FROM rdb_region_layer[X]_link_mapping
                  ) as a
                  left join rdb_link
                  on link_id_14[seq_num] = link_id
              ) as b
              group by region_link_id
          ) as c
          left join rdb_region_link_layer[X]_tbl
          on region_link_id = link_id
          where (7 = any (type_array) and link_type <> 7) or 
                (not(7 = any (type_array)) and (2 = any (type_array)) and link_type <> 2) or
                (array_upper(type_array, 1) = 1 and type_array[1] <> link_type) or 
                (not(7 = any (type_array)) and  not(2 = any (type_array)) and array_upper(type_array, 1) > 1 and link_type <> 1); 
        """
        rst    = True
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level)) 
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.error("rdb_region_link_layer[X]_tbl.".replace('[X]', level))
                rst = False;
            else:
                pass
        return rst;
    
class CCheckRegionLink_fazm(CCheckRegionBase):
    '''起点角度。'''
    def _do(self):
        sqlcmd = """
            SELECT count(a.link_id)
              FROM rdb_region_link_layer[X]_tbl as a
              left join rdb_region_layer[X]_link_mapping as b
              on region_link_id = link_id 
              left join rdb_link  as c
              on link_id_14[1] = c.link_id
              left join rdb_region_layer[X]_node_mapping as d
              on a.start_node_id = d.region_node_id
              where (c.start_node_id = d.node_id_14 and a.fazm_path <> c.fazm_path)
                 or (c.end_node_id = d.node_id_14 and a.fazm_path <> c.tazm_path)
             ;
          """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row = self.pg.fetchone()
            if row and row[0] == 0:
                pass
            else:
                return False
        
        return True;
    
class CCheckRegionLink_tazm(CCheckRegionBase):
    '''终点角度'''
    def _do(self):
        sqlcmd = """
            SELECT count(a.link_id)
              FROM rdb_region_link_layer[X]_tbl as a
              left join rdb_region_layer[X]_link_mapping as b
              on region_link_id = link_id 
              left join rdb_link  as c
              on link_id_14[array_upper(link_id_14,1)] = c.link_id
              left join rdb_region_layer[X]_node_mapping as d
              on a.end_node_id = d.region_node_id
              where (c.end_node_id = d.node_id_14 and a.tazm_path <> c.tazm_path)
                 or (c.start_node_id = d.node_id_14 and a.tazm_path <> c.fazm_path)
             ;
          """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row = self.pg.fetchone()
            if row and row[0] == 0:
                pass
            else:
                return False
        
        return True;
    
class CCheckRegionLinkAddInfoFlag(CCheckRegionBase):
    '''终点角度'''
    def _do(self):
        sqlcmd1 = """
                SELECT count(a.link_id)
                  FROM rdb_region_link_layer[X]_tbl as a
                  left join rdb_region_link_add_info_layer[X]_tbl as b
                  on a.link_id = b.link_id
                  where link_add_info_flag = true and b.link_id is null
                
                union 
                 
                SELECT count(a.link_id)
                  FROM rdb_region_link_layer[X]_tbl as a
                  left join rdb_region_link_add_info_layer[X]_tbl as b
                  on a.link_id = b.link_id
                  where link_add_info_flag = false and b.link_id is not null;
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd1.replace('[X]', level))
            row = self.pg.fetchone()
            while row:
                if row[0] != 0:  
                    self.logger.warning('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
                row = self.pg.fetchone()            
        return True;
    
class CCheckRegionLinkRegulationExistState(CCheckRegionBase):
    '''条件规制'''
    def _do(self):
        # check 值
        sqlcmd = """
            ALTER TABLE rdb_region_link_layer[X]_tbl DROP CONSTRAINT if exists check_regulation_exist_state;
            ALTER TABLE rdb_region_link_layer[X]_tbl
              ADD CONSTRAINT check_regulation_exist_state CHECK (
                regulation_exist_state = ANY (ARRAY[0, 1, 2, 3]));
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if self.pg.execute(sqlcmd.replace('[X]', level)) == -1:
                self.logger.warning('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False
            else:
                self.pg.commit()
                pass
            
        # 0：该link不涉及规制信息
        # 1：该link为规制的first link
        # 2: 该link为规制的last ink
        # 3: 该link既是规制的first link，又是last link
        sqlcmd = """
            select count(link_id)
              from (
                select link_id, regulation_exist_state
                  from rdb_region_link_layer[X]_tbl
                  where regulation_exist_state <> 0
              ) as a
              left join (
                -- first link
                SELECT link_array[1]::bigint as first_link
                  from (
                    SELECT regexp_split_to_array(key_string, E'\\\,+') as link_array
                      FROM rdb_region_link_regulation_layer[X]_tbl
                  ) as a
              ) as f
              on a.link_id = first_link
              left join (
                -- last link
                SELECT link_array[array_upper(link_array, 1)]::bigint as last_link
                  from (
                    SELECT regexp_split_to_array(key_string, E'\\\,+') as link_array
                      FROM rdb_region_link_regulation_layer[X]_tbl
                  ) as a
              ) as l
              on a.link_id = last_link
              where (regulation_exist_state = 1 and first_link is  null)
                    or (regulation_exist_state = 2 and last_link is null)
                    or (regulation_exist_state = 3 and (first_link is null or last_link is null));
            """
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.warning('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False
            else:
                self.pg.commit()
                pass
            
        return True;
    
class CCheckRegionLinkSpeedRegulationFlag(CCheckRegionBase):
    '''速度规制'''
    def _do(self):
        sqlcmd = """
                SELECT count(link_id)
                  FROM rdb_region_link_layer[X]_tbl as a
                  left join (
                    select region_link_id, array_agg(speed_regulation_flag) as speed_regulation_flag_14
                      from (
                        SELECT region_link_id, speed_regulation_flag
                         from (
                            SELECT region_link_id, link_id_14,
                                   generate_series(1, array_upper(link_id_14, 1)) as seq_num
                              FROM rdb_region_layer[X]_link_mapping 
                          ) as b
                          left join rdb_link 
                          on link_id_14[seq_num] = link_id
                      ) as c
                      group by region_link_id
                  ) as d
                  on a.link_id = d.region_link_id
                  where (a.speed_regulation_flag = true and not (true = any (speed_regulation_flag_14)))
                        or (a.speed_regulation_flag = false and (true = any (speed_regulation_flag_14)));
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row = self.pg.fetchone()
            if row[0] != 0:
                self.logger.warning('rdb_region_link_layer[X]_tbl'.replace('[X]', level))
                return False
            else:
                self.pg.commit()
                pass
            
        return True;

class CCheckRegionLinkIdValidate(CCheckRegionBase):
    '''Start Node'''
    def _do(self):
        sqlcmd = """
                SELECT max(link_id & ((1::bigint <<32) - 1))
                FROM rdb_region_link_layer[X]_tbl;
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            max_link_id_in_tile = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (not max_link_id_in_tile) or (max_link_id_in_tile > 65535):
                return False
        return True;

class CCheckRegionAbsLinkIdValidate(CCheckRegionBase):
    '''Start Node'''
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select abs_link_id
                    FROM rdb_link_abs
                    group by abs_link_id 
                    having count(*) > 1
                    limit 1
                )as t
        """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if (rec_count > 0):
            return False
    
        sqlcmd = """
                select  count(*)
                FROM rdb_region_link_layer[X]_tbl as a
                left join rdb_region_layer[X]_link_mapping as b
                on a.link_id = b.region_link_id
                where array_upper(b.link_id_14, 1) != (abs(a.abs_link_diff) + 1)
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;
    
        sqlcmd = """
                select count(*)
                from
                (
                    select  *
                    from
                    (
                        select  a.region_link_id,
                                a.abs_link_base,
                                a.abs_link_diff,
                                a.abs_link_dir,
                                seq,
                                b.abs_link_id
                        from
                        (
                            SELECT  a.link_id as region_link_id,
                                    a.abs_link_id as abs_link_base,
                                    a.abs_link_diff,
                                    a.abs_link_dir,
                                    b.link_id_14,
                                    generate_series(1, a.abs_link_diff + 1) as seq
                            FROM rdb_region_link_layer[X]_tbl as a
                            left join rdb_region_layer[X]_link_mapping as b
                            on a.link_id = b.region_link_id
                        )as a
                        left join rdb_link_abs as b
                        on a.link_id_14[seq] = b.link_id
                    )as t
                    where (abs_link_dir is true and (abs_link_base + seq - 1) != abs_link_id)
                          or
                          (abs_link_dir is false and (abs_link_base + abs_link_diff + 1 - seq) != abs_link_id)
                    limit 1
                )as t
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

