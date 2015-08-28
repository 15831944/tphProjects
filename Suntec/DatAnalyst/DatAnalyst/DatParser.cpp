#include "StdAfx.h"
#include "DatParser.h"
#include <fstream>
#include <malloc.h>
#include "MyImageType.h"

const int xxxxx1 = 10121; // 路径找不到
const int xxxxx2 = 10121; // 文件的长度小于等于0
const int xxxxx3 = 1012133; // 无法打开文件
const int xxxxx4 = 101233; // 读取的文件不是一个dat文件。
const int xxxxx5 = 1012423; // 输入的下标志超出合法范围。
const int xxxxx6 = 1232124;
const int xxxxx7 = 112213; // 二进制流的类型既不是jpg也不是png，报错。

/************************************************************************/
DatBinInfo::DatBinInfo( unsigned char* p )
{
    char cInfo = p[0];
    m_binType = (DatBinType)((cInfo >> 6) & 3);
    m_langInfo = (DatLangInfo)((cInfo >> 2) & 15);
    m_dayNightInfo = (DatDayNightInfo)((cInfo >> 0) & 3);
    memcpy(&m_dataOffset, p+1, 4);
    memcpy(&m_dataLength, p+5, 4);
}

DatBinInfo::~DatBinInfo(){}

CString DatBinInfo::GetPicInfoString()
{
    CString strPicType;
    if(m_binType == DatBinType_Arrow || m_binType == DatBinType_Pattern)
    {
        strPicType = _T("A pattern/arrow picture");
    }
    else if(m_binType == DatBinType_Pointlist)
    {
        strPicType = _T("A pointlist binary data");
    }
    else
    {
        strPicType = _T("Unknown binary data");
    }

    CString strLanguage;
    if(m_langInfo == DatLangInfo_Common)
    {
        strLanguage = _T("Common");
    }

    CString strDayNight;
    if(m_dayNightInfo == DatDayNightInfo_Common)
    {
        strDayNight = _T("Day&Night Common");
    }
    else if(m_dayNightInfo == DatDayNightInfo_Day)
    {
        strDayNight = _T("Day");
    }
    else if(m_dayNightInfo == DatDayNightInfo_Night)
    {
        strDayNight = _T("Night");
    }
    else if(m_dayNightInfo == DatDayNightInfo_Evening)
    {
        strDayNight = _T("Evening");
    }
    else
    {
        strDayNight = _T("");
    }

    CString strDatInfo;
    strDatInfo.Format("%s, %s, %s,  %d bytes", 
        strPicType, strLanguage, strDayNight, m_dataLength);
    return strDatInfo;
}

/************************************************************************/

DatParser::DatParser()
{
    m_nPicCount = -1;
    m_vecDatInfoList.clear();
    m_pBuff = NULL;
}

void DatParser::Clear()
{
    m_nPicCount = -1;
    m_vecDatInfoList.clear();
    if(m_pBuff)
    {
        delete m_pBuff;
        m_pBuff = NULL;
    }
}

void DatParser::Init(int& iErr, CString strDatPath)
{
    this->Clear();

    //获得文件的大小
    HANDLE hFile = CreateFile(strDatPath, GENERIC_READ, 
    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
    FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile==INVALID_HANDLE_VALUE)
    {
        iErr = xxxxx1;
        return;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size))
    {
        iErr = xxxxx2;
        return;
    }

    std::ifstream inFile(strDatPath, std::ios::in|std::ios::binary);
    if (!inFile.is_open())
    {
        iErr = xxxxx3;
        return;
    }

    m_strDatPath = strDatPath;
    //开辟相应的缓冲区
    m_pBuff = new char[size.QuadPart + 1];
    memset(m_pBuff, 0, size.QuadPart + 1);
    if (m_pBuff != NULL)
    {
        inFile.read(m_pBuff, size.QuadPart);
    }
    inFile.clear();
    inFile.close();

    m_pBuff[0];
    unsigned char c1 = m_pBuff[0] ;
    unsigned char c2 = m_pBuff[1];
    
    if(c1 != 0xfe || c2 != 0xfe)
    {
        iErr = xxxxx4;
        delete m_pBuff;
        m_pBuff = NULL;
        return;
    }

    memcpy(&m_nPicCount, &m_pBuff[2], 2);

    for(int i=0; i<m_nPicCount; i++)
    {
        unsigned char strPicInfo[10] = {0};
        memcpy(strPicInfo, &m_pBuff[4+9*i], 9);
        m_vecDatInfoList.push_back(DatBinInfo(strPicInfo));
    }
    return;
}

