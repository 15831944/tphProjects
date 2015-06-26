# -*- coding: UTF8 -*-
'''
Created on 2015-4-14

@author: zhaojie
'''

import platform.TestCase

class CChecksignpost1Number(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_1
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CChecksignpost2Number(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_2
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CChecksignpost1_idvalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_1
                where sign_id is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)

class CChecksignpost1_seqnrvalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select sign_id
                    from org_signpost_table_1
                    where seqnr is null or seqnr < 1
                    
                    union
    
                    select sign_id
                    from
                    (
                        select sign_id, array_agg(seqnr) as seqnr_array, count(*) as num
                        from
                        ( 
                            select sign_id, seqnr
                            from org_signpost_table_1                
                            order by sign_id, seqnr
                        )temp
                        group by sign_id
                    )temp1
                    where seqnr_array[1] <> 1 or seqnr_array[num] <> num
                )a;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        return (reg_number_count == 0)
    
class CChecksignpost1_edge_idvalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select sign_id
                    from org_signpost_table_1
                    where edge_id is null
                    
                    union    

                    select sign_id
                    from org_signpost_table_1 as a
                    left join org_city_nw_gc_polyline as b
                    on a.edge_id = b.id
                    where b.id is null
                )a;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost2_idvalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_2
                where sign_id is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost2_rowvalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_2
                where "row" is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost2_columnvalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_2
                where "column" is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost2_itemtypevalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_2
                where itemtype is null or itemtype not in (1,2,3,4,5,6,7);
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)

class CChecksignpost2_iteminfovalues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_signpost_table_2
                where iteminfo is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost2_signpost1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select a.sign_id
                    from org_signpost_table_1 as a
                    left join org_signpost_table_2 as b
                    on a.sign_id = b.sign_id
                    where b.sign_id is null
                    
                    union
                    
                    select a.sign_id
                    from org_signpost_table_2 as a
                    left join org_signpost_table_1 as b
                    on a.sign_id = b.sign_id
                    where b.sign_id is null
                )temp;               
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    