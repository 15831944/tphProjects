'''
Created on 2012-8-14

@author: liuxinxing
'''

import os
import shutil

import component.component_base
import common.common_func

class comp_guideinfo_etc_jpn(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'GuideInfo_Etc')
        
    def _Do(self):
        
        self.__loadOrginalData_for_sp()
        self.__loadOrginalData_for_link()
        
        return 0
    
    def __loadOrginalData_for_sp(self):
        self.log.info('load original etc data to road_etc_sp...')
        
        #
        self.CreateTable2('road_etc_sp')
        
        #
        strIllustPath = common.common_func.GetPath('illust')
        if strIllustPath:
            listFileType = ['etc', 'etc2']
            for filetype in listFileType:
                objFileAnalyst = CFileAnalyst_Nodelist(strIllustPath)
                listrecord = objFileAnalyst.analyse(filetype)
                print filetype, len(listrecord)
                for record in listrecord:
                    self.pg.insert('road_etc_sp', 
                                  (
                                   'roadname',
                                   'roaddir',
                                   'facname',
                                   'nodeid',
                                   'picname'
                                  ),
                                  (
                                   record[1],
                                   record[3],
                                   record[2],
                                   record[4],
                                   record[0]
                                  )
                                  )
            
            #
            self.pg.commit2()
        
        self.log.info('load original etc data to road_etc_sp end.')
    
    def __loadOrginalData_for_link(self):
        self.log.info('load original etc data to road_etc_link...')
        
        #
        self.CreateTable2('road_etc_link')
        
        #
        strHighwayPath = common.common_func.GetPath('highway')
        if strHighwayPath:
            listFileType = ['etc_link']
            for filetype in listFileType:
                objFileAnalyst = CFileAnalyst_Highway(strHighwayPath)
                listrecord = objFileAnalyst.analyse(filetype)
                print filetype, len(listrecord)
                for record in listrecord:
                    self.pg.insert('road_etc_link', 
                                  (
                                   'typecode',
                                   'snodeid',
                                   'midnodeid',
                                   'enodeid'
                                  ),
                                  (
                                   record[0],
                                   record[1],
                                   record[2],
                                   record[3]
                                  )
                                  )
            
            #
            self.pg.commit2()
        
        self.log.info('load original etc data to road_etc_link end.')

class CFileAnalyst:
    def __init__(self, strDataPath):
        self.rootpath = strDataPath
    
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
        pass
    
    def _analyseLine(self, line):
        pass
    
class CFileAnalyst_Nodelist(CFileAnalyst):
    def __init__(self, strIllustDataPath):
        CFileAnalyst.__init__(self, strIllustDataPath)
    
    def _getDataFilePath(self, filetype):
        strNodeList = os.path.join(self.rootpath, 'nodelist')
        listfilename = os.listdir(strNodeList)
        for filename in listfilename:
            if filename.find('_%s.txt' % filetype) != -1:
                filepath = os.path.join(strNodeList, filename)
                return filepath
        else:
            self.log.warning('%s file does not exist.' % filetype)
            raise Exception, '%s file does not exist.' % filetype
    
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
        
        # nodeid
        record.append(int(listrow[5]))
        
        return record


class CFileAnalyst_Highway(CFileAnalyst):
    def __init__(self, strHighwayDataPath):
        CFileAnalyst.__init__(self, strHighwayDataPath)
    
    def _getDataFilePath(self, filetype):
        listfilename = os.listdir(self.rootpath)
        for filename in listfilename:
            if filename.find('%s.txt' % filetype) != -1:
                filepath = os.path.join(self.rootpath, filename)
                return filepath
        else:
            self.log.warning('%s file does not exist.' % filetype)
            raise Exception, '%s file does not exist.' % filetype
    
    def _analyseLine(self, line):
        record = []
        
        #
        listrow = line.split("\t")
        length = len(listrow)
        
        # typecode
        record.append(int(listrow[0]))
        
        # snodeid
        record.append(int(listrow[2]))
        
        # midnodeid
        record.append(",".join(listrow[3:length-1]))
        
        # enodeid
        record.append(int(listrow[length-1]))
                
        return record
    