// 通过下标获取某张图片的相关信息。
CString DatParser::GetPicInfoByIndex(int& iErr, int iIdx)
{
    if(iIdx<0 || iIdx>=m_vecDatInfoList.size())
    {
        iErr = xxxxx5;
        return _T("");
    }

    CString strDatInfo;
    strDatInfo.Format("%d/%d, %s", 
        iIdx+1, m_vecDatInfoList.size(), 
        m_vecDatInfoList[iIdx].GetPicInfoString());
    return strDatInfo;
}

// pResult: I will return a 'new' buffer by pResult, you must delete it by yourself.
void DatParser::GetPicBufferByIndex(int& iErr, int iIdx, char** pResult)
{
    if(iIdx<0 || iIdx>=m_vecDatInfoList.size())
    {
        iErr = xxxxx6;
        pResult = NULL;
        return;
    }

    DatBinInfo theDat = m_vecDatInfoList[iIdx];
    *pResult = new char[theDat.m_dataLength];
    memcpy(*pResult, m_pBuff+theDat.m_dataOffset, theDat.m_dataLength-10);
    iErr = 0;
    return;
}

// get the image length by index
long DatParser::GetPicLengthByIndex(int& iErr, int iIdx)
{
    if(iIdx<0 || iIdx>=m_vecDatInfoList.size())
    {
        iErr = xxxxx5;
        return -1;
    }

    return m_vecDatInfoList[iIdx].m_dataLength;
}

// get binary segment data type by index
DatBinType DatParser::GetPicTypeByIndex(int& iErr, int iIdx)
{
    if(iIdx<0 || iIdx>=m_vecDatInfoList.size())
    {
        iErr = xxxxx5;
        return DatBinType_Invalid;
    }
    return m_vecDatInfoList[iIdx].m_binType;
}

// get dat file name that without file ext.
CString DatParser::GetDatFileName()
{
    CString str1 = m_strDatPath.Right(m_strDatPath.GetLength()-1-m_strDatPath.ReverseFind('\\'));
    str1 = str1.Left(str1.Find(_T(".dat")));
    return str1;
}

void DatParser::DatToJpgs(int& iErr, CString strOutputDir)
{
    // output dat
    CString strOutDat = strOutputDir + "\\" + GetDatFileName() + ".dat";
    std::ofstream oFStream(strOutDat, std::ios::out|std::ios::binary|std::ios::trunc);
    if (!oFStream.is_open())
    {
        iErr = xxxxx3;
        return;
    }
    int sizex = _msize(m_pBuff);
    oFStream.write(m_pBuff, _msize(m_pBuff));
    oFStream.close();

    for(int i=0; i<m_nPicCount; i++)
    {
        DatBinInfo binInfo = m_vecDatInfoList[i];
        char* pTempBuf = new char[binInfo.m_dataLength];
        memcpy(pTempBuf, m_pBuff+binInfo.m_dataOffset, binInfo.m_dataLength);

        CString strOutJpg;
        ImageType imgType = GetBinaryDataTypeByBuffer((unsigned char*)pTempBuf);
        if(imgType == ImageType_Jpg)
        {
            strOutJpg.Format(_T("%s\\%s_%d.jpg"), strOutputDir, GetDatFileName(), i);
        }
        else if(imgType == ImageType_Png)
        {
            strOutJpg.Format(_T("%s\\%s_%d.png"), strOutputDir, GetDatFileName(), i);
        }
        else
        {
            iErr = xxxxx7;
            return;
        }
        std::ofstream oFStream(strOutJpg, std::ios::out|std::ios::binary|std::ios::trunc);
        if (!oFStream.is_open())
        {
            iErr = xxxxx3;
            return;
        }
        int sizex = _msize(pTempBuf);
        oFStream.write(pTempBuf, _msize(pTempBuf));
        oFStream.close();
        delete pTempBuf;
    }
}

