# -*- coding: cp936 -*-
'''
Created on 2014-11-13

@author: zhaojie
'''
from common import rdb_log
import threading
import common


class rdb_linksequence_info():
    
    strorg_link = ''
    strorg_node = ''
    strdest_table_temp = ''
    strdest_table = ''
    strregulation_table = ''
    pg_client = ''
    
    def __init__(self,pg_temp,org_link,org_node, dest_table_temp,dest_table,regulation_table):
        
        self.strorg_link = org_link
        self.strorg_node = org_node
        self.strdest_table = dest_table
        self.strdest_table_temp = dest_table_temp
        self.strregulation_table = regulation_table
        self.pg_client = pg_temp
        
    def _CreateTable(self):
        
        self.pg_client.CreateTable2(self.strdest_table_temp)
        self.pg_client.CreateTable2(self.strdest_table)
        return 0
    
    def _Add_gid_link(self):
        
        sqlcmd = '''
                select count(*)
                from   information_schema.columns
                where table_name = %s and column_name = 'gid';
                '''     
        if self.pg_client.pg.execute2(sqlcmd,(self.strorg_link,)) == -1:
            return -1
        else:
            igidnum = self.pg_client.pg.fetchone2()[0] 
        
        if igidnum == 0:
            sqlcmd = '''
                    ALTER TABLE %s ADD COLUMN gid serial;
                    '''% str(self.strorg_link)
                    
            if self.pg_client.pg.execute2(sqlcmd) == -1:
                return -1
            else:
                self.pg_client.pg.commit2() 
                
        self.pg_client.CreateIndex2(str(self.strorg_link) + '_gid_idx')
        return 0
    
    def _Get_road_sequence(self):
        
              
        self.pg_client.CreateTable2('temp_link_sequence_dir')       
        self.pg_client.CreateFunction2('rdb_get_rdb_link_sequence')
        self.pg_client.CreateFunction2('rdb_get_outlink_position_sequence')
        self.pg_client.CreateFunction2('rdb_get_dir_sequence')
        self.pg_client.CreateFunction2('rdb_get_outlink_sequence')
        self.pg_client.CreateFunction2('rdb_get_inlink_sequence')
        self.pg_client.CreateFunction2('rdb_get_azm_sequence')
        self.pg_client.CreateFunction2('rdb_get_position_array_sequence')
        self.pg_client.CreateFunction2('rdb_get_angle_sequence')
        self.pg_client.CreateFunction2('rdb_sequence_combine')
        self.pg_client.CreateFunction2('rdb_sequence_get')
        self.pg_client.CreateFunction2('rdb_jude_two_link_type_sequence')
        self.pg_client.CreateFunction2('rdb_get_outlinkid')
        self.pg_client.CreateFunction2('rdb_get_angle_sequence_temp')         
                   
        if self._Add_gid_link() == -1:
            return -1
       
        sqlcmd = '''
                select max(gid),min(gid)
                from %s;
                '''% str(self.strorg_link)
        self.pg_client.pg.execute2(sqlcmd)
        row = self.pg_client.pg.fetchone2()
        min_id  = row[1]
        max_id  = row[0]
        
