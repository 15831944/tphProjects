'''
Created on 2012-11-29

@author: liuxinxing
'''

import sys
import common.ConfigReader
import common.Logger
import platform.TestCaseManager

class CApp:
    __instance = None
    @staticmethod
    def instance():
        if CApp.__instance is None:
            CApp.__instance = CApp()
        return CApp.__instance
    
    def __init__(self):
        self.strConfigPath = "config.ini"
        if len(sys.argv) > 1:
            self.strConfigPath = sys.argv[1]
    
    def run(self):
        try:
            common.ConfigReader.CConfigReader.instance().load(self.strConfigPath)
            common.Logger.CLogger.instance().init()
            common.Logger.CLogger.instance().logger("Autocheck").info("autocheck begin...")
            bAllOK = platform.TestCaseManager.CTestCaseManager.instance().do()
            common.Logger.CLogger.instance().logger("Autocheck").info("autocheck end.")
            common.Logger.CLogger.instance().end()
            return (bAllOK == True)
        except:
            common.Logger.CLogger.instance().logger("Autocheck").exception("error happened...")
            return 1
