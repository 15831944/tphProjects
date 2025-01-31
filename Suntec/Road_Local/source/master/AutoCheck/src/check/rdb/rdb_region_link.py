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
            if (not max_link_id_in_tile) or (max_link_id_in_tile > 65535 * 2):
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

class CCheckRegionLinkRoadName(CCheckRegionBase):
    '''Start Node'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_road_name,
                            b.link_id,
                            b.road_name
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.road_name as region_road_name, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                )as t
                where region_road_name is distinct from road_name
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkLength(CCheckRegionBase):
    '''Start Node'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  region_link_id, region_link_length,
                            sum(link_length) as link_length
                    from
                    (
                        select  a.region_link_id,
                                a.region_link_length,
                                b.link_id,
                                b.link_length
                        from
                        (
                            select  a.link_id as region_link_id,
                                    (a.link_length_modify * 4 ^ a.link_length_unit) as region_link_length, 
                                    unnest(b.link_id_14) as link_id
                            from rdb_region_link_layer[X]_tbl as a
                            left join rdb_region_layer[X]_link_mapping as b
                            on a.link_id = b.region_link_id
                        )as a
                        left join rdb_link as b
                        on a.link_id = b.link_id
                    )as t
                    group by region_link_id, region_link_length
                )as t
                where abs(region_link_length - link_length) * 1.0 / link_length > 0.05
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkToll(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_toll,
                            b.link_id,
                            b.toll
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.toll as region_toll, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                )as t
                where region_toll is distinct from toll
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkToll(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_toll,
                            b.link_id,
                            b.toll
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.toll as region_toll, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                )as t
                where region_toll is distinct from toll
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkPDMFlag(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_pdm_flag,
                            b.link_id,
                            b.pdm_flag
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.pdm_flag::boolean as region_pdm_flag, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link_with_all_attri_view as b
                    on a.link_id = b.link_id
                )as t
                where region_pdm_flag is distinct from pdm_flag
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkBypassFlag(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_bypass_flag,
                            b.link_id,
                            b.bypass_flag
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.bypass_flag as region_bypass_flag, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                )as t
                where region_bypass_flag is distinct from bypass_flag
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkHighcostFlag(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_highcost_flag,
                            b.link_id,
                            b.highcost_flag
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.highcost_flag as region_highcost_flag, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                )as t
                where region_highcost_flag is distinct from highcost_flag
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkForecastFlag(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_forecast_flag,
                            b.link_id,
                            b.forecast_flag
                    from
                    (
                        select  a.link_id as region_link_id,
                                a.forecast_flag as region_forecast_flag, 
                                unnest(b.link_id_14) as link_id
                        from rdb_region_link_layer[X]_tbl_view as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                    )as a
                    left join rdb_link_with_all_attri_view as b
                    on a.link_id = b.link_id
                )as t
                where region_forecast_flag is distinct from forecast_flag
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkSequenceLink(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_s_sequence_link_ids,
                            a.region_e_sequence_link_ids,
                            a.s_link,
                            case
                                when ls.start_node_id = a.s_node then ls.s_sequence_link_id
                                else ls.e_sequence_link_id
                                end 
                                as s_sequence_link_id,
                            case
                                when le.start_node_id = a.e_node then le.s_sequence_link_id
                                else le.e_sequence_link_id
                                end 
                                as e_sequence_link_id
                    from
                    (
                        select  a.link_id as region_link_id,
                                ss.link_id_14 as region_s_sequence_link_ids, 
                                se.link_id_14 as region_e_sequence_link_ids, 
                                b.link_id_14[1] as s_link,
                                ns.node_id_14 as s_node,
                                b.link_id_14[array_upper(b.link_id_14, 1)] as e_link,
                                ne.node_id_14 as e_node
                        from rdb_region_link_layer[X]_tbl_view as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_id = b.region_link_id
                        left join rdb_region_layer[X]_node_mapping as ns
                        on a.start_node_id = ns.region_node_id
                        left join rdb_region_layer[X]_node_mapping as ne
                        on a.end_node_id = ne.region_node_id
                        left join rdb_region_layer[X]_link_mapping as ss
                        on a.s_sequence_link_id = ss.region_link_id
                        left join rdb_region_layer[X]_link_mapping as se
                        on a.e_sequence_link_id = se.region_link_id
                    )as a
                    left join rdb_link_with_all_attri_view as ls
                    on a.s_link = ls.link_id
                    left join rdb_link_with_all_attri_view as le
                    on a.e_link = le.link_id
                )as t
                where  not (s_sequence_link_id = ANY(region_s_sequence_link_ids))
                       or
                       not (e_sequence_link_id = ANY(region_e_sequence_link_ids))
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkAbsLinkID(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  a.region_link_id,
                            a.region_abs_link_id,
                            b.link_id,
                            b.abs_link_id
                    from
                    (
                        select  region_link_id,
                                link_id_14[nIndex+1] as link_id,
                                case 
                                    when abs_link_dir then abs_link_id + nIndex
                                    else abs_link_id + (abs_link_diff - nIndex)
                                end as region_abs_link_id
                        from
                        (
                            select  a.link_id as region_link_id,
                                    a.abs_link_id, 
                                    a.abs_link_diff,
                                    generate_series(0, a.abs_link_diff) as nIndex,
                                    a.abs_link_dir,
                                    b.link_id_14
                            from rdb_region_link_layer[X]_tbl_view as a
                            left join rdb_region_layer[X]_link_mapping as b
                            on a.link_id = b.region_link_id
                        )as t
                    )as a
                    left join rdb_link_with_all_attri_view as b
                    on a.link_id = b.link_id
                )as t
                where region_abs_link_id is distinct from abs_link_id
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkPassable(CCheckRegionBase):
    '''Toll'''
    def _do(self):
        self.pg.CreateFunction_ByName('rdb_check_region_linkrow_passable')
        sqlcmd = """
                select  count(*)
                from
                (
                    select  region_link_id,
                            region_one_way,
                            region_s_node,
                            region_e_node,
                            array_agg(link_id) as link_array,
                            array_agg(link_dir) as link_dir_array,
                            array_agg(one_way) as one_way_array,
                            array_agg(s_node) as s_node_array,
                            array_agg(e_node) as e_node_array
                    from
                    (
                        select  a.region_link_id,
                                a.region_one_way,
                                a.region_s_node,
                                a.region_e_node,
                                a.nIndex,
                                b.link_id,
                                a.link_dir_14[a.nIndex] as link_dir,
                                b.one_way,
                                b.start_node_id as s_node,
                                b.end_node_id as e_node
                        from
                        (
                            select  a.link_id as region_link_id,
                                    a.one_way as region_one_way,
                                    ns.node_id_14 as region_s_node,
                                    ne.node_id_14 as region_e_node,
                                    b.link_id_14,
                                    b.link_dir_14,
                                    generate_series(1, array_upper(b.link_id_14, 1)) as nIndex
                            from rdb_region_link_layer[X]_tbl as a
                            left join rdb_region_layer[X]_link_mapping as b
                            on a.link_id = b.region_link_id
                            left join rdb_region_layer[X]_node_mapping as ns
                            on a.start_node_id = ns.region_node_id
                            left join rdb_region_layer[X]_node_mapping as ne
                            on a.end_node_id = ne.region_node_id
                        )as a
                        left join rdb_link as b
                        on a.link_id_14[nIndex] = b.link_id
                        order by a.region_link_id, a.region_one_way, a.region_s_node, a.region_e_node, a.nIndex
                    )as t
                    group by region_link_id, region_one_way, region_s_node, region_e_node
                )as t
                where not rdb_check_region_linkrow_passable(region_link_id,
                                                            region_one_way,
                                                            region_s_node,
                                                            region_e_node,
                                                            link_array,
                                                            link_dir_array,
                                                            one_way_array,
                                                            s_node_array,
                                                            e_node_array)
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;

class CCheckRegionLinkCircle(CCheckRegionBase):
    '''检查link是否自身构成环'''
    def _do(self):
        sqlcmd = """
                SELECT count(link_id)
                  FROM rdb_region_link_layer[X]_tbl
                  where start_node_id = end_node_id;
        """
        levels = self._GetLevels()
        for level in levels:
            sqlcmd = sqlcmd.replace('[X]', level)
            
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            
            if rec_cnt != 0:
                return False
            else:
                pass
            
        return True;