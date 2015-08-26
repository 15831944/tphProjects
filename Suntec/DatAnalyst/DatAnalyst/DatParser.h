#pragma once
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
    DatBinInfo(char* p)
    {
        char cInfo = p[0];
        binType = (cInfo >> 6) & 3;
        langInfo = (cInfo >> 2) & 15;
        dayNightInfo = (cInfo >> 0) & 3;

        char offsetBuf[5] = {0};
        memcpy(offsetBuf, p+1, 4);
        dataOffset = atoi(offsetBuf);
        char lenBuf[5] = {0};
        memcpy(offsetBuf, p+5, 4);
        dataLength = atoi(lenBuf);
    }
    ~DatBinInfo(){}
public:
    DatBinType binType;
    DatLangInfo langInfo;
    DatDayNightInfo dayNightInfo;
    int dataOffset;
    int dataLength;
};

class DatParser
{
public:
    DatParser(){ Clear(); }
    ~DatParser(){}
public:
    void Init(int& iErr, CString strDatPath);
    CString m_strDatPath;
private:
    void Clear();

private:
    char* m_pBuff;
    short m_nPicCount;
    std::vector<DatBinInfo> m_vecDatInfoList;
};

