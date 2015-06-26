'''
Created on 2012-3-20

@author: zhangliang
'''

import os
import sys

import common.log
import common.common_func
import subproj.proj_factory

def main():
    common.log.common_log.instance().init()
    try:
        #common.log.common_log.instance().logger('System').info('org2middle start...')
        proj_name = common.common_func.GetProjName()
        proj_object = subproj.proj_factory.proj_factory.instance().createProject(proj_name)
        proj_object.ConstructComponent()
        proj_object.Start2DealwithOriginalData()
        proj_object.Start2CheckMiddleData()
        #common.log.common_log.instance().logger('System').info('org2middle end.')
    except:
        common.log.common_log.instance().end()
        common.log.common_log.instance().logger('System').exception('error happened in org2middle...')
        common.common_func.proj_exit(1)
    
    common.log.common_log.instance().end()
    common.common_func.proj_exit(0)

def setCurrentDir():
    reload(sys)
    sys.setdefaultencoding('utf8')
    execfilepath = os.path.abspath(sys.argv[0])
    execdirpath = execfilepath[:execfilepath.rfind(os.path.sep)]
    os.chdir(execdirpath)

if __name__ == '__main__':
    setCurrentDir()
    main()
