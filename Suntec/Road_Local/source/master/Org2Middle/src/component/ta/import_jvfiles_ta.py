# -*- coding: UTF-8 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''
import io
import os
import StringIO

import component.component_base
import common.common_func

class comp_import_sidefiles_uc(component.component_base.comp_base):

    def __init__(self):

        component.component_base.comp_base.__init__(self, 'sidefiles')
        
    def _DoCreateTable(self):
        if self.CreateTable2('jv') == -1:
            return -1
        
        if self.CreateTable2('jv_ld') == -1:
            return -1
        
        if self.CreateTable2('jv_nw') == -1:
            return -1
 
        if self.CreateTable2('jv_si') == -1:
            return -1
                                     
        return 0    
    
    def _Do(self):

        # 非欧美      
        if self._IsOverSeaData() != True:
            return 0
        # 欧美
        sf_path = common.common_func.GetPath('ta_jv_sidefile')
        if sf_path:
            self._Do_jv(sf_path)
            self._Do_jv_ld(sf_path)
            self._Do_jv_nw(sf_path)
            self._Do_jv_si(sf_path)        
    
    def _Do_jv(self,sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        f_jv = io.open(os.path.join(sf_path, 'na_na____________jv.txt'), 'r', 8192, 'utf-8')
         
        pgcur.copy_from(f_jv, 'jv', '|', "", 8192, ('id','jvtyp','complex','lat','lon','f_trpeltyp','f_trpelid_mnl','f_trpeldset_gdf','f_trpelsect_gdf','f_trpelid_gdf','jnctid_mnl','jnctdset_gdf','jnctsect_gdf','jnctid_gdf','t_trpeltyp','t_trpelid_mnl','t_trpeldset_gdf','t_trpelsect_gdf','t_trpelid_gdf','background','mirroring','road','arrow','sign_color','text_color'))
        pg.commit2()
        f_jv.close()
  
        return 0
    
    def _Do_jv_ld(self,sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        f_jv_ld = io.open(os.path.join(sf_path, 'na_na____________jvld.txt'), 'r', 8192, 'utf-8')
         
        pgcur.copy_from(f_jv_ld, 'jv_ld', '|', "", 8192, ('id','seqnr','lanediv'))
        pg.commit2()
        f_jv_ld.close()
  
        return 0
    
    def _Do_jv_nw(self,sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        f_jv_nw = io.open(os.path.join(sf_path, 'na_na____________jvnw.txt'), 'r', 8192, 'utf-8')
         
        pgcur.copy_from(f_jv_nw, 'jv_nw', '|', "", 8192, ('id','seqnr','trpeltyp','trpelid_mnl','trpeldset_gdf','trpelsect_gdf','trpelid_gdg'))
        pg.commit2()
        f_jv_nw.close()
  
        return 0       

    def _Do_jv_si(self,sf_path):
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        f_jv_si = open(os.path.join(sf_path, 'na_na____________jvsi.txt'))
        
        listline = f_jv_si.readlines()
     
        f_jv_si.close()
        
        for line in listline:
#            print line

            line_new = line.decode('ISO-8859-2').encode('utf-8')              
#            print line.decode(line_code['encoding'])
#            print line.encode('utf-8') 
            f = StringIO.StringIO(line_new) 
            pgcur.copy_from(f, 'jv_si', '|', "", 8192, ('id','seqnr','destseq','infotyp','shield','txtcont'))
#            print 'ok'
        pg.commit2()  
  
        return 0
    
    def _IsOverSeaData(self):
        '''判断是不是欧美数据。'''
        sqlcmd = """
            select gid, name from org_a0 where name = 'United States' or name = 'Deutschland';
             """
             
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            if self.pg.getcnt2() > 0:
                return True
            else:
                return False