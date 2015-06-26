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
    
    # 解析gjv.csv
    def insert_gjv_data(self, jcv_file_list, jv_country, jcv_file_dir):
        jcv_files = jcv_file_list.split(',')
        for jcv_file in jcv_files:
            jcv_path = os.path.join(jcv_file_dir, jcv_file)
            if jcv_path:
                jv_countrys = tuple(jv_country.split(','))
                recordList = self.analyse_csv(jcv_path)
                for record in recordList:
                    #此数据所属国家不在我们所需要的国家列表里，丢弃
                    if str(record['iso_country_code'].replace('"','')) not in jv_countrys:
                        continue
                    # gjv对应的svg名(filename列）为空，丢弃
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
                                    'lat',
                                    'lon',
                                    'mdps',
                                    'mdps_approx',
                                    'tunnel_origin_link',
                                    'tunnel_dest_link',
                                    'origin_bearing',
                                    'dest_bearing',
                                    'dp2_node_id'
                                      ),
                                      (
                                        record['dp_node_id'],
                                        record['originating_link_id'],
                                        record['dest_link_id'],
                                        record['ramp'].replace('"',''),
                                        record['bif'].replace('"',''),
                                        record['ca'].replace('"',''),
                                        record['filename'],
                                        record['side'].replace('"',''),
                                        record['sign_dest'],
                                        record['jv_origin'],
                                        record['iso_country_code'].replace('"',''),
                                        record['gms_svg'].replace('"',''),
                                        record['gms_template'].replace('"',''),
                                        record['lat'],
                                        record['lon'],
                                        record['mdps'].replace('"',''),
                                        record['mdps_approx'].replace('"',''),
                                        record['tunnel_origin_link'].replace('"',''),
                                        record['tunnel_dest_link'].replace('"',''),
                                        record['origin_bearing'],
                                        record['dest_bearing'],
                                        record['dp2_node_id']
                                      )
                                      )
            self.pg.commit2()

    #解析 ejv.csv
    def insert_all_jv_data(self, jcv_file_list, jv_country, jcv_file_dir):
        jcv_files = jcv_file_list.split(',')
        for jcv_file in jcv_files:
            jcv_path = os.path.join(jcv_file_dir, jcv_file)
            if jcv_path:
                jv_countrys = tuple(jv_country.split(','))
                recordList = self.analyse_csv(jcv_path)
                for record in recordList:
                    # 此数据所属国家不在我们所需要的国家列表内，丢弃
                    if str(record['iso_country_code'].replace('"','')) not in jv_countrys:
                        continue
                    # gjv对应的svg名(filename列）和ejv对应的svg名(jcv_filename列)都为空，丢弃。
                    # todo by tangpinghui: 该数据是sar相关，是否应该丢弃?
                    if (not record['filename']) and (not record['jcv_filename']):
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
                                    'lat',
                                    'lon',
                                    'mdps',
                                    'mdps_approx',
                                    'tunnel_origin_link',
                                    'tunnel_dest_link',
                                    'origin_bearing',
                                    'dest_bearing',
                                    'dp2_node_id',
                                    'ejv_filename',
                                    'dp1_dest_link',
                                    'jv_dest_link'
                                      ),
                                      (
                                        record['dp_node_id'],
                                        record['originating_link_id'],
                                        record['dest_link_id'],
                                        record['ramp'].replace('"',''),
                                        record['bif'].replace('"',''),
                                        record['ca'].replace('"',''),
                                        record['filename'],
                                        record['side'].replace('"',''),
                                        record['sign_dest'],
                                        record['jv_origin'],
                                        record['iso_country_code'].replace('"',''),
                                        record['gms_svg'].replace('"',''),
                                        record['gms_template'].replace('"',''),
                                        record['lat'],
                                        record['lon'],
                                        record['mdps'].replace('"',''),
                                        record['mdps_approx'].replace('"',''),
                                        record['tunnel_origin_link'].replace('"',''),
                                        record['tunnel_dest_link'].replace('"',''),
                                        record['origin_bearing'],
                                        record['dest_bearing'],
                                        record['dp2_node_id'],
                                        record['ejv_filename'],
                                        record['dp1_dest_link'],
                                        record['jv_dest_link']
                                      )
                                      )
            self.pg.commit2()

    # 将csv文件的列名作为键，将每行内容作为值。
    # 每行数据将作成一个字典，返回字典的列表
    # srcCsvFile -> 要解析的csv文件路径
    # 出错时返空列表
    def analyse_csv(self, srcCsvFile):
        if os.path.exists(srcCsvFile) == False:
            return []
    
        inFStream = open(srcCsvFile)
        listline = inFStream.readlines()
        inFStream.close()
        
        #解析csv表头，用作字典的键
        csvHeader = listline[0].split(",")
        csvDataList = []
        for line in listline[1:]:
            line = line.strip()
            if line:
                #解析出每行内容
                listrow = line.split(",")
                #以列名为键，对应列的内容为值生成键值对
                record = dict(map(lambda x,y:[x,y], csvHeader,listrow))
                csvDataList.append(record)
        return csvDataList
