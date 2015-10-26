# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: liuxinxing
'''

import time
import threading
import common.Logger
import platform.TestSuite
import platform.CaseReadWriter
import common.ConfigReader
import os
import json
from TAP.tap_processor import *

class CTestCaseManager:
    __instance = None
    @staticmethod
    def instance():
        if CTestCaseManager.__instance is None:
            CTestCaseManager.__instance = CTestCaseManager()
        return CTestCaseManager.__instance
    
    def __init__(self):
        self.suitelist = []
        self.suitecontent = {}
        self.logger = common.Logger.CLogger.instance().logger("Case Manager")
        strCheckListPath = common.ConfigReader.CConfigReader.instance().getPara("checklist")
        self.strProjName = common.ConfigReader.CConfigReader.instance().getProjName()
        self.strCountryName = common.ConfigReader.CConfigReader.instance().getCountryName()
        self.objCaseReadWriter = platform.CaseReadWriter.CCaseReadWriter(strCheckListPath)
        self.sheet_name = list()
    
    def do(self):
        try:
            self.__loadCaseList()
            #self.__doAllCase()
            self.__doAllCase_parallel()
            self.__saveReport()
            self.__saveReportForJson()
            return self.__isAllCaseOK()
        except:
            self.logger.exception("error happened...")
            return 0
    
    def __loadCaseList(self):
        self.logger.info("Loading caselist begin...")
        
        #
#        objCaseInfo = platform.CaseReadWriter.CCaseInfo()
#        objCaseInfo.setCaseID(200001)
#        objCaseInfo.setCheckCode("check.rdb.rdb_link.CCheckLinkIDUnique")
#        objSuiteInfo = platform.CaseReadWriter.CSuiteInfo()
#        objSuiteInfo.setSuiteName("rdb_link")
#        objSuiteInfo.addCaseInfo(objCaseInfo)
#        allSuiteInfo = [objSuiteInfo]
        allSuiteInfo = self.objCaseReadWriter.getSuiteInfoList()
        
        for suiteinfo in allSuiteInfo:
            objSuite = platform.TestSuite.CTestSuite(self, suiteinfo)
            self.suitelist.append(objSuite.getSuiteName())
            self.suitecontent[objSuite.getSuiteName()] = objSuite
            objSuite.init()
        
        self.logger.info("Loading caselist end.")
    
    def __doAllCase(self):
        self.logger.info("Executing caselist begin...")
        while True:
            for suitename in self.suitelist:
                objSuite = self.suitecontent[suitename]
                objCase = objSuite.getActiveCase()
                if objCase is not None:
                    objCase.do()
                    break
            else:
                break
        self.logger.info("Executing caselist end.")
    
    def __doAllCase_parallel(self):
        self.sem = threading.Semaphore(4)
        self.runningcaselist = {}
        self.logger.info("Executing caselist begin...")
        while True:
            for suitename in self.suitelist:
                objSuite = self.suitecontent[suitename]
                objCase = objSuite.getActiveCase()
                if objCase is not None:
                    self.sem.acquire()
                    self.runningcaselist[objCase.getCaseID()] = 1
                    objCase.start()
                    break
            else:
                if len(self.runningcaselist) == 0:
                    break
                else:
                    time.sleep(1.0)
        self.logger.info("Executing caselist end.")
    def __saveReportForJson(self):

        base_path = common.ConfigReader.CConfigReader.instance().getPara('json_path')
        if not os.path.exists(base_path):
            os.makedirs(base_path)
        file_path = os.path.join(base_path, common.ConfigReader.CConfigReader.instance().getDBName())
        list_route = list()
        list_highway = list()
        list_guide = list()

        for suitename in self.suitelist:
            list_temp = list()
            objSuite = self.suitecontent[suitename]               
            caselist = objSuite.getCaseList()
            for objCase in caselist:
                if objCase.isOK():
                    list_temp.append([TAP_OK,suitename + '::' + objCase.get_combin_keys()])
                else:
                    list_temp.append([TAP_NG,suitename + '::' + objCase.get_combin_keys()])
            
            list_temp = objSuite.getNoRuncase()
            
            if suitename.find("rdb_guide") == 0 :
                list_guide.extend(list_temp)               
            elif suitename.find("rdb_highway") == 0 :
                list_highway.extend(list_temp)               
            else:
                list_route.extend(list_temp)
        #write file
        
        pg_ptocessor = TAP_Processor()
        pg_ptocessor.addResultList(list_route)
        pg_ptocessor.dump2file(file_path + "_route.tap")
        pg_ptocessor.clear()
        
        pg_ptocessor.addResultList(list_guide)
        pg_ptocessor.dump2file(file_path + "_guide.tap")
        pg_ptocessor.clear()
        
        pg_ptocessor.addResultList(list_highway)
        pg_ptocessor.dump2file(file_path + "_highway.tap")
        pg_ptocessor.clear()
              
        return 0      
        
    def __saveReport(self):
        allsuiteinfo = []
        for suitename in self.suitelist:
            objSuite = self.suitecontent[suitename]
            suiteinfo = objSuite.getCaseListReport()
            allsuiteinfo.append(suiteinfo)
        self.objCaseReadWriter.saveCaseListReport(allsuiteinfo)
        
        allSummaryInfo = self.objCaseReadWriter.getSummarySuiteInfo()
        allinfo = None
        for suite_summary_info in allSummaryInfo.getCaseInfoList():
            suitename = suite_summary_info.getSuiteName()
            if not self.suitecontent.has_key(suitename):
                allinfo = suite_summary_info
                allinfo.setCaseNum(0)
                allinfo.setAutoCaseNum(0)
                allinfo.setSucceedCaseNum(0)
                allinfo.setFailedCaseNum(0)
                allinfo.setBlockCaseNum(0)
        for suite_summary_info in allSummaryInfo.getCaseInfoList():
            suitename = suite_summary_info.getSuiteName()
            if self.suitecontent.has_key(suitename):
                objSuite = self.suitecontent[suitename]
                suite_summary_info = objSuite.getSummaryReport(suite_summary_info)
                allinfo.setCaseNum(allinfo.getCaseNum() + suite_summary_info.getCaseNum())
                allinfo.setAutoCaseNum(allinfo.getAutoCaseNum() + suite_summary_info.getAutoCaseNum())
                allinfo.setSucceedCaseNum(allinfo.getSucceedCaseNum() + suite_summary_info.getSucceedCaseNum())
                allinfo.setFailedCaseNum(allinfo.getFailedCaseNum() + suite_summary_info.getFailedCaseNum())
                allinfo.setBlockCaseNum(allinfo.getBlockCaseNum() + suite_summary_info.getBlockCaseNum())
        self.logger.info("AUTOCASE:%d, SUCCESS:%d, FAIL:%d, BLOCK:%d" % (allinfo.getAutoCaseNum(), \
                                                                         allinfo.getSucceedCaseNum(), \
                                                                         allinfo.getFailedCaseNum(), \
                                                                         allinfo.getBlockCaseNum()))
        self.objCaseReadWriter.saveSummaryReport(allSummaryInfo)
    
    def __isAllCaseOK(self):
        for suitename in self.suitelist:
            objSuite = self.suitecontent[suitename]
            caselist = objSuite.getCaseList()
            for objCase in caselist:
                if not objCase.isOK():
                    return False
        else:
            return True
    
    def findCase(self, nCaseID):
        for objSuite in self.suitecontent.values():
            objCase = objSuite.findCase(nCaseID)
            if objCase is not None:
                return objCase
    
    def getProj(self):
        return self.strProjName

    def getCountry(self):
        return self.strCountryName    