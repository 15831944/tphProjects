# -*- coding: utf8 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_link_add_info(ItemBase):
    '''
    Link锟侥革拷锟斤拷锟斤拷息锟斤拷
    '''

    def __init__(self):
        ItemBase.__init__(self, 'Link_add_info')
       
    def Do_CreateTable(self):
        self.CreateTable2('rdb_link_add_info')
        self.CreateTable2('rdb_link_add_info2')
        
        return 1
    
    def Do_CreateFunction(self):
        'Create Function'
        self.CreateFunction2('rdb_cnv_road_struct_code')
        self.CreateFunction2('rdb_cnv_path_extra_info')
        self.CreateFunction2('rdb_cnv_link_add_info2')       
        return 0
     
    def Do(self):
        self.Do_AddInfo2()
        self.Do_AddInfo()

    def Do_AddInfo2(self):
        sqlcmd1 = """
                INSERT INTO rdb_link_add_info2(link_id, link_id_t, link_add_info2)
                (
                    SELECT distinct n.tile_link_id, n.tile_id, link_add_info2
                    FROM ( 
                          select a.link_id, rdb_cnv_link_add_info2(
                                 erp, rodizio, soi, b.linkid is not null, 0::smallint, c.link_id is not null
                             ) AS link_add_info2
                          from link_tbl a
                          left join (
                            select distinct linkid from safety_zone_tbl a
                                left join rdb_tile_link b
                                on a.linkid = b.old_link_id
                                left join rdb_guideinfo_safety_zone c
                                on b.tile_link_id = c.link_id
                                where c.link_id is not null
                          ) b
                          on a.link_id = b.linkid
                          left join (
                              select distinct link_id from safety_alert_tbl 
                              where type not in (10,11)
                          ) c
                          on a.link_id = c.link_id
                          where erp != 0 or rodizio != 0 
                          or soi != 0
                          or b.linkid is not null
                          or c.link_id is not null
                    ) m
                   LEFT JOIN rdb_tile_link n
                   ON  n.old_link_id = m.link_id
                );
            """
            
        if self.pg.execute2(sqlcmd1) == -1:
            rdb_log.log(self.ItemName, 'Insert record into rdb_link_add_info2 failed.', 'info') 
            return -1
        else:
            self.pg.commit2()
       
    def Do_AddInfo(self):                         
        sqlcmd1 = """
                INSERT INTO rdb_link_add_info(link_id, link_id_t, struct_code, 
                    shortcut_code, parking_flag, etc_lane_flag, path_extra_info)
                (
                  SELECT distinct A.tile_link_id
                        , A.tile_id
                        , A.struct_code as struct_code
                        , 0 as shortcut_code
                        , 0 as parking_code
                        , A.etc_lane_flag as etc_lane_flag
                        , case when B.link_id is null then path_extra_info 
                            else (path_extra_info | (1::smallint << 15))
                          end as path_extra_info
                  FROM 
                  (
                     SELECT  d.tile_link_id, d.tile_id, 
                             rdb_cnv_road_struct_code(elevated, structure, tunnel, 
                                 rail_cross) AS struct_code,
                             (case when etc_only_flag = 1 then 1 
                                   when b.gid is not null then 2 
                                   else 0 
                             end) as etc_lane_flag,
                             rdb_cnv_path_extra_info(paved, uturn, ipd, urban, a.display_class = 19, e.pos_type) 
                                 AS path_extra_info                             
                     FROM link_tbl as a
                     left join (
                         select * from spotguide_tbl where type in (9,10)
                     ) as b
                     on a.link_id = b.inlinkid
                     left join  rdb_tile_link d
                     on a.link_id = d.old_link_id 
                     left join (
                         select link_id
                            ,case when 2 = any(pos_flag_array) and 3 = any(pos_flag_array) then 1
                                else pos_flag_array[1]
                            end as pos_type
                        from (
                            select link_id,array_agg(pos_flag) as pos_flag_array 
                            from (
                                select distinct link_id,pos_flag
                                from stopsign_tbl
                            ) a
                            group by link_id 
                        ) b
                     ) e
                     on a.link_id = e.link_id                
                     where (elevated <> 0 or tunnel <> 0 or structure <> 0 
                            or rail_cross <> 0
                            or (etc_only_flag <> 0 or b.gid is not null)
                            or paved = 0 or uturn = 1
                            or ipd != 0 or urban != 0
                            or display_class in (19,20))
                            or e.link_id is not null
                  ) AS A
                  LEFT JOIN rdb_link_add_info2 B
                  ON A.tile_link_id = B.link_id             
                  ORDER BY tile_link_id
                );
                
                INSERT INTO rdb_link_add_info(link_id, link_id_t, struct_code, 
                    shortcut_code, parking_flag, etc_lane_flag, path_extra_info)
                SELECT distinct a.link_id
                                , a.link_id_t
                                , 0 as struct_code
                                , 0 as shortcut_code
                                , 0 as parking_code
                                , 0 as etc_lane_flag
                                , (1::smallint << 15) as path_extra_info
                FROM rdb_link_add_info2 a
                left join rdb_link_add_info b
                on a.link_id = b.link_id
                where b.link_id is null
                order by a.link_id;                
            """
            
        if self.pg.execute2(sqlcmd1) == -1:
            rdb_log.log(self.ItemName, 'Insert record into rdb_link_add_info failed.', 'info') 
            return -1
        else:
            self.pg.commit2()
            
        return 0
    
    def Do_CreatIndex(self):
        '锟斤拷锟斤拷锟斤拷乇锟斤拷锟斤拷锟�'
        self.CreateIndex2('rdb_link_add_info_link_id_idx')
        self.CreateIndex2('rdb_link_add_info_link_id_t_idx')
        self.CreateIndex2('rdb_link_add_info2_link_id_idx')
        self.CreateIndex2('rdb_link_add_info2_link_id_t_idx')     
        return 0
    
    def _DoContraints(self):
        '锟斤拷锟斤拷锟斤拷'
        sqlcmd = """
                ALTER TABLE rdb_link_add_info DROP CONSTRAINT if exists rdb_link_add_info_link_id_fkey;
                ALTER TABLE rdb_link_add_info
                  ADD CONSTRAINT rdb_link_add_info_link_id_fkey FOREIGN KEY (link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # 锟斤拷锟絣ink锟斤拷link_add_info_flag
        self._DoCheckLinkAddInfoFlg()
        return 0
        
    def _DoCheckLinkAddInfoFlg(self):
        '锟斤拷锟絣ink锟斤拷link_add_info_flag'
        
        sqlcmd = """
                SELECT count(link_id)
                  from (
                    SELECT A.link_id, B.link_add_info_flag
                      FROM rdb_link_add_info AS A
                      LEFT JOIN rdb_link AS B
                      ON A.link_id = B.link_id
                    ) as c
                  where link_add_info_flag <> True;
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0:
                rdb_log.log(self.ItemName, 'Not all link_add_info_flag have been set for link_add_info_flag.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Checked link_add_info_flag for link_add_info_flag.', 'info')
        return 0 

        sqlcmd = """
                SELECT count(link_id)
                  from (
                    SELECT A.link_id,(A.path_extra_info >> 15) as A_flag
                        ,(B.link_id is null) as B_flag,B.link_add_info2
                      FROM rdb_link_add_info AS A
                      LEFT JOIN rdb_link_add_info2 AS B
                      ON A.link_id = B.link_id
                    ) as c
                  where (A_flag = 1 and B_flag = 'f')
                  or (A_flag = 0 and B_flag = 't')
                  or (A_flag = 1 and link_add_info2 = 0);
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0:
                rdb_log.log(self.ItemName, 'Not all link_add_info2_flag have been set.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Checked link_add_info2_flag.', 'info')
        return 0 
        
    def _DoCheckNumber(self):
        'check锟斤拷目'
        
        sqlcmd = """
            select 
                (SELECT  count(a.link_id)
                  FROM link_tbl as a
                  left join (select * from spotguide_tbl where type in (9,10)) as b
                  on a.link_id = b.inlinkid
                  left join (
                        select distinct linkid from safety_zone_tbl 
                  ) c
                  on a.link_id = c.linkid
                  left join (
                        select distinct link_id from safety_alert_tbl 
                  ) d
                  on a.link_id = d.link_id
                  left join (
                        select distinct link_id from stopsign_tbl 
                  ) e
                  on a.link_id = e.link_id
                  where elevated <> 0 or tunnel <> 0 or structure <> 0 or rail_cross <> 0 or (etc_only_flag <> 0 or b.gid is not null)
                  or paved = 0 or uturn = 1 or ipd != 0 or urban != 0 or erp != 0 or rodizio != 0 or soi != 0
                  or display_class in (19,20)
                  or c.linkid is not null or d.link_id is not null or e.link_id is not null
                ) as src_num,
                (SELECT count(link_id) FROM 
                    (
                        select link_id from rdb_link_add_info
                        union
                        select link_id from rdb_link_add_info2
                    ) add_info
                ) as desc_num;
            """
        
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            src_num  = row[0]
            desc_num = row[1]
            if src_num != desc_num:
                rdb_log.log(self.ItemName, 'Number of add info attributes: ' + str(src_num) + ','\
                                           'But Number of rdb_link_add_info: ' + str(desc_num) + '.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Number of add info attributes and rdb_link_add_info: ' + str(src_num) + '.', 'info')
        else:
            rdb_log.log(self.ItemName, "Can't find Number of add info attributes  or  rdb_link_add_info .", 'warning')
        return 0