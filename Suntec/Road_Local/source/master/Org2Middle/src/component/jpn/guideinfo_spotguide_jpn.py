'''
Created on 2012-3-26

@author: sunyifeng
'''

import os
import shutil

import common.common_func
import component.component_base

class comp_guideinfo_spotguide_jpn(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'GuideInfo_SpotGuide')
        
    def _Do(self):
        self.__loadOrginalData()
        self.__convert2Middle()
        return 0
    
    def _DoCreateTable(self):
        return 0
    
    def __loadOrginalData(self):
        self.log.info('load original spotguide data to road_illust...')
        
        #
        self.CreateTable2('road_illust')
        
        #
        strIllustPath = common.common_func.GetPath('illust')
        if strIllustPath:
            listIllust = [
                          ('grade', 4), ('branch', 1), ('inic', 3), 
                          ('etc', 9), ('etc2', 10), ('exit', 6), 
                          ('properjct', 8), ('realcross', 5)
                          ]
            for (filetype, pictype) in listIllust:
                objFileAnalyst = CFileAnalyst(strIllustPath)
                listrecord = objFileAnalyst.analyse(filetype)
                print filetype, len(listrecord)
                for record in listrecord:
                    self.pg.insert('road_illust', 
                                  (
                                   'roadname',
                                   'roaddir',
                                   'facname',
                                   'nodelist',
                                   'pictype',
                                   'picname'
                                  ),
                                  (
                                   record[1],
                                   record[3],
                                   record[2],
                                   record[4],
                                   pictype,
                                   record[0]
                                  )
                                  )
            
            #
            self.pg.commit2()
        
        self.log.info('load original spotguide data to road_illust end.')
            
    def __convert2Middle(self):
        self.log.info('convert road_illust to spotguide_tbl...')
        
        self.CreateTable2('spotguide_tbl')
        
        self.CreateFunction2('mid_convert_spotguide')
        self.CreateFunction2('rdb_cnv_oneway_code')
        self.CreateFunction2('mid_getsplitlinkid')
        self.CreateFunction2('mid_getConnectionNodeID')
        self.CreateFunction2('mid_get_new_passlid')
        self.CreateFunction2('mid_getSplitLinkIDArray')
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')
        
        
        self.pg.callproc('mid_convert_spotguide')
        
        self.pg.commit2()
        
        self.log.info('convert road_illust to spotguide_tbl end.')

class CFileAnalyst:
    def __init__(self, strIllustDataPath):
        self.rootpath = strIllustDataPath
    
    def analyse(self, filetype):
        listrecord = []
        
        filepath = self._getDataFilePath(filetype)
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
        
    def _getDataFilePath(self, filetype):
        strNodeList = os.path.join(self.rootpath, 'nodelist')
        listfilename = os.listdir(strNodeList)
        for filename in listfilename:
            if filename.find(filetype+".txt") != -1:
                filepath = os.path.join(strNodeList, filename)
                return filepath
        else:
            self.log.warning('%s file of spotguide does not exist.' % filetype)
            raise Exception, '%s file of spotguide does not exist.' % filetype
    
    def _analyseLine(self, line):
        record = []
        
        #
        listrow = line.split("\t")
        
        # picname
        record.append(listrow[0].lower())
        
        # roadname
        record.append(listrow[1].decode('shift_jis').encode('utf-8'))
        
        # facname
        record.append(listrow[2].decode('shift_jis').encode('utf-8'))
        
        # roaddir
        record.append(listrow[3].decode('shift_jis').encode('utf-8'))
        
        # nodelist
        nodelist = []
        nodenum = ( len(listrow) - 4 ) / 2
        for i in range(nodenum):
            #meshcode = listrow[4 + 2*i]
            nodeid = listrow[4 + 2*i + 1]
            nodelist.append(nodeid)
        record.append(','.join(nodelist))
        
        return record


class CFileFilter:
    def __init__(self, strDataPath, strBackupPath, filter_file_path):
        self.srcroot = strDataPath
        self.bkroot = strBackupPath
        self.filter_file_path = filter_file_path
    
    def filt(self):
        #
        dictmeshcode = {}
        objFile = open(self.filter_file_path)
        listline = objFile.readlines()
        objFile.close()
        for line in listline:
            line = line.strip()
            if line:
                dictmeshcode[line] = True
        
        #
        listfilename = os.listdir(self.srcroot)
        for filename in listfilename:
            srcpath = os.path.join(self.srcroot, filename)
            bkpath = os.path.join(self.bkroot, filename)
            shutil.copyfile(srcpath, bkpath)
            
            objFile = open(bkpath)
            listline = objFile.readlines()
            objFile.close()
            
            objFile = open(srcpath, 'w')
            for line in listline:
                if not self.__filterLine(line, dictmeshcode):
                    objFile.write(line)
                else:
                    print filename, line.strip()
            objFile.close()
                    
    def __filterLine(self, line, dictmeshcode):
        #
        listrow = line.strip().split("\t")
        
        # nodelist
        nodenum = ( len(listrow) - 4 ) / 2
        for i in range(nodenum):
            meshcode = listrow[4 + 2*i]
            if not dictmeshcode.has_key(meshcode):
                print meshcode
                return True
        else:
            return False

if __name__ == "__main__":
    data_path = "D:/WebGIS/Data/iPC-Org/IllustData_20120127/nodelist"
    backup_path = "D:/WebGIS/Data/iPC-Org/IllustData_20120127/nodelist_backup"
    filter_file = "D:/WebGIS/Data/iPC-Org/Road/NW_SD-120103S-020A/data_spec/meshlist.txt"
    objFilter = CFileFilter(data_path, backup_path, filter_file)
    objFilter.filt()
    
    