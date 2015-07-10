# -*- coding: UTF8 -*-
'''
Created on 2013-3-22

@author: hongchenzai
'''

import platform.TestCase
class CCheckRegionNodeBase(platform.TestCase.CTestCase):
    def _GetLevels(self):
        levels = []
        sqlcmd = """
            SELECT  tablename, btrim(translate(tablename, 'abcdefghijklmnopqrstxyzuvw_', ''))
             FROM  pg_tables 
             where tablename like 'rdb_region_node_layer\%_tbl'
             order by tablename;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        while row:
            levels.append(row[1])
            row = self.pg.fetchone()
        return levels
    
class CCheckRegionNodebranchesLinkValidate(CCheckRegionNodeBase):
    '''Branches Link'''
    def _do(self):
        sqlcmd = """
                select count(b.link_id) 
                  from (
                    select branches[seq_num] as link_id
                      from (
                        SELECT branches, generate_series(1, array_upper(branches, 1)) as seq_num
                          FROM rdb_region_node_layer[X]_tbl
                      ) as a
                  ) as b
                  left join rdb_region_link_layer[X]_tbl as c
                  on b.link_id = c.link_id
                  where c.link_id is null;
            """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row = self.pg.fetchone()
            if row:
                if row[0] != 0:
                    self.logger.error('rdb_region_node_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
        return True
    
class CCheckRegionNodebranchesNumValidate(CCheckRegionNodeBase):
    '''Branches Number'''
    def _do(self):
        sqlcmd = """
            ALTER TABLE rdb_region_node_layer[X]_tbl DROP CONSTRAINT if exists check_branches_num;
            ALTER TABLE rdb_region_node_layer[X]_tbl
              ADD CONSTRAINT check_branches_num CHECK (link_num = array_upper(branches, 1));
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            if self.pg.execute(sqlcmd.replace('[X]', level)) == -1:
                self.logger.error('rdb_region_node_layer[X]_tbl'.replace('[X]', level))
                return False;
            else:
                self.pg.commit()
        return True;
    
class CCheckRegionNodeSeqValidate(CCheckRegionNodeBase):
    '''Branches Seqes(rdb_node里link顺序号一致)'''
    def _do(self):

        sqlcmd = """
select count(c.region_node_id)
  from (
    select region_node_id, region_link_id, seq
      from (
        select region_node_id, branches[seq_num] as region_link_id, seqs[seq_num] as seq
          from (
            SELECT node_id as region_node_id, branches, seqs, 
                   generate_series(1, array_upper(branches, 1)) as seq_num
              FROM rdb_region_node_layer[X]_tbl
          ) as a
      ) as b
  ) as c
  left join rdb_region_layer[X]_link_mapping as d
  on c.region_link_id = d.region_link_id
  left join rdb_region_link_layer[X]_tbl as e
  on  c.region_link_id = e.link_id
  left join rdb_region_layer[X]_node_mapping as f
  on c.region_node_id = f.region_node_id
  left join rdb_node as g
  on f.node_id_14 = g.node_id
  where not ((c.region_node_id = start_node_id and d.link_id_14[1] = g.branches[seq + 1]) 
        or (c.region_node_id = end_node_id and d.link_id_14[array_upper(d.link_id_14, 1)] = g.branches[seq + 1]))  
        ;
        """
            
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_node_layer[X]_tbl'.replace('[X]', level))
                    return False
                else:
                    pass
            else:
                return False
            
        return True;
    
