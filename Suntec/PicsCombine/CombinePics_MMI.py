# -*- coding: cp936 -*-
import os
import psycopg2
import struct

PIC_TYPE = {'Day':'1', 'Night':'2'}

#==============================================================================
# comp_picture
#==============================================================================
class comp_picture(object):
    ''' picture entity'''
    def __init__(self):
        self.gid = 0
        self.dayName = None
        self.nightName = None
        self.dayArrow = None
        self.nightArrow = None
        self.commonPatter = None
        self.commonArrow = None

    def getGid(self):
        return self.gid

    def setGid(self, gid):
        self.gid = gid

    def getDayName(self):
        return self.dayName

    def setDayName(self, dayName):
        self.dayName = dayName

    def getNightName(self):
        return self.nightName

    def setNightName(self, nightName):
        self.nightName = nightName

    def getNightArrow(self):
        return self.nightArrow

    def setNightArrow(self, nightArrow):
        self.nightArrow = nightArrow

    def getDayArrow(self):
        return self.dayArrow

    def setDayArrow(self, dayArrow):
        self.dayArrow = dayArrow

    def getCommonPatter(self):
        return self.commonPatter

    def setCommonPatter(self, commonPatter):
        self.commonPatter = commonPatter

    def getCommonArrow(self):
        return self.commonArrow

    def setCommonArrow(self, commonArrow):
        self.commonArrow = commonArrow


#==============================================================================
# comp_picture
#==============================================================================
class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.184'
                        dbname='17cy_IND_MMI_CI'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()

    def selectData(self):
        self.pgcur2.execute('''SELECT distinct patternno, arrowno FROM spotguide_tbl;''')
        rows = self.pgcur2.fetchall()
        pics = []
        i = 1
        for row in rows:
            patter = row[0]
            arrow = row[1]
            dayCode = PIC_TYPE.get('Day')
            nightCode = PIC_TYPE.get('Night')
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(patter + '.' + dayCode)
            pic.setNightName(patter + '.' + nightCode)
            pic.setDayArrow(arrow + '.' + dayCode)
            pic.setNightArrow(arrow + '.' + nightCode)
            pic.setCommonPatter(patter)
            pic.setCommonArrow(arrow)
            pics.append(pic)
            i = i + 1
        return pics

    def makeJunctionResultTable(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "input directory not exist! " + srcDir
            return
        else:
            if os.path.isdir(destDir) == False:
                print "destination directory not exist! " + destDir
                print "creating directory..."
                os.mkdir(destDir)
                
            pictures = self.selectData()
            i=1
            for pic in pictures:
                print "-%d----------------------------------------------------------------------------------------"% i
                i += 1
                # day and night illust
                destFile = os.path.join(destDir, pic.getCommonPatter() + '.dat')
                dayPicPath = os.path.join(srcDir, pic.getDayName() + ".png")
                if(os.path.exists(dayPicPath) == False):
                    print "day file not exist: " + dayPicPath
                    continue
                
                nightPicPath = os.path.join(srcDir, pic.getNightName() + ".png")
                if(os.path.exists(nightPicPath) == False):
                    print "night file not exist: " + nightPicPath
                    continue
                
                dayFis = open(dayPicPath, 'rb')
                nightFis = open(nightPicPath, 'rb')
                dayPicLen = os.path.getsize(dayPicPath)
                nightPicLen = os.path.getsize(nightPicPath)
                headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                           dayPicLen, 2, 22 + dayPicLen, \
                                           nightPicLen)
                resultBuffer = headerBuffer + dayFis.read() + nightFis.read()
                dayFis.close()
                nightFis.close()
                
                fos = open(destFile, 'wb')
                fos.write(resultBuffer)
                fos.close()
                print "    " + dayPicPath
                print "    " + nightPicPath
                print "        >>>>>>>>  " + destFile
                
                # ARROW PIC BUILD
                destArrowFile = os.path.join(destDir, pic.getCommonArrow() + '.dat')
                d_arrow_path = os.path.join(srcDir, pic.getDayArrow() + '.png')
                if(os.path.exists(dayPicPath) == False):
                    print "day arrow file not exist: " + d_arrow_path
                    continue
                
                n_arrow_path = os.path.join(srcDir, pic.getNightArrow() + '.png')
                if(os.path.exists(nightPicPath) == False):
                    print "night arrow file not exist: " + n_arrow_path
                    continue
                
                dayArrowFis = open(d_arrow_path, 'rb')
                nightArrowFis = open(n_arrow_path, 'rb')
                dayArrowLen = os.path.getsize(d_arrow_path)
                nightArrowLen = os.path.getsize(n_arrow_path)
                a_headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                           dayArrowLen, 2, 22 + dayArrowLen, \
                                           nightArrowLen)
                a_resultBuffer = a_headerBuffer + dayArrowFis.read() + nightArrowFis.read()
                dayArrowFis.close()
                nightArrowFis.close()
                
                a_fos = open(destArrowFile, 'wb')
                a_fos.write(a_resultBuffer)
                a_fos.close()
                print "    " + d_arrow_path
                print "    " + n_arrow_path
                print "        >>>>>>>>  " + destFile

if __name__ == '__main__':
    test = GeneratorPicBinary()
    test.makeJunctionResultTable("C:\\My\\work\\20150410_mmi_pic\\Pattern", "C:\\My\\work\\20150410_mmi_pic\\illust_pic")
    pass