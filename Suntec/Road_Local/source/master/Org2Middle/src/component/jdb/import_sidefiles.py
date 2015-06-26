# -*- coding: UTF-8 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''

import io
import os
import codecs
import component.component_base
import common.common_func

class comp_import_sidefiles_jdb(component.component_base.comp_base):

    def __init__(self):
        component.component_base.comp_base.__init__(self, 'sidefiles')

    def _DoCreateTable(self):
#         if self.CreateTable2('road_highway') == -1:
#             return -1
#         if self.CreateTable2('road_hwysame') == -1:
#             return -1
#         if self.CreateTable2('road_tollguide') == -1:
#             return -1
#         if self.CreateTable2('road_ferry_code') == -1:
#             return -1
#         if self.CreateTable2('road_gs') == -1:
#             return -1
#         if self.CreateTable2('road_multiple_store') == -1:
#             return -1
#         if self.CreateTable2('road_road_code') == -1:
#             return -1
#         if self.CreateTable2('road_road_code_highway') == -1:
#             return -1
#         if self.CreateTable2('road_roadpoint') == -1:
#             return -1
#         if self.CreateTable2('road_roadpoint_normal') == -1:
#             return -1
#         if self.CreateTable2('road_roadpoint_bunnki') == -1:
#             return -1
#         if self.CreateTable2('road_store_code') == -1:
#             return -1
        if self.CreateTable2('road_sapa_illust') == -1:
            return -1
        if self.CreateTable2('road_guidance_caution_tbl') == -1:
            return -1
        if self.CreateTables('road_roundabout_link_tbl') == -1:
            return -1
        
        return 0

    def _DoCreateIndex(self):
        return 0

    def _DoCreateFunction(self):
#         if self.CreateFunction2('deal_type_of_sidefile') == -1:
#             return -1
#
#         if self.CreateFunction2('create_geom_of_sidefile') == -1:
#             return -1
        return 0

    def _Do(self):
#         sqlcmd = """
#                 UPDATE spatial_ref_sys
#                   SET proj4text='+proj=longlat +ellps=bessel towgs84=-146.43,507.89,681.46'
#                   WHERE srid=4301;
#                 """
#         self.pg.execute2(sqlcmd)
#         self.pg.commit2()
#         kensaku_path = common.common_func.GetPath('sf_highway')
#         highway_path = common.common_func.GetPath('highway')
        sapa_illust_path = common.common_func.GetPath('illust')

#         self._Do_highway(highway_path)
#         self._Do_hwysame(highway_path)
#         self._Do_tollguide(highway_path)
#
#         self._Do_ferry_code(kensaku_path)
#         self._Do_gs(kensaku_path)
#         self._Do_multiple_store(kensaku_path)
#         self._Do_road_code(kensaku_path)
#         self._Do_road_code_highway(kensaku_path)
#         self._Do_roadpoint(kensaku_path)
#         self._Do_roadpoint_normal(kensaku_path)
#         self._Do_roadpoint_bunnki(kensaku_path)
#         self._Do_store_code(kensaku_path)
        self._Do_sapa_illust(sapa_illust_path)
        self._Do_store_code_mapping()
#         self._Do_toll_same_facility()
        self._Do_guidance_caution()
