#include "StdAfx.h"
#include "DatParser.h"
#include <fstream>

const int xxxxx1 = 10121; // 路径找不到
const int xxxxx2 = 10121; // 文件的长度小于等于0
const int xxxxx3 = 1012133; // 无法打开文件
const int xxxxx4 = 101233; // 读取的文件不是一个dat文件。
const int xxxxxx5 = 1012423; // 输入的下标志超出合法范围。

DatParser::DatParser()
{
    m_nPicCount = -1;
    m_vecDatInfoList.empty();
    m_pBuff = NULL;
}

void DatParser::Clear()
{
    m_nPicCount = -1;
    m_vecDatInfoList.empty();
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
        iErr = xxxxxx5;
        return _T("");
    }

    CString strPicType;
    if(m_vecDatInfoList[iIdx].m_binType == DatBinType_Arrow ||
        m_vecDatInfoList[iIdx].m_binType == DatBinType_Pattern)
    {
        strPicType = _T("A pattern/arrow picture");
    }
    else if(m_vecDatInfoList[iIdx].m_binType == DatBinType_Pointlist)
    {
        strPicType = _T("A pointlist binary data");
        CString strDatInfo;
        strDatInfo.Format("%d/%d, %s", strPicType);
        return strDatInfo;
    }
    else
    {
        strPicType = _T("Unknown binary data");
    }

    CString strDayNight;
    if(m_vecDatInfoList[iIdx].m_dayNightInfo == DatDayNightInfo_Common)
    {
        strDayNight = _T("Day&Night Common");
    }
    else if(m_vecDatInfoList[iIdx].m_dayNightInfo == DatDayNightInfo_Day)
    {
        strDayNight = _T("Day");
    }
    else if(m_vecDatInfoList[iIdx].m_dayNightInfo == DatDayNightInfo_Night)
    {
        strDayNight = _T("Night");
    }
    else if(m_vecDatInfoList[iIdx].m_dayNightInfo == DatDayNightInfo_Evening)
    {
        strDayNight = _T("Evening");
    }
    else
    {
        strDayNight = _T("");
    }

    CString strDatInfo;
    strDatInfo.Format("%d/%d, %s, %s, length: %d", 
        iIdx+1, m_vecDatInfoList.size(), strPicType, strDayNight,
        m_vecDatInfoList[iIdx].m_dataLength);
    return strDatInfo;
}