#        if self.strorg_link == 'rdb_link':           
#            sqlcmd = '''
#                    insert into [to_table_temp](link_id,dir,sequence_id,sequence_link_id,node_id)
#                    (
#                        select (regexp_split_to_array(link_dir_one,E'\\\|+'))[1]::bigint,
#                            (regexp_split_to_array(link_dir_one,E'\\\|+'))[2]::smallint,
#                            (regexp_split_to_array(link_dir_one,E'\\\|+'))[3]::smallint,
#                            (regexp_split_to_array(link_dir_one,E'\\\|+'))[4]::bigint,
#                            node_id
#                        from
#                        (
#                            select node_id,unnest(link_dir) as link_dir_one
#                            from
#                            (
#                                select a.node_id,rdb_get_rdb_link_sequence(a.node_id,array_agg(a.link_id),array_agg(start_node_id),
#                                    array_agg(end_node_id),array_agg(one_way),array_agg(fazm),array_agg(tazm),array_agg(toll),
#                                    array_agg(road_type),array_agg(link_type),array_agg(road_name),array_agg(road_number)) as link_dir
#                                from
#                                (
#                                    select node_id,unnest(branches) as link_id
#                                    from [node_tbl]
#                                    where link_num > 1 and (node_id::bit(32))::integer < 1<<23
#                                        and gid >= %s and gid <= %s
#                                )a
#                                left join [link_tbl] as b
#                                on a.link_id = b.link_id
#                                group by a.node_id
#                            )temp
#                            where link_dir is not null
#                        )temp1
#                    );
#                    '''
#        else:
#            sqlcmd = '''
#                    insert into [to_table_temp](link_id,dir,sequence_id,sequence_link_id,node_id)
#                    (
#                        select (regexp_split_to_array(link_dir_one,E'\\\|+'))[1]::bigint,
#                            (regexp_split_to_array(link_dir_one,E'\\\|+'))[2]::smallint,
#                            (regexp_split_to_array(link_dir_one,E'\\\|+'))[3]::smallint,
#                            (regexp_split_to_array(link_dir_one,E'\\\|+'))[4]::bigint,
#                            node_id
#                        from
#                        (
#                            select node_id,unnest(link_dir) as link_dir_one
#                            from
#                            (
#                                select a.node_id,rdb_get_rdb_link_sequence(a.node_id,array_agg(a.link_id),array_agg(start_node_id),
#                                    array_agg(end_node_id),array_agg(one_way),array_agg(fazm_path),array_agg(tazm_path),array_agg(toll),
#                                    array_agg(road_type),array_agg(link_type),array_agg(road_name),null) as link_dir
#                                from
#                                (
#                                    select node_id,unnest(branches) as link_id
#                                    from [node_tbl]
#                                    where link_num > 1 and (node_id::bit(32))::integer < 1<<23
#                                        and gid >= %s and gid <= %s
#                                )a
#                                left join [link_tbl] as b
#                                on a.link_id = b.link_id
#                                left join rdb_highway_mapping as c
#                                on a.link_id = c.link_id
#                                group by a.node_id
#                            )temp
#                            where link_dir is not null
#                        )temp1
#                    );
#                    '''
        if self.strorg_link == 'rdb_link':
                       
            sqlcmd = '''
                    insert into [to_table_temp](link_id,dir,sequence_id,sequence_link_id,node_id)
                    (
                        select link_id, dir, 0, out_link, node_id
                        from
                        (
                                select a.link_id, a.dir, node_id, rdb_get_outlinkid(array[a.link_id]||array_agg(b.link_id), array[a.toll]||array_agg(b.toll), 
                                    array[a.angle]||array_agg(case when a.node_id = b.start_node_id then b.fazm else b.tazm end),array[a.road_type]||array_agg(b.road_type),
                                    array[a.link_type]||array_agg(b.link_type),array[a.road_name]||array_agg(b.road_name),array[a.road_number]||array_agg(b.road_number)) as out_link
                                from
                                (
                                    select link_id, toll, road_type, link_type, road_name, road_number, start_node_id, end_node_id,
                                        (regexp_split_to_array(node_angle,E'\\\|+'))[1]::bigint as node_id,
                                        (regexp_split_to_array(node_angle,E'\\\|+'))[2]::smallint as angle,
                                        (regexp_split_to_array(node_angle,E'\\\|+'))[3]::smallint as dir
                                    from
                                    (    
                                        select link_id, toll, road_type, link_type, road_name, road_number,unnest(node_angle_array) as node_angle, start_node_id, end_node_id
                                        from
                                        (
                                            select link_id, toll, road_type, link_type, road_name, road_number, start_node_id, end_node_id,
                                                (case when one_way = 1 then array[start_node_id::varchar || '|' || fazm::varchar || '|' || 0::varchar, end_node_id :: varchar || '|' || tazm::varchar || '|' || 1::varchar]
                                                      when one_way = 3 then array[start_node_id::varchar || '|' || fazm::varchar || '|' || 0::varchar]
                                                      else array[end_node_id :: varchar || '|' || tazm::varchar || '|' || 1::varchar] end ) as node_angle_array     
                                            from [link_tbl]
                                            where gid >= %s and gid <= %s and one_way <> 0
                                        )temp
                                    )temp1
                                )a
                                left join [link_tbl] as b
                                on ((a.node_id = b.start_node_id and b.one_way in (1,2)) or
                                   (a.node_id = b.end_node_id and b.one_way in (1,3)))
                                   and b.link_id <> a.link_id
                                group by a.link_id, a.toll, a.road_type, a.link_type, a.road_name, a.road_number, a.node_id, a.angle, a.dir
                        )temp3
                        where out_link is not null
                    );
                    '''
        else:
            self.pg_client.CreateIndex2(str(self.strorg_link) + '_start_node_id_idx')
            self.pg_client.CreateIndex2(str(self.strorg_link) + '_end_node_id_idx')
            
            sqlcmd = '''
                    insert into [to_table_temp](link_id,dir,sequence_id,sequence_link_id,node_id)
                    (
                        select link_id, dir, 0, out_link, node_id
                        from
                        (
                                select a.link_id, a.dir, node_id, rdb_get_outlinkid(array[a.link_id]||array_agg(b.link_id), array[a.toll]||array_agg(b.toll), 
                                    array[a.angle]||array_agg(case when a.node_id = b.start_node_id then b.fazm_path else b.tazm_path end),array[a.road_type]||array_agg(b.road_type),
                                    array[a.link_type]||array_agg(b.link_type),array[a.road_name]||array_agg(b.road_name), null) as out_link
                                from
                                (
                                    select link_id, toll, road_type, link_type, road_name, start_node_id, end_node_id,
                                        (regexp_split_to_array(node_angle,E'\\\|+'))[1]::bigint as node_id,
                                        (regexp_split_to_array(node_angle,E'\\\|+'))[2]::smallint as angle,
                                        (regexp_split_to_array(node_angle,E'\\\|+'))[3]::smallint as dir
                                    from
                                    (    
                                        select link_id, toll, road_type, link_type, road_name, unnest(node_angle_array) as node_angle, start_node_id, end_node_id
                                        from
                                        (
                                            select link_id, toll, road_type, link_type, road_name, start_node_id, end_node_id,
                                                (case when one_way = 1 then array[start_node_id::varchar || '|' || fazm_path::varchar || '|' || 0::varchar, end_node_id :: varchar || '|' || tazm_path::varchar || '|' || 1::varchar]
                                                      when one_way = 3 then array[start_node_id::varchar || '|' || fazm_path::varchar || '|' || 0::varchar]
                                                      else array[end_node_id :: varchar || '|' || tazm_path::varchar || '|' || 1::varchar] end ) as node_angle_array     
                                            from [link_tbl]
                                            where gid >= %s and gid <= %s and one_way <> 0
                                        )temp
                                    )temp1
                                )a
                                left join [link_tbl] as b
                                on ((a.node_id = b.start_node_id and b.one_way in (1,2)) or
                                   (a.node_id = b.end_node_id and b.one_way in (1,3)))
                                   and b.link_id <> a.link_id
                                group by a.link_id, a.toll, a.road_type, a.link_type, a.road_name, a.node_id, a.angle, a.dir
                        )temp3
                        where out_link is not null
                    );
                    '''

        sqlcmd = sqlcmd.replace('[to_table_temp]',str(self.strdest_table_temp))
        sqlcmd = sqlcmd.replace('[link_tbl]',str(self.strorg_link))
        pg = common.rdb_database.rdb_pg()
        pg.multi_execute(sqlcmd,min_id,max_id,4,50000)
        
        self.pg_client.CreateIndex2(self.strdest_table_temp + '_link_id_idx')
            
        sqlcmd = '''
                insert into %s(link_id,sequence, s_link_id, e_link_id)
                (
                    select link_id, sequence, link_array[1], link_array[2]
                    from
                    (
                        select link_id,rdb_sequence_combine(array_agg(dir),array_agg(sequence_id)) as sequence,
                                  rdb_sequence_get(array_agg(dir),array_agg(sequence_link_id)) as link_array 
                        from %s as a
                        left join %s as b
                        on a.link_id = b.first_link_id and a.sequence_link_id = b.last_link_id
                            and b.regulation_type = 0 and link_num = 2
                        where b.regulation_id is null
                        group by link_id
                    )temp
                );
                '''% (self.strdest_table,self.strdest_table_temp,self.strregulation_table)
        if self.pg_client.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg_client.pg.commit2()
        
        self.pg_client.CreateIndex2(self.strdest_table + '_link_id_idx')

        return 0
