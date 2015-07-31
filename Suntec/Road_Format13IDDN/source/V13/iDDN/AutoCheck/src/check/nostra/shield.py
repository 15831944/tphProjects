# -*- coding: UTF8 -*-
'''
Created on 2013-12-26

@author: minghongtu
'''
import platform.TestCase
import re
#key的值
class CCheckShieldKey(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        key=["lang", "left_right", "tts_type", "type", "val"]
        
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        k=road_name_dict.keys()
                        k.sort()
                        if k!= key:
                            return False
            rows = self.pg.fetchmany(200000)
        
        return True

#key的顺序
class CCheckShieldKeyOrder(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        key_order='lang:.*left_right:.*tts_type:.*type:.*val:.*'
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                row = row[0]
                sign=['[',']','"',' ']
                for s in sign:
                    row = row.replace(s,'')
                row = row.split('},')
                for road_name_dict in row:
                    m = re.search(key_order, road_name_dict)
                    if m is None:
                        return False
            rows = self.pg.fetchmany(200000)
        
        return True
    

#type的有效值
class CCheckShieldType(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        type = ["shield"]
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['type'] not in type:
                            return False
            rows = self.pg.fetchmany(200000)
        
        return True

#tts_type有效值
class CCheckShieldTTSType(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        tts_type = ['not_tts']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['tts_type'] not in tts_type:
                            return False
            rows = self.pg.fetchmany(200000)
        
        return True

#left_right有效值    
class CCheckShieldLeftRight(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        left_right = ['left_right']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['left_right'] not in left_right:
                            return False
            rows = self.pg.fetchmany(200000)
        
        return True

#lang有效值    
class CCheckShieldLanguage(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        language = ['THA', 'THE']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['lang'] not in language:
                            return False
            rows = self.pg.fetchmany(200000)
        
        return True  

#一个名称的不同语言之间的顺序
class CCheckShieldLangOrder(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        order_dict = {'THAnot_tts':1, 'THAtext':2, 'THEnot_tts':3}
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    order = []
                    for road_name_dict in road_name:
                        order.append(order_dict[road_name_dict['lang']+road_name_dict['tts_type']])
                    if len(order) == 1 :
                        pass                    
                    else:
                        count = 0
                        while count < len(order)-1:
                            if order[count] > order[count+1] :
                                return False
                            count += 1                    
            rows = self.pg.fetchmany(200000)
        return True    


class CCheckShieldIDValid(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                 select distinct road_number from link_tbl where road_number is not null
                 
                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        shield_id = ['4001', '4002', '4003', '4004']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['val'][0:4] not in shield_id:
                            return False
            rows = self.pg.fetchmany(200000)
        
        return True
