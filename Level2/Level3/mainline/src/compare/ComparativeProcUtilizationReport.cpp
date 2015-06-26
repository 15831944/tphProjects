#include "StdAfx.h"
#include "ComparativeProcUtilizationReport.h"
#include "Common\TERMFILE.H"
#include "Common\exeption.h"
#include "Main\MultiRunsReportDataLoader.h"

CmpProcUtilizationDetailData::CmpProcUtilizationDetailData()
    : m_dScheduledTime(0L),
    m_dOverTime(0L),
    m_dActualTime(0L),
    m_dServiceTime(0L),
    m_dIdleTime(0L),
    m_fUtilization(0.0f)
{
}

CmpProcUtilizationDetailData::~CmpProcUtilizationDetailData()
{
}

void CmpProcUtilizationDetailData::ReadData(ArctermFile& file)
{
    file.getField(m_strProc.GetBuffer(256), 255);
    m_strProc.ReleaseBuffer();
    file.getTime(m_dScheduledTime);
    file.getTime(m_dOverTime);
    file.getTime(m_dActualTime);
    file.getTime(m_dServiceTime);
    file.getTime(m_dIdleTime);
    file.getFloat(m_fUtilization);
}

void CmpProcUtilizationDetailData::WriteData(ArctermFile& file) const
{
    CString strProc = m_strProc;
    file.writeField(strProc.GetBuffer());
    file.writeTime(m_dScheduledTime);
    file.writeTime(m_dOverTime);
    file.writeTime(m_dActualTime);
    file.writeTime(m_dServiceTime);
    file.writeTime(m_dIdleTime);
    file.writeFloat(m_fUtilization);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CmpProcUtilizationSummaryData::CmpProcUtilizationSummaryData()
    : m_nProcCount(0)
{
}

CmpProcUtilizationSummaryData::~CmpProcUtilizationSummaryData()
{

}

void CmpProcUtilizationSummaryData::ReadData(ArctermFile& file)
{
    file.getField(m_strProc.GetBuffer(256), 255);
    m_strProc.ReleaseBuffer();
    file.getInteger(m_nProcCount);
    file.getTime(m_dScheduledTime);
    file.getTime(m_dOverTime);
    file.getTime(m_dActualTime);
    file.getTime(m_dServiceTime);
    file.getTime(m_dIdleTime);
    file.getFloat(m_fUtilization);
}

void CmpProcUtilizationSummaryData::WriteData(ArctermFile& file) const
{
    CString strProc = m_strProc;
    file.writeField(strProc.GetBuffer());
    file.writeInt(m_nProcCount);
    file.writeTime(m_dScheduledTime);
    file.writeTime(m_dOverTime);
    file.writeTime(m_dActualTime);
    file.writeTime(m_dServiceTime);
    file.writeTime(m_dIdleTime);
    file.writeFloat(m_fUtilization);
    file.writeFloat(m_fUtilization);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CComparativeProcUtilizationReport::CComparativeProcUtilizationReport()
{
}

CComparativeProcUtilizationReport::~CComparativeProcUtilizationReport()
{
}

void CComparativeProcUtilizationReport::MergeSample(const ElapsedTime& tInteval)
{
    if(m_cmpParam.GetReportDetail()==REPORT_TYPE_DETAIL)
    {
        MergeSampleDetail(tInteval);
    }
    else if(m_cmpParam.GetReportDetail()==REPORT_TYPE_SUMMARY)
    {
        MergeSampleSummary(tInteval);
    }
}
void CComparativeProcUtilizationReport::MergeSampleDetail(const ElapsedTime& tInterval)
{
    ASSERT(m_vSampleRepPaths.size()>0);
    if(m_vSampleRepPaths.size()==0) return;

    ArctermFile file;
    size_t nCount = m_vSimName.size();
    for(size_t i=0; i<nCount; i++)
    {
        CString strSimName = m_vSimName.at(i);
        m_mapDetail[strSimName].clear();

        std::string strRepFile = m_vSampleRepPaths.at(i);
        try
        {
            if(file.openFile(strRepFile.c_str(), READ))
            {
                while(file.getLine())
                {
                    CmpProcUtilizationDetailData tempData;
                    tempData.ReadData(file);
                    m_mapDetail[strSimName].push_back(tempData);
                }
                file.closeIn();
            }
        }
        catch(...)
        {
            return ;
        }
    }
}
void CComparativeProcUtilizationReport::MergeSampleSummary(const ElapsedTime& tInteval)
{
    ASSERT(m_vSampleRepPaths.size()>0);
    if(m_vSampleRepPaths.size()==0) return;

    ArctermFile file;
    size_t nCount = m_vSimName.size();
    for(size_t i=0; i<nCount; i++)
    {
        CString strSimName = m_vSimName.at(i);
        m_mapSummary[strSimName].clear();

        std::string strRepFile = m_vSampleRepPaths.at(i);
        try
        {
            if(file.openFile(strRepFile.c_str(), READ))
            {
                while(file.getLine())
                {
                    CmpProcUtilizationSummaryData tempSummaryData;
                    tempSummaryData.ReadData(file);
                    m_mapSummary[strSimName].push_back(tempSummaryData);
                }
                file.closeIn();
            }
        }
        catch(...)
        {
            return ;
        }
    }
}

bool CComparativeProcUtilizationReport::SaveReport(const std::string& _sPath)const
{
    ArctermFile file;
    if(file.openFile(_sPath.c_str(),WRITE)==false) 
        return false;

    file.writeField("Comparative Report - Processor Utilization Report");
    file.writeLine();

    //write comparative report name
    file.writeField(m_cmpReportName);
    file.writeLine();

    //write original sample count
    int nSampleCount = m_vSampleRepPaths.size();
    file.writeInt(nSampleCount);
    file.writeLine();

    //write simulation name
    int count = m_vSimName.size();
    for(int i=0; i<count; i++)
    {
        file.writeField(m_vSimName[i]);
    }
    file.writeLine();

    //write original sample path
    for(std::vector<std::string>::const_iterator iterPath=m_vSampleRepPaths.begin(); iterPath!=m_vSampleRepPaths.end(); iterPath++)
    {
        file.writeField(iterPath->c_str());
        file.writeLine();
    }
    file.writeLine();

    //detail or summary?
    ENUM_REPORT_DETAIL isDetail = m_cmpParam.GetReportDetail();
    file.writeInt((int)isDetail);
    file.writeLine();
    //write summary or detail data
    if(isDetail == REPORT_TYPE_DETAIL)
    {
        SaveReportDetail(file);
    }
    else if(isDetail == REPORT_TYPE_SUMMARY)
    {
        SaveReportSummary(file);
    }
    else
    {
        file.closeOut();
        return false;
    }

    //time
    CTime tm = CTime::GetCurrentTime();
    file.writeLine();
    file.writeField(tm.Format(_T("%d/%m/%Y,%H:%M:%S")));
    file.writeLine();

    file.closeOut();
    return true;
}

bool CComparativeProcUtilizationReport::SaveReportDetail(ArctermFile& file) const
{
    mapProcUtilizationDetail::const_iterator itor = m_mapDetail.begin();
    int nSimCount = m_mapDetail.size();
    ASSERT(nSimCount == m_vSampleRepPaths.size() && nSimCount == m_vSimName.size());
    file.writeInt(nSimCount);
    file.writeLine();

    for(; itor!=m_mapDetail.end(); ++itor)
    {
        CString strSimName = itor->first;
        file.writeField(strSimName.GetBuffer());
        int nProcCount = itor->second.size();
        file.writeInt(nProcCount);
        file.writeLine();

        std::vector<CmpProcUtilizationDetailData>::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            itor2->WriteData(file);
            file.writeLine();
        }
    }
    return true;
}

bool CComparativeProcUtilizationReport::SaveReportSummary(ArctermFile& file) const
{
    mapProcUtilizationSummary::const_iterator itor = m_mapSummary.begin();
    int nSimCount = m_mapSummary.size();
    ASSERT(nSimCount == m_vSampleRepPaths.size() && nSimCount == m_vSimName.size());
    file.writeInt(nSimCount);
    file.writeLine();

    for(; itor!=m_mapSummary.end(); ++itor)
    {
        CString strSimName = itor->first;
        file.writeField(strSimName.GetBuffer());
        int nProcCount = itor->second.size();
        file.writeInt(nProcCount);
        file.writeLine();

        std::vector<CmpProcUtilizationSummaryData>::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            itor2->WriteData(file);
            file.writeLine();
        }
    }
    return true;
}

