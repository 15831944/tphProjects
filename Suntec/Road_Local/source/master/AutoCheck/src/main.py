'''
Created on 2012-11-29

@author: hongchenzai
'''

import os
import sys

def setCurrentDir():
    reload(sys)
    sys.setdefaultencoding('utf8')
    execfilepath = os.path.abspath(sys.argv[0])
    execdirpath = execfilepath[:execfilepath.rfind(os.path.sep)]
    os.chdir(execdirpath)

if __name__ == "__main__":
    setCurrentDir()
    import platform.App
    nReturnCode = platform.App.CApp.instance().run()
    exit(nReturnCode)