class CCheckRegionNodeUpgradeNode(CCheckRegionNodeBase):
    '''upgrade node'''
    def _do(self):
        
        levels = self._GetLevels()
        levels.sort()
        
        ### 下层的upgrade_nodeid，都能在上层找到
        sqlcmd = """
                SELECT count(upgrade_node_id)
                  FROM rdb_region_node_layer[X]_tbl
                  where upgrade_node_id is not null and upgrade_node_id not in (
                    SELECT node_id
                      FROM rdb_region_node_layer[Y]_tbl
                  );
        """
        i = 0
        rst = True
        while i < (len(levels) - 1):
            #self.logger.info(levels[i])
            self.pg.query(sqlcmd.replace('[X]', levels[i]).replace('[Y]', levels[i + 1]))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_node_layer[X]_tbl'.replace('[X]', levels[i]))
                    rst = False
                else:
                    pass
            else:
                rst = False
            i = i + 1;
                
        ### 上层的nodeid，都是下层的upgrade_nodeid
        sqlcmd = """
             SELECT count(node_id)
              FROM rdb_region_node_layer[Y]_tbl
              where node_id not in (
                SELECT upgrade_node_id
                  FROM rdb_region_node_layer[X]_tbl
                  where upgrade_node_id is not null
              );
        """
        i = 1
        rst = True
        while i < len(levels):
            #self.logger.info(levels[i])
            self.pg.query(sqlcmd.replace('[X]', levels[i - 1]).replace('[Y]', levels[i]))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    self.logger.error('rdb_region_node_layer[Y]_tbl'.replace('[Y]', levels[i]))
                    rst = False
                else:
                    pass
            else:
                rst = False
            i = i + 1;
            
        return rst
        
class CCheckRegionNodeSignalFlag(CCheckRegionNodeBase):
    '''信号机'''
    def _do(self):
        
        ### 信号机所占的百分比
        sqlcmd = """
                select 100.0 * signal_number / all_number 
                 from (
                    select 
                    (SELECT count(node_id)
                      FROM rdb_region_node_layer[X]_tbl
                      where signal_flag = true) as signal_number,
                
                    (SELECT count(node_id)
                      FROM rdb_region_node_layer[X]_tbl) as all_number
                 ) as a;
                """
        levels = self._GetLevels()
        levels.sort(reverse=True)
        i = 0
        while i < len(levels):
            level = levels[i]
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] > (10 * i):
                    log_str = 'rdb_region_node_layer[X]_tbl--'.replace('[X]', level)
                    log_str = log_str + 'Percent of signal is : ' + str(row2[0])
                    self.logger.warning(log_str)
                else:
                    pass
            else:
                pass
            i = i + 1
            
        ### 判断是否和14层的信号机一致
        sqlcmd = """
                SELECT count(a.node_id)
                  FROM rdb_region_node_layer[X]_tbl as a
                  left join rdb_region_layer[X]_node_mapping as b
                  on node_id = region_node_id
                  left join rdb_node as c
                  on b.node_id_14 = c.node_id
                  where signal_flag <> (extend_flag & (1 << 9) > 0);
                """
        levels = self._GetLevels()
        rst    = True
        for level in levels:
            #self.logger.info(level)
            self.pg.query(sqlcmd.replace('[X]', level))
            row2 = self.pg.fetchone()
            if row2:
                if row2[0] != 0:
                    log_str = 'rdb_region_node_layer[X]_tbl--'.replace('[X]', level)
                    log_str = log_str + "Region signal are not same as tile_14's."
                    rst     = False
                    self.logger.warning(log_str)
                else:
                    pass
            else:
                rst = False
            i = i + 1
        return rst;

class CCheckRegionNodeIdValidate(CCheckRegionNodeBase):
    '''Start Node'''
    def _do(self):
        sqlcmd = """
                SELECT max(node_id & ((1::bigint <<32) - 1))
                FROM rdb_region_node_layer[X]_tbl;
        """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            max_node_id_in_tile = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (not max_node_id_in_tile) or (level != '6' and max_node_id_in_tile > 65535):
                return False
        return True;

class CCheckRegionNodeBoundaryFlag(CCheckRegionNodeBase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from rdb_region_node_layer[X]_tbl as a
                    left join 
                    (
                        select distinct node_id
                        from rdb_region_node_layer[X]_tbl as a
                        inner join rdb_region_link_layer[X]_tbl as b
                        on a.node_id in (b.start_node_id, b.end_node_id) and a.node_id_t != b.link_id_t
                    )as b
                    on a.node_id = b.node_id
                    where (a.node_boundary_flag is true and b.node_id is null)
                       or (a.node_boundary_flag is false and b.node_id is not null);
                """
        levels = self._GetLevels()
        for level in levels:
            #self.logger.info(level)
            rec_count = self.pg.getOnlyQueryResult(sqlcmd.replace('[X]', level))
            if (rec_count > 0):
                return False
        return True;
