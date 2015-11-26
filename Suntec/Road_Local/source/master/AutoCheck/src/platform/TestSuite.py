# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: liuxinxing
'''

import common.Logger
import platform.TestCase
from TAP.tap_processor import *

class CTestSuite:
    def __init__(self, casemanager, suiteinfo):
        self.objCaseManager = casemanager
        self.suiteinfo = suiteinfo
        self.strSuiteName = suiteinfo.getSuiteName()
        self.projName = self.objCaseManager.getProj()
        self.countryName = self.objCaseManager.getCountry()
        self.cases = {}
        self.no_run_cases = []
        self.logger = common.Logger.CLogger.instance().logger("Suite %s"%self.strSuiteName)
    
    def init(self):
        for caseinfo in self.suiteinfo.getCaseInfoList():
#            for key in caseinfo.keys():
#                self.logger.info(key)
#                self.logger.info(caseinfo.get(key))
            if caseinfo.isAuto() and caseinfo.isProj(self.projName.upper()) \
            and caseinfo.isCountry(self.countryName.upper()):
                caseid = int(caseinfo.getCaseID())
                check_logic_class = caseinfo.getCheckCode()
                
                if self.objCaseManager.findCase(caseid) is not None:
                    self.logger.error("repeated caseid, (id=%d, logic=%s)" % (caseid, check_logic_class))
                    continue
                
                try:
                    check_logic_package = check_logic_class[:check_logic_class.rfind(".")]
                    exec("import " + check_logic_package)
                    objCase = eval(check_logic_class + "(self, caseinfo)")
                    self.cases[caseid] = objCase
                except:
                    self.logger.exception("invalid autocheck caseinfo, (id=%d, logic=%s)" % (caseid, check_logic_class))
            else:
                if caseinfo.getCheckCode():                  
                    self.no_run_cases.append([TAP_SKIP, self.strSuiteName + "::" + caseinfo.get_combin_keys()])
    
    def getActiveCase(self):
        for caseID in self.cases.keys():
            objCase = self.cases[caseID]
            if objCase.isActive():
                return objCase
    
    def findCase(self, nCaseID):
        if self.cases.has_key(nCaseID):
            return self.cases[nCaseID]
    
    def getSuiteName(self):
        return self.strSuiteName
    
    def getCaseList(self):
        return self.cases.values()
    
    def getNoRuncase(self):
        return self.no_run_cases
    
    def getCaseListReport(self):
        for caseID in self.cases.keys():
            objCase = self.cases[caseID]
            objCase.getResult()
        return self.suiteinfo
    
    def getSummaryReport(self, summaryinfo):
        nAll = 0
        nAuto = 0
        nOK = 0
        nFAIL = 0
        nBLOCK = 0
        for caseinfo in self.suiteinfo.getCaseInfoList():
            if caseinfo.isProj(self.projName.upper()) \
            and caseinfo.isCountry(self.countryName.upper()):
                nAll += 1
                if caseinfo.isAuto():
                    nAuto += 1
                    if not self.cases.has_key(caseinfo.getCaseID()):
                        nFAIL += 1
                    else:
                        objCase = self.cases[caseinfo.getCaseID()]
                        if objCase.isOK():
                            nOK += 1
                        elif objCase.isFAIL():
                            nFAIL += 1
                        else:
                            nBLOCK += 1
        summaryinfo.setCaseNum(nAll)
        summaryinfo.setAutoCaseNum(nAuto)
        summaryinfo.setSucceedCaseNum(nOK)
        summaryinfo.setFailedCaseNum(nFAIL)
        summaryinfo.setBlockCaseNum(nBLOCK)
        return summaryinfo
