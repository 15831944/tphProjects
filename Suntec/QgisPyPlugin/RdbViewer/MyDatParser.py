# -*- coding: utf-8 -*-
from os.path import getsize
import struct
import MyGetBinType

###########dat file type###########
DatBinType_Invalid = -1
DatBinType_Pattern = 0
DatBinType_Arrow = 0
DatBinType_Pointlist = 1
###################################

#########dat language type#########
DatLangInfo_Common = 0
###################################

#########dat dat&night type########
DatDayNightInfo_Common = 0
DatDayNightInfo_Day = 1
DatDayNightInfo_Night = 2
DatDayNightInfo_Evening = 3
###################################

class DatBinInfo(object):

    def __init__(self, pBuff):
        cDatInfo = struct.unpack("<h", pBuff[0:2])[0] & 255
        self.mBinType = (cDatInfo >> 6) & 3
        self.mLangInfo = (cDatInfo >> 2) & 15
        self.mDayNightInfo = (cDatInfo >> 0) & 3
        self.mDataOffset, self.mDataLength = struct.unpack("<ii", pBuff[1:9])
        return

    def __del__(self):
        return

    def getPicInfoString(self):
        strPicType = ''
        if (self.mBinType == DatBinType_Arrow or 
            self.mBinType == DatBinType_Pattern):
            strPicType = "pattern/arrow picture"
        elif (self.mBinType == DatBinType_Pointlist):
            strPicType = "pointlist data"
        else:
            strPicType = "Unknown binary data"

        strLanguage = ''
        if (self.mLanInfo == DatLangInfo_Common):
            strLanguage = _T("Common language")

        strDayNight = ''
        if (self.mDayNightInfo == DatDayNightInfo_Common):
            strDayNight = _T("Day/Night Common")
        elif (self.mDayNightInfo == DatDayNightInfo_Day):
            strDayNight = _T("Day")
        elif(self.mDayNightInfo == DatDayNightInfo_Night):
            strDayNight = _T("Night")
        elif(self.mDayNightInfo == DatDayNightInfo_Evening):
            strDayNight = _T("Evening")
        else:
            strDayNight = _T("")

        strDatInfo = """%s,\n%s,\n%s,\n%d bytes\n""" % \
        (strPicType, strLanguage, strDayNight, mDataLength)
        return strDatInfo
