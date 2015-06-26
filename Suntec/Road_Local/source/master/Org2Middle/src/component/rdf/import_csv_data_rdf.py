# -*- coding: UTF-8 -*-
'''
Created on 2013-8-21

@author: zym
'''
import os
import codecs
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
            
    def insert_gjv_data(self, jcv_file_list, jv_country, jcv_file_dir):
        jcv_files = jcv_file_list.split(',')
        for jcv_file in jcv_files:
            jcv_path = os.path.join(jcv_file_dir, jcv_file)
            if jcv_path:
                jv_countrys = tuple(jv_country.split(','))
                objFileAnalyst = CFileAnalyst(jcv_path, jv_countrys)
                listrecord = objFileAnalyst.analyse('gjv')
                for record in listrecord:
                    if str(record[10]) in jv_countrys:
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
                                           record[0],
                                           record[1],
                                           record[2],
                                           record[3],
                                           record[4],
                                           record[5],
                                           record[6],
                                           record[7],
                                           record[8],
                                           record[9],
                                           record[10],
                                           record[11],
                                           record[12],
                                           record[13],
                                           record[14],
                                           record[15],
                                           record[16],
                                           record[17],
                                           record[18],
                                           record[19],
                                           record[20],
                                           record[21]
                                          )
                                          )
            self.pg.commit2()

    def insert_all_jv_data(self, jcv_file_list, jv_country, jcv_file_dir):
        jcv_files = jcv_file_list.split(',')
        for jcv_file in jcv_files:
            jcv_path = os.path.join(jcv_file_dir, jcv_file)
            if jcv_path:
                jv_countrys = tuple(jv_country.split(','))
                objFileAnalyst = CFileAnalyst(jcv_path, jv_countrys)
                listrecord = objFileAnalyst.analyse('all_jv')
                for record in listrecord:
                    if record[10] in jv_countrys:
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
                                           record[0],
                                           record[1],
                                           record[2],
                                           record[3],
                                           record[4],
                                           record[5],
                                           record[6],
                                           record[7],
                                           record[8],
                                           record[9],
                                           record[10],
                                           record[11],
                                           record[12],
                                           record[13],
                                           record[14],
                                           record[15],
                                           record[16],
                                           record[17],
                                           record[18],
                                           record[19],
                                           record[20],
                                           record[21],
                                           record[22],
                                           record[23],
                                           record[24]
                                          )
                                          )
            self.pg.commit2()


class CFileAnalyst:
    def __init__(self, strDataPath, countrys):
        self.strDataPath = strDataPath
        self.countrys = countrys

    def analyse(self, flag):
        listrecord = []
        objFile = None
        if os.path.exists(self.strDataPath):
            objFile = open(self.strDataPath)
        else:
            return listrecord
        listline = objFile.readlines()
        objFile.close()
        for line in listline[1:]:
            line = line.strip()
            if line:
                record = self._analyseLine(line, flag)
                if record:
                    listrecord.append(record)

        return listrecord

    def _analyseLine(self, line, flag):
        record = []
        #
        listrow = line.split(",")
        filename = listrow[6]
        if flag == 'gjv':
            if not filename:
                return None
        if flag == 'all_jv':
            if not filename and (len(listrow) < 24 or not listrow[23]):
                return None
        nodeid2 = None
        if len(listrow) > 21:
            nodeid2 = listrow[21]
        
        # DP_NODE_ID
        record.append(listrow[0])

        # ORIGINATING_LINK_ID
        record.append(listrow[1])

        # DEST_LINK_ID
        record.append(listrow[2])

        # RAMP
        record.append(listrow[3].replace('"', ''))

        # BIF
        record.append(listrow[4].replace('"', ''))

        # CA
        record.append(listrow[5].replace('"', ''))

        # FILENAME
        if not filename:
            record.append(None)
        else:
            if not nodeid2:
                record.append('SDPS/ASPECT RATIO 16x9/GJV_' + filename.replace('"', ''))
            else:
                record.append('MDPS/ASPECT RATIO 16x9/GJV_' + filename.replace('"', ''))
        # SIDE
        record.append(listrow[7].replace('"', ''))

        # SIGN_DEST
        record.append(listrow[8])

        # JV_ORIGIN
        record.append(listrow[9])

        # ISO_COUNTRY_CODE
        record.append(listrow[10].replace('"', ''))

        # GMS_SVG
        record.append(listrow[11].replace('"', ''))

        # GMS_TEMPLATE
        record.append(listrow[12].replace('"', ''))

        # LATITUDE
        record.append(listrow[13])

        # LONGITUDE
        record.append(listrow[14])

        # MDPS
        record.append(listrow[15].replace('"', ''))

        # MDPS_APPROX
        record.append(listrow[16].replace('"', ''))

        # TUNNEL_ORIGIN_LINK
        record.append(listrow[17].replace('"', ''))

        # TUNNEL_DEST_LINK
        record.append(listrow[18].replace('"', ''))

        # ORIGIN_BEARING
        record.append(listrow[19])

        # DEST_BEARING
        record.append(listrow[20])

        # DP2_NODE_ID
        if not nodeid2:
            record.append(None)
        else:
            record.append(nodeid2)

        if flag == 'all_jv':
            if 'USA' in self.countrys or 'CAN' in self.countrys or 'MEX' in self.countrys or 'TWN' in self.countrys or 'PRI' in self.countrys \
                or 'ARE' in self.countrys or 'BHR' in self.countrys or 'EGY' in self.countrys or 'KWT' in self.countrys or 'ISR' in self.countrys\
                or 'MAR' in self.countrys or 'OMN' in self.countrys or 'QAT' in self.countrys or 'SAU' in self.countrys or 'ZAF' in self.countrys\
                or 'BRA' in self.countrys or 'ARG' in self.countrys:
                # ejv_filename
                if len(listrow) < 25 or not listrow[24]:
                    record.append(None)
                else:
                    record.append(listrow[24])
                # dp1_dest_link
                if  len(listrow) < 39 or not listrow[38]:
                    record.append(None)
                else:
                    record.append(listrow[38])
    
                if  len(listrow) < 38 or not listrow[37]:
                    record.append(None)
                else:
                    record.append(listrow[37])
            else:
                # ejv_filename
                if len(listrow) < 24 or not listrow[23]:
                    record.append(None)
                else:
                    record.append(listrow[23])
                # dp1_dest_link
                if  len(listrow) < 38 or not listrow[37]:
                    record.append(None)
                else:
                    record.append(listrow[37])

                if  len(listrow) < 37 or not listrow[36]:
                    record.append(None)
                else:
                    record.append(listrow[36])

        return record
