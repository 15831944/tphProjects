# -*- coding: UTF8 -*-
'''
Created on 2012-9-17

@author: zhangliang
'''

from component.default.guideinfo_spotguide import comp_guideinfo_spotguide
jv_map = {0:'left',
          1:'straight',
          2:'right',
          3:'left_left',
          4:'left_right',
          5:'right_left',
          6:'right_right',
          7:'straight_left',
          8:'straight_right'
          }

class comp_guideinfo_spotguide_sensis(comp_guideinfo_spotguide):
    '''
    deal with junction view
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('temp_extend_jv_tbl')
        self.CreateTable2('spotguide_tbl')

    def _DoCreateFunction(self):
        self.CreateFunction2('mid_findpasslinkbybothnodes')
        return 0

    def _Do(self):
        # extend jv information 
        self._extend_jv_records()
        
        sqlcmd = '''
            drop table if exists temp_prepare_jv_tbl;
    
            with temp_full_sensis_jv_info 
            as
            (
                select in_node_id, out_link_id, picture_type, picture_name,node_connect_links,
                    n.one_way_code as connect_link_dir, s_node, e_node,out_link_dir,out_link_s_node,out_link_e_node
                from
                (
                    SELECT in_node_id, out_link_id, picture_type, picture_name, 
                        unnest(string_to_array(b.node_lid,'|'))::bigint as node_connect_links, c.one_way_code as out_link_dir, c.s_node as out_link_s_node, c.e_node as out_link_e_node
                      FROM temp_extend_jv_tbl as a
                      left join node_tbl as b
                      on a.in_node_id = b.node_id
                      left join link_tbl as c
                      on a.out_link_id = c.link_id
                ) as m
                left join link_tbl as n
                on m.node_connect_links = n.link_id
            ) 
            select *
            into temp_prepare_jv_tbl
            from
            (
                select node_connect_links as in_link_id, in_node_id as node_id, out_link_id, out_link_dir, out_link_s_node, out_link_e_node,
                    picture_type, picture_name, case when out_link_dir in (1,2) then out_link_s_node else out_link_e_node end as search_node
                from temp_full_sensis_jv_info as a
                where in_node_id = s_node and connect_link_dir in (1,3)
                union
                select node_connect_links as in_link_id, in_node_id as node_id, out_link_id, out_link_dir, out_link_s_node, out_link_e_node,
                    picture_type, picture_name, case when out_link_dir in (1,2) then out_link_s_node else out_link_e_node end as search_node
                from temp_full_sensis_jv_info as a
                where in_node_id = e_node and connect_link_dir in (1,2)
            ) as a;
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        sqlcmd = '''
                insert into spotguide_tbl
                (
                nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                direction, guideattr, namekind, guideclass,
                patternno, arrowno, type
                )
                (
                    select node_id, in_link_id, out_link_id, 
                       (case when pass_links_array is null then null
                             else array_to_string(pass_links_array, '|') end) as passlid,
                       (case when pass_links_array is null then 0
                             else array_upper(pass_links_array, 1) end) as passlink_cnt,                             
                        0,0,0,0,
                        picture_name, null,type
                    from 
                    (
                        select node_id, in_link_id, out_link_id, string_to_array(mid_findpasslinkbybothnodes(node_id, search_node,30),',') as pass_links_array,
                            picture_name, case when b.road_type in (0,1) then 2 else 5 end as type
                        from temp_prepare_jv_tbl as a
                        left join link_tbl as b
                        on a.in_link_id = b.link_id
                    ) as c
                    where in_link_id <> out_link_id
                );
            '''
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self)
        return 0

    def _extend_jv_records(self):
        # transform jv table format
        
        sqlcmd = """
            SELECT dataset,junctionview_id,junction_id,junction_id_shape,junction_id_x,junction_id_y,
                  left_arc_id,left_arc_id_shape,l_arc_x,l_arc_y,
                  straight_arc_id,straight_arc_id_shape,s_arc_x,s_arc_y,
                  right_arc_id,right_arc_id_shape,r_arc_x,r_arc_y,
                  left_left_arc_id,left_left_arc_id_shape,ll_arc_x,ll_arc_y,
                  left_right_arc_id,left_right_arc_id_shape,lr_arc_x,lr_arc_y,
                  right_left_arc_id,right_left_arc_id_shape,rl_arc_x,rl_arc_y,
                  right_right_arc_id,right_right_arc_id_shape,rr_arc_x,rr_arc_y,
                  straight_left_arc_id,straight_left_arc_id_shape,sl_arc_x,sl_arc_y,
                  straight_right_arc_id,straight_right_arc_id_shape,sr_arc_x,sr_arc_y
            FROM mid_sensis_jv_tbl;
         """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            dataset = row[0]
            junctionview_id = row[1]
            junction_id = row[3]
            for i in range(9):
                cur_output_link = row[7 + i*4]
                
                if cur_output_link.upper() <> 'NULL':
                    sqlcmd = '''
                        insert into temp_extend_jv_tbl (in_node_id, out_link_id, picture_type,picture_name)
                        values (%s, %s, %s, %s)
                    ''' %(junction_id, cur_output_link, i, ('\'' + junctionview_id + '_' + jv_map[i] + '\'').lower())
                    
                    self.pg.execute(sqlcmd)
                
        self.pg.commit()
        
        return 0