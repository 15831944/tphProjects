# -*- coding: utf8 -*-
'''
Created on 2014-3-13

@author: liuxinxing
'''
from base import comp_base

class comp_regulation_mmi(comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Regulation')
        
    def _Do(self):
        self.__convert_condition_table()
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        self.__convert_regulation_oneway_link()
        self.__convert_regulation_linkrow()

        self.__make_linklist_for_linkdir()

        self.__delete_dummy_regulation()
    
        return 0
    
    def __convert_condition_table(self):
        self.log.info('Begin convert condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
        
    def __convert_regulation_oneway_link(self):
        self.log.info('Begin convert regulation for oneway link...')
        
        self.CreateFunction2('mid_convert_regulation_oneway_link')
        
        self.pg.callproc('mid_convert_regulation_oneway_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for oneway link.')
    
    def __convert_regulation_linkrow(self):
        self.log.info('Begin convert regulation for linkrow...')
        
        self.CreateFunction2('mid_get_connect_junction')
        self.CreateFunction2('mid_convert_regulation_linkrow')
        
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('End convert regulation for linkrow.')
    
    def __make_linklist_for_linkdir(self):
        self.log.info('Begin make linklist for linkdir...')
        
        self.CreateTable2('temp_link_regulation_forbid_traffic')
        self.CreateIndex2('temp_link_regulation_forbid_traffic_link_id_idx')
        self.pg.commit2()
        
        self.CreateTable2('temp_link_regulation_permit_traffic')
        self.CreateIndex2('temp_link_regulation_permit_traffic_link_id_idx')
        self.pg.commit2()
        
        self.CreateTable2('temp_link_regulation_traffic')
        self.CreateIndex2('temp_link_regulation_traffic_link_id_idx')
        self.pg.commit2()
        
        self.log.info('End make linklist for linkdir.')
        
    def __delete_dummy_regulation(self):
        self.log.info('Begin deleting dummy regulation...')
        
        self.CreateIndex2('regulation_relation_tbl_regulation_id_idx')
        self.CreateIndex2('regulation_relation_tbl_gid_idx')
        
        # backup
        sqlcmd = """
                drop table if exists regulation_relation_tbl_bak_dummy;
                create table regulation_relation_tbl_bak_dummy
                as 
                select * from regulation_relation_tbl;
                
                drop table if exists regulation_item_tbl_bak_dummy;
                create table regulation_item_tbl_bak_dummy
                as 
                select * from regulation_item_tbl;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_relation_tbl, reason1: dependent regulation
        sqlcmd = """
                delete from regulation_relation_tbl as x
                using
                (
                    select b.gid, b.regulation_id, b.condtype, b.cond_id
                    from regulation_relation_tbl as a
                    inner join regulation_relation_tbl as b
                    on  (a.condtype = 4 and b.condtype = 1)
                        and 
                        (a.inlinkid in (b.inlinkid, b.outlinkid))
                        and 
                        (a.cond_id is not distinct from b.cond_id)
                )as y
                where x.gid = y.gid
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_relation_tbl, reason2: impossible regulation
        sqlcmd = """
                delete from regulation_relation_tbl
                where condtype in (2,3,4,10) and cond_id is null;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_relation_tbl, reason3: unwalkable regulation
        self.CreateFunction2('mid_is_linkrow_continuable')
        sqlcmd = """
                delete from regulation_relation_tbl as x
                using
                (
                    select regulation_id
                    from
                    (
                        select regulation_id, array_agg(linkid) as link_array
                        from
                        (
                            select regulation_id, seq_num, linkid
                            from regulation_item_tbl
                            where seq_num != 2
                            order by regulation_id, seq_num
                        )as t1
                        group by regulation_id having count(*) > 1
                    )as t2
                    where not mid_is_linkrow_continuable(link_array)
                )as y
                where x.regulation_id = y.regulation_id and x.condtype = 1;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_item_tbl
        sqlcmd = """
                delete from regulation_item_tbl as x
                using
                (
                    select distinct a.regulation_id
                    from
                    (
                        select distinct regulation_id
                        from regulation_item_tbl
                    )as a
                    left join regulation_relation_tbl as b
                    on a.regulation_id = b.regulation_id
                    where b.regulation_id is null
                )as y
                where x.regulation_id = y.regulation_id
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('End deleting dummy regulation.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
    