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
        # ���ɱ�temp_country_link��¼����ұ߽��ཻ��link��Ϣ�������ɣ�����link���˵Ľ�����ֱ� 4: Country Border Crossing��
        # ���ɱ�temp_country_node��¼���ұ߽��ϵ�node�㡣�����ɣ����ݽ�����ֱ�4: Country Border Crossing��
        # ����������ɣ�
        # 1����temp_country_nodeȡһNode���ڱ�temp_country_link�и��ݽ�ͨ�������ѯ��Ӧ�Ľ���link������ָ��Node�����ѳ�link����ָ��Ndoe�ѳ���
        # 2���������ý���link��Node���ѳ�link���Թ���һ���������link����
        
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
        
        # ���ɱ�temp_rs_df_todo��¼Direction of Traffic Flow������Ϣ��������ͨ������ʱ��/�������ƣ�
        # ʱ�����Ʋ�Ϊ��  && �������ƣ�-1�� 0�� 11��
        
        self.CreateTable2('temp_rs_df_todo')
        self.CreateFunction2('mid_org_into_temp_rs_df_todo')
        self.pg.callproc('mid_org_into_temp_rs_df_todo')
        self.pg.commit2()
        
        #mul link
        # ���ɱ�mid_org_into_temp_rs_linkrow_todo��¼feattyp in (2101, 2103)link�еĹ�����Ϣ���������Ʒ���ʱ��/�������ƣ�
        # �������ƣ�-1�� 0�� 11��
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
        
        # ���ɱ�temp_regulation_forbid��¼һ������ͨ����Ϣ������linkid����ͨ������һ��ͨ�й��Ʒ���/ʱ��/�������ͣ������ɣ�
        # ��¼������Ϣ����Է�ָ����������������vt in (-1, 0, 11)�����ͻ��24Сʱ��timedom��Ϊ�գ�������ͨ��
        
        self.CreateFunction2('mid_org_into_temp_regulation_permit')
        self.pg.callproc('mid_org_into_temp_regulation_permit')
        self.pg.commit2()
        
        # ���±�temp_link_regulation_forbid_permit---����һ������ͨ�У�˳��ͨ��/����ͨ�У���Ϣ�����ɣ�
        # ʹ��temp_regulation_permit��ͨ������һ��ͨ�й��Ʒ����ж�һ������ͨ�й����ֱ�˳��ͨ��/����ͨ�У�
        
        self.CreateFunction2('mid_permit_into_regulation')
        self.pg.callproc('mid_permit_into_regulation')
        self.pg.commit2()
        
        # ���ɱ�temp_regulation_forbid��¼һ����ֹͨ����Ϣ������linkid����ͨ������һ��ͨ�й��Ʒ���/ʱ��/�������ͣ������ɣ�
        # ��¼������Ϣ�����ָ����������������vt in (-1, 0, 11)��24Сʱ��timedomΪ�գ�������ͨ��
        
        self.CreateFunction2('mid_org_into_temp_regulation_forbid')       
        self.pg.callproc('mid_org_into_temp_regulation_forbid')
        self.pg.commit2()
        
        # ���±�temp_link_regulation_forbid_permit---����һ��ͨ�н�ֹ��˫���ֹ/˳�н�ֹ/���н�ֹ����Ϣ�����ɣ�
        # ʹ��temp_regulation_forbid��ͨ������һ��ͨ�й��Ʒ����ж�һ����ֹͨ�й����ֱ�˫���ֹ/˳�н�ֹ/���н�ֹ��
        
        self.CreateFunction2('mid_forbid_into_regulation')
        self.pg.callproc('mid_forbid_into_regulation')
        self.pg.commit2()
        
        self.log.info('end to temp_link_regulation_forbid_permit...')
    
    def __convert_condition_table(self):
        
        self.log.info('Begin convert condition_regulation_tbl...')
        
        # ���ɱ�temp_condition_regulation_tbl��¼��·����id���·���ݵĶ��չ�ϵ
        # ��temp_condition_regulation_tblʹ�÷�������·���ȸ��ݶ��չ�ϵ�ҵ���Ӧ�ĵ�·����id���ٸ���id�ڱ�condition_regulation_tbl��ѯ\
        # ��·���������鱨(����idΪ-1����û�й��ƣ�Ϊ�մ���ȫʱ���ƣ�>0���������ʱ�䡢������������)
        # ���ɱ�condition_regulation_tbl��¼��·���Ƶ������鱨������ʱ�䡢�����������ƣ�

        self.CreateFunction2('mid_convert_condition_regulation_tbl')       
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
    
    def __convert_regulation_oneway_link(self):
        
        self.log.info('Begin convert regulation for oneway link...')
        
        # ���±�regulation_relation_tbl/regulation_item_tbl---����link��ֹͨ�й��ƣ����ɣ�
        # �ӱ�temp_rs_df_todo��ȡ����link��ֹͨ�й�����Ϣ
        
        self.CreateFunction2('mid_convert_regulation_oneway_link')
        self.pg.callproc('mid_convert_regulation_oneway_link')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation for oneway link.')
    
    def __convert_regulation_linkrow(self):
        
        self.log.info('Begin convert regulation for linkrow...')
        
        # ���±�regulation_relation_tbl/regulation_item_tbl---link�н�ֹͨ�й��ƣ����ɣ�
        # 1���ӱ�temp_rs_linkrow_todo��ȡlink�н�ֹͨ�еĹ�����Ϣ
        # 2���ӱ�org_mp��ȡ��ֹͨ�е�link����
                
        self.CreateFunction2('mid_convert_regulation_linkrow')       
        self.pg.callproc('mid_convert_regulation_linkrow')     
        self.pg.commit2()
        
        self.log.info('End convert regulation for linkrow.')
    
    def __convert_alter_link_dir(self):
        
        self.log.info('alter link traffic begin...')
        
        #create temp table for link's traffic flow
        # ���ɱ�temp_link_regulation_forbid_traffic��¼link��ֹͨ�еķ���
        # ���ɱ�temp_link_regulation_permit_traffic��¼link����ͨ�еķ���
        # ������linkһ��ͨ��ʱ�ῼ����������
        self.CreateTable2('temp_link_regulation_forbid_traffic')
        self.CreateTable2('temp_link_regulation_permit_traffic')
        self.pg.commit2()
    
        self.log.info('alter link traffic end.')
        
    def __delete_from_link(self):
        
        self.log.info('delete linkrow regulation from link begin...')
        
        # ���±�regulation_item_tbl / regulation_relation_tbl---ɾ���ѳ�link���򲻰�����ͨ�����������
        # ������һ��link���ն��ѳ�������link˫���ֹ���߷���ͨ�У�����Ϊ������¼���࣬��Ҫɾ��
        # ���ɣ�
        # �Ա�regulation_item_tbl��link�е����ν��д����жϵ�ǰlink����һlink���ѳ������Ƿ������ͨ����������������ɾ��
        # ���ڣ�
        # ��ʱ���ж��������ѳ�link�ķ����뽻ͨ������Ĺ�ϵ��δ�����������Σ���Ȼ�������࣬�����Ľ�
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
    