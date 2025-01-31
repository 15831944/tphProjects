# -*- coding: cp936 -*-
'''
Created on 2012-4-6

@author: hongchenzai
'''

import os
import psycopg2
import struct

from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_guideinfo_pic_blob_bytea(ItemBase):
    '''
    Illust图片
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Picture_Blob')
        
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_pic_blob_bytea')
        self.CreateTable2('temp_point_list')
        
        return 0
    
    def Do_CreatIndex(self):
        self.CreateIndex2('rdb_guideinfo_pic_blob_bytea_image_id_idx')
        self.CreateIndex2('temp_point_list_image_id_idx')
        
        return 0
    
    def Do(self):
        self.__load_illust_pic()
        self.__load_point_list()
        
        return 0
    
    def __load_illust_pic(self):
        rdb_log.log(self.ItemName, 'insert data to rdb_guideinfo_pic_blob_bytea ...', 'info')
        
        picroot = rdb_common.GetPath('illust')
        if picroot:
            self.__loadPic(picroot)
        
        self.pg.commit2()
        
        rdb_log.log(self.ItemName, 'insert data to rdb_guideinfo_pic_blob_bytea end.', 'info')
    
    def __loadPic(self, path):
        listsub = os.listdir(path)
        for sub in listsub:
            subpath = os.path.join(path, sub)
            if os.path.isfile(subpath):
                exttype = os.path.splitext(sub)[1].lower()
                if exttype in (".jpg", ".png", ".dat"):
                    self.__insertPic(subpath, sub)
            else:
                self.__loadPic(subpath)
    
    def __insertPic(self, filepath, filename):
        index = filename.rfind(".")
        picname = filename[:index].lower().encode("utf-8")
        objFile = open(filepath, 'rb')
        alldata = objFile.read()
        objFile.close()
        
        if self.pg.insert('rdb_guideinfo_pic_blob_bytea', 
                         ('image_id', 'data'), 
                         (picname, psycopg2.Binary(alldata))
                         ) == -1:
            rdb_log.log(self.ItemName, 'fail to load spotguide pic %s.' % filepath, 'error')
            return -1
    
    def __load_point_list(self):
        rdb_log.log(self.ItemName, 'Inserting record to temp_point_list...', 'info')
        
        ptroot = rdb_common.GetPath('pointlist')
        if ptroot:
            self.__loadPt(ptroot)
        
        self.pg.commit2()
        
        rdb_log.log(self.ItemName, 'insert record to temp_point_list end.', 'info')
    
    def __loadPt(self, path):
        listsub = os.listdir(path)
        for sub in listsub:
            subpath = os.path.join(path, sub)
            if os.path.isfile(subpath):
                exttype = os.path.splitext(sub)[1].lower()
                if exttype in (".txt"):
                    self.__insertPt(subpath, sub)
            else:
                self.__loadPt(subpath)
    
    def __insertPt(self, filepath, filename):
        ptname = filename.split(".")[0].lower().encode("utf-8")
        objFile = open(filepath, 'r') 
        listline = objFile.readlines()
        objFile.close()
        res = ''
        for line in listline:
            line = line.strip()
            if line:
                ox, oy = line.split(',')
                x = int(float(ox) * 240.0 / 196.0 + 0.5)
                y = int(float(oy) * 234.0 / 180.0 + 0.5)
                res = res + struct.pack("h", x)
                res = res + struct.pack("h", y)

        if self.pg.insert('temp_point_list', ('image_id', 'data'), (ptname, psycopg2.Binary(res))) == -1:
            rdb_log.log(self.ItemName, 'fail to load pointlist %s.' % filepath, 'error')
            return -1
    
    def RenameAndSaveIllust(self):
        self.pg.connect2()
        if not self.pg.IsExistTable('rdb_guideinfo_spotguidepoint'):
            self.log.warning('not exist table rdb_guideinfo_spotguidepoint!')
            return 0
        '保存Illust图片'
        print 'Start saving Illust Pictures.'
        binary_picroot = self._CreatBinaryPath()
        # 每种Illust种别的图片存放的目录
        type_dict = {1: '2DIllust',
                     2: 'Exit',
                     3: 'Enter',
                     4: '2DIllust',
                     5: 'NwyRealIllust',
                     6: 'Exit',
                     7: '3DIllust',
                     8: 'Highwayjct',
                     9: 'CurrentEct',
                     10: 'NoCurrentEct'
        }
        
        # 取得当前数据里所有的Illust种别
        sqlcmd = """     
                SELECT distinct("type")
                  FROM rdb_guideinfo_spotguidepoint;
                """
        self.pg.connect1()
        all_type = []
        self.pg.query(sqlcmd)
        rows  = self.pg.fetchall()
        for row in rows:
            all_type.append(row[0])
          
        for _type in all_type:
            if type_dict.get(_type):
                # 在binary目录创建子目录
                type_path = os.path.join(binary_picroot, type_dict.get(_type))
            else:
                rdb_log.log(self.ItemName, "Doesn't exit directory for Illust type " + str(_type) + '.', 'warning')
                type_path = os.path.join(binary_picroot, 'other\\' + str(_type))

            try:
                os.stat(type_path)            # 测试文件路径存不存在
            except:
                os.makedirs(type_path)        # 创建文件路径
              
            # 保存patterns
            sqlcmd = """
                SELECT gid, data
                  FROM rdb_guideinfo_pic_blob_bytea
                  where gid in (
                        SELECT distinct(pattern_id)
                        FROM rdb_guideinfo_spotguidepoint
                        where type = %s
                        )
                    order by gid;
                """
            self._SavePattern(type_path, _type, sqlcmd)
            # 保存Arrows
            sqlcmd = """
                    SELECT gid, data
                      FROM rdb_guideinfo_pic_blob_bytea
                      where gid in (
                            SELECT distinct(arrow_id)
                            FROM rdb_guideinfo_spotguidepoint
                            where type = %s
                            )
                        order by gid;
                    """
            self._SaveArrow(type_path, _type, sqlcmd)
            
        # 结束，关闭数据库连接
        self.pg.close1()
        print 'End saving Illust Pictures.'
        return 0
       
    def RenameAndSaveSignPost(self):
        self.pg.connect2()
        if not self.pg.IsExistTable('rdb_guideinfo_signpost'):
            self.log.warning('not exist table rdb_guideinfo_signpost!')
            return 0
        '保存Illust图片'
        print 'Start saving SignPost Pictures.'
        # 取得binary目录
        binary_picroot = self._CreatBinaryPath()
        # 在binary目录创建子目录SignPost
        signpost_path = os.path.join(binary_picroot, 'SignPost')
        try:
            os.stat(signpost_path)            # 测试文件路径存不存在
        except:
            os.makedirs(signpost_path)        # 创建文件路径
        # 保存patterns
        sqlcmd = """
                SELECT gid, data
                  FROM rdb_guideinfo_pic_blob_bytea
                  where gid in (
                        SELECT distinct(pattern_id)
                          FROM rdb_guideinfo_signpost
                  )
                  order by gid;
                """  
        self._SavePattern(signpost_path, None, sqlcmd)
        # 保存Arrows
        sqlcmd = """
                SELECT gid, data
                  FROM rdb_guideinfo_pic_blob_bytea
                  where gid in (
                        SELECT distinct(arrow_id)
                          FROM rdb_guideinfo_signpost
                  )
                  order by gid;
                """  
        self._SaveArrow(signpost_path, None, sqlcmd)
        # 结束，关闭数据库连接
        print 'End saving SignPost Pictures.'
        return 0
    
    def _CreatBinaryPath(self):
        '创建存放二进制数据图片的目录'
        picroot = rdb_common.GetPath('illust')
        binary_picroot = os.path.join(picroot, 'binary')
        try:
            os.stat(binary_picroot)            # 测试文件路径存不存在
        except:
            os.makedirs(binary_picroot)        # 创建文件路径
        return binary_picroot
    
    def _SavePattern(self, type_path, _type, sqlcmd):
        'patter图片 '
        
        # pattern目录
        pattern_root = os.path.join(type_path, 'pattern')
        try:
            os.stat(pattern_root)            # 测试文件路径存不存在
        except:
            os.makedirs(pattern_root)        # 创建文件路径
        pic_num = 0
        # 取得第一张图片
        rows = None
        if _type:
            rows = self.pg.get_batch_data2(sqlcmd, (_type,), batch_size = 1024)
        else:
            rows = self.pg.get_batch_data2(sqlcmd, batch_size = 1024)
        for row in rows:
            pic_name = os.path.join(pattern_root, str(row[0]))
            # 以二进制写方
            file_object= open(pic_name, 'wb')
            file_object.write(row[1])
            file_object.close()
            pic_num += 1
        return pic_num
    
    def _SaveArrow(self, type_path, _type, sqlcmd):
        'Arrow图片'
        # pattern目录
        arrow_root = os.path.join(type_path, 'arrow')
        try:
            os.stat(arrow_root)            # 测试文件路径存不存在
        except:
            os.makedirs(arrow_root)        # 创建文件路径
        pic_num = 0
        # 取得第一张图片
        rows = None
        if _type != None:
            rows = self.pg.get_batch_data2(sqlcmd, (_type,), batch_size = 1024)
        else:
            rows = self.pg.get_batch_data2(sqlcmd, batch_size = 1024)
        for row in rows:
            pic_name = os.path.join(arrow_root, str(row[0]))
            # 以二进制写方
            file_object= open(pic_name, 'wb')
            file_object.write(row[1])
            file_object.close()
            pic_num += 1

        return pic_num
    
    def DownLoadImageByID(self, Image_Id):
        '按照指定的image id, 下载图片。'
        if Image_Id is None:
            print 'Image ID is None, Please input a Image ID.'
            return -1
        
        picroot = rdb_common.GetPath('illust')
        sqlcmd = """
                SELECT gid, data, image_id
                  FROM rdb_guideinfo_pic_blob_bytea
                  where gid = %s;
                """
        self.pg.connect1()
        self.pg.execute1(sqlcmd, (Image_Id,))
        row  = self.pg.fetchone()
        while row:
            pic_name = os.path.join(picroot, str(row[0]))
            # 以二进制写方
            file_object= open(pic_name, 'wb')
            file_object.write(row[1])
            file_object.close()
            row = self.pg.fetchone()
        self.pg.close1()
        return 0

    def RenameAndSaveSapaIllust(self):
        self.pg.connect2()
        if not self.pg.IsExistTable('rdb_highway_illust_info'):
            self.log.warning('not exist table rdb_highway_illust_info!')
            return 0
        '保存Illust图片'
        print 'Start saving sapaIllust Pictures.'
        # 取得binary目录
        binary_picroot = self._CreatBinaryPath()
        # 在binary目录创建子目录SignPost
        sapa_path = os.path.join(binary_picroot, 'SaPa')
        try:
            os.stat(sapa_path)  # 测试文件路径存不存在
        except:
            os.makedirs(sapa_path)  # 创建文件路径
        # 保存sapa图片
        sqlcmd = """
                select gid,data
                from
                (
                SELECT distinct image_id
                FROM rdb_highway_illust_info
                )as a
                left join rdb_guideinfo_pic_blob_bytea as b
                on a.image_id::integer = b.gid
                """
        self._SavePattern(sapa_path, None, sqlcmd)
        print 'End saving sapa Pictures.'
        return 0
