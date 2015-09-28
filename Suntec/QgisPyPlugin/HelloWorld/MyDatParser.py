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
        cDatInfo = struct.unpack("i", pBuff[0:4])[0] & 15
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
        self.m_strDatPath = None
        self.m_nPicCount = -1
        self.m_vecDatInfoList = []
        self.m_pBuff = ''
        return

    def __del__(self):
        return

    def clear():
        self.m_strDatPath = None
        self.m_nPicCount = -1
        self.m_vecDatInfoList = []
        self.m_pBuff = ''
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
        self.m_pBuff = pBuff
        a = self.m_pBuff[0]
        b = self.m_pBuff[1]
        if (a != '\xfe' or b != '\xfe'):
            errMsg[0] = """Input file: %s\nis not a dat file."""
            return
        self.m_nPicCount = struct.unpack("<H", self.m_pBuff[2:4])[0]
        for i in range(0, self.m_nPicCount):
            self.m_vecDatInfoList.append(DatBinInfo(self.m_pBuff[(4+9*i):(4+9*i+9)]))
        return

    def getPicInfoByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.m_vecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return

        strDatInfo = """A dat file.\n%d/%d,\n%s""" % \
            (iIdx+1, len(self.m_vecDatInfoList), self.m_vecDatInfoList[iIdx].GetPicInfoString())

        if (self.m_vecDatInfoList[iIdx].m_binType == DatBinType_Pointlist):
            strDatInfo += _T("\npointlist:\n")
            strDatInfo += self.GetPointListStringByIndex(errMsg, iIdx)
        return strDatInfo

    def getPicCount(self):
        return len(self.m_vecDatInfoList)

    def getPicBufferByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.m_vecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return

        theDat = self.m_vecDatInfoList[iIdx]
        return self.m_pBuff[theDat.mDataOffset : theDat.mDataOffset+theDat.mDataLength]

    def getPicLengthByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.m_vecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return
        return self.m_vecDatInfoList[iIdx].mDataLength

    def getPicTypeByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.m_vecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return
        return self.m_vecDatInfoList[iIdx].mBinType

    def getPointCoordinateListByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.m_vecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return
        
        vecResult = []
        theDat = self.m_vecDatInfoList[iIdx];
        pTemp = self.m_pBuff[theDat.m_dataOffset, theDat.m_dataOffset+theDat.m_dataLength]

        # 0-1 byte is point list total length.
        # 2-3 byte is count of point.
        # so we start at 4.
        for i in range(4, theDat.m_dataLength, 2):
            oneCoordinate = pTemp[i, i+2]
            vecResult.append(oneCoordinate)
        return vecResult;

    def convertDatToJpgs(self, errMsg, strOutputDir):
        # output dat
        strOutDat = os.path.join(strOutputDir, self.getDatFileName())
        with open(strOutDat, 'wb') as oFStream:
            oFStream.write(self.m_pBuff)

        for i in range(0, self.m_nPicCount):
            binInfo = self.m_vecDatInfoList[i];
            pTempBuf = self.m_pBuff[binInfo.m_dataOffset, binInfo.m_dataOffset+binInfo.m_dataLength]

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
        return os.path.split(self.m_strDatPath)[1]

    def getDatFileNameWithoutExt(self):
        strDatFileName = self.getDatFileName()
        return os.path.splitext(strDatFileName)[0]

    def getPointListStringByIndex(self, errMsg, iIdx):
        if (iIdx<0 or iIdx>=len(self.m_vecDatInfoList)):
            errMsg[0] = """Input index excceed list length. Input: %d, list length: %d""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return

        dataLen = m_vecDatInfoList[iIdx].m_dataLength
        if dataLen > 128:
            errMsg[0] = """Point list data should not be longer than 128.""" %\
                (iIdx, len(self.m_vecDatInfoList))
            return

        strRes = ''
        strTemp = ''
        pTemp = self.m_pBuff[ m_vecDatInfoList[iIdx].m_dataOffset, \
                              m_vecDatInfoList[iIdx].m_dataOffset+dataLen]

        vecCoor = self.getPointCoordinateListByIndex(errMsg, iIdx)
        for i in range(0, len(vecCoor), 2):
            oneX = vecCoor[i];
            oneY = vecCoor[i+1];
            strTemp = """%d, %d\n""" % (oneX, oneY)
            strRes = strTemp+strRes
        return strRes

    def getDatFullPath(self):
        return m_strDatPath

    def setDatFullPath(self, str):
        m_strDatPath = str
        return

    def hasPointlist(self):
        for i in range(0, len(self.m_vecDatInfoList)):
            if self.m_vecDatInfoList[i].m_binType == DatBinType_Pointlist:
                return true
        return false

    def getPointlistIndex(self):
        for i in range(0, len(self.m_vecDatInfoList)):
            if(self.m_vecDatInfoList[i].m_binType == DatBinType_Pointlist):
                return i
        return -1





