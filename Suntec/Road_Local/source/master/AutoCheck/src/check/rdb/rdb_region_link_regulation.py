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
                CREATE OR REPLACE FUNCTION array_reverse(anyarray) RETURNS anyarray AS $$
                SELECT ARRAY(
                    SELECT $1[i]
                    FROM generate_series(
                        array_lower($1,1),
                        array_upper($1,1)
                    ) AS s(i)
                    ORDER BY i DESC
                );
                $$ LANGUAGE 'sql' STRICT IMMUTABLE;
                
                CREATE OR REPLACE FUNCTION test_get_linkrow_dirs(
                                                                 snode_array bigint[],
                                                                 enode_array bigint[],
                                                                 turn_node_id bigint
                                                                ) 
                    RETURNS boolean[]
                    LANGUAGE plpgsql VOLATILE
                AS $$
                DECLARE
                    link_num      integer;
                    link_index    integer;
                    dir_array     boolean[];
                BEGIN
                    link_num    = array_upper(snode_array,1);
                    if link_num = 1 then
                        dir_array = ARRAY[true];
                    else
                        if enode_array[1] = turn_node_id then
                            dir_array = ARRAY[true];
                        else
                            dir_array = ARRAY[false];
                        end if;
                        for link_index in 2..link_num loop
                            if snode_array[link_index] = turn_node_id then
                                dir_array = array_append(dir_array, true);
                                turn_node_id = enode_array[link_index];
                            elseif enode_array[link_index] = turn_node_id then
                                dir_array = array_append(dir_array, false);
                                turn_node_id = snode_array[link_index];
                            else
                                raise EXCEPTION 'error linkrow of region regulation, turn_node_id = %', turn_node_id;
                            end if;
                        end loop;
                    end if;
                    return dir_array; 
                END;
                $$;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        sqlcmd = """
                drop table if exists test_regulation_turn_node;
                create table test_regulation_turn_node
                as
                select  record_no,
                        b.region_node_id as turn_node_id
                from
                (
                    select    record_no,
                        (
                        case 
                        when b.start_node_id in (c.start_node_id, c.end_node_id) then b.start_node_id
                        else b.end_node_id
                        end
                        )as turn_node_id
                    from
                    (
                        select record_no, string_to_array(key_string, ',')::bigint[] as link_array
                        from rdb_link_regulation
                        where link_num > 1 and not (link_num = 2 and first_link_id = last_link_id)
                    )as a
                    left join rdb_link as b
                    on a.link_array[1] = b.link_id
                    left join rdb_link as c
                    on a.link_array[2] = c.link_id
                )as a
                left join rdb_region_layer[X]_node_mapping as b
                on a.turn_node_id = b.node_id_14;
                
                create index test_regulation_turn_node_record_no_idx
                    on test_regulation_turn_node
                    using btree
                    (record_no);
                    
                select count(*)
                from (
                    select    record_no, regulation_type, link_num, 
                        sum(link_num_14) as link_num_14,
                        array_to_string(array_agg(link_id_14), ',') as key_string_14
                    from
                    (
                        select    record_no, regulation_type, link_num, seq, 
                            array_upper(b.link_id_14, 1) as link_num_14,
                            (
                            case 
                            when dir_array[seq] then array_to_string(b.link_id_14, ',')
                            else array_to_string(array_reverse(b.link_id_14), ',')
                            end
                            )as link_id_14
                        from
                        (
                            select  *,
                                    generate_series(1,link_num) as seq,
                                    test_get_linkrow_dirs(snode_array, enode_array, turn_node_id) as dir_array
                            from
                            (
                                select    record_no, regulation_type, turn_node_id, link_num, link_array,
                                    array_agg(start_node_id) as snode_array,
                                    array_agg(end_node_id) as enode_array
                                from
                                (
                                    select    record_no, regulation_type, turn_node_id, link_num, link_array,
                                        seq, b.start_node_id, b.end_node_id
                                    from
                                    (
                                        select     a.record_no, regulation_type, turn_node_id,
                                            link_num,
                                            generate_series(1, link_num) as seq,
                                            string_to_array(key_string, ',')::bigint[] as link_array
                                        from rdb_region_link_regulation_layer[X]_tbl as a
                                        left join test_regulation_turn_node as b
                                        on a.record_no = b.record_no
                                        --where a.record_no = 98488
                                    )as a
                                    left join rdb_region_link_layer[X]_tbl as b
                                    on a.link_array[a.seq] = b.link_id
                                    order by record_no, regulation_type, turn_node_id, link_num, link_array, seq
                                )as t
                                group by record_no, regulation_type, turn_node_id, link_num, link_array
                            )as t2
                        )as a
                        left join rdb_region_layer[X]_link_mapping as b
                        on a.link_array[a.seq] = b.region_link_id
                        order by record_no, regulation_type, link_num, seq
                    )as t
                    group by record_no, regulation_type, link_num
                  ) as f
                  left join rdb_link_regulation as g
                  on f.record_no = g.record_no
                  where f.regulation_type <> g.regulation_type 
                       --or f.last_link_dir <> g.last_link_dir
                       or (key_string_14 is null or strpos(key_string_14,key_string) = 0);
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
        