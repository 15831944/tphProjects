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
						CmpThroughputData newData;
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
	for(int i=0; i<nSampleCount; i++)
	{
		try
		{
			if(file.openFile(m_vSampleRepPaths[i].c_str(), READ))
			{
				ElapsedTime startTime, endTime;
				int n1, n2, n3, n4=0;

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
					file.getInteger(n1);

					//served passenger count
					file.setToField(4);
					file.getInteger(n2);

					//served passenger count
					file.setToField(5);
					file.getInteger(n3);

					//served passenger count
					file.setToField(6);
					file.getInteger(n4);

					for (unsigned j = 0; j < m_vThoughputData.size(); j++)
					{
						if ((m_vThoughputData[i].m_startTime == startTime) &&
							(m_vThoughputData[i].m_endTime == endTime))
						{
							m_vThoughputData[j].m_v1[i] += n1;
							m_vThoughputData[j].m_v2[i] += n4;
							m_vThoughputData[j].m_v3[i] += n3;
							m_vThoughputData[j].m_v4[i] += n2;
							bFound = TRUE;
							break;
						}
					}

					if (!bFound)//if not find, then insert the data
					{
						CmpThroughputData newData;
						newData.m_startTime = startTime;
						newData.m_endTime = endTime;
						newData.m_v1 = std::vector<int>(nSampleCount, 0);
						newData.m_v2 = std::vector<int>(nSampleCount, 0);
						newData.m_v3 = std::vector<int>(nSampleCount, 0);
						newData.m_v4 = std::vector<int>(nSampleCount, 0);
						newData.m_v1[i] += n1;
						newData.m_v2[i] += n2;
						newData.m_v3[i] += n3;
						newData.m_v4[i] += n4;
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
		for(std::vector<CmpThroughputData>::const_iterator iterLine=m_vThoughputData.begin(); 
			iterLine != m_vThoughputData.end(); iterLine++)//line
		{
			file.writeTime( iterLine->m_startTime );//start time
			file.writeTime( iterLine->m_endTime );//end time
			//passenger served.
			int nCount = static_cast<int>(iterLine->m_vPaxServed.size());
			ASSERT(nCount == nSampleCount);
			char buf1[1024] = {0}, buf2[16] = {0};
			if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				for(int i=0; i<nCount; i++)
				{
					itoa(iterLine->m_vPaxServed[i], buf2, 10);
					strcat(buf1, buf2);
					strcat(buf1, ";");
				}
				file.writeField(buf1);
			}
			else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
			{
				for(int i=0; i<nCount; i++)
				{
					itoa(iterLine->m_v1[i], buf2, 10);
					strcat(buf1, buf2);
					strcat(buf1, ";");
				}
				file.writeField(buf1);

				for(int i=0; i<nCount; i++)
				{
					itoa(iterLine->m_v2[i], buf2, 10);
					strcat(buf1, buf2);
					strcat(buf1, ";");
				}
				file.writeField(buf1);

				for(int i=0; i<nCount; i++)
				{
					itoa(iterLine->m_v3[i], buf2, 10);
					strcat(buf1, buf2);
					strcat(buf1, ";");
				}
				file.writeField(buf1);

				for(int i=0; i<nCount; i++)
				{
					itoa(iterLine->m_v4[i], buf2, 10);
					strcat(buf1, buf2);
					strcat(buf1, ";");
				}
				file.writeField(buf1);
			}
			file.writeLine();
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

		//read report data
		CmpThroughputData data;
		int nServedPax ,n1 ,n2 ,n3 ,n4;
		while( file.getLine() == 1)
		{
			data.clear();
			file.getTime(data.m_startTime);//get the start time
			file.getTime(data.m_endTime);//get the end time
			char buf1[16] = {0};
			if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				for(int n=0; n<nSampleCount; n++)
				{
					file.getSubField(buf1, ';');
					data.m_vPaxServed.push_back(atoi(buf1));
				}
			}
			else if(m_cmpParam.GetReportDetail() == REPORT_TYPE_SUMMARY)
			{
				for(int n=0; n<nSampleCount; n++)
				{
					file.getSubField(buf1, ';');
					data.m_v1.push_back(atoi(buf1));
				}
				for(int n=0; n<nSampleCount; n++)
				{
					file.getSubField(buf1, ';');
					data.m_v2.push_back(atoi(buf1));
				}
				for(int n=0; n<nSampleCount; n++)
				{
					file.getSubField(buf1, ';');
					data.m_v3.push_back(atoi(buf1));
				}
				for(int n=0; n<nSampleCount; n++)
				{
					file.getSubField(buf1, ';');
					data.m_v4.push_back(atoi(buf1));
				}
			}
			m_vThoughputData.push_back(data);
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