bool CComparativeProcUtilizationReport::LoadReport(const std::string& _sPath)
{
    m_vSampleRepPaths.clear();
    m_vSimName.clear();
    try
    {
        ArctermFile file;
        file.openFile(_sPath.c_str(), READ);

        // get report name
        file.getField(m_cmpReportName.GetBuffer(256), 256);
        m_cmpReportName.ReleaseBuffer();

        //get model number
        int nSampleCount =0;
        file.getLine();
        if (file.getInteger(nSampleCount)==false || nSampleCount<=0)
            return false;

        //get simulation name list
        char buffer[MAX_PATH]="";
        file.getLine();
        for(int i=0; i<nSampleCount; i++)
        {
            file.getField(buffer, MAX_PATH);
            m_vSimName.push_back(CString(buffer));
        }

        //get sample file name
        for(int i=0; i<nSampleCount; i++)
        {
            file.getLine();
            file.getField(buffer, MAX_PATH);
            m_vSampleRepPaths.push_back(std::string(buffer));
        }
        file.skipLine();

        //load m_cmpParam
        file.getLine();
        int iDetail;
        file.getInteger(iDetail);
        m_cmpParam.SetReportDetail((ENUM_REPORT_DETAIL)iDetail);

        //get data
        if(m_cmpParam.GetReportDetail()==REPORT_TYPE_DETAIL)
        {
            LoadReportDetail(file);
        }
        else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
        {
            LoadReportSummary(file);
        }
        else
        {
            file.closeIn();
            return false;
        }
        file.closeIn();
    }
    catch(Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        return false;
    }

    return true;
}

