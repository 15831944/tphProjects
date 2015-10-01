#pragma once
#include <map>
#include <vector>
/*
    dat ��ʽ�ĵ���鿴dat���Э�顣
*/

typedef enum 
{
    DatBinType_Invalid = -1,
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
    DatBinInfo(unsigned char* p);
    ~DatBinInfo();
public:
    CString GetPicInfoString();
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
    void GetPicBufferByIndex(int& iErr, int iIdx, char** pResult);
    long GetPicLengthByIndex(int& iErr, int iIdx);
    DatBinType GetPicTypeByIndex(int& iErr, int iIdx);
    std::vector<short> GetPointCoordinateListByIndex(int& iErr, int iIdx);
    void DatToJpgs(int& iErr, CString strOutputDir);
    CString GetDatFileName();
    CString GetDatFileNameWithoutExt();
    CString GetPointListStringByIndex(int& iErr, int iIdx);
    CString GetDatFullPath() { return m_strDatPath; }
    void SetDatPath(const CString& val) { m_strDatPath = val; }
    bool HasPointlist();
    int GetPointlistIndex();

private:
    void Clear();
private:
    CString m_strDatPath;
    short m_nPicCount;
    std::vector<DatBinInfo> m_vecDatInfoList;
    char* m_pBuff;
};

