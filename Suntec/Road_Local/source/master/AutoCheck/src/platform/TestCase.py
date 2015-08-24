# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: liuxinxing
'''

import threading
import common.Logger
import common.database

class CTestCase(threading.Thread):
    def __init__(self, suite, caseinfo):
        threading.Thread.__init__(self)
        self.objSuite = suite
        self.caseinfo = caseinfo
        self.nCaseID = int(caseinfo.getCaseID())
        try:
            self.nCaseID_unique = int(caseinfo.getCaseID_unique())
        except:
            self.nCaseID_unique = self.nCaseID
        self.dependlist = caseinfo.getDependCaseIDList()
        if self.dependlist:
            self.dependlist = [int(caseid) for caseid in str(self.dependlist).split(";")]
        if self.dependlist:
            self.state = "BLOCK"
        else:
            self.state = "ACTIVE"
        strLoggerName = "Case %s-%s" % (self.objSuite.getSuiteName(), str(self.nCaseID))
        self.logger = common.Logger.CLogger.instance().logger(strLoggerName)
        self.pg = common.database.CDB()
    
    def isActive(self):
        if self.state == "BLOCK":
            for caseID in self.dependlist:
                objCase = self.objSuite.objCaseManager.findCase(caseID)
                if objCase is not None and not objCase.isOK():
                    break
            else:
                self.state = "ACTIVE"
        
        return self.state == "ACTIVE"
    
    def isOK(self):
        return self.state == "OK"
    
    def isFAIL(self):
        return self.state == "FAIL"
        
    def isRunning(self):
        return self.state == "RUNNING"
    
    def do(self):
        self.logger.info("Case start...")
        try:
            self.pg.connect()
            if self._do():
                self.state = "OK"
                self.logger.info("Case OK.")
            else:
                self.state = "FAIL"
                self.logger.error("Case FAIL.")
        except:
            self.state = "FAIL"
            self.logger.exception("Case Fail...")
            self.pg.rollback()
        finally:
            self.pg.close()
        self.logger.info("Case end.")
    
    def _do(self):
        return True
    
    def getCaseID(self):
        return self.nCaseID
    
    def getCaseID_unique(self):
        return self.nCaseID_unique
    
    def getResult(self):
        self.caseinfo.setCheckResult(self.state)
        return self.caseinfo
    
    def start(self):
        self.state = "RUNNING"
        threading.Thread.start(self)
    
    def run(self):
        try:
            self.do()
        finally:
            self.objSuite.objCaseManager.sem.release()
            del self.objSuite.objCaseManager.runningcaselist[self.getCaseID()]
    
    def join(self, timeout=None):
        if self.isRunning():
            threading.Thread.join(self, timeout)
    
    