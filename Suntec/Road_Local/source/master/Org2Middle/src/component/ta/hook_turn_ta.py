# -*- coding: UTF8 -*-
'''
Created on 2015-3-13
@author: zhaojie
'''

import component.component_base

class comp_hook_turn_ta(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.component_base.comp_base.__init__(self, 'hook_turn')
    
    #Creat table
    def _DoCreateTable(self):
        
        self.CreateTable2('hook_turn_tbl')
        return 0
    #alter table    
    def _Do(self):
        if not (self.pg.IsExistTable("scpoint") and self.pg.IsExistTable("scpoint_ext") and
                self.pg.IsExistTable("scpoint_ll") and self.pg.IsExistTable("scpoint_ext_ll") and
                self.pg.IsExistTable("scpoint_status") and self.pg.IsExistTable("scpoint_ext_status")):
            self.log.warning('table is not exist!!!')
            return 0
        self.__get_link_array()
        self.__order_link_array()
        self.__insert_into_hook_turn()
        
        return 0
        
    def __get_link_array(self):            
        self.log.info('Begin get_link_array.')
        self.CreateIndex2('scpoint_ext_featid_idx')
        self.CreateIndex2('scpoint_ext_ll_featid_idx')
        self.CreateTable2('temp_scpoint_ext_link')
        
        sqlcmd = """
                insert into temp_scpoint_ext_link(id, link_id_array, s_node_id_array, 
                                e_node_id_array, oneway_array, link_num)
                (
                    select  id,
                            array_agg(link_id) as link_id_array,
                            array_agg(s_node_id) as s_node_id_array,
                            array_agg(e_node_id) as e_node_id_array,
                            array_agg(oneway) as oneway_array,
                            count(*) as link_num
                    from
                    (
                        select  a.featid::bigint as id, 
                                c.id as link_id, 
                                c.f_jnctid as s_node_id, 
                                c.t_jnctid as e_node_id, 
                                (case when c.oneway is null then 1
                                      when c.oneway = 'FT' then 2
                                      when c.oneway = 'TF' then 3
                                      else 4 end) as oneway
                        FROM scpoint_ext as a
                        left join scpoint_ext_ll as b
                        on a.featid = b.featid
                        left join org_nw as c
                        on c.id = b.shape_line_id::bigint and 
                          (c.oneway is null or c.oneway <> 'N')
                        where a.cameratype = '43' and c.id is not null
                        order by a.featid, b.gid
                    )temp
                    group by id having count(*) > 1
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()
        
        return 0
    
    def __order_link_array(self):           
        self.log.info('Begin order_link_array.')
        self.CreateFunction2('mid_in_array_count')
        self.CreateFunction2('mid_get_link_order')
        self.CreateTable2('temp_scpoint_ext_link_order')
        sqlcmd = """
                insert into temp_scpoint_ext_link_order(id, inlink, outlink, nodeid, link_array, link_num)
                (
                    select id, link_node_array[1] as inlink, 
                        link_node_array[array_upper(link_node_array,1)-1],
                        link_node_array[array_upper(link_node_array,1)] as nodeid,
                        (case when array_upper(link_node_array,1) > 3 then 
                            link_node_array[2:(array_upper(link_node_array,1)-2)]
                            else null end) as link_array,
                        (array_upper(link_node_array,1)-3) as link_num
                    from
                    (
                        select id, mid_get_link_order(link_id_array, s_node_id_array, 
                                        e_node_id_array,oneway_array, link_num) as link_node_array
                        from temp_scpoint_ext_link
                    )temp
                    where link_node_array is not null
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()        
        
        return 0
        
    def __insert_into_hook_turn(self):           
        self.log.info('Begin insert_into_hook_turn.')
        
        sqlcmd = """
                insert into hook_turn_tbl(id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt)
                (
                    select id, nodeid, inlink, outlink,
                        (case when link_array is not null then array_to_string(link_array,'|')
                            else null end) as passlib, link_num
                    from temp_scpoint_ext_link_order
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()
        
        self.CreateIndex2('hook_turn_tbl_inlinkid_idx')
        self.CreateIndex2('hook_turn_tbl_nodeid_idx')
        
        return 0

    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
                
    def _DoCheckValues(self):
        self.log.info('Begin CheckValues.')
        
        sqlcmd = """
                select count(*)
                from
                (
                    select a.inlinkid
                    from hook_turn_tbl as a
                    left join link_tbl as b
                    on a.inlinkid = b.link_id
                    left join node_tbl as c
                    on a.nodeid = c.node_id
                    left join link_tbl as d
                    on a.outlinkid = d.link_id
                    where b.link_id is null or c.node_id is null or d.link_id is null
                    
                    union
                    
                    select a.linkid::bigint
                    from
                    (
                        select unnest(string_to_array(passlid, '|')) as linkid
                        from hook_turn_tbl
                        where passlid is not null
                    )a
                    left join link_tbl as b
                    on a.linkid::bigint = b.link_id
                    where b.link_id is null
                )temp;
                """
        row_num = (self.__GetRows(sqlcmd))[0][0]
        
        if row_num <> 0:
#            self.log.error('hook_turn_tbl values is error! %d',row_num)
            self.log.warning('hook_turn_tbl values is error! %d',row_num)
        
        return 0
    def _DoCheckNumber(self):
        self.log.info('Begin CheckNumber.')
        
        sqlcmd = """
                select count(*)
                from hook_turn_tbl;
                """
        row_num_tbl = (self.__GetRows(sqlcmd))[0][0]
        
        if not (self.pg.IsExistTable("scpoint") and self.pg.IsExistTable("scpoint_ext") and
                self.pg.IsExistTable("scpoint_ll") and self.pg.IsExistTable("scpoint_ext_ll") and
                self.pg.IsExistTable("scpoint_status") and self.pg.IsExistTable("scpoint_ext_status")):
            row_num_org = 0
        else:       
            sqlcmd = """             
                    select count(*)
                    from scpoint_ext
                    where cameratype = '43';
                    """
            row_num_org = (self.__GetRows(sqlcmd))[0][0]
        
        if row_num_tbl <> row_num_org:
            self.log.error('org_data num is %d, table_data num is %d',row_num_org, row_num_tbl)
            
        return 0

    def _DoCheckLogic(self):
        self.log.info('Begin CheckLogic.')
        
                
        sqlcmd = """
                select count(*)
                from hook_turn_tbl as a
                left join link_tbl as b
                on b.link_id in (a.inlinkid, a.outlinkid) and b.one_way_code = 4              
                where b.link_id is not null
                """
        row_num = (self.__GetRows(sqlcmd))[0][0]
        
        if row_num <> 0:
            self.log.error('hook_turn_tbl link oneway is error! %d',row_num)
        
        return 0  

        