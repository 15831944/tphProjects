# -*- coding: UTF8 -*-
'''
Created on 2012-12-10

@author: hongchenzai
'''

import platform.TestCase

class CCheckNumber(platform.TestCase.CTestCase):
    '''检查[交叉点名称]的条目数。'''        
    def _do(self):
        
        srcNum = self._getSrcNumber()
        if srcNum == None: return False
        
        destNum = self._getDestNumber()
        if destNum == None: return False
        
        if srcNum == destNum:
            return True
        else:
            return False
        
    def _getSrcNumber(self):
        sqlcmd = """
                SELECT count(distinct(name_id))
                  FROM name_dictionary_tbl;
                """
                
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            return row[0]
        else:
            return None
        
    def _getDestNumber(self):
        # 取得所有的语言表
        sqlcmd = """
                SELECT l_talbe
                from (
                    SELECT distinct("language") as lang_id
                      FROM name_dictionary_tbl
                ) as a
                left join temp_language_tbl
                on lang_id = language_id;
                """
                
        self.pg.query(sqlcmd)
        language_table = []
        rows = self.pg.fetchall()
        if rows == None:
            return False
        for row in rows:
            language_table.append(row[0])
        
        
        # 统计所有语言表里name_id的数目
        sqlcmd = "SELECT count(distinct(name_id)) FROM ( "
        sqlcmd1 = "SELECT name_id FROM rdb_name_dictionary where name_id <> -1 "
        
        for i, tbl in enumerate(language_table):
            if i != 0:
                sqlcmd += ' UNION '
            sqlcmd += sqlcmd1.replace('rdb_name_dictionary', tbl)
        sqlcmd += ') as a;'
        
        self.pg.query(sqlcmd)
        language_table = []
        row = self.pg.fetchone()
        if row:
            return row[0]
        else:
            return None
        
class CCheckName(platform.TestCase.CTestCase):
    '''检查名称。'''
    
    def _do(self):
        sql_script = """
                    SELECT count(*)
                      FROM (
                        select name_id, name
                          from name_dictionary_tbl
                          where language = %s
                      ) AS a
                      left join temp_name_dictionary AS b
                      ON a.name_id = old_name_id
                      left join rdb_name_dictionary as c
                      ON b.new_name_id = c.name_id
                      where a.name <> c.name;
                    """
        for lang_tbl in self._getAllLanguageTbl():
            sqlcmd = sql_script.replace('rdb_name_dictionary', lang_tbl[1])
            self.pg.execute(sqlcmd, (lang_tbl[0],))
            row = self.pg.fetchone()
            if row:
                if row[0] != 0:
                    return False
                else: 
                    pass # continue
            else:
                return False
        
        return True
        
    def _getAllLanguageTbl(self):
        '''取得所有语言表。'''
        sqlcmd = '''
                SELECT language_id, l_talbe
                FROM (
                    SELECT distinct("language") as lang_id
                      FROM name_dictionary_tbl
                ) as a
                left join temp_language_tbl
                on lang_id = language_id;
                '''
        if self.pg.query(sqlcmd) == -1:
            return None
        rows = self.pg.fetchall()
        table_list = []
        for row in rows:
            table_list.append((row[0], row[1]))
        return table_list
        
     
        
        