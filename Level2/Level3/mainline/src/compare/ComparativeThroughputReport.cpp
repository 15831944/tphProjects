// ComparativeThroughputReport.cpp: implementation of the CComparativeThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeThroughputReport.h"
#include "common\exeption.h"
#include "Common\TERMFILE.H"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeThroughputReport::CComparativeThroughputReport()
{
	m_vDetail.clear();
}

CComparativeThroughputReport::~CComparativeThroughputReport()
{
}

void CComparativeThroughputReport::MergeSample( const ElapsedTime& tInteval )
{
	if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
	{
		MergeDetailSample(tInteval);
	}
	else
	{
		MergeSummarySample(tInteval);
	}
}

void CComparativeThroughputReport::MergeDetailSample(const ElapsedTime& tInteval)
{
	//clear the old data
	m_vDetail.clear();

	ASSERT(m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;

	int nSampleCount = static_cast<int>(m_vSampleRepPaths.size());
	for(int i=0; i<nSampleCount; i++)
	{
		try
		{
			if(file.openFile(m_vSampleRepPaths[i].c_str(), READ))
			{
				OneCmpThroughputDetailVector vec;
				m_vDetail.push_back(vec);
				ElapsedTime startTime, endTime;
				int nPaxServed=0;

				while(file.getLine())
				{
					//start time
					file.setToField(1);
					file.getTime(startTime);

					//end time
					file.setToField(2);
					file.getTime(endTime);

					//served passenger count
					file.setToField(3);
					file.getInteger(nPaxServed);

					BOOL bFound = FALSE;
					for (unsigned j = 0; j < m_vDetail[i].size(); j++)
					{
						if ((m_vDetail[i][j].m_startTime == startTime) &&
							(m_vDetail[i][j].m_endTime == endTime))
						{
							m_vDetail[i][j].m_nPaxServed += nPaxServed;
							bFound = TRUE;
							break;
						}
					}

					if (!bFound)//if not find, then insert the data
					{
						CmpThroughputDetailData newData;
						newData.m_startTime = startTime;
						newData.m_endTime = endTime;
						newData.m_nPaxServed += nPaxServed;
						m_vDetail[i].push_back(newData);
					}
				}
				file.closeIn();
			}
		}
		catch(...)
		{
			m_vDetail.clear();
			return ;
		}
	}
}

void CComparativeThroughputReport::MergeSummarySample( const ElapsedTime& tInteval )
{
	//clear the old data
	m_vDetail.clear();

	ASSERT(m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;
	BOOL bFound = FALSE;

	int nSampleCount = static_cast<int>(m_vSampleRepPaths.size());

	CmpThroughputSummaryData data;
	for(int i=0; i<nSampleCount; i++)
	{
		data.clear();
		try
		{
			if(file.openFile(m_vSampleRepPaths[i].c_str(), READ))
			{
				file.getLine();

				// Total Pax
				file.setToField(2);
				file.getInteger(data.m_totalPax);

				// Avg Pax
				file.setToField(3);
				file.getInteger(data.m_avgPax);

				// Total / Hour
				file.setToField(4);
				file.getInteger(data.m_totalPerHour);

				// Avg / Hour
				file.setToField(5);
				file.getInteger(data.m_avgPerHour);

				m_vSummary.push_back(data);
				file.closeIn();
			}
		}
		catch(...)
		{
			m_vDetail.clear();
			return;
		}
	}
}

bool CComparativeThroughputReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		//write original sample count
		file.writeField("Comparative Report - Throughput Report");
		file.writeLine();

		//write comparative report name
		file.writeField(m_cmpReportName);
		file.writeLine();

		//write original sample count
		int nSimCount = m_vSimName.size();
		file.writeInt(nSimCount);
		file.writeLine();

		for(int i=0; i<nSimCount; i++)
		{
			//write simulation name
			file.writeField(m_vSimName[i]);
			file.writeLine();

			//write original sample path
			file.writeField(m_vSampleRepPaths[i].c_str());
			file.writeLine();

			//write data lines
			if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				int nDataCount = static_cast<int>(m_vDetail[i].size());
				file.writeInt(nDataCount);
				file.writeLine();
				for(int j=0; j<nDataCount; j++)
				{
					file.writeTime(m_vDetail[i][j].m_startTime);	//start time
					file.writeTime(m_vDetail[i][j].m_endTime);		//end time
					file.writeInt(m_vDetail[i][j].m_nPaxServed);	//passenger served.
					file.writeLine();
				}
			}
			else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
			{
				file.writeInt(m_vSummary[i].m_totalPax);
				file.writeInt(m_vSummary[i].m_avgPax);
				file.writeInt(m_vSummary[i].m_totalPerHour);
				file.writeInt(m_vSummary[i].m_avgPerHour);
				file.writeLine();
			}
		}

		CTime tm = CTime::GetCurrentTime();
		file.writeLine();
		file.writeField(tm.Format(_T("%d/%m/%Y,%H:%M:%S")));
		file.writeLine();

		file.closeOut();
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool CComparativeThroughputReport::LoadReport(const std::string& _sPath)
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_vDetail.clear();

	try
	{
		ArctermFile file;
		file.openFile( _sPath.c_str(), READ);

		// get report name
		file.getField(m_cmpReportName.GetBuffer(256), 256);
		m_cmpReportName.ReleaseBuffer();


		//get model number
		int nSimCount =0;
		file.getLine();
		if (file.getInteger(nSimCount)==false || nSimCount<=0)
			return false;


		char buffer[MAX_PATH]="";
		for(int i=0; i<nSimCount; i++)
		{
			file.getLine();
			//get simulation name
			file.getField(buffer, MAX_PATH);
			m_vSimName.push_back(CString(buffer));

			//get sample file name
			file.getLine();
			file.getField(buffer, MAX_PATH);
			m_vSampleRepPaths.push_back(std::string(buffer));

			if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				//read report data
				file.getLine();
				int nDataCount = 0;
				file.getInteger(nDataCount);

				OneCmpThroughputDetailVector vDetail;
				CmpThroughputDetailData data;
				for(int j=0; j<nDataCount; j++)
				{
					data.clear();
					file.getLine();
					file.getTime(data.m_startTime);		//get the start time
					file.getTime(data.m_endTime);		//get the end time
					file.getInteger(data.m_nPaxServed);	//get pax served
					vDetail.push_back(data);
				}
				m_vDetail.push_back(vDetail);
			}
			else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
			{
				CmpThroughputSummaryData data;

				file.getLine();
				// Total Pax
				file.getInteger(data.m_totalPax);
				// Avg Pax
				file.getInteger(data.m_avgPax);
				// Total / Hour
				file.getInteger(data.m_totalPerHour);
				// Avg / Hour
				file.getInteger(data.m_avgPerHour);
			}
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