# -*- coding: UTF8 -*-
'''
Created on 2015-5-6

@author: zhaojie
'''

import component.component_base

class comp_Guideinfo_signpost_uc_ni(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_signpost_uc')
    
    def _Do(self):

        self.log.info('Begin insert into signpost_uc')
               
        self.CreateTable2('signpost_uc_tbl')      
        sqlcmd = '''
            INSERT INTO signpost_uc_tbl(
                        id, nodeid, inlinkid,
                        outlinkid, passlid, passlink_cnt,
                        sp_name, route_no1, route_no2,
                        route_no3, route_no4, exit_no)
            (
                select sign_id, (case when a.s_node in (b.s_node, b.e_node) then a.s_node
                                      when a.e_node in (b.s_node, b.e_node) then a.e_node
                                      else -1::bigint end) as nodeid,
                      inlinkid, outlinkid, passlid, passlink_cnt, signpost_name, route_no1,
                      route_no2, route_no3, route_no4, exit_no
                from
                (
                   SELECT a.sign_id, inlinkid::bigint,outlinkid::bigint, 
                       (case when passlid is null or length(passlid) < 1 then null 
                           else passlid end) as passlid, 
                       (case when passlid is null or length(passlid) < 1 then 0
                            else array_upper(string_to_array(passlid,'|'), 1)
                           end ) as passlink_cnt,
                       (case when passlid is null or length(passlid) < 1 then outlinkid 
                           else (string_to_array(passlid,'|'))[1] end)::bigint as second_link,
                       signpost_name, route_no1, route_no2,
                       route_no3, route_no4, exit_no
                  FROM temp_signpost_uc_name AS a
                  LEFT JOIN temp_signpost_uc_path as b
                  ON a.sign_id = b.path_id
                )temp
                left join link_tbl as a
                on temp.inlinkid = a.link_id
                left join link_tbl as b
                on temp.second_link = b.link_id
                order by sign_id       
            );
                '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('signpost_uc_tbl_node_id_idx')
        self.CreateIndex2('signpost_uc_tbl_nodeid_inlinkid_outlinkid_idx')
        
        return 0
        