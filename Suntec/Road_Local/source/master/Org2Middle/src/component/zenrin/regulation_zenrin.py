# -*- coding: UTF-8 -*-
'''
Created on 2015-6-17

@author: liushengqiang
'''



import component.component_base

class comp_regulation_zenrin(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _Do(self):
        
        self.__convert_condition_table()
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        self.__convert_regulation_oneway()
        self.__convert_regulation_linkrow()
        
        return 0
    
    def __convert_condition_table(self):
        self.log.info('convert condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        
        self.CreateFunction2('mid_get_day_of_week')
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        self.pg.commit2()
        
        self.CreateIndex2('temp_condition_regulation_tbl_day_shour_ehour_sdate_edate_cartype_idx')
        
        self.log.info('convert condition_regulation_tbl end.')
    
    def __convert_regulation_oneway(self):
        self.log.info('convert regulation of oneway...')
        
        self.CreateTable2('temp_org_one_way')
        sqlcmd = """
                insert into temp_org_one_way (
                    linkno, dir, day, shour,
                    ehour, sdate, edate, cartype
                    ) 
                select (a.meshcode || lpad(a.linkno::varchar,7,'0'))::bigint as linkno,
                       (case 
                             when a.snodeno = b.snodeno then 2 --positive direction
                             else 3 --negative direction
                        end)::smallint as dir, 
                        day, shour, ehour, sdate, edate, cartype
                from "org_one-way" a
                left join (
                    select *
                    from org_road_bicycle
                    union 
                    select *
                    from org_road_croad
                    union 
                    select *
                    from org_road_ferry
                    union 
                    select *
                    from org_road_general1
                    union 
                    select *
                    from org_road_general2
                    union 
                    select *
                    from org_road_general3
                    union 
                    select *
                    from org_road_hiway1
                    union 
                    select *
                    from org_road_hiway2
                    union 
                    select *
                    from org_road_motorcycle
                    union 
                    select *
                    from org_road_thin
                ) b
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where b.gid is not null
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.CreateIndex2('temp_org_one_way_day_shour_ehour_sdate_edate_cartype_idx')
        
        self.CreateFunction2('mid_convert_regulation_oneway')
        self.pg.callproc('mid_convert_regulation_oneway')
        self.pg.commit2()
        
        self.log.info('convert regulation of oneway end.')
    
    def __convert_regulation_linkrow(self):
        self.log.info('convert regulation of linkrow...')
        
        self.CreateTable2('temp_org_not_in')
        sqlcmd = """
                insert into temp_org_not_in (
                    fromlinkno, tolinkno, snodeno, tnodeno, enodeno, day, shour,
                    ehour, sdate, edate, cartype, carwait, carthaba, carthigh
                    ) 
                select (meshcode || lpad(fromlinkno::varchar,7,'0'))::bigint as fromlinkno,
                       (meshcode || lpad(tolinkno::varchar,7,'0'))::bigint as tolinkno,
                       (meshcode || lpad(snodeno::varchar,7,'0'))::bigint as snodeno,
                       (meshcode || lpad(tnodeno::varchar,7,'0'))::bigint as tnodeno,
                       (meshcode || lpad(enodeno::varchar,7,'0'))::bigint as enodeno,
                       day, shour, ehour, sdate, edate, cartype, carwait, carthaba, carthigh
                from "org_not-in"
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.CreateIndex2('temp_org_not_in_day_shour_ehour_sdate_edate_cartype_idx')
        
        self.CreateFunction2('mid_convert_regulation_linkrow')
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('convert regulation of linkrow end.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0