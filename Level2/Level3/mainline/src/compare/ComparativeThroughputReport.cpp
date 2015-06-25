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
	m_vThoughputData.clear();
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
	m_vThoughputData.clear();

	ASSERT(m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;
	BOOL bFound = FALSE;

	int nSampleCount = static_cast<int>(m_vSampleRepPaths.size());
	for(int i=0; i<nSampleCount; i++)
	{
		try
		{
			if(file.openFile(m_vSampleRepPaths[i].c_str(), READ))
			{
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

					for (unsigned j = 0; j < m_vThoughputData.size(); j++)
					{
						if ((m_vThoughputData[i].m_startTime == startTime) &&
							(m_vThoughputData[i].m_endTime == endTime))
						{
							m_vThoughputData[j].m_vPaxServed[i] += nPaxServed;
							bFound = TRUE;
							break;
						}
					}

					if (!bFound)//if not find, then insert the data
					{
						CmpThroughputDetailData newData;
						newData.m_startTime = startTime;
						newData.m_endTime = endTime;
						newData.m_vPaxServed = std::vector<int>(nSampleCount, 0);
						newData.m_vPaxServed[i] += nPaxServed;
						m_vThoughputData.push_back(newData);
					}
				}
				file.closeIn();
			}
		}
		catch(...)
		{
			m_vThoughputData.clear();
			return ;
		}
	}
}

void CComparativeThroughputReport::MergeSummarySample( const ElapsedTime& tInteval )
{
	//clear the old data
	m_vThoughputData.clear();

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
			m_vThoughputData.clear();
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
		int nSampleCount = m_vSampleRepPaths.size();
		file.writeInt( nSampleCount );
		file.writeLine();

		//write simulation name
		int count = m_vSimName.size();
		for(int i=0; i<count; i++)
		{
			file.writeField(m_vSimName[i]);
		}
		file.writeLine();

		//write original sample path
		for( std::vector<std::string>::const_iterator iterPath=m_vSampleRepPaths.begin(); iterPath!=m_vSampleRepPaths.end(); iterPath++)
		{
			file.writeField( iterPath->c_str() );
			file.writeLine();
		}
		file.writeLine();

		//write data lines
		if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
		{
			for(std::vector<CmpThroughputDetailData>::const_iterator iterLine=m_vThoughputData.begin(); 
				iterLine != m_vThoughputData.end(); iterLine++)//line
			{
				file.writeTime( iterLine->m_startTime );//start time
				file.writeTime( iterLine->m_endTime );//end time
				//passenger served.
				int count = static_cast<int>(iterLine->m_vPaxServed.size());
				ASSERT(count == nSampleCount);
				for(int i=0; i<nSampleCount; i++)
				{
					file.writeInt(iterLine->m_vPaxServed[i]);
				}
				file.writeLine();
			}
		}
		else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
		{
			for(std::vector<CmpThroughputSummaryData>::const_iterator iterLine=m_vSummary.begin(); 
				iterLine != m_vSummary.end(); iterLine++)//line
			{
				file.writeInt(iterLine->m_totalPax);
				file.writeInt(iterLine->m_avgPax);
				file.writeInt(iterLine->m_totalPerHour);
				file.writeInt(iterLine->m_avgPerHour);
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
	m_vThoughputData.clear();

	try
	{
		ArctermFile file;
		file.openFile( _sPath.c_str(), READ);

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

		//skip a blank line
		file.skipLine();

		if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
		{
			//read report data
			CmpThroughputDetailData data;
			int nServedPax;
			while(file.getLine() == 1)
			{
				data.clear();
				file.getTime(data.m_startTime);//get the start time
				file.getTime(data.m_endTime);//get the end time

				for(int i=0; i<nSampleCount; i++)
				{
					file.getInteger(nServedPax);
					data.m_vPaxServed.push_back(nServedPax);
				}
				m_vThoughputData.push_back(data);
			}
		}
		else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
		{
			CmpThroughputSummaryData data;
			for(int i=0; i<nSampleCount; i++)
			{
				data.clear();

				file.getLine();

				// Total Pax
				file.getInteger(data.m_totalPax);

				// Avg Pax
				file.getInteger(data.m_avgPax);

				// Total / Hour
				file.getInteger(data.m_totalPerHour);

				// Avg / Hour
				file.getInteger(data.m_avgPerHour);

				m_vSummary.push_back(data);
				file.closeIn();
			}
		}
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