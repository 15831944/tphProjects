# -*- coding: UTF-8 -*-
'''
Created on 2013-8-21

@author: zym
'''
import os
import common.common_func
import component.component_base

class import_junctionview_data_rdf(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'ImportJCV')

    def _DoCreateTable(self):
        return

    def _Do(self):
        strGjvFileList = common.common_func.GetPath('gjvCsvPath')
        strEjvFileList = common.common_func.GetPath('ejvCsvPath')
        strInterestCountryList = common.common_func.GetPath('strInterestCountryList')

        self.CreateTable2('rdfpl_gjv_lat_display_org')
        self.insert_gjv_data(strGjvFileList, strInterestCountryList)
        self.CreateTable2('rdfpl_all_jv_lat_display_org')
        self.insert_ejv_data(strEjvFileList, strInterestCountryList)
        return
    
    # 解析gjv.csv
    def insert_gjv_data(self, strGjvFileList, strInterestCountryList):
        gjvFileList = strGjvFileList.split(',')
        for gjvFile in gjvFileList:
            if os.path.isfile(gjvFile):
                insterestCountryList = tuple(strInterestCountryList.split(','))
                recordList = self.analyse_csv(gjvFile)
                for record in recordList:
                    #此数据所属国家不在我们所需要的国家列表里，丢弃
                    if str(record['iso_country_code'].replace('"','')) not in insterestCountryList:
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
                                        None if not record['filename'] else
                                        'MDPS/ASPECT RATIO 16x9/GJV_' + record['filename'].replace('"','') if record['dp2_node_id'] 
                                        else 'SDPS/ASPECT RATIO 16x9/GJV_' + record['filename'].replace('"',''),
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

    # 解析 ejv.csv
    def insert_ejv_data(self, strEjvFileList, strInterestCountryList):
        ejvFileList = strEjvFileList.split(',')
        for ejvFile in ejvFileList:
            if os.path.isfile(ejvFile):
                interestCountryList = tuple(strInterestCountryList.split(','))
                recordList = self.analyse_csv(ejvFile)
                for record in recordList:
                    # 此数据所属国家不在我们所需要的国家列表内，丢弃
                    if str(record['iso_country_code'].replace('"','')) not in interestCountryList:
                        continue
                    # gjv对应的svg名(filename列）和ejv对应的svg名(jcv_filename列)都为空，丢弃。
                    # todo by tangpinghui: 该数据是sar相关，是否应该丢弃?
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
                                    'sar_filename',
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
                                        None if not record['filename'] else
                                        'MDPS/ASPECT RATIO 16x9/GJV_' + record['filename'].replace('"','') if record['dp2_node_id'] 
                                        else 'SDPS/ASPECT RATIO 16x9/GJV_' + record['filename'].replace('"',''),
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
                                        record['ejv_filename'] if record['ejv_filename'] else None,
                                        record['sar_filename'] if record['sar_filename'] else None,
                                        record['dp1_dest_link'] if record['dp1_dest_link'] else None,
                                        record['jv_dest_link'] if record['jv_dest_link'] else None
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
        csvHeader = listline[0].lower().split(",")
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
