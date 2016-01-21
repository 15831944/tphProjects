# -*- coding: UTF8 -*-    

from check import check_route
from common import log
import os
import sys

def setCurrentDir():
    reload(sys)
    sys.setdefaultencoding('utf8')
    execfilepath = os.path.abspath(sys.argv[0])
    execdirpath = execfilepath[:execfilepath.rfind(os.path.sep)]
    os.chdir(execdirpath)
     
def main():
    log_pg = log.common_log.instance()
    log_pg.init()
    try:
        log_pg.logger("main").info("project is start!")
        start_file = sys.argv[1]
        end_file = ''
        pg_server = check_route.check_route_com(start_file, end_file)
        if pg_server.do_task() == -1:
            exit(0)

    except Exception,e:
        log_pg.logger("main").exception("error happened in main...")
        print Exception,":",e
        exit(0)

if __name__ == "__main__":
    setCurrentDir()
    main()
    
     