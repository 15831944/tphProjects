# -*- coding: UTF8 -*-
'''
Created on 2012-94

@author: liuxinxing
@alter: zhaojie 2014-02-13
'''
import component.component_base

class comp_regulation_ta(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _DoCreateTable(self):
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
    def _Do(self):
        self.__prepare()
        self.__forbidAndpermit_insertto_Link()
        self.__convert_alter_link_dir()
        
        self.__convert_condition_table()
        self.__convert_regulation_oneway_link()
        self.__convert_regulation_linkrow()
        self.__country_condition_regulation()
        self.__delete_from_link()
        
        return 0
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_Jude_Cartype_Array')
        self.CreateFunction2('mid_Jude_Cartype')        
        self.CreateFunction2('mid_get_cartype')
        self.CreateFunction2('mid_insert_condtion_record')
        self.CreateFunction2('mid_update_datetime')
        self.CreateFunction2('mid_update_date')
        self.CreateFunction2('linkdir_str_in')
        self.CreateFunction2('Get_new_dir')
        self.CreateFunction2('mid_delete_item_relation_table')
        self.CreateFunction2('mid_get_seq_numidx')
    
    def __country_condition_regulation(self):
        
        self.log.info('country condition regulation start..')
        
        #country condition
        # 作成表单temp_country_link记录与国家边界相交的link信息。（作成：根据link两端的交叉点种别 4: Country Border Crossing）
        # 作成表单temp_country_node记录国家边界上的node点。（作成；根据交叉点种别4: Country Border Crossing）
        # 跨国规制作成：
        # 1、从temp_country_node取一Node，在表单temp_country_link中根据交通流方向查询对应的进入link（进入指定Node）、脱出link（从指定Ndoe脱出）
        # 2、上述所得进入link、Node、脱出link可以构成一条跨国规制link序列
        
        self.CreateTable2('temp_country_link')
        self.CreateTable2('temp_country_node')
        
        self.CreateFunction2('mid_country_condition')
        self.pg.callproc('mid_country_condition')
        self.pg.commit2()
        
        self.log.info('country condition regulation end..')
        
    def __prepare(self):
        
        self.log.info('Preprocessing regulation begin...')
        
        # create regulation table to be convert
        # one link
        self.CreateIndex2('nw_id_idx')
        self.CreateIndex2('rs_id_idx')
        
        # 作成表单temp_rs_df_todo记录Direction of Traffic Flow规制信息（包括交通流方向、时间/车辆限制）
        # 时间限制不为空  && 车辆限制（-1， 0， 11）
        
        self.CreateTable2('temp_rs_df_todo')
        self.CreateFunction2('mid_org_into_temp_rs_df_todo')
        self.pg.callproc('mid_org_into_temp_rs_df_todo')
        self.pg.commit2()
        
        #mul link
        # 作成表单mid_org_into_temp_rs_linkrow_todo记录feattyp in (2101, 2103)link列的规制信息（包括规制方向、时间/车辆限制）
        # 车辆限制（-1， 0， 11）
        # 2101: Calculated/Derived Prohibited Maneuver / 2103: Prohibited Maneuver
        
        self.CreateTable2('temp_rs_linkrow_todo')
        self.CreateFunction2('mid_org_into_temp_rs_linkrow_todo')
        self.pg.callproc('mid_org_into_temp_rs_linkrow_todo')
        self.pg.commit2()
        
        self.log.info('Preprocessing regulation end.')
        
    def __forbidAndpermit_insertto_Link(self):
        
        self.log.info('Begin to temp_link_regulation_forbid_permit...')
        
        #no time condition
        self.CreateTable2('temp_regulation_forbid')
        self.CreateTable2('temp_regulation_permit')
        self.CreateTable2('temp_link_regulation_forbid_permit')
        
        # 作成表单temp_regulation_forbid记录一方允许通行信息（包括linkid、交通流方向、一方通行规制方向/时间/车辆类型），作成：
        # 收录如下信息：针对非指定车辆（车辆类型vt in (-1, 0, 11)）类型或非24小时（timedom不为空）不允许通行
        
        self.CreateFunction2('mid_org_into_temp_regulation_permit')
        self.pg.callproc('mid_org_into_temp_regulation_permit')
        self.pg.commit2()
        
        # 更新表单temp_link_regulation_forbid_permit---更新一方允许通行（顺行通行/逆行通行）信息，作成：
        # 使用temp_regulation_permit交通流方向、一方通行规制方向判定一方允许通行规制种别（顺行通行/逆行通行）
        
        self.CreateFunction2('mid_permit_into_regulation')
        self.pg.callproc('mid_permit_into_regulation')
        self.pg.commit2()
        
        # 作成表单temp_regulation_forbid记录一方禁止通行信息（包括linkid、交通流方向、一方通行规制方向/时间/车辆类型），作成：
        # 收录如下信息：针对指定车辆（车辆类型vt in (-1, 0, 11)）24小时（timedom为空）不允许通行
        
        self.CreateFunction2('mid_org_into_temp_regulation_forbid')       
        self.pg.callproc('mid_org_into_temp_regulation_forbid')
        self.pg.commit2()
        
        # 更新表单temp_link_regulation_forbid_permit---更新一方通行禁止（双向禁止/顺行禁止/逆行禁止）信息，作成：
        # 使用temp_regulation_forbid交通流方向、一方通行规制方向判定一方禁止通行规制种别（双向禁止/顺行禁止/逆行禁止）
        
        self.CreateFunction2('mid_forbid_into_regulation')
        self.pg.callproc('mid_forbid_into_regulation')
        self.pg.commit2()
        
        self.log.info('end to temp_link_regulation_forbid_permit...')
    
    def __convert_condition_table(self):
        
        self.log.info('Begin convert condition_regulation_tbl...')
        
        # 作成表单temp_condition_regulation_tbl记录道路规制id与道路数据的对照关系
        # 表单temp_condition_regulation_tbl使用方法：道路首先根据对照关系找到对应的道路规制id，再根据id在表单condition_regulation_tbl查询\
        # 道路规制条件情报(规制id为-1代表没有规制；为空代表全时规制；>0代表规制有时间、车辆类型限制)
        # 作成表单condition_regulation_tbl记录道路规制的条件情报（包括时间、车辆类型限制）

        self.CreateFunction2('mid_convert_condition_regulation_tbl')       
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
    
    def __convert_regulation_oneway_link(self):
        
        self.log.info('Begin convert regulation for oneway link...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---单条link禁止通行规制，作成：
        # 从表单temp_rs_df_todo抽取单条link禁止通行规制信息
        
        self.CreateFunction2('mid_convert_regulation_oneway_link')
        self.pg.callproc('mid_convert_regulation_oneway_link')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation for oneway link.')
    
    def __convert_regulation_linkrow(self):
        
        self.log.info('Begin convert regulation for linkrow...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---link列禁止通行规制，作成：
        # 1、从表单temp_rs_linkrow_todo抽取link列禁止通行的规制信息
        # 2、从表单org_mp抽取禁止通行的link序列
                
        self.CreateFunction2('mid_convert_regulation_linkrow')       
        self.pg.callproc('mid_convert_regulation_linkrow')     
        self.pg.commit2()
        
        self.log.info('End convert regulation for linkrow.')
    
    def __convert_alter_link_dir(self):
        
        self.log.info('alter link traffic begin...')
        
        #create temp table for link's traffic flow
        # 作成表单temp_link_regulation_forbid_traffic记录link禁止通行的方向
        # 作成表单temp_link_regulation_permit_traffic记录link允许通行的方向
        # 在制作link一方通行时会考虑上述两表
        self.CreateTable2('temp_link_regulation_forbid_traffic')
        self.CreateTable2('temp_link_regulation_permit_traffic')
        self.pg.commit2()
    
        self.log.info('alter link traffic end.')
        
    def __delete_from_link(self):
        
        self.log.info('delete linkrow regulation from link begin...')
        
        # 更新表单regulation_item_tbl / regulation_relation_tbl---删除脱出link方向不包含交通流方向的情形
        # 例：从一条link的终端脱出，但该link双向禁止或者反向通行，则认为该条记录冗余，需要删除
        # 作成：
        # 对表单regulation_item_tbl中link列的情形进行处理，判断当前link到后一link的脱出方向是否包含交通流方向，若不包含，删除
        # 现在：
        # 暂时仅判断了最终脱出link的方向与交通流方向的关系，未考虑其他情形，依然存在冗余，后续改进
        self.CreateFunction2('mid_delete_linkrow_from_link')
        self.pg.callproc('mid_delete_linkrow_from_link')
        self.pg.commit2()
        
        self.log.info('delete linkrow regulation from link end.')
        
    def _DoCheckLogic(self):        
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        self.pg.commit2()
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        self.pg.commit2()        
        
        self.log.info('Check regulation end.')
        return 0
    