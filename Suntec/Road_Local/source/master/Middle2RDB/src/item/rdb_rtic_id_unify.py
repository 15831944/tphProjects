# -*- coding: cp936 -*-
'''
Created on 2015-11-10

@author: zhangliang
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log
import common.rdb_common


class rdb_rtic_id_unify(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'rtic_id_unify')
        
    def Do(self):
        self.log.info("start to make rtic id fund...")
        self.__makeIDFund()
        self.log.info("end to make rtic id fund")
    
    def __makeIDFund(self):
        max_rtic_id = self.__get_max_rtic_id()
        current_db_version = common.rdb_common.GetPara('db1')
        dst_dbname = self.__analyzeDBParas(current_db_version).get('dbname')
        
        # judgment whether needs to operate rtic id
        if (common.rdb_common.GetPara('support_rtic_id_unify').lower() == 'true'):
#            if (False == self.is_exist_old_version_rtic_id_data()):
            sqlcmd = """
                drop sequence if exists rtic_id_fund_link_seq;
                
                create sequence rtic_id_fund_link_seq start %s;
                
                drop table if exists temp_all_rdb_link_info;
                create temp table temp_all_rdb_link_info
                as
                (
                    select link_id, link_id_t, rdb_split_tileid(link_id) as link_id_low_32, b.link_id_32, b.version
                    from rdb_link as a
                    left join rtic_id_fund_link as b
                    on a.link_id = b.link_id_64
                );
                
                insert into rtic_id_fund_link(link_id_64, link_id_32, version)
                (
                    select link_id, nextval('rtic_id_fund_link_seq'), 
                     '%s'
                    from
                    (
                        select link_id
                        from temp_all_rdb_link_info
                        where link_id_32 is null
                        order by link_id_t, link_id_low_32
                    ) as a
                );
            """ %(max_rtic_id+1, dst_dbname)
            
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            self.CreateIndex2('rtic_id_fund_link_idx')
            
            sqlcmd = """
                drop table if exists rdb_rtic_id_matching;
                
                create table rdb_rtic_id_matching
                as
                (
                    select a.link_id_64, a.link_id_32, case when b.rdb_link_id is not null then 1::smallint else 0::smallint end as link_attribute
                    from rtic_id_fund_link as a
                    left join 
                    (
                        select distinct rdb_link_id as rdb_link_id
                        from rdb_trf_org2rdb
                    ) as b
                    on a.link_id_64 = b.rdb_link_id
                );
            """
                
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            self.CreateIndex2('rdb_rtic_id_matching_link_id_64_idx')  
            
#    def is_exist_old_version_rtic_id_data(self):
#        sqlcmd = """
#                select count(*) 
#                from rtic_id_fund_link
#            """
#        self.pg.execute2(sqlcmd)
#        rows = self.pg.fetchone2()
#        if  rows[0] > 0:
#            return True
#        else:
#            return False
        
    def __get_max_rtic_id(self):
        sqlcmd = """
            select link_id_32
            from rtic_id_fund_link
            order by link_id_32 desc
            limit 1
        """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchone2()
        
        if rows is None:
            return 0;
        else:
            return rows[0];
    
    def __analyzeDBParas(self, db_para_string):
        paras = {}
        for one_para_string in db_para_string.split(' '):
            one_para = one_para_string.split('=')
            para_name = one_para[0]
            para_value = one_para[1].strip("'")
            paras[para_name] = para_value
        return paras
    
    