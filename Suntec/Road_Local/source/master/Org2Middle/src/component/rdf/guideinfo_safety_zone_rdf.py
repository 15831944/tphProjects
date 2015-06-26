# -*- coding: UTF8 -*-
'''
Created on 2013-7

@author: zym
'''
from component.component_base import comp_base
import common


SPEEDUNIT_CODE = {'MPH':1,'KPH':2}
class comp_guideinfo_safety_zone_rdf(comp_base):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Guideinfo_Safety_Zone')
        
    def _DoCreateTable(self):
        if self.CreateTable2('safety_zone_tbl') == -1:
            return -1
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        return 0
    
    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        safetyzone_flag = common.common_func.GetPath('safetyzone_flag')
        if safetyzone_flag == 'true':
            pass
        
#            RDF safety camera data has no type of average speed camera, cancle.
#            self._organize_org_data()
        return
    
    def _organize_org_data(self):
        if not self.pg.IsExistTable("xroadsensepoi"):
            self.log.warning('table is not exsit!!!')
            return 0
        
        insert_sqlcmd = '''
            INSERT INTO safety_zone_tbl(
            linkid, speedlimit, speedunit_code, direction, safety_type
            )
            VALUES (%s, %s, %s, %s, %s);
        '''
       
        sqlcmd = '''
            select a.speedlimit_unit,a.speedlimit,a.linkid,drivingdirection_linkheading,b.ref_node_id,b.nonref_node_id
            from
            (
                SELECT speedlimit_unit, speedlimit_text::integer as speedlimit,linkid_text::integer as linkid,drivingdirection_linkheading
                FROM xroadsensepoi
                where speedlimit_text is not null
            ) as a
            left join rdf_link as b
            on a.linkid = b.link_id;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            speedlimit_unit = row[0]
            speedlimit = row[1]
            linkid = row[2]
            drivingdirection = row[3]
            if drivingdirection:
                if drivingdirection == 'F':
                    self.pg.execute2(insert_sqlcmd, (linkid,speedlimit,SPEEDUNIT_CODE.get(speedlimit_unit),1,1))
                elif drivingdirection == 'T':
                    self.pg.execute2(insert_sqlcmd, (linkid,speedlimit,SPEEDUNIT_CODE.get(speedlimit_unit),2,1))
                elif drivingdirection == 'B':
                    self.pg.execute2(insert_sqlcmd, (linkid,speedlimit,SPEEDUNIT_CODE.get(speedlimit_unit),3,1))
                else:
                    self.log.warning('drivingdirection is wrong!!!')
            else:
                self.log.warning('not provider drivingdirection!!!')
        self.pg.commit2()
        return
    
    
    
    
    
    