#        roundabout_links_path = common.common_func.GetPath('roundabout_links_txt')
#        self._Do_roundabout_links(roundabout_links_path)

    def _Do_highway(self, sf_path):
        f_highway = os.path.join(sf_path, 'highway.txt')
        objFileAnalyst = CFileAnalyst_highway(f_highway)
        listrecord = objFileAnalyst.analyse()

        for record in listrecord:
            self.pg.insert('road_highway',
                          (
                           'road_code',
                           'linkcls_c',
                           'dir_f',
                           'dir_point',
                           'road_point',
                           'next_point',
                           'next_dis',
                           'end_f',
                           'forward_num',
                           'backward_num',
                           'branches',
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
                           record[10]
                          )
                          )
        self.pg.commit2()
        return 0

    def _Do_hwysame(self, sf_path):
        f_hwysame = os.path.join(sf_path, 'hwysame.txt')
        objFileAnalyst = CFileAnalyst_hwysame(f_hwysame)
        listrecord = objFileAnalyst.analyse()

        for record in listrecord:
            self.pg.insert('road_hwysame',
                          (
                           'road_code',
                           'road_point',
                           'dir_f',
                           'name',
                           'roads',
                          ),
                          (
                           record[0],
                           record[1],
                           record[2],
                           record[3],
                           record[4]
                          )
                          )
        self.pg.commit2()
        return 0

    def _Do_tollguide(self, sf_path):
        f_tollguide = os.path.join(sf_path, 'tollguide.txt')
        objFileAnalyst = CFileAnalyst_tollguide(f_tollguide)
        listrecord = objFileAnalyst.analyse()
        for record in listrecord:
            self.pg.insert('road_tollguide',
                          (
                           'mesh_code',
                           'node_id',
                           'toll_type',
                           'toll_num',
                           'roads',
                          ),
                          (
                           record[0],
                           record[1],
                           record[2],
                           record[3],
                           record[4]
                          )
                          )
        self.pg.commit2()
        return 0

    def _Do_ferry_code(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_ferry_code = io.open(os.path.join(sf_path, 'ferry.code.doc'),
                               'r', 8192, 'euc-jp')
        pgcur.copy_from(f_ferry_code, 'road_ferry_code', '\t', "", 8192,
                        ('type', 'ferry_id', 'ferry_name'))
        pg.commit2()
        f_ferry_code.close()
        sqlcmd = """
              select deal_type_of_sidefile('road_ferry_code');
              """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _Do_gs(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_gs = io.open(os.path.join(sf_path, 'gs.txt'), 'r', 8192, 'euc-jp')
        pgcur.copy_from(f_gs, 'road_gs', '\t', "", 8192,
                        ('road_code', 'road_point', 'side_f',
                         'chain_id', 'open_time', 'sunday',
                         'saturday', 'friday', 'thursday',
                         'wednesday', 'tuesday', 'monday',
                         'obon_day', 'year_end', 'year_start',
                         'holiday', 'shukusai'))
        pg.commit2()
        f_gs.close()
        return 0

    def _Do_multiple_store(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_multiple_store = io.open(os.path.join(sf_path, 'multiple_store.txt'),
                                   'r', 8192, 'euc-jp')
        pgcur.copy_from(f_multiple_store, 'road_multiple_store',
                        '\t', "", 8192,
                        ('road_code', 'road_point', 'side_f',
                         'chain_id', 'open_time', 'sunday',
                         'saturday', 'friday', 'thursday',
                         'wednesday', 'tuesday', 'monday',
                         'obon_day', 'year_end', 'year_start',
                         'holiday', 'shukusai'))
        pg.commit2()
        f_multiple_store.close()
        return 0

    def _Do_road_code(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_road_code = io.open(os.path.join(sf_path, 'road.code.doc'),
                              'r', 8192, 'euc-jp')
        pgcur.copy_from(f_road_code, 'road_road_code', '\t', "", 8192,
                        ('type', 'road_code', 'road_name', 'road_yomi'))
        pg.commit2()
        f_road_code.close()
        sqlcmd = """
              select deal_type_of_sidefile('road_road_code');
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _Do_road_code_highway(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_road_code_highway = io.open(os.path.join(sf_path,
                                                   'road.code.doc.highway'),
                                      'r', 8192, 'euc-jp')
        pgcur.copy_from(f_road_code_highway, 'road_road_code_highway',
                        '\t', "", 8192,
                        ('type', 'road_code', 'road_name', 'road_yomi'))
        pg.commit2()
        f_road_code_highway.close()
        sqlcmd = """
              select deal_type_of_sidefile('road_road_code_highway');
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _Do_roadpoint(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_roadpoint = io.open(os.path.join(sf_path, 'roadpoint.txt'),
                              'r', 8192, 'euc-jp')
        pgcur.copy_from(f_roadpoint, 'road_roadpoint', '\t', "", 8192,
                        ('point_mesh_code', 'point_x', 'point_y',
                         'mesh_id', 'fac_mesh_code', 'fac_x',
                         'fac_y', 'addr_id', 'road_point',
                         'fac_name', 'fac_yomi', 'road_code',
                         'fac_type', 'point_pos', 'point_name',
                         'point_yomi', 'sa_pa', 'name_f',
                         'fac_f'))
        pg.commit2()
        f_roadpoint.close()
        sqlcmd = """
        select AddGeometryColumn('road_roadpoint', 'geom', 4326, 'POINT', 2);
        update road_roadpoint
          set geom = create_geom_of_sidefile(fac_mesh_code, fac_x, fac_y);
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _Do_roadpoint_normal(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_roadpoint_normal = io.open(os.path.join(sf_path,
                                                  'roadpoint_normal.txt'),
                                     'r', 8192, 'euc-jp')
        pgcur.copy_from(f_roadpoint_normal, 'road_roadpoint_normal',
                        '\t', "", 8192,
                        ('point_mesh_code', 'point_x', 'point_y',
                         'mesh_id', 'fac_mesh_code', 'fac_x',
                         'fac_y', 'addr_id', 'road_point',
                         'fac_name', 'fac_yomi', 'road_code',
                         'fac_type', 'point_pos', 'point_name',
                         'point_yomi', 'sa_pa', 'name_f',
                         'fac_f'))
        pg.commit2()
        f_roadpoint_normal.close()
        sqlcmd = """
        select AddGeometryColumn('road_roadpoint_normal',
                                 'geom', 4326, 'POINT', 2);
        update road_roadpoint_normal
           set geom = create_geom_of_sidefile(fac_mesh_code, fac_x, fac_y);
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _Do_roadpoint_bunnki(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_roadpoint_bunnki = io.open(os.path.join(sf_path,
                                                  'roadpoint.bunnki.txt'),
                                     'r', 8192, 'euc-jp')
        pgcur.copy_from(f_roadpoint_bunnki, 'road_roadpoint_bunnki',
                        '\t', "", 8192,
                        ('point_mesh_code', 'point_x', 'point_y',
                         'mesh_id', 'fac_mesh_code', 'fac_x',
                         'fac_y', 'addr_id', 'road_point',
                         'fac_name', 'fac_yomi', 'road_code',
                         'fac_type', 'point_pos', 'point_name',
                         'point_yomi', 'sa_pa', 'name_f',
                         'fac_f')
                        )
        pg.commit2()
        f_roadpoint_bunnki.close()
        sqlcmd = """
        select AddGeometryColumn('road_roadpoint_bunnki',
                                 'geom', 4326, 'POINT', 2);
        update road_roadpoint_bunnki
          set geom = create_geom_of_sidefile(fac_mesh_code, fac_x, fac_y);
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _Do_store_code(self, sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_store_code = io.open(os.path.join(sf_path,
                                            'store_code.txt'),
                               'r', 8192, 'euc-jp')
        pgcur.copy_from(f_store_code, 'road_store_code', '\t', "",
                        8192, ('chain_id', 'chain_name'))
        pg.commit2()
        f_store_code.close()
        return 0

    def _Do_sapa_illust(self, sf_path):
        'import SAPA Illust information.'
        self.log.info('Import SAPA Illust information')
        filepath = ''
        strNodeList = os.path.join(sf_path, 'nodelist')
        listfilename = os.listdir(strNodeList)
        for filename in listfilename:
            if filename.find("sapa.txt") != -1:
                filepath = os.path.join(strNodeList, filename)
        if filepath == '':
            self.log.warning('Does not exist SAPA Illust file.')
            return 0
        objFileAnalyst = CFileAnalyst_sapa_illust(filepath)
        listrecord = objFileAnalyst.analyse()
        tempstr = '''insert into road_sapa_illust(
                                illust_name, road_name,
                                fac_name, updown_flag,
                                mesh_id, node_id
                                )
                      values ( '''
        length = len(listrecord[len(listrecord) - 1])
        while length > 0:
            tempstr = tempstr + '%s, '
            length = length - 1
        tempstr = tempstr.rstrip(', ') + ' );'
        #print tempstr
        for record in listrecord:
            values = []
            for temp in record[0:]:
                if (temp != ""):
                    values.append(temp)
                else:
                    values.append(None)
            self.pg.execute2(tempstr, tuple(values))
        self.pg.commit2()
        #print 'insert IC into database finished'
        return 0

    def _Do_store_code_mapping(self):
        "import DDN store code mapping table."
        self.log.info('Import Store Code.')
        self.CreateTable2('store_code_mapping')
        path = common.common_func.GetPath('store_code_mapping')
        if path == None or path == '':
            self.log.warning('Does not exist store code mapping file.')
            return 0
        code_list = common.common_func.GetSheetRegion(path, 'B', 'H', 4, 62)
        sqlcmd = """
             INSERT INTO store_code_mapping(
                                org_store_code
                              , org_store_name
                              , ddn_store_code
                              , ddn_store_code_hex
                              , ddn_store_name
                              , picture_name)
                    VALUES (%s, %s, %s, %s, %s, %s);
             """
        for record in code_list:
            #print record
            org_store_code = record[0]
            org_store_name = record[1]
            ddn_store_code_hex = record[4]
            ddn_store_name = record[5]
            try:
                picture_name = str(int(record[6]))
            except:
                picture_name = record[6]
                if not picture_name:
                    picture_name = None
            if ddn_store_code_hex != '':
                ddn_store_code = int(ddn_store_code_hex, 16)
            else:
                ddn_store_code = None
            self.pg.execute2(sqlcmd, (org_store_code, org_store_name,
                                      ddn_store_code, ddn_store_code_hex,
                                      ddn_store_name, picture_name))
        self.pg.commit2()
        return 0

    def _Do_toll_same_facility(self):
        'Import Same Facility Toll.'
        self.log.info('Import Same Facility Toll.')
        table_name = 'toll_same_facility'
        self.CreateTable2(table_name)
        path = common.common_func.GetPath('toll_same_facility')
        if not path:
            self.log.error('Dose not indicate toll_same_facility.txt.')
            return 0
        fp = open(path)
        fp.readline()
        self.pg.copy_from2(fp, table_name, columns=('road_code1', 'road_seq1',
                                                    'road_code2', 'road_seq2')
                           )
        self.pg.commit2()
        fp.close()
        return 0
    
    def _Do_guidance_caution(self):
        """import caution information"""
        self.log.info('Import Caution information.')
#        self.CreateTable2('road_guidance_caution_tbl')
        path = common.common_func.GetPath('guidance_caution')
        if path == None or path == '':
            self.log.warning('Does not exist guidance caution file.')
            return 0

        objFileAnalyst = CFileAnalyst_caution_data(path)
        listrecord = objFileAnalyst.analyse()
        tempstr = '''insert into road_guidance_caution_tbl(
                                gid, Lon, Lat,
                                in_node_meshid, in_node_id,
                                out_node_meshid, out_node_id,
                                safety_type, 
                                road_name_dir, road_name_dir_yomi,
                                address, description
                                ) 
                      values ( '''
        length = len(listrecord[len(listrecord) - 1])
        while length > 0:
            tempstr = tempstr + '%s, '
            length = length - 1
        tempstr = tempstr.rstrip(', ') + ' );'

        for record in listrecord:
            values = []
            for temp in record[0:]:
                if (temp != ""):
                    values.append(temp)
                else:
                    values.append(None)
            self.pg.execute2(tempstr, tuple(values))
        self.pg.commit2()
        print 'insert caution into database finished'
        return 0
    
    def _Do_roundabout_links(self, rb_links_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        f_roundabout_links = io.open(rb_links_path,'r', 8192, 'euc-jp')
        pgcur.copy_from(f_roundabout_links, 'road_roundabout_link_tbl',
                        '\t', "", 8192,
                        ('mesh_id ', 'node1', 'node2'))
        pg.commit2()
        f_roundabout_links.close()
        return 0
        

#==============================================================================
# CFileAnalyst_tollguide
#==============================================================================
class CFileAnalyst_tollguide:
    def __init__(self, DataPath):
        self.rootpath = DataPath

    def analyse(self):
        listrecord = []
        filepath = self.rootpath
        objFile = open(filepath)
        listline = objFile.readlines()
        objFile.close()
        for line in listline:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)
        return listrecord

    def _analyseLine(self, line):
        record = []
        listrow = line.split("\t")
        # mesh_code
        record.append(listrow[0])
        # node_id
        record.append(listrow[1])
        # toll_type
        record.append(listrow[2])
        # toll_num
        record.append(listrow[3])
        # roads
        record_small = []
        record_out = []
        num = (len(listrow) - 3) / 2
        for i in range(num):
            array = []
            for j in range(1, 3):
                element = listrow[3 + 2 * i + j]
                array.append(element)
            record_small = ','.join(array)
            record_out.append(record_small)
        record_big = '|'.join(record_out)
        record.append(record_big)
        return record


#==============================================================================
# CFileAnalyst_highway
#==============================================================================
class CFileAnalyst_highway:
    def __init__(self, DataPath):
        self.rootpath = DataPath

    def analyse(self):
        listrecord = []
        filepath = self.rootpath
        objFile = open(filepath)
        listline = objFile.readlines()
        objFile.close()
        for line in listline:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)
        return listrecord

    def _analyseLine(self, line):
        record = []
        listrow = line.split("\t")
        # road_code
        record.append(listrow[0])
        # linkcls_c
        record.append(listrow[1])
        # dir_f
        record.append(listrow[2])
        # dir_no
        record.append(listrow[3])
        # road_no
        record.append(listrow[4])
        # next_no
        record.append(listrow[5])
        # next_dis
        record.append(listrow[6])
        # end_f
        record.append(listrow[7])
        # forward_num
        record.append(listrow[8])
        # backward_num
        record.append(listrow[9])
        # branchlist
        record_small = []
        record_out = []
        num = (len(listrow) - 9) / 5
        for i in range(num):
            array = []
            for j in range(1, 6):
                element = listrow[9 + 5 * i + j]
                array.append(element)
            record_small = ','.join(array)
            record_out.append(record_small)
        record_big = '|'.join(record_out)
        record.append(record_big)
        return record


#==============================================================================
# CFileAnalyst_hwysame
#==============================================================================
class CFileAnalyst_hwysame:
    def __init__(self, DataPath):
        self.rootpath = DataPath

    def analyse(self):
        listrecord = []
        filepath = self.rootpath
        objFile = open(filepath)
        listline = objFile.readlines()
        objFile.close()
        for line in listline:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)
        return listrecord

    def _analyseLine(self, line):
        record = []
        listrow = line.split("\t")
        # road_code
        record.append(listrow[0])
        # road_no
        record.append(listrow[1])
        # dir_f
        record.append(listrow[2])
        # name
        #record.append(listrow[3])
        #print listrow[0]
        #print listrow[1]
        #print listrow[2]
        #print listrow[3]
        record.append(listrow[3].decode('euc-jp', 'ignore').encode('utf-8'))
        # roads
        num = len(listrow) - 4
        array = []
        for i in range(num):
            element = listrow[3 + i + 1]
            array.append(element)
        record_out = '|'.join(array)
        record.append(record_out)
        return record


#==============================================================================
# CFileAnalystBase
#==============================================================================
class CFileAnalystBase(object):
    def __init__(self, DataPath):
        self.rootpath = DataPath

    def analyse(self):
        listrecord = []
        filepath = self.rootpath
        objFile = codecs.open(filepath, 'r', 'Shift_JIS')
        listline = objFile.readlines()
        objFile.close()
        for line in listline:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)
        return listrecord

    def _analyseLine(self, line):
        pass


#==============================================================================
# CFileAnalyst_sapa_illust
#==============================================================================
class CFileAnalyst_sapa_illust(CFileAnalystBase):
    def __init__(self, DataPath):
        CFileAnalystBase.__init__(self, DataPath)
        self.rootpath = DataPath

    def _analyseLine(self, line):
        record = []
        listrow = line.split("\t")
        for i in range(0, 6):
            record.append(listrow[i])
        return record

#==============================================================================
# CFileAnalyst_caution_data
#==============================================================================    
class CFileAnalyst_caution_data(CFileAnalystBase):
    def __init__(self, DataPath):
        self._item_cnt = 12  # you can change it depend on format
        CFileAnalystBase.__init__(self, DataPath)
        self.rootpath = DataPath
        
    def _analyseLine(self, line):
        record = []
        listrow = line.split("\t")
        for i in range(0, self._item_cnt):
            record.append(listrow[i])
        return record