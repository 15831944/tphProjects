# -*- coding: UTF8 -*-
'''
Created on 2014-9-17

@author: yuanrui
'''
import component.component_base


class comp_update_patch_jdb(component.component_base.comp_base):
    '''当link属于SAPA和IC共同经过时，link_type修改成SAPA_link.'''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'update_patch_sapa_link')

    def _Do(self):
        # Highway机能link全部作成有料道路
        self._update_toll_link()
        # Highway机能link的display class全部作成10
        self._update_toll_display_class()        
        # 取得Uturn、IC、SAPA都经过的link
        self._get_uturn_ic_sapa_link()
        # 取得所有SAPA路径经过的link及原有的link_type
        self._get_sapa_path_link()
        # 把SAPA路径经过的，且link_type原本是Ramp的link_type,改成SAPA link
        self._update_sapa_link_type()
        # 检查是否存在其他预料之外的link_type(Sapa, JCT, ramp之外)
        self._check_other_link_type()
#        # 检查无料区间的RAMP
#        self._check_no_toll_ramp()
        # 检查不经过SAPA的UTURN&RAMP
        self._check_isolated_uturn_ramp()

    def _update_toll_link(self):
        '''Highway机能的link，全部作成有料道路。
        '''
        sqlcmd = """
        UPDATE link_tbl SET toll = 1
          FROM highway_mapping as a
          where link_tbl.link_id = a.link_id
          and link_tbl.toll <> 1;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_toll_display_class(self):    
        '''Highway机能的link，若其road_type非高速，将其display class作成10(toll road)。
        '''
        sqlcmd = """
        UPDATE link_tbl SET display_class = 10
          FROM highway_mapping as a
          where link_tbl.link_id = a.link_id
          and link_tbl.display_class <> 10
          and link_tbl.road_type not in (0,1);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def _get_uturn_ic_sapa_link(self):
        '''取得Uturn、IC、SAPA都经过的link.'''
        self.CreateTable2('temp_link_uturn_ic_sapa')
        sqlcmd = """
        INSERT INTO temp_link_uturn_ic_sapa
        (
        SELECT link_id, type_array
        from (
              select link_id, array_agg(path_type) AS type_array
              FROM (
                select regexp_split_to_table(rtrim(ltrim(link_lid,'{'),
                                            '}'), E',')::BIGINT as link_id,
                       path_type, toll_flag
                from mid_hwy_ic_path
              ) AS A GROUP BY link_id
        ) as b
        where 'UTURN' = any(type_array)
              and 'IC' = any(type_array)
              and 'SAPA' = any(type_array)
              and not 'JCT' = any(type_array)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_sapa_path_link(self):
        '''取得所有SAPA路径经过的link及原有的link_type.
           toll_flag: 1--高速Model，0--无料区间
        '''
        self.CreateTable2('mid_temp_sapa_path_link')
        sqlcmd = """
        INSERT INTO mid_temp_sapa_path_link(link_id, link_type, toll_flag)
        (
        SELECT n.link_id, link_type, toll_flag
        FROM (
            select distinct
                   regexp_split_to_table(link_lid, E',')::BIGINT as link_id,
                   toll_flag
            from (
              select rtrim(ltrim(link_lid,'{'),'}') as link_lid, toll_flag
              from mid_hwy_ic_path
              where (path_type = 'SAPA' or
                     (path_type = 'UTURN' and s_facilcls in (1, 2))
                    ) and link_lid is not null
            ) m

            union
            -- All links of Uturn (Uturn Path crossing with SAPA.)
            select distinct m.link_id, m.toll_flag
            from (
                select distinct unnest(link_lid) as link_id, toll_flag
                from (
                    -- Get All Uturn Links
                    select regexp_split_to_array(rtrim(ltrim(link_lid,'{'), '}'
                                                ), E',')::BIGINT[] as link_lid,
                           path_type, toll_flag, s_facilcls, t_facilcls
                    from mid_hwy_ic_path
                    where path_type = 'UTURN'
                ) a
                left join temp_link_uturn_ic_sapa b
                on b.link_id = any(a.link_lid)
                where b.link_id is not null
            ) m
            left join (
                select distinct
                       regexp_split_to_table(rtrim(ltrim(link_lid, '{'),
                                             '}'), E',')::BIGINT as link_id,
                       path_type, toll_flag, s_facilcls, t_facilcls
                from mid_hwy_ic_path
                where path_type = 'IC'
            ) n
            on m.link_id = n.link_id
            where n.link_id is not null
        ) AS n
        LEFT JOIN link_tbl
        ON n.link_id = link_tbl.link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_sapa_link_type(self):
        '''据SAPA路径经过的，且link_type原本是Ramp的link_type,改成SAPA link.
           JCT/本线等不做修改, 无料区间
           (大概修改记录193条)
        '''
        sqlcmd = """
        UPDATE link_tbl SET link_type = 7
          FROM mid_temp_sapa_path_link as a
          where link_tbl.link_id = a.link_id
                and a.link_type = 5;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _check_other_link_type(self):
        '''检查是否存在其他预料之外的link_type(Sapa, JCT, ramp之外)'''
        sqlcmd = """
        SELECT count(*)
          FROM mid_temp_sapa_path_link
          WHERE link_type not in (3, 5, 7);
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] != 0:
            self.log.warning('Exist other link_type.')

    def _check_no_toll_ramp(self):
        '''检查无料区间的RAMP'''
        sqlcmd = """
        SELECT count(*)
          FROM mid_temp_sapa_path_link
          WHERE link_type = 5 and toll_flag = 0;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] != 0:
            self.log.warning('Exist Ramp Link'
                             '(In No Toll SAPA Path).')

    def _check_isolated_uturn_ramp(self):
        '''检查所有link_type=5并是Uturn经过的link.'''
        sqlcmd = """
        select count(distinct m.link_id)
        from (
           -- Get All Uturn Links
           select regexp_split_to_table(rtrim(ltrim(link_lid, '{'), '}'),
                                        E',')::BIGINT as link_id,
                  path_type, toll_flag
             from mid_hwy_ic_path
             where path_type = 'UTURN'
        ) m
        left join link_tbl l
        on m.link_id = l.link_id
        where l.link_type = 5;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] != 0:
            self.log.warning('Exist Uturn & Ramp Link '
                             'that not connected to SAPA.')
