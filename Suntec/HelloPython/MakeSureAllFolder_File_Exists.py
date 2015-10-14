import os

if __name__ == '__main__':
    filePath = r"C:\Users\hexin\Desktop\20150813\idata_list.txt"
    with open(filePath, "r") as iFStream:
        configFileList = iFStream.readlines()
        for oneConfigFile in configFileList:
            if os.path.isdir(oneConfigFile) == True:
                print "Find  dir: %s" % oneConfigFile
            elif os.path.isfile(oneConfigFile) == True:
                print "Find file: %s" % oneConfigFile
            else:
                print "Can't find: %s" % oneConfigFile
            
    pass