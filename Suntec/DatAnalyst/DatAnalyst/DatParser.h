#pragma once
#include <map>
#include <vector>
/*
dat 格式文档请查看dat相关协议。
*/

typedef enum 
{
    DatBinType_Pattern = 0,
    DatBinType_Arrow = 0,
    DatBinType_Pointlist,
} DatBinType;

typedef enum 
{
    DatLangInfo_Common = 0
} DatLangInfo;

typedef enum 
{
    DatDayNightInfo_Common = 0,
    DatDayNightInfo_Day = 1,
    DatDayNightInfo_Night = 2,
    DatDayNightInfo_Evening = 3
} DatDayNightInfo;


class DatBinInfo
{
public:
    DatBinInfo(unsigned char* p)
    {
        char cInfo = p[0];
        m_binType = (DatBinType)((cInfo >> 6) & 3);
        m_langInfo = (DatLangInfo)((cInfo >> 2) & 15);
        m_dayNightInfo = (DatDayNightInfo)((cInfo >> 0) & 3);
        memcpy(&m_dataOffset, p+1, 4);
        memcpy(&m_dataLength, p+5, 4);
    }
    ~DatBinInfo(){}
public:
    DatBinType m_binType;
    DatLangInfo m_langInfo;
    DatDayNightInfo m_dayNightInfo;
    int m_dataOffset;
    int m_dataLength;
};

class DatParser
{
public:
    DatParser();
    ~DatParser(){ Clear(); }
public:
    void Init(int& iErr, CString strDatPath);
    CString GetPicInfoByIndex(int& iErr, int iIdx);
    size_t GetPicCount(){ return m_vecDatInfoList.size(); };

private:
    void Clear();

private:
    char* m_pBuff;
    short m_nPicCount;
    CString m_strDatPath;
    std::vector<DatBinInfo> m_vecDatInfoList;
};

