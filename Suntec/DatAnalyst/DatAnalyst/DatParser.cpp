#include "StdAfx.h"
#include "DatParser.h"
#include <iostream>

const int xxxxx1 = 10121; // 路径找不到
const int xxxxx2 = 10121; // 无法打开文件
const int xxxxx3 = 1012133; // 文件的长度小于等于0
const int xxxxx4 = 101233; // 读取的文件不是一个dat文件。

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

    iErr = 0;
    if(strDatPath == "")
    {
        iErr = xxxxx1;
        return;
    }

    std::ifstream inFile(strDatPath.GetBuffer(), std::ios::in|std::ios::binary);
    if (!inFile.is_open())
    {
        iErr = xxxxx2;
        return;
    }

    //获得文件的大小
    if (get_filesize(strDatPath.GetBuffer()) <= 0)
    {
        iErr = xxxxx3;
        return;
    }

    //开辟相应的缓冲区
    m_pBuff = new char[fileSize];
    if (m_pBuff != NULL)
    {
        while(!inFile.eof() && readBytes<fileSize)
        {
            inFile.read(szBuf+readBytes, fileSize-readBytes);
            if(inputStream.fail())
            {
                delete m_pBuff;
                m_pBuff = NULL;
                return;
            }
            readBytes += inFile.gcount();
        }
    }
    inFile.clear();
    inFile.close();

    m_pBuff[0];
    if(m_pBuff[0] != 0xFE && m_pBuff[1] != 0xFE)
    {
        iErr = xxxxx4;
        delete m_pBuff;
        m_pBuff = NULL;
        return;
    }

    char strCount[3] = {0};
    memcpy(strCount, &m_pBuff[2], 2);
    m_nPicCount = atoi(strCount);

    for(int i=0; i<nPicCount; i++)
    {
        char strPicInfo[10] = {0};
        memcpy(strPicInfo, m_pBuff[4+9*i], 9);
        m_vecDatInfoList.push_back(DatBinInfo(strPicInfo));
    }
    return 0;
}

