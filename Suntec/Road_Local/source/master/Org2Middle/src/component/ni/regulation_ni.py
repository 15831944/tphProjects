# -*- coding: UTF-8 -*-
'''
Created on 2015-5-7

@author: liuxinxing
'''
import component.component_base

class comp_regulation_ni(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _Do(self):
        self.__convert_condition_table()
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        self.__convert_regulation_access()
        self.__convert_regulation_oneway()
        self.__convert_regulation_linkrow()
        self.__convert_regulation_country_boundry()
        return 0
    
    def __convert_condition_table(self):
        self.log.info('convert condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        
        self.CreateFunction2('mid_get_access')
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        self.pg.commit2()
        self.CreateIndex2('temp_condition_regulation_tbl_vperiod_vehcl_type_idx')
        
        self.log.info('convert condition_regulation_tbl end.')
    
    def __convert_regulation_access(self):
        self.log.info('convert regulation of access...')
        
        self.CreateIndex2('org_r_accesscrid_idx')
        self.CreateIndex2('org_cr_crid_idx')
        self.CreateIndex2('org_cr_vperiod_vehcl_type_idx')
        self.CreateFunction2('mid_convert_regulation_access')
        self.pg.callproc('mid_convert_regulation_access')
        self.pg.commit2()
        
        self.log.info('convert regulation of access end.')
    
    def __convert_regulation_oneway(self):
        self.log.info('convert regulation of oneway...')
        
        self.CreateIndex2('org_r_onewaycrid_idx')
        self.CreateIndex2('org_cr_crid_idx')
        self.CreateIndex2('org_cr_vperiod_vehcl_type_idx')
        self.CreateFunction2('mid_convert_regulation_oneway')
        self.pg.callproc('mid_convert_regulation_oneway')
        self.pg.commit2()
        
        self.log.info('convert regulation of oneway end.')
    
    def __convert_regulation_linkrow(self):
        self.log.info('convert regulation of linkrow...')
        
        self.CreateIndex2('org_cond_crid_idx')
        self.CreateIndex2('org_cr_crid_idx')
        self.CreateIndex2('org_cr_vperiod_vehcl_type_idx')
        self.CreateFunction2('mid_convert_regulation_linkrow')
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('convert regulation of linkrow end.')
    
    def __convert_regulation_country_boundry(self):
        self.log.info('convert regulation of country boundry...')
        
        # 创建表单temp_node_nation_boundary记录国家/地区边界上的node信息
        # 四维中国仕向地港澳之间不需要作成跨国规制
        
        sqlcmd = """
                drop table if exists temp_node_nation_boundary;
                CREATE TABLE temp_node_nation_boundary 
                as (
                    select node_id
                    from (
                        select node_id, array_agg(iso_country_code) as iso_country_code_array
                        from (
                            select distinct a.node_id, b.iso_country_code
                            from node_tbl a
                            left join link_tbl b
                                on a.node_id in (b.s_node, b.e_node)
                        ) c
                        group by node_id
                    ) d
                    where array_upper(iso_country_code_array, 1) > 1 and 
                        'CHN' = any(iso_country_code_array)
                );
                
                DROP INDEX IF EXISTS temp_node_nation_boundary_node_id_idx;
                CREATE INDEX temp_node_nation_boundary_node_id_idx
                  ON temp_node_nation_boundary
                  USING btree
                  (node_id);
                  
                analyze temp_node_nation_boundary;
            """
               
        self.pg.do_big_insert2(sqlcmd)
        
        # 以国家/地区边界上的node作为规制点，进入node的link作为规制进入link，从node脱出的link作为脱出link
        # 跨国规制对link种别无限定
        
        self.CreateFunction2('mid_convert_regulation_nation_boundary')
        self.pg.callproc('mid_convert_regulation_nation_boundary')
        self.pg.commit2()
        
        self.log.info('convert regulation of country boundry end.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
    