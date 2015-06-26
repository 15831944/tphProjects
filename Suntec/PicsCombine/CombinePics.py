#encoding=utf-8
#!/bin/python
import os
import sys
import CombinePics_Sensis
import CombinePics_Here

class ConsoleArgs(object):
    def __init__(self):
        self._vendor = ""
        self._srcdir = ""
        self._destdir = ""
        self._host = ""
        self._dbname = ""
        self._user = ""
        self._password = ""
        self._gjvsrcdir = ""
        self._gjvdestdir = ""
        self._ejvsrcdir = ""
        self._ejvdestdir = ""
        self._ejvcondition = ""
        self._birdsrcdir = ""
        self._birddestdir = ""
        self._ejvcondition = ""
        
def main():
    arglen = len(sys.argv)
    if(arglen <= 2):
        help()
        return
    else:
        conArgs = ConsoleArgs()
        for i in range(1, arglen):
            arg = sys.argv[i]
            if(arg == "/vendor"):
                conArgs._vendor = str(sys.argv[i+1])
            elif(arg == "/srcdir"):
                conArgs._srcdir = str(sys.argv[i+1])
            elif(arg == "/destdir"):
                conArgs._destdir = str(sys.argv[i+1])
            elif(arg == "/host"):
                conArgs._host = str(sys.argv[i+1])
            elif(arg == "/dbname"):
                conArgs._dbname = str(sys.argv[i+1])
            elif(arg == "/user"):
                conArgs._user = str(sys.argv[i+1])
            elif(arg == "/password"):
                conArgs._password = str(sys.argv[i+1])
            elif(arg == "/gjvsrcdir"):
                conArgs._gjvsrcdir = str(sys.argv[i+1])
            elif(arg == "/gjvdestdir"):
                conArgs._gjvdestdir = str(sys.argv[i+1])
            elif(arg == "/ejvsrcdir"):
                conArgs._ejvsrcdir = str(sys.argv[i+1])
            elif(arg == "/ejvdestdir"):
                conArgs._ejvdestdir = str(sys.argv[i+1]) 
            elif(arg == "/ejvcondition"):
                conArgs._ejvcondition = str(sys.argv[i+1])
            elif(arg == "/birdsrcdir"):
                conArgs._birdsrcdir = str(sys.argv[i+1]) 
            elif(arg == "/birddestdir"):
                conArgs._birddestdir = str(sys.argv[i+1])
            else:
                pass
    if(check_input_arguments(conArgs) == -1):
        return
    dic_vendor_func = {'here':handle_here_pics,'sensis':handle_sensis_pics}
    dic_vendor_func[conArgs._vendor](conArgs)

def handle_sensis_pics(consoleArgs):
    test = CombinePics_Sensis.GeneratorPicBinary_Sensis()
    test.combine_images(consoleArgs._srcdir, consoleArgs._destdir)

def handle_here_pics(consoleArgs):
    test = CombinePics_Here.GeneratorPicBinary_Here(consoleArgs._host, consoleArgs._dbname, 
                                               consoleArgs._user, consoleArgs._password)
    test.makeGJunctionResultTable(consoleArgs._gjvsrcdir, consoleArgs._gjvdestdir)
    pics = test.makeEJunctionResultTable(consoleArgs._ejvsrcdir, consoleArgs._ejvdestdir, '')
    test.makeBIRDJunctionResultTable(consoleArgs._birdsrcdir, consoleArgs._birddestdir, pics)
    return
    
def check_input_arguments(consoleArgs):
    print "_vendor: " + consoleArgs._vendor
    print "_srcdir: " + consoleArgs._srcdir
    print "_destdir: " + consoleArgs._destdir
    print "_host: " + consoleArgs._host
    print "_dbname: " + consoleArgs._dbname
    print "_user: " + consoleArgs._user
    print "_password: " + consoleArgs._password
    print "_gjvsrcdir: " + consoleArgs._gjvsrcdir
    print "_gjvdestdir: " + consoleArgs._gjvdestdir
    print "_ejvsrcdir: " + consoleArgs._ejvsrcdir
    print "_ejvdestdir: " + consoleArgs._ejvdestdir
    print "_ejvcondition" + consoleArgs._ejvcondition
    print "_birdsrcdir: " + consoleArgs._birdsrcdir
    print "_birddestdir: " + consoleArgs._birddestdir
    return 0        
    
def check_vendor_name(vendor):
    strVendors = ["here", "sensis"]
    if(strVendors.count(str(vendor).lower()) <= 0):
        print "vendor name not correct"
        return False
    return True
    
def check_directory(strdir):
    if(os.path.exists(strdir) == False):
        return False
    return True
    
def error1():
    print "invalid argument."
    my_help()
        
def my_help():
    print "/help         help"
    print "/vendor       set the vendor"
    print "/inputdir     set the input directory"
    print "/outputdir    set the output directory"
        
main()
