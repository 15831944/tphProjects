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
        self.__make_linklist_for_linkdir()
        
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

        self.CreateIndex2('temp_link_mapping_idx')
        self.CreateIndex2('org_one_way_meshcode_linkno_idx')
        
        self.CreateTable2('temp_org_one_way')
        sqlcmd = """
                insert into temp_org_one_way (
                    linkno, dir, day, shour,
                    ehour, sdate, edate, cartype
                    ) 
                select link_id as linkno,
                       (case 
                             when a.snodeno = b.snodeno then 2 --positive direction
                             else 3 --negative direction
                        end)::smallint as dir, 
                        day, shour, ehour, sdate, edate, cartype
                from "org_one-way" a
                left join (
                    select c.meshcode, c.linkno, d.link_id, snodeno, enodeno, oneway
                    from org_road c
                    left join temp_link_mapping d
                        on c.meshcode = d.meshcode and c.linkno = d.linkno
                    where oneway != 0
                ) b
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.CreateIndex2('temp_org_one_way_day_shour_ehour_sdate_edate_cartype_idx')
        
        self.CreateFunction2('mid_convert_regulation_oneway')
        self.pg.callproc('mid_convert_regulation_oneway')
        self.pg.commit2()
        
        self.log.info('convert regulation of oneway end.')
    
    def __convert_regulation_linkrow(self):
        self.log.info('convert regulation of linkrow...')
        
        self.CreateIndex2('org_not_in_meshcode_fromlinkno_idx')
        self.CreateIndex2('org_not_in_meshcode_tolinkno_idx')
        self.CreateIndex2('org_not_in_meshcode_snodeno_idx')
        self.CreateIndex2('org_not_in_meshcode_tnodeno_idx')
        self.CreateIndex2('org_not_in_meshcode_enodeno_idx')
        self.CreateIndex2('temp_link_mapping_idx')
        self.CreateIndex2('temp_node_mapping_idx')
        
        self.CreateTable2('temp_org_not_in')
        sqlcmd = """
                insert into temp_org_not_in (
                    fromlinkno, tolinkno, snodeno, tnodeno, enodeno, day, shour,
                    ehour, sdate, edate, cartype, carwait, carthaba, carthigh
                    ) 
                select b.link_id as fromlinkno,
                       c.link_id as tolinkno,
                       d.node_id as snodeno,
                       e.node_id as tnodeno,
                       f.node_id as enodeno,
                       day, shour, ehour, sdate, edate, cartype, carwait, carthaba, carthigh
                from "org_not-in" a
                left join temp_link_mapping b
                    on a.meshcode = b.meshcode and a.fromlinkno = b.linkno
                left join temp_link_mapping c
                    on a.meshcode = c.meshcode and a.tolinkno = c.linkno
                left join temp_node_mapping d
                    on a.meshcode = d.meshcode and a.snodeno = d.nodeno
                left join temp_node_mapping e
                    on a.meshcode = e.meshcode and a.tnodeno = e.nodeno
                left join temp_node_mapping f
                    on a.meshcode = f.meshcode and a.enodeno = f.nodeno
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.CreateIndex2('temp_org_not_in_day_shour_ehour_sdate_edate_cartype_idx')
        
        self.CreateFunction2('mid_convert_regulation_linkrow')
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('convert regulation of linkrow end.')
    
    def __make_linklist_for_linkdir(self):
        self.log.info('Begin make linklist for linkdir...')
        
        self.CreateTable2('temp_link_regulation_permit_traffic')
        
        self.log.info('End make linklist for linkdir.')
        
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0