##################################################################################################
##################################################################################################
##################################################################################################
##################################################################################################
class MyDatParser(object):

    def __init__(self):
        self.mStrDatPath = None
        self.mPicCount = -1
        self.mVecDatInfoList = []
        self.mBuff = ''
        return

    def __del__(self):
        return

    def clear():
        self.mStrDatPath = None
        self.mPicCount = -1
        self.mVecDatInfoList = []
        self.mBuff = ''
        return

    def initFromDisk(self, errMsg, strDatPath):
        self.clear();
        try:
            with open(strDatPath, 'rb') as iFStream:
                pBuff = iFStream.read()

            self.initFromMemory(errMsg, pBuff)
            if errMsg[0] != '':
                return
        except Exception, ex:
            errMsg[0] = ex.message
            return

    def initFromMemory(self, errMsg, pBuff):
        self.mBuff = pBuff
        a = self.mBuff[0]
        b = self.mBuff[1]
        if (a != '\xfe' or b != '\xfe'):
            errMsg[0] = """Input file is not a dat file."""
            return
        self.mPicCount = struct.unpack("<H", self.mBuff[2:4])[0]
        for i in range(0, self.mPicCount):
            self.mVecDatInfoList.append(DatBinInfo(self.mBuff[(4+9*i):(4+9*i+9)]))
        return

    def getPicInfoByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.mVecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.mVecDatInfoList))
            return

        strDatInfo = """A dat file.\n%d/%d,\n%s""" % \
            (iIdx+1, len(self.mVecDatInfoList), self.mVecDatInfoList[iIdx].GetPicInfoString())

        if (self.mVecDatInfoList[iIdx].mBinType == DatBinType_Pointlist):
            strDatInfo += _T("\npointlist:\n")
            strDatInfo += self.GetPointListStringByIndex(errMsg, iIdx)
        return strDatInfo

    def getPicCount(self):
        return len(self.mVecDatInfoList)

    def getDatContentByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.mVecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.mVecDatInfoList))
            return

        theDat = self.mVecDatInfoList[iIdx]
        return self.mBuff[theDat.mDataOffset : theDat.mDataOffset+theDat.mDataLength]

    def getPicLengthByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.mVecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.mVecDatInfoList))
            return
        return self.mVecDatInfoList[iIdx].mDataLength

    def getPicTypeByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.mVecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.mVecDatInfoList))
            return
        return self.mVecDatInfoList[iIdx].mBinType

    def getPointListCoordinatesByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.mVecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.mVecDatInfoList))
            return None
        
        vecResult = []
        theDat = self.mVecDatInfoList[iIdx];
        pPointlistData = self.mBuff[theDat.mDataOffset: theDat.mDataOffset+theDat.mDataLength]

        # 0-1 byte is point list total length.
        # 2-3 byte is count of point.
        # so we start at 4.
        for i in range(4, theDat.mDataLength, 4):
            x_y = struct.unpack('<hh', pPointlistData[i: i+4])
            vecResult.append(x_y)
        return vecResult;

    def convertDatToJpgs(self, errMsg, strOutputDir):
        # output dat
        strOutDat = os.path.join(strOutputDir, self.getDatFileName())
        with open(strOutDat, 'wb') as oFStream:
            oFStream.write(self.mBuff)

        for i in range(0, self.mPicCount):
            binInfo = self.mVecDatInfoList[i];
            pTempBuf = self.mBuff[binInfo.mDataOffset, binInfo.mDataOffset+binInfo.mDataLength]

            strOutJpg = ''
            imgType = MyGetBinType.GetBinaryDataTypeByBuffer(pTempBuf)
            if (imgType == ImageType_Jpg):
                strOutJpg = """%s\\%s_%d.jpg"""%(strOutputDir, self.getDatFileNameWithoutExt(), i)
            elif(imgType == ImageType_Png):
                strOutJpg = """%s\\%s_%d.png"""%(strOutputDir, self.getDatFileNameWithoutExt(), i)
            else:
                errMsg[0] = """dat internal error: invalid dat data type."""
                return
            with open(strOutJpg, 'w') as oFStream:
                oFStream.write(pTempBuf)
        errMsg[0] = ''
        return

    def getDatFileName(self):
        return os.path.split(self.mStrDatPath)[1]

    def getDatFileNameWithoutExt(self):
        strDatFileName = self.getDatFileName()
        return os.path.splitext(strDatFileName)[0]

    def getPointListStringByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.mVecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.mVecDatInfoList))
            return

        dataLen = self.mVecDatInfoList[iIdx].mDataLength
        if dataLen > 128:
            errMsg[0] = """Point list data should not be longer than 128.""" %\
                (iIdx, len(self.mVecDatInfoList))
            return

        strRes = ''
        strTemp = ''
        vecCoor = self.getPointListCoordinatesByIndex(errMsg, iIdx)
        for oneX_Y in vecCoor:
            strTemp = """%d, %d\n""" % (oneX_Y[0], oneX_Y[1])
            strRes = strTemp+strRes
        return strRes

    def getDatFullPath(self):
        return mStrDatPath

    def setDatFullPath(self, str):
        mStrDatPath = str
        return

    def hasPointlist(self):
        for i in range(0, len(self.mVecDatInfoList)):
            if self.mVecDatInfoList[i].mBinType == DatBinType_Pointlist:
                return True
        return False

    def getPointlistIndex(self):
        for i in range(0, len(self.mVecDatInfoList)):
            if(self.mVecDatInfoList[i].mBinType == DatBinType_Pointlist):
                return i
        return -1





