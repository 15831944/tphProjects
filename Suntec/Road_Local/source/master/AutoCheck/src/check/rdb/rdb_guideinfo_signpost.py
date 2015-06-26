# -*- coding: UTF8 -*-
'''
Created on 2012-12-6

@author: hongchenzai
检查方面看板用的类
'''
import platform.TestCase

class CCheckSignPost(platform.TestCase.CTestCase):
    def _isNone(self):
        sqlcmd = """
                SELECT count(gid)
                  FROM rdb_guideinfo_signpost;
            """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        # 判断版本
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        return True
    
    def _isJP(self):
        '''pattern_id(日本版和中国版不同， 日本是纯数字的PatternNO，其他版本都是图片ID)。'''
        sqlcmd = """
                SELECT count(gid)
                  FROM signpost_tbl
                  where  patternno not SIMILAR TO '%[A-Z]%' and length(patternno) <= 1;
                """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        # 判断版本
        if row:
            if row[0] != 0:   
                return True
            else:
                return False
        return False
    
    def _isSignAsReal(self):
        '''是不是sign as real，如果是，arrow_id 为-1'''
        sqlcmd = """
                SELECT count(gid)
                FROM rdb_guideinfo_signpost
                where arrow_id = -1;
                """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:   
                return True
            else:
                return False
        return False
    
class CCheckPatternIDFKey(CCheckSignPost):
    def _do(self):
        if self._isNone() == True: return True  # 没有记录
        # 判断版本
        if self._isJP() == True:# 日本版 
            sqlcmd = """
                    ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT 
                      if exists check_pattern_id;
                    ALTER TABLE rdb_guideinfo_signpost
                      ADD CONSTRAINT check_pattern_id CHECK (pattern_id = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7]));
                    """
        else: # 非日本版 
                # 创建外键
                sqlcmd = """
                    ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT 
                      if exists rdb_guideinfo_signpost_pattern_id_fkey;
                    ALTER TABLE rdb_guideinfo_signpost
                      ADD CONSTRAINT rdb_guideinfo_signpost_pattern_id_fkey FOREIGN KEY (pattern_id)
                      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            self.pg.commit()
            return True
        return False
    
class CCheckArrowIDFKey(CCheckSignPost):
    def _do(self):
        if self._isNone() == True: return True  # 没有记录
        # 判断版本
        if self._isJP() == True:# 日本版 
            sqlcmd = """
                    ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT 
                      if exists check_arrow_id;
                    ALTER TABLE rdb_guideinfo_signpost
                      ADD CONSTRAINT check_arrow_id CHECK (arrow_id = ANY (ARRAY[(-1), 0, 1, 2]));
                    """
        else:     # 非日本版
            #sign as real
            if self._isSignAsReal() == True:
                sqlcmd = """
                        ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT 
                          if exists check_arrow_id;
                        ALTER TABLE rdb_guideinfo_signpost
                        ADD CONSTRAINT check_arrow_id CHECK (arrow_id = -1);
                        """
            else:
                # 创建外键
                sqlcmd = """
                        ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT 
                            if exists rdb_guideinfo_signpost_arrow_id_fkey;
                        ALTER TABLE rdb_guideinfo_signpost
                            ADD CONSTRAINT rdb_guideinfo_signpost_arrow_id_fkey FOREIGN KEY (arrow_id)
                                REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                                ON UPDATE NO ACTION ON DELETE NO ACTION;
                    """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            self.pg.commit()
            return True
        return False
    
class CCheckPatternID(CCheckSignPost):
    '''pattern_id(日本版和中国版不同， 日本是纯数字的PatternNO，其他版本都是图片ID)。'''
    def _do(self):
        if self._isNone() == True: return True  # 没有记录
        # 判断版本
        if self._isJP() == True:# 日本版 
            sqlcmd = """
                    SELECT count(a.gid)
                      FROM signpost_tbl as a
                      LEFT JOIN rdb_guideinfo_signpost as b
                      ON a.gid = b.gid
                      where a.patternno::integer <> b.pattern_id;
                    """
        else:            # 非日本版 
            sqlcmd = """
                    SELECT count(a.gid)
                      FROM signpost_tbl as a
                      LEFT JOIN rdb_guideinfo_signpost as b
                      ON a.gid = b.gid
                      left join rdb_guideinfo_pic_blob_bytea as c
                      on b.pattern_id = c.gid
                      where lower(a.patternno) <> lower(c.image_id);
                """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:   
                return False
            else:
                return True
        return False
    
class CCheckArrowID(CCheckSignPost):
    '''Arrow_id(日本版和中国版不同)。'''
    def _do(self):
        if self._isNone() == True: return True  # 没有记录
        # 判断版本
        if self._isJP() == True:# 日本版 
            sqlcmd = """
                    SELECT count(a.gid)
                      FROM signpost_tbl as a
                      LEFT JOIN rdb_guideinfo_signpost as b
                      ON a.gid = b.gid
                      where a.arrowno::integer <> b.arrow_id;
                    """
        else:     # 非日本版 
            sqlcmd = """
                    SELECT count(a.gid)
                      FROM signpost_tbl as a
                      LEFT JOIN rdb_guideinfo_signpost as b
                      ON a.gid = b.gid
                      left join rdb_guideinfo_pic_blob_bytea as c
                      on b.arrow_id = c.gid
                      where lower(a.arrowno) <> lower(c.image_id);
                """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0: 
                return False
            else:
                return True
        return False
  
class CCheckNameID(platform.TestCase.CTestCase):
    '''name_id'''
    def _do(self):
        # 取得name_id 不为-1的条目
        sqlcmd = """
                SELECT count(gid)
                  FROM rdb_guideinfo_signpost
                  where name_id <> - 1;
                """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] == 0:
                self.logger.warning('All name_id are equal to: -1')
                return True
        
        # 取得name_id不在新字典条
        sqlcmd = """
                SELECT count(*)
                  from signpost_tbl as a
                  left join rdb_guideinfo_signpost as b
                  on a.gid = b.gid
                  left join temp_name_dictionary AS c
                  ON a.name_id = new_name_id
                  where old_name_id <> b.name_id;
                """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
        return True
  
import rdb_common_check
class CCheckPassLinkCount_SP(rdb_common_check.CCheckPassLinkCount):
    def __init__(self, suite, caseinfo):
        rdb_common_check.CCheckPassLinkCount.__init__(self, suite, caseinfo, 'signpost_tbl', 'passlink_cnt')
        pass
    
    
