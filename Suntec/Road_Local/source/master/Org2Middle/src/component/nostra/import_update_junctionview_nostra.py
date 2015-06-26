# -*- coding: UTF-8 -*-
'''
Created on 2013-8-21

@author: zym
'''
import os
import codecs

import common.common_func
import component.component_base


class import_org_update_junctionview_nostra(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'importupdatejv')

    def _DoCreateTable(self):
        return

    def _Do(self):
        update_file_dir = common.common_func.GetPath('update_jv_dir')
        self.CreateTable2('update_junctionview_tbl')
        if update_file_dir:
            self.insert_update_jv_data(update_file_dir)

    def insert_update_jv_data(self, update_file_dir):
        objFileAnalyst = CFileAnalyst(update_file_dir)
        listrecord = objFileAnalyst.analyse()
        for record in listrecord:
            self.pg.insert('update_junctionview_tbl',
                            (
                            'old_inlinkid',
                            'old_outlinkid',
                            'new_inlinkid',
                            'new_outlinkid',
                            'patterno',
                            'arrow_name'
                                      ),
                              (
                               record[0],
                               record[1],
                               record[2],
                               record[3],
                               record[4],
                               record[5]
                              )
                              )
        self.pg.commit2()


class CFileAnalyst:
    def __init__(self, strDataPath):
        self.strDataPath = strDataPath

    def analyse(self):
        listrecord = []
        objFile = None
        if os.path.exists(self.strDataPath):
            objFile = codecs.open(self.strDataPath, 'r')
        else:
            return listrecord
        listline = objFile.readlines()
        objFile.close()
        for line in listline[1:]:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)

        return listrecord

    def _analyseLine(self, line):
        record = []
        #
        listrow = line.split("\t")
        # old_inlinkid
        record.append(listrow[0])

        # old_outlinkid
        record.append(listrow[1])

        # new_inlinkid
        record.append(listrow[2])

        # new_outlinkid
        record.append(listrow[3])

        # patterno
        record.append(listrow[4])

        # arrow_name
        record.append(listrow[5])

        return record
