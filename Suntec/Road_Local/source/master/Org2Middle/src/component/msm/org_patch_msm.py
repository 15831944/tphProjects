# -*- coding: UTF8 -*-
'''
Created on 2014-8-19

@author: yuanrui
'''
import codecs
import component.component_base

class comp_org_patch_msm(component.component_base.comp_base):
    '''
    Correcting original data
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'ORG_patch')

    def _Do(self):
                   
        self.log.info(': Now it is correcting original data...')

        '''原始数据修订(主要修正：道路的通行方向有误、无名称等明显的错误)'''
        #===============================================================================
        # 配置文档格式如下：
        #     folder;link_id;field;value
        # 修订方式：
        #     将org_processed_line表中匹配folder&link_id的记录，其field更新为value.  
        #===============================================================================
      
        file_obj = codecs.open('..\docs\org_patch_msm.txt')
        if file_obj:
            self.listline = file_obj.readlines()  
            
            for line in self.listline[1:]:
                line = line.encode('utf-8').strip('\n').strip('\r')
                Valstr = line.split(';')

                sqlcmd = 'update org_processed_line set ' + Valstr[1] \
                    + ' = ' + Valstr[2] + ' where ST_Astext(the_geom) = ' + Valstr[0] 

                #===============================================================
                # For check original data:
                # sqlcmd = 'select * from org_processed_line where ' + Valstr[1] \
                #    + ' = ' + Valstr[2] + ' and ST_Astext(the_geom) = ' + Valstr[0] 
                #===============================================================
                
                self.pg.execute2(sqlcmd)
                
                #===============================================================
                # For check original data:
                # if self.pg.getcnt2() > 0:
                #    print Valstr[0] + ' has already been fixed.'
                #===============================================================
                
            self.pg.commit2()
                
        file_obj.close() 
                
        self.log.info(': end of correcting original data...')
