# -*- coding: cp936 -*-
'''
Created on 2014-8-5

@author: zhaojie
'''
import component.component_base

class comp_regulation_msm(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _DoCreateTable(self):
        self.CreateTable2('condition_regulation_tbl')
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl') 
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_get_link_array')
        self.CreateFunction2('mid_get_acc_mask')
        return 0
    
    def _Do(self):
        self.__prepare_link()
        self.__make_one_link()
        self.__delete_unnecessary_link()
        self.__make_link_array()
        self.__make_country_link()
        
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('regulation_relation_tbl_regulation_id_idx')
        self.CreateIndex2('regulation_relation_tbl_inlinkid_idx')
        self.CreateIndex2('regulation_item_tbl_regulation_id_idx')
        return 0
    
    def __prepare_link(self):
        self.log.info('Begin prepare for regulation link...')
        
        #get regulation for turn_rstrs
        self.CreateTable2('temp_turn_rstrs')
        sqlcmd = '''
            insert into temp_turn_rstrs(f_link_id,node_id,t_link_id,s_e,folder)
            (
                select distinct link_id,0,substr(link_array[seq],2,length(link_array[seq]) - 1)::varchar,
                    substr(link_array[seq],1,1)::varchar,folder
                from
                (
                    select link_id, link_array,folder,generate_series(1,array_upper(link_array,1)) as seq
                    from
                    (
                        select link_id::bigint, 
                            regexp_split_to_array(turn_rstrs,E'\\\;+') as link_array, folder
                        from org_processed_line
                        where turn_rstrs is not null
                    )temp
                )temp1
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_turn_link')     
        sqlcmd = '''
            INSERT INTO temp_turn_link(reg_id, in_link_id, node_id, out_link_id, pass_link, pass_link_count, s_e)
            (
                select reg_id,in_link_id,(case when s_e = 'F' then c.start_node_id else c.end_node_id end)::bigint as node_id,
                    link_lib[array_upper(link_lib,1)] as out_link_id,(case when array_upper(link_lib,1) = 1 then null else
                        array_to_string(link_lib[1:array_upper(link_lib,1)-1],'|') end) as pass_link, 
                        array_upper(link_lib,1)-1 as pass_link_count,s_e
                from
                (
                    select a.gid as reg_id,b.new_link_id as in_link_id,mid_get_link_array(a.t_link_id,a.folder) as link_lib,a.s_e
                    from temp_turn_rstrs as a
                    left join
                    temp_topo_link as b
                    on a.f_link_id = b.link_id and a.folder = b.folder
                )temp
                left join temp_topo_link as c
                on temp.in_link_id = c.new_link_id
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end prepare for regulation link...')
        return 0
    
    def __make_one_link(self):
        self.log.info('Begin convert regulation for one link...')

        sqlcmd = '''
            insert into condition_regulation_tbl(cond_id, car_type, start_year, start_month, 
               start_day, end_year, end_month, end_day, start_hour, start_minute, end_hour, 
               end_minute, day_of_week, exclude_date)
            (
                select cond_id, car_type,0,0,0,0,0,0,0,0,23,59,0,0
                from
                (
                    select row_number()over(order by car_type) as cond_id, car_type
                    from
                    (
                        select distinct mid_get_acc_mask(acc_mask) as car_type
                        from org_processed_line
                        where acc_mask is not null
                    )temp
                    where car_type > 0
                )temp1
            )
            '''
        #self.pg.execute2(sqlcmd) 
        #self.pg.commit2()
        
        
        self.CreateTable2('temp_acc_mask') 
        sqlcmd = '''
            insert into temp_acc_mask(link_id, one_way, cond_id)
            (
                select b.new_link_id, a.one_way, 0
                from org_processed_line as a
                join temp_topo_link as b
                on a.link_id = b.link_id and a.folder = b.folder
                where acc_mask is not null
            );        
            '''     
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        
        self.CreateFunction2('mid_convert_regulation_from_one_link')
        #self.pg.callproc('mid_convert_regulation_from_one_link')   
        #self.pg.commit2()
        
        self.log.info('end convert regulation for one link.')
        return 0
    
    def __delete_unnecessary_link(self):
        self.log.info('Begin delete unnecessary link...')
        
        self.CreateFunction2('mid_get_node_fromtwolink')
        self.CreateIndex2('temp_turn_link_reg_id')
        sqlcmd = '''
            delete from temp_turn_link
            where reg_id in(
                select reg_id
                from
                (
                    select reg_id,array[in_link_id,out_link_id] || (case when pass_link_count = 0 then null
                        else regexp_split_to_array(pass_link,E'\\\|+')::bigint[] end) as link_array
                    from temp_turn_link 
                )as a
                left join temp_acc_mask as b
                on b.link_id = any(link_array)
                where b.link_id is not null
                
                union
                
                select  reg_id
                from
                (
                    select reg_id,(case when inlink = out_link_id then node_id else mid_get_node_fromtwolink(inlink,out_link_id) end) as nodeid,out_link_id
                        ,temp1.start_node_id,temp1.end_node_id,temp2.one_way
                    from
                    (
                        select reg_id,(case when pass_link is null then in_link_id else (regexp_split_to_array(pass_link,E'\\\|+'))
                            [array_upper(regexp_split_to_array(pass_link,E'\\\|+'),1)]::bigint end) as inlink,out_link_id,node_id
                        from temp_turn_link
                    )temp
                    left join temp_topo_link as temp1
                    on temp.out_link_id = temp1.new_link_id
                    left join org_processed_line as temp2
                    on temp1.folder = temp2.folder and temp1.link_id = temp2.link_id
                )temp3
                where nodeid = end_node_id and one_way = 1
            );
            '''
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        self.log.info('End delete unnecessary link.')
        return 0
        
    def __make_link_array(self):
        self.log.info('Begin convert regulation for link_array')
        
        self.CreateFunction2('mid_convert_regulation_from_link_array')     
        self.pg.callproc('mid_convert_regulation_from_link_array')   
        self.pg.commit2()
        
        self.log.info('End convert regulation for link_array')
        return 0
    
    def __make_country_link(self):
        self.log.info('start convert regulation for country')
        
        self.CreateTable2('temp_node_country')
        self.CreateTable2('temp_link_country')
        
        self.CreateFunction2('mid_get_country_regulation')     
        self.pg.callproc('mid_get_country_regulation')   
        self.pg.commit2()
        
        self.log.info('End convert regulation for country')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
        