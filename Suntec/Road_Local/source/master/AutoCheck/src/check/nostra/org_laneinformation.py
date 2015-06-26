# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: zym
'''
import platform.TestCase

        
class CCheckLaneNum(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                SELECT count(*)
                FROM org_laneinformation;
                """
        
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] > 0:
                    return True
            return False          

                    
class CCheckFirstLinkExist(platform.TestCase.CTestCase):
    def _do(self):
 
            sqlcmd = """
                SELECT count(*)
                FROM org_laneinformation 
                where arcin is null;
                """
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
                    
class CCheckLinksInLinkTab(platform.TestCase.CTestCase):  
    def _do(self):

        sqlcmd = """
                SELECT arcin, arcout1,arcout2, arcout3
                FROM org_laneinformation;
                """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            rowList = self.pg.fetchall()
            lenth = len(rowList)
            if lenth == 0:
                return 0
            else:
                for row in rowList:
                    for element in row:
                        sqlcmd = '''
                        SELECT count(*)
                        FROM org_l_tran 
                        where routeid = %s;
                        '''
                        if element != 0:
                            self.pg.execute(sqlcmd, (element,))
                            count = self.pg.fetchone()
                            if count == 0:
                                return 0
        return 1

class CCheckLaneNoCmp(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                SELECT *
                FROM org_laneinformation 
                where lane < no;
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckInLinkCmpOutLink(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                SELECT *
                FROM org_laneinformation
                where (arcin = arcout1 or arcout1 = arcout2 or arcin = arcout2 
                or arcin = arcout3 or arcout1 = arcout3 or arcout2 = arcout3) 
                and 0 not in (arcin,arcout1,arcout2,arcout3);
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckFirstOutLinkCheck(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                SELECT *
                FROM org_laneinformation
                where arcout1 in (0,null)         
                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0
