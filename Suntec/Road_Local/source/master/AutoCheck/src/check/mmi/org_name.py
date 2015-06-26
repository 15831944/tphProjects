# -*- coding: UTF8 -*-
'''
Created on 2015-4-14

@author: zhaojie
'''

import platform.TestCase

class CCheckphonemeNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_phoneme;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CCheckphoneme_idvalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select id
                    from org_phoneme
                    where id is null
                    
                    union
                    
                    select a.id
                    from org_phoneme as a
                    left join org_city_nw_gc_polyline as b
                    on a.id = b.id and a."table" = 'City_nw_gc'
                    where b.id is null
                )temp;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckphoneme_tablevalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_phoneme
                where "table" is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckphoneme_typevalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_phoneme
                where "type" is null or type <> 'MN';
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckphoneme_name_englishvalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_phoneme
                where name_english is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckphoneme_name_nuancevalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_phoneme
                where name_nuance is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckregional_dataNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_regional_data;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CCheckregional_data_idvalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select id
                    from org_regional_data
                    where id is null
                    
                    union
                    
                    select a.id
                    from org_regional_data as a
                    left join org_city_nw_gc_polyline as b
                    on a.id = b.id and a."table" = 'City_nw_gc'
                    where b.id is null
                )temp;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckregional_data_tablevalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_regional_data
                where "table" is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckregional_data_typevalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_regional_data
                where "type" is null or type <> 'MN';
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckregional_data_name_englishvalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_regional_data
                where name_english is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckregional_data_name_regionalvalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_regional_data
                where name_regional is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)

class CCheckregional_data_regional_lang_typevalue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_regional_data
                where regional_lang_type is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)  
  