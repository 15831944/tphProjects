# -*- coding: cp936 -*-
'''
Created on 2011-12-21

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_regulation(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Regulation')
        
    def Do(self):
        
        self.CreateTempLinkRegulation()
        
        self.CreateLinkRegulation()
        
        self.CreateLinkCondRegulation()
        
        return 0
    
    def CreateTempLinkRegulation(self):
        
        if self.CreateTable2('temp_link_regulation') == -1:
            exit(1)
        
        rdb_log.log('Regulation', 'Begin converting temp_link_regulation...', 'info')
        
        sqlcmd = """
                    insert into temp_link_regulation
                    (
                        "new_regulation_id", "old_regulation_id", "condtype", 
                        "link_id", "link_dir", "tile_id", "seq_num"
                    )
                    (
                        select   new_regulation_id, old_regulation_id, condtype,
                                 tl.tile_link_id as link_id, 
                                 tr.link_dir, 
                                 tl.tile_id, 
                                 tr.seq_num
                         from
                         (
                             select  a.new_regulation_id,
                                     a.old_regulation_id,
                                     a.condtype,
                                     linkid as old_link_id, 
                                     link_dir,
                                     seq_num
                             from
                             (
                                 select  row_number() over (order by regulation_id, condtype) as new_regulation_id, 
                                         regulation_id as old_regulation_id, 
                                         condtype
                                 from regulation_relation_tbl
                                 where condtype in (1,2,3,4,42,43,10,11,12)
                                 group by regulation_id, condtype
                             )as a
                             left join
                             (
                                 -- for link regulation
                                 select  y.regulation_id, condtype, linkid, seq_num,
                                         (
                                          case condtype 
                                             when 2 then 1 
                                             when 3 then 2 
                                             when 4 then 0 
                                             when 42 then 1
                                             when 43 then 2 
                                          end
                                         ) as link_dir
                                 from
                                 (
                                     select regulation_id, condtype
                                     from regulation_relation_tbl 
                                     where condtype in (2,3,4,42,43)
                                     group by regulation_id, condtype
                                 ) as x
                                 left join regulation_item_tbl as y
                                 on x.regulation_id = y.regulation_id
                                 
                                 union
                                 
                                 -- for linkrow regulation
                                 select  regulation_id, condtype, link_array[old_seq_num] as linkid,
                                         (case old_seq_num when 1 then 1 else old_seq_num-1 end) as seq_num,
                                         (case old_seq_num
                                          when 1 then (case when enode_array[1] = node_array[2] then 1 else 2 end)
                                          when 3 then (case when snode_array[3] = node_array[2] then 1 else 2 end)
                                          else (case when snode_array[old_seq_num] in (snode_array[old_seq_num-1],enode_array[old_seq_num-1]) then 1 else 2 end)
                                          end
                                         )as link_dir
                                 from
                                 (
                                     select  regulation_id, condtype,
                                             generate_series(1, array_upper(seq_array,1)) as old_seq_num,
                                             link_array, node_array, snode_array, enode_array
                                     from
                                     (
                                         select  regulation_id, condtype, array_agg(old_seq_num) as seq_array,
                                                 array_agg(linkid) as link_array, array_agg(nodeid) as node_array, 
                                                 array_agg(s_node) as snode_array, array_agg(e_node) as enode_array
                                         from
                                         (
                                             select y.regulation_id, condtype, seq_num as old_seq_num, linkid, nodeid, s_node, e_node
                                             from
                                             (
                                                 select regulation_id, condtype
                                                 from regulation_relation_tbl
                                                 where condtype in (1,10,11,12)
                                                 group by regulation_id, condtype
                                             )as x
                                             left join regulation_item_tbl as y
                                             on x.regulation_id = y.regulation_id
                                             left join link_tbl as z
                                             on y.linkid = z.link_id
                                             order by y.regulation_id asc, condtype, y.seq_num asc
                                         )as r
                                         group by regulation_id, condtype
                                     )as s
                                 )as t
                                 where old_seq_num != 2
                             )as b
                             on a.old_regulation_id = b.regulation_id and a.condtype = b.condtype and b.linkid is not null
                         )as tr
                         left join rdb_tile_link as tl
                         on tr.old_link_id = tl.old_link_id
                         order by new_regulation_id asc, condtype asc, seq_num asc
                     );
                """
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'converting temp_link_regulation failed.', 'error')
            exit(1)
        else:
            self.pg.commit2()
        
        self.CreateIndex2('temp_link_regulation_link_id_idx')
        
        rdb_log.log('Regulation', 'converting temp_link_regulation end.', 'info')
        
        return 0
    
    def CreateLinkRegulation(self):
        
        if self.CreateTable2('rdb_link_regulation') == -1:
            exit(1)
        
        if self.CreateFunction2('rdb_array2string') == -1:
            exit(1)
        
        rdb_log.log('Regulation', 'Begin converting rdb_link_regulation...', 'info')
        
        sqlcmd = """
                    insert into rdb_link_regulation
                        ("regulation_id", "regulation_type", "is_seasonal", 
                         "first_link_id", "first_link_id_t", 
                         "last_link_id", "last_link_dir", "last_link_id_t", 
                         "link_num", "key_string"
                        )
                        (
                            select  (case when c.regulation_id is null then new_regulation_id else 0 end) as regulation_id,
                                (
                                case 
                                when r.condtype in (1,4,42,43) then 0 
                                when r.condtype in (2,3,10) then 1
                                when r.condtype = 11 then 2 
                                when r.condtype = 12 then 3
                                end
                                ) as regulation_type,
                                s.is_seasonal, 
                                links[1] as first_link_id,
                                tiles[1] as first_link_id_t,
                                links[array_upper(links,1)] as last_link_id,
                                ldirs[array_upper(ldirs,1)] as last_link_dir,
                                tiles[array_upper(tiles,1)] as last_link_id_t,
                                array_upper(links,1) as link_num,
                                rdb_array2string(links) as key_string
                            from
                            (
                                select  new_regulation_id, old_regulation_id, condtype,
                                        array_agg(link_id) as links, array_agg(link_dir) as ldirs, array_agg(tile_id) as tiles
                                from
                                (
                                    select *
                                    from temp_link_regulation
                                    order by new_regulation_id, old_regulation_id, condtype, seq_num
                                )as a
                                group by new_regulation_id, old_regulation_id, condtype
                            )as r
                            left join
                            (
                                select distinct regulation_id, condtype, is_seasonal, cond_id
                                from regulation_relation_tbl
                                where cond_id is null
                            )as c
                            on r.old_regulation_id = c.regulation_id and r.condtype = c.condtype
                            left join 
                            (
                                select distinct regulation_id, condtype, is_seasonal
                                from regulation_relation_tbl
                            )as s
                            on r.old_regulation_id = s.regulation_id and r.condtype = s.condtype
                            order by new_regulation_id asc, old_regulation_id asc
                        )
                """
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'converting rdb_link_regulation failed.', 'error')
            return -1
        
        self.pg.commit2()
        
        
        self.pg.CreateIndex2_ByName('rdb_link_regulation_idx')
        self.pg.CreateIndex2_ByName('rdb_link_regulation_key_string_idx')
        self.pg.CreateIndex2_ByName('rdb_link_regulation_regulation_id_idx')    
        self.pg.CreateIndex2_ByName('rdb_link_regulation_key_string_last_link_id_idx')
        self.pg.CreateIndex2_ByName('rdb_link_regulation_first_link_id_idx')
        self.pg.CreateIndex2_ByName('rdb_link_regulation_first_link_id_t_idx')
        self.pg.CreateIndex2_ByName('rdb_link_regulation_last_link_id_idx')
        self.pg.CreateIndex2_ByName('rdb_link_regulation_last_link_id_t_idx')
        
        rdb_log.log('Regulation', 'converting rdb_link_regulation end.', 'info')
    
    def CreateLinkCondRegulation(self):
        
        if self.CreateTable2('rdb_link_cond_regulation') == -1:
            exit(1)
        
        rdb_log.log('Regulation', 'Begin converting rdb_link_cond_regulation...', 'info')
        
        sqlcmd = """
                    insert into rdb_link_cond_regulation
                        (  
                           "regulation_id", 
                           "car_type", 
                           "start_date", 
                           "end_date", 
                           "month_flag", 
                           "day_flag",
                           "week_flag",
                           "start_time",
                           "end_time",
                           "exclude_date"
                        )
                        (
                            select  regulation_id, car_type, start_date, end_date, month_flag, 
                                    day_flag, day_of_week, start_time, end_time, exclude_date
                            from
                            (
                            select  new_regulation_id as regulation_id,
                                    car_type,
                                    (start_year * 10000 + start_month * 100 + start_day) as start_date,
                                    (end_year * 10000 + end_month * 100 + end_day) as end_date,
                                    0 as month_flag,
                                    0 as day_flag,
                                    day_of_week,
                                    (start_hour * 100 + start_minute) as start_time,
                                    (end_hour * 100 + end_minute) as end_time,
                                    exclude_date
                                from
                                (
                                    select  new_regulation_id, old_regulation_id, condtype
                                     from temp_link_regulation
                                     group by new_regulation_id, old_regulation_id, condtype
                                ) as a
                                left join regulation_relation_tbl as b
                                on a.old_regulation_id = b.regulation_id and a.condtype = b.condtype
                                left join condition_regulation_tbl as c
                                on b.cond_id = c.cond_id
                                where b.cond_id is not null
                            ) as t
                            order by regulation_id, car_type, start_date, end_date, month_flag, 
                                    day_flag, day_of_week, start_time, end_time, exclude_date
                        )
                """
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'converting rdb_link_cond_regulation failed.', 'error')
            return -1
        
        self.pg.CreateIndex2_ByName('rdb_link_cond_regulation_idx')
        
        self.pg.commit2()
        
        rdb_log.log('Regulation', 'converting rdb_link_cond_regulation end.', 'info')
            
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_link_regulation DROP CONSTRAINT if exists rdb_link_regulation_last_link_id_fkey;    
                ALTER TABLE rdb_link_regulation
                  ADD CONSTRAINT rdb_link_regulation_last_link_id_fkey FOREIGN KEY (last_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
    
    def _DoCheckLogic(self):
        rdb_log.log('Regulation', 'Check regulation...', 'info')
        
        self.CreateFunction2('rdb_check_regulation_item')
        self.pg.callproc('rdb_check_regulation_item')
        
        self.CreateFunction2('rdb_check_regulation_condition')
        self.pg.callproc('rdb_check_regulation_condition')
        
        self.CreateFunction2('rdb_check_regulation_flag')
        self.pg.callproc('rdb_check_regulation_flag')
        
        rdb_log.log('Regulation', 'Check regulation end.', 'info')
        return 0
    
    def _DoCheckNumber(self):
        
        src_tbl  = 'regulation_relation_tbl'
        src_col  = 'regulation_id, condtype'
        dest_tbl = 'rdb_link_regulation'
        dest_col = 'record_no'
        sqlcmd = "SELECT "
        sqlcmd = sqlcmd + "(SELECT count(distinct(" + src_col +")) FROM " + src_tbl +") as src_num, "
        sqlcmd = sqlcmd + "(SELECT count(" + dest_col +") FROM " + dest_tbl +") as desc_num;"
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            src_num  = row[0]
            desc_num = row[1]
            if src_num != desc_num:
                rdb_log.log(self.ItemName, 'Number of ' + src_tbl + ': ' + str(src_num) + ','\
                                           'But Number of ' + dest_tbl + ': ' + str(desc_num) + '.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Number of ' + src_tbl + ' and ' + dest_tbl + ': ' + str(src_num) + '.', 'info')
        else:
            rdb_log.log(self.ItemName, "Can't find Number of " + src_tbl + ' or ' + dest_tbl +'.', 'warning')
        return 0
  