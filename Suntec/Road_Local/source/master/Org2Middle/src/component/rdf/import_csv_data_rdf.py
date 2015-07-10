# -*- coding: UTF-8 -*-
'''
Created on 2013-8-21

@author: zym
'''
import os
import common.common_func
import component.component_base
import io

gjv_file_list = 'gjv.csv'
all_jv_file_list = 'all_jv.csv'


class import_junctionview_data_rdf(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'ImportJCV')

    def _DoCreateTable(self):
        return

    def _Do(self):
#         jcv_file_dir = common.common_func.GetPath('junctionview_dir')
        jv_country = common.common_func.GetPath('jv_country')
#         gjv_file_list = common.common_func.GetPath('gjv_files')
#         all_jv_file_list = common.common_func.GetPath('all_jv_files')
        junctionview_data = common.common_func.GetPath('junctionview_data')
#         jcv_file_dir = junctionview_data[:junctionview_data.find('Q114_APAC_2D_') - 1]
#         print jcv_file_dir, jv_country
        if gjv_file_list:
            self.CreateTable2('rdfpl_gjv_lat_display_org')
            self.insert_gjv_data(gjv_file_list, jv_country, junctionview_data)

        if all_jv_file_list:
            self.CreateTable2('rdfpl_all_jv_lat_display_org')
            self.insert_all_jv_data(all_jv_file_list, jv_country, junctionview_data)
            
        all_sign_as_real_file_name = common.common_func.GetPath('all_sign_as_real_file_name')
        self.CreateTable2('mid_all_sar_files')
        if all_sign_as_real_file_name <> '':
            self.insert_all_sar_file_names(all_sign_as_real_file_name)
    
        return
    
    def insert_all_sar_file_names(self, file_name):
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        f_sar = io.open(file_name, 'r', 8192,'utf-8')
        if f_sar is None:
            return -1
        
        pgcur.copy_from(f_sar, 'mid_all_sar_files', '|', "", 8192)
        pg.commit2()
        f_sar.close()
        
        return 0
    
    # ����gjv.csv
    def insert_gjv_data(self, gjv_file_list, jv_country, gjv_file_dir):
        jcv_files = gjv_file_list.split(',')
        for jcv_file in jcv_files:
            jcv_path = os.path.join(gjv_file_dir, jcv_file)
            if os.path.isfile(jcv_path):
                jv_countrys = tuple(jv_country.split(','))
                recordList = self.analyse_csv(jcv_path)
                for record in recordList:
                    #�������������Ҳ�����������Ҫ�Ĺ����б������
                    if str(record['iso_country_code'].replace('"','')) not in jv_countrys:
                        continue
                    # gjv��Ӧ��svg��(filename�У�Ϊ�գ�����
                    if (not record['filename']):
                        continue
                    
                    self.pg.insert('rdfpl_gjv_lat_display_org',
                                    (
                                    'dp_node_id',
                                    'originating_link_id',
                                    'dest_link_id',
                                    'ramp',
                                    'bif',
                                    'ca',
                                    'filename',
                                    'side',
                                    'sign_dest',
                                    'jv_origin',
                                    'iso_country_code',
                                    'gms_svg',
                                    'gms_template',
                                    'latitude',
                                    'longitude',
                                    'mdps',
                                    'mdps_approx',
                                    'tunnel_origin_link',
                                    'tunnel_dest_link',
                                    'origin_bearing',
                                    'dest_bearing',
                                    'dp2_node_id'
                                      ),
                                      (
                                        record['dp_node_id'] if record['dp_node_id'] else None,
                                        record['originating_link_id'] if record['originating_link_id'] else None,
                                        record['dest_link_id'] if record['dest_link_id'] else None,
                                        record['ramp'].replace('"',''),
                                        record['bif'].replace('"',''),
                                        record['ca'].replace('"',''),
                                        record['filename'],
                                        record['side'].replace('"',''),
                                        record['sign_dest'] if record['sign_dest'] else None,
                                        record['jv_origin'] if record['jv_origin'] else None,
                                        record['iso_country_code'].replace('"',''),
                                        record['gms_svg'].replace('"',''),
                                        record['gms_template'].replace('"',''),
                                        record['latitude'],
                                        record['longitude'],
                                        record['mdps'].replace('"',''),
                                        record['mdps_approx'].replace('"',''),
                                        record['tunnel_origin_link'].replace('"',''),
                                        record['tunnel_dest_link'].replace('"',''),
                                        record['origin_bearing'],
                                        record['dest_bearing'],
                                        record['dp2_node_id'] if record['dp2_node_id'] else None
                                      )
                                    )
            self.pg.commit2()

    #���� ejv.csv
    def insert_all_jv_data(self, jcv_file_list, jv_country, jcv_file_dir):
        jcv_files = jcv_file_list.split(',')
        for jcv_file in jcv_files:
            jcv_path = os.path.join(jcv_file_dir, jcv_file)
            if os.path.isfile(jcv_path):
                jv_countrys = tuple(jv_country.split(','))
                recordList = self.analyse_csv(jcv_path)
                for record in recordList:
                    # �������������Ҳ�����������Ҫ�Ĺ����б��ڣ�����
                    if str(record['iso_country_code'].replace('"','')) not in jv_countrys:
                        continue
                    # gjv��Ӧ��svg��(filename�У���ejv��Ӧ��svg��(jcv_filename��)��Ϊ�գ�������
                    # todo by tangpinghui: ��������sar��أ��Ƿ�Ӧ�ö���?
                    if (not record['filename']) and (not record['ejv_filename']):
                        continue
                    self.pg.insert('rdfpl_all_jv_lat_display_org',
                                    (
                                    'dp_node_id',
                                    'originating_link_id',
                                    'dest_link_id',
                                    'ramp',
                                    'bif',
                                    'ca',
                                    'filename',
                                    'side',
                                    'sign_dest',
                                    'jv_origin',
                                    'iso_country_code',
                                    'gms_svg',
                                    'gms_template',
                                    'mdps',
                                    'mdps_approx',
                                    'tunnel_origin_link',
                                    'tunnel_dest_link',
                                    'origin_bearing',
                                    'dest_bearing',
                                    'dp2_node_id',
                                    'latitude',
                                    'longitude',
                                    'ejv_filename',
                                    'dp1_dest_link',
                                    'jv_dest_link'
                                      ),
                                      (
                                        record['dp_node_id'] if record['dp_node_id'] else None,
                                        record['originating_link_id'] if record['originating_link_id'] else None,
                                        record['dest_link_id'] if record['dest_link_id'] else None,
                                        record['ramp'].replace('"',''),
                                        record['bif'].replace('"',''),
                                        record['ca'].replace('"',''),
                                        record['filename'],
                                        record['side'].replace('"',''),
                                        record['sign_dest'] if record['sign_dest'] else None,
                                        record['jv_origin'] if record['jv_origin'] else None,
                                        record['iso_country_code'].replace('"',''),
                                        record['gms_svg'].replace('"',''),
                                        record['gms_template'].replace('"',''),
                                        record['mdps'].replace('"',''),
                                        record['mdps_approx'].replace('"',''),
                                        record['tunnel_origin_link'].replace('"',''),
                                        record['tunnel_dest_link'].replace('"',''),
                                        record['origin_bearing'],
                                        record['dest_bearing'],
                                        record['dp2_node_id'] if record['dp2_node_id'] else None,
                                        record['latitude'],
                                        record['longitude'],
                                        record['ejv_filename'],
                                        record['dp1_dest_link'] if record['dp1_dest_link'] else None,
                                        record['jv_dest_link'] if record['jv_dest_link'] else None
                                      )
                                      )
            self.pg.commit2()

    # ��csv�ļ���������Ϊ������ÿ��������Ϊֵ��
    # ÿ�����ݽ�����һ���ֵ䣬�����ֵ���б�
    # srcCsvFile -> Ҫ������csv�ļ�·��
    # ����ʱ�����б�
    def analyse_csv(self, srcCsvFile):
        if os.path.exists(srcCsvFile) == False:
            return []
    
        inFStream = open(srcCsvFile)
        listline = inFStream.readlines()
        inFStream.close()
        
        #����csv��ͷ�������ֵ�ļ�
        csvHeader = listline[0].lower().split(",")
        csvDataList = []
        for line in listline[1:]:
            line = line.strip()
            if line:
                #������ÿ������
                listrow = line.split(",")
                #������Ϊ������Ӧ�е�����Ϊֵ���ɼ�ֵ��
                record = dict(map(lambda x,y:[x,y], csvHeader,listrow))
                csvDataList.append(record)
        return csvDataList
