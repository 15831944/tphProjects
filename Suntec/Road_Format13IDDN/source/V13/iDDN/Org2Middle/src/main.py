'''
Created on 2012-3-20

@author: zhangliang
'''
import common

import sys
reload(sys)
sys.setdefaultencoding('utf8')

# proj_name_list = ('proj_chn', 'proj_jpn', 'proj_abroad')

def main():
    common.common_log.instance().init()
    try:
        import subproj
        proj_name = common.GetProjName()
        proj_object = subproj.proj_factory().instance(proj_name)
        proj_object.ConstructComponent()
        proj_object.Start2DealwithOriginalData()
        proj_object.Start2CheckMiddleData()
    except:
        common.common_log.instance().end()
        common.common_log.instance().logger('System').exception('error happened in org2middle...')
        common.proj_exit(1)
    
    common.common_log.instance().end()
    common.proj_exit(0)

if __name__ == '__main__':
    main()
