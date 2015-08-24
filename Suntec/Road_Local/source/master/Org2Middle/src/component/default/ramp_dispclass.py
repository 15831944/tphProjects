# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: hongchenzai
@alter: zhengchao
'''
import component.component_base
import common.common_func
import common.search_road

class comp_ramp_dispclass(component.component_base.comp_base):
    '''
    Ramp RoadType
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Ramp_DisplayClass')
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('temp_ramp_update_dispclass') == -1:
            return -1 
        if self.CreateTable2('link_tbl_bak_dispclass') == -1:
            return -1 
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
        
    def _DoCreateIndex(self): 
        
        return 0
    
    def _Do(self):
        
        self._UpdateRampDisplayClass()
        return 0

    def _insertinto_tmptable(self,link_list,disp_class):
        sqlcmd_insert='''
            insert into temp_ramp_update_dispclass
                    values(%d,%d)
                '''
        for link_id in link_list:
            self.pg.execute(sqlcmd_insert%( link_id,disp_class))
        self.pg.commit()
    
    def _updatedipclass(self):
        
        sqlcmd='''
                update link_tbl a
                set display_class=b.max
                from 
                (
                    select link_id,max(display_class) from 
                    temp_ramp_update_dispclass
                    group by link_id
                ) b
                where a.link_id=b.link_id
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
    def _UpdateRampDisplayClass(self):
        
        self.log.info('Updating Disp Class of Ramp Start.')
        self.search_road=common.search_road.search_road(link_id=None,dir=0,onewayflg=True,thrucondition=' link_type=5',pg=self.pg)
        sqlcmd_se= '''
                select link_id
                from link_tbl a
                left join temp_ramp_update_dispclass b
                using(link_id)
                where link_type = 5 and b.link_id is null
                limit 1
                '''
        while True:

            self.pg.execute(sqlcmd_se)
            link_id_list=self.pg.fetchall2()

            if not link_id_list:
                break

            self.search_road.start_link_id=link_id_list[0][0]
            self.search_road.init_path()
            self.search_road.search()
            results=self.search_road.return_paths()
            
            if not results:
                continue

            sqlcmd=''' select display_class from link_tbl where link_id=%d'''

            for result in results:

                self.pg.execute(sqlcmd%(result['path'][-1]))
                last_disp_class=self.pg.fetchall2()[0][0]
                result['disp_class']=last_disp_class
                
            result_1=filter(lambda x:x['dir']==1, results)
            result_2=filter(lambda x:x['dir']==2, results)

            for x in result_1:
                for y in result_2:
                    self._insertinto_tmptable(set(x['path'][:-1])|set(y['path'][:-1]),min([x['disp_class'],y['disp_class']]))

        self._updatedipclass()
        
        self.log.info('Updating Disp Class of Ramp End.')