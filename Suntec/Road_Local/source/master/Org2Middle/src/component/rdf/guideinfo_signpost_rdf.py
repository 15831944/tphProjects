# -*- coding: utf8 -*-
'''
Created on 2012-3-27

@author: zym
'''
import common
import os
import re
import psycopg2
import struct
from component.component_base import comp_base

class comp_guideinfo_signpost_rdf(comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'GuideInfo_SignPost')
    
    def _DoCreateTable(self):
        self.CreateTable2('signpost_tbl')
    
    def _Do(self):
        sign_as_real_path = common.common_func.GetPath('sign_as_real_path')
        if sign_as_real_path:
            self._import_sign_as_real_data(sign_as_real_path)
            self._make_temp_sign_as_real_tbl()
            self.log.info('End convert SignPost from road_dir.')

        if common.common_func.GetPath('proj_country').lower() in ('hkg','mac'):                 
            self._makeSignpost_CHN()             
                  
        return 0
    
    def _import_sign_as_real_data(self,sign_as_real_path):
        self.CreateTable2('org_sign_as_real')
        objFile = open(sign_as_real_path)
        listline = objFile.readlines()
        for line in listline:
            line = line.strip()
            data_array = line.split(',')
            self.pg.execute('''
                INSERT INTO org_sign_as_real(filename, inlinkid)
                    VALUES (%s, %s);
            ''', (data_array[0],int(data_array[1])))
        self.pg.commit2()
        return
    
    def _make_temp_sign_as_real_tbl(self):
        insert_sqlcmd = """
            insert into signpost_tbl(
                nodeid,inlinkid,patternno,passlink_cnt
                )
                VALUES (%s, %s, %s, %s)
            """
        #姹傝妭鐐�
        sqlcmd = '''
            SELECT filename, inlinkid,s_node,e_node,one_way_code
            FROM org_sign_as_real as a
            left join link_tbl as b
            on a.inlinkid = b.link_id;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            filename = row[0]
            inlinkid = row[1]
            one_way_code = row[4]
            flag = filename[len(filename)-1:]
            if not row[3]:
                self.log.error('Error record,can not get node!!!')
                continue
            
            if flag not in ('R','N'):
                if one_way_code == 1:
                    self.log.error('Error record,can not get direction!!!')
                    continue
                elif one_way_code == 2:
                    self.pg.execute2(insert_sqlcmd, (row[3],inlinkid,filename,0))
                elif one_way_code == 3:
                    self.pg.execute2(insert_sqlcmd, (row[2],inlinkid,filename,0))
                else:
                    pass
            elif flag == 'R':
                self.pg.execute2(insert_sqlcmd, (row[2],inlinkid,filename,0))
            else:
                self.pg.execute2(insert_sqlcmd, (row[3],inlinkid,filename,0))
        self.pg.commit2()

    def _makeSignpost_CHN(self):

            # create temp signpost table.
            self.CreateTable2('temp_guideinfo_signpost_chn')

            sqlcmd = """
            insert into signpost_tbl(nodeid, inlinkid, outlinkid,
                passlid,passlink_cnt,direction,
                guideattr,namekind,guideclass,patternno,arrowno,sp_name)
            (
                select all_nodes[1] as nodeid, all_links[1] as inlinkid,
                            all_links[all_link_cnt] as outlinkid,
                        case when all_link_cnt = 2 then null
                        else array_to_string(all_links[2:all_link_cnt-1], '|')
                        end as passlinks,
                        all_link_cnt - 2 as pass_cnt,
                        0 as direction,0 as guideattr,
                        0 as namekind,0 as guideclass,
                        pattern as patternno,
                        arrow as arrowno,
                        null as sp_name 
                from
                (
                  select A.*, array_upper(all_links,1) as all_link_cnt
                  from temp_guideinfo_signpost_chn as A
                ) as B
            );
            """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect('''host='172.26.179.184'
                        dbname='post13_MEA8_RDF_CI'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()
        
    def _create_sign_name_tbl(self,sign_file_path):
        self.pgcur2.execute('''
                            CREATE TABLE temp_signname_tbl
                            (
                              gid serial NOT NULL,
                              filename character varying(128),
                              inlinkid integer
                            );
                            ''')
        filenames = os.listdir(sign_file_path)
        for filename in filenames:
            inlinkid = int((re.findall("\d{1,16}",filename))[0])
            filename = (os.path.splitext(filename))[0]
            self.pgcur2.execute('''INSERT INTO temp_signname_tbl(filename, inlinkid) VALUES (%s, %s)''', (filename,inlinkid))
        self.conn.commit()
        return 0
    
    def _create_sign_info_difference_set_tbl(self):
        self.pgcur2.execute('''
                        CREATE TABLE temp_signname_difference_tbl
                        as
                        (
                          SELECT a.gid, a.filename, a.inlinkid
                          FROM temp_signname_tbl as a
                          left join (
                            SELECT *
                            FROM rdfpl_all_jv_lat_display_org
                            where ejv_filename is not null
                          ) as b
                         on a.inlinkid = b.originating_link_id
                         where b.gid is null
                        );
                        ''')
        self.conn.commit()
        return 0
    
    def _create_sign_info_difference_set_file(self,out_file_path):
        self.pgcur2.execute('''
                        SELECT gid, filename, inlinkid
                        FROM temp_signname_difference_tbl;
                        ''')
        rows = self.pgcur2.fetchall()
        output = open(out_file_path, 'w')
        for row in rows:
            filename = row[1]
            inlinkid = row[2]
            output.write(filename + ',' + str(inlinkid) + '\n')
        output.close()
        return 0
    def _handler_illust(self,input_file_path,output_file_path):
        self.pgcur2.execute('''
                        SELECT gid, filename, inlinkid
                        FROM temp_signname_difference_tbl;
                        ''')
        rows = self.pgcur2.fetchall()
        for row in rows:
            input_file = input_file_path + '\\' + row[1] + '.png'
            output_file = output_file_path + '\\' + row[1] + '.dat'
            if  os.path.isfile(input_file_path + '\\' + row[1] + '.png') == False:
                print 'sign not exsit!!!'
                continue
            fis = open(input_file, 'rb')
            fos = open(output_file, 'wb')
            headerBuffer = struct.pack("<HB", 1, 0)
            resultBuffer = headerBuffer + fis.read()
            fis.close()
            fos.write(resultBuffer)
            fos.close()
        return 0
    
class GeneratorPicBinaryMethorBase(object):
    def __init__(self):
        pass
    
    def analyze(self,struct_pack):
        elements = struct_pack.split(',')
        format_str = elements[0]
        str_len = len(format_str)
        if str_len < 2:
            return None
        endian_flag = format_str[0]
        pack_init = ''
        i = 1
        while i < str_len:
            sub_foramt_str = endian_flag + format_str[i]
            pack_init = pack_init + struct.pack(sub_foramt_str, int(elements[i]))
            i = i + 1
        return pack_init
    
class GeneratorPicBinaryMethord(GeneratorPicBinaryMethorBase):
    def __init__(self):
        GeneratorPicBinary.__init__()
        
    def _create_pic_binary(self,struct_pack):
        headbuffer = super(GeneratorPicBinaryMethord, self).analyze(struct_pack)
        return headbuffer

class GeneratorPicBinaryOld(GeneratorPicBinary):
    def __init__(self):
        super(GeneratorPicBinaryOld, self).__init__()
        
    def _handler_illust(self, input_file_path, output_file_path):
        self.pgcur2.execute('''
                        SELECT gid, filename, inlinkid
                        FROM temp_signname_difference_tbl;
                        ''')
        rows = self.pgcur2.fetchall()
        for row in rows:
            input_file = input_file_path + '\\' + row[1] + '.png'
            output_file = output_file_path + '\\' + row[1] + '.dat'
            if  os.path.isfile(input_file_path + '\\' + row[1] + '.png') == False:
                print 'sign not exsit!!!'
                continue
            fis = open(input_file, 'rb')
            fos = open(output_file, 'wb')
            pic_len = os.path.getsize(input_file)
            headerBuffer  = struct.pack("<hhbii", 0xFEFE, 1, 0, 13, pic_len)
            resultBuffer = headerBuffer + fis.read()
            fis.close()
            fos.write(resultBuffer)
            fos.close()
        return 0
    
class GeneratorPicBinaryNew(GeneratorPicBinary):
    def __init__(self):
        GeneratorPicBinary.__init__()
    def _handler_illust(self, input_file_path, output_file_path):
        return super(GeneratorPicBinaryNew, self)._handler_illust(input_file_path, output_file_path)
if __name__ == '__main__':
    test = GeneratorPicBinaryOld()
    test._create_sign_name_tbl("E:\\orgdata\\here\\MEA\\sign\\SaR\\MEA\\ASPECT_RATIO_4x3")
    test._create_sign_info_difference_set_tbl()
    test._create_sign_info_difference_set_file("E:\\orgdata\\here\\MEA\\sign\\SaR\\MEA\\test.csv")
    test._handler_illust("E:\\orgdata\\here\\MEA\\sign\\SaR\\MEA\\ASPECT_RATIO_4x3", "E:\\orgdata\\here\\MEA\\sign\\SaR\\MEA\\test")
    pass