bool CComparativeProcUtilizationReport::LoadReportDetail(ArctermFile& file)
{
    m_mapDetail.clear();
    file.getLine();
    int nSimCount;
    file.getInteger(nSimCount);
    ASSERT(nSimCount == m_vSimName.size() && nSimCount == m_vSampleRepPaths.size());

    for(int i=0; i<nSimCount; i++)
    {
        CString strSimName;
        file.getLine();
        file.getField(strSimName.GetBuffer(256), 255);
        m_mapDetail[strSimName].clear();
        int nProcCount;
        file.getInteger(nProcCount);

        for(int j=0; j<nProcCount; j++)
        {
            file.getLine();
            CmpProcUtilizationDetailData tempDetailData;
            tempDetailData.ReadData(file);
            m_mapDetail[strSimName].push_back(tempDetailData);
        }
    }
    return true;
}

bool CComparativeProcUtilizationReport::LoadReportSummary(ArctermFile& file)
{
    m_mapSummary.clear();
    file.getLine();
    int nSimCount;
    file.getInteger(nSimCount);
    ASSERT(nSimCount == m_vSimName.size() && nSimCount == m_vSampleRepPaths.size());

    for(int i=0; i<nSimCount; i++)
    {
        CString strSimName;
        file.getLine();
        file.getField(strSimName.GetBuffer(256), 255);
        m_mapSummary[strSimName].clear();
        int nProcCount;
        file.getInteger(nProcCount);

        for(int j=0; j<nProcCount; j++)
        {
            file.getLine();
            CmpProcUtilizationSummaryData tempSummaryData;
            tempSummaryData.ReadData(file);
            m_mapSummary[strSimName].push_back(tempSummaryData);
        }
    }
    return true;
}

CString CComparativeProcUtilizationReport::GetFooter( int iSubType ) const
{
	CString strFooter;
	strFooter.Format(_T("Comparative Report Processor Utilization(%s) %s %s"),GetModelName(),m_cmpParam.GetStartTime().printTime(),m_cmpParam.GetEndTime().printTime());
	return strFooter;
}
