// ComparativeQTimeReport.cpp: implementation of the CComparativeQTimeReport class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeQTimeReport.h"
#include "Common\TERMFILE.H"
#include "Common\exeption.h"
#include "Main\MultiRunsReportDataLoader.h"

#define TotalTimeCol 1
#define ProcCountCol 3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeQTimeReport::CComparativeQTimeReport()
{

}

CComparativeQTimeReport::~CComparativeQTimeReport()
{
	m_mapQTime.clear();
}

void CComparativeQTimeReport::MergeSampleDetail(const ElapsedTime& tInterval)
{
	//clear the old data
	m_mapQTime.clear();
	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	CmpQTimeVector vQTime;
	std::vector<CmpQTimeVector> vMultiQTimeReports;
	CmpQTime qTime;
	ArctermFile file;

	for(std::vector<std::string>::iterator iter=m_vSampleRepPaths.begin(); iter!=m_vSampleRepPaths.end(); iter++)
	{
		try
		{
			if( file.openFile( iter->c_str(), READ) )
			{
				while( file.getLine() )
				{
					//total time
					file.setToField( TotalTimeCol);
					file.getTime( qTime.totalTime );
					//processor count
					file.setToField( ProcCountCol );
					file.getInteger( qTime.procCount );
					vQTime.push_back( qTime );
				}
				file.closeIn();
			}
			vMultiQTimeReports.push_back( vQTime );
			vQTime.clear();		
		}
		catch(...)
		{
			vMultiQTimeReports.clear();
			return ;
		}
	}

	// find max time
	ElapsedTime t;
	t.set(0, 0, 0);
	for (std::vector<CmpQTimeVector>::const_iterator iterQTime = vMultiQTimeReports.begin(); 
		iterQTime != vMultiQTimeReports.end(); iterQTime++)
	{
		CmpQTimeVector v = *iterQTime;
		for (CmpQTimeVector::const_iterator iterItem = v.begin(); iterItem != v.end(); iterItem++)
			t = max(t, iterItem->totalTime);
	}
 
	//get the time interval number
	int nCount = t.asSeconds() / tInterval.asSeconds();
	nCount = (t.asSeconds() % tInterval.asSeconds()) ? (nCount + 1 ) : nCount;

	//	fill data structure
	t.set(0, 0, 0);
	for (int i = 0; i < nCount; i++)
	{
		t += tInterval;
		std::vector<int>& v = m_mapQTime[t];
		if (v.size() != m_vSampleRepPaths.size())
			v.resize(m_vSampleRepPaths.size(), 0);
	}

	for (unsigned k = 0; k < vMultiQTimeReports.size(); k++)
	{
		CmpQTimeVector v = vMultiQTimeReports[k];
		for (CmpQTimeVector::const_iterator iterItem = v.begin(); iterItem != v.end(); iterItem++)
		{
			std::vector<int>& vi = GetTimePos(iterItem->totalTime, tInterval);
			if(int(vi.size()) > k)
				vi[k]++;
		}
	}
}

bool CComparativeQTimeReport::SaveReportDetail(ArctermFile& file) const
{		
	for(QTimeMap::const_iterator iterLine=m_mapQTime.begin(); iterLine!=m_mapQTime.end(); iterLine++)//line
	{
		file.writeTime( iterLine->first, TRUE );//time
		//queue length
		for(std::vector<int>::const_iterator iterLength=iterLine->second.begin(); iterLength!=iterLine->second.end(); iterLength++)//fields of per model
		{
			file.writeInt( *iterLength );
		}
		file.writeLine();
	}
	return true;
}

bool CComparativeQTimeReport::LoadReportDetail(ArctermFile& file)
{
	//clear old data	
	m_mapQTime.clear();

	int nSampleCount = (int)m_vSimName.size();
	ElapsedTime time(0L);
	int nCount = 0;
	while( file.getLine() == 1)
	{
		file.getTime( time, TRUE );
		std::vector<int>& vLengths = m_mapQTime[time];
		for(int n=0; n<nSampleCount; n++)
		{
			file.getInteger( nCount );
			vLengths.push_back( nCount );
		}
	}	
	return true;
}

std::vector<int>& CComparativeQTimeReport::GetTimePos(const ElapsedTime &t, const ElapsedTime& tDuration)
{
	QTimeMap::iterator iter;
	ElapsedTime tPrev;
	if (t.asSeconds() > tDuration.asSeconds())
	{
		int j = 0;
	}
	for (iter = m_mapQTime.begin(); iter != m_mapQTime.end(); iter++)
	{
		tPrev = iter->first - tDuration;
		if ((t.asSeconds() >= tPrev.asSeconds()) && (t.asSeconds() < iter->first.asSeconds()))
			return iter->second;
	}

	throw new Exception("Function GetTimePos Error.");
	return iter->second;
}

void CComparativeQTimeReport::MergeSample(const ElapsedTime& tInteval)
{
	if(m_cmpParam.GetReportDetail()==REPORT_TYPE_DETAIL)
	{
		return MergeSampleDetail(tInteval);
	}
	else
		return MergeSampleSummary(tInteval);
}

bool CComparativeQTimeReport::SaveReport( const std::string& _sPath )const
{
	ArctermFile file;
	if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

	file.writeField("Comparative Report - QTime Summary Report");
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
	//write m_cmpParam
	file.writeInt(m_cmpParam.GetReportDetail());
	file.writeLine();

	//write summary or detail data
	if(m_cmpParam.GetReportDetail()== REPORT_TYPE_DETAIL)
	{
		SaveReportDetail(file);
	}
	else
	{
		SaveReportSummary(file);
	}

	//
	CTime tm = CTime::GetCurrentTime();
	file.writeLine();
	file.writeField(tm.Format(_T("%d/%m/%Y,%H:%M:%S")));
	file.writeLine();

	file.closeOut();
	return true;
}

bool CComparativeQTimeReport::LoadReport( const std::string& _sPath )
{
	m_vSampleRepPaths.clear();
	m_vSimName.clear();
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
		if (file.getInteger( nSampleCount )==false || nSampleCount<=0)
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
		int ReportType;
		file.getInteger(ReportType);
		m_cmpParam.SetReportDetail((ENUM_REPORT_DETAIL)ReportType);
		file.getLine();
		//
		if(m_cmpParam.GetReportDetail()==REPORT_TYPE_DETAIL)
		{
			LoadReportDetail(file);
		}
		else
		{
			LoadReportSummary(file);
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

void CComparativeQTimeReport::MergeSampleSummary( const ElapsedTime& tInteval )
{
	summaryQTimeList.clear();

	for(std::vector<std::string>::iterator iter=m_vSampleRepPaths.begin(); iter!=m_vSampleRepPaths.end(); iter++)
	{
		try
		{
			std::string filename = *iter;
			MultiRunsReport::Summary::SummaryQTimeLoader loader(filename.c_str());
			loader.LoadData();
			MultiRunsReport::Summary::SummaryQTimeLoader::DataList& data = loader.GetData();
			summaryQTimeList.push_back(data);
		
		}
		catch(...)
		{			
			return ;
		}
	}	
}

#define SUMMARYDATAPAX_START _T("---summary pax data---")

bool CComparativeQTimeReport::SaveReportSummary( ArctermFile& file) const
{
	
	file.writeInt( (int)summaryQTimeList.size() );//time
	file.writeLine();
	//write data
	for(size_t i=0;i<summaryQTimeList.size();i++)//line
	{		
		
		//queue length
		size_t nPaxCount = summaryQTimeList[i].size();
		file.writeInt((int)nPaxCount);file.writeLine();
		for(size_t j=0;j<nPaxCount;j++)
		{
			const MultiRunsReport::Summary::SummaryQueueTimeValue& data = summaryQTimeList[i][j];
			file.writeField(data.strPaxType);
			file.writeInt(data.eMinimum.getPrecisely());
			file.writeInt(data.eAverage.getPrecisely());
			file.writeInt(data.eMaximum.getPrecisely());
			file.writeInt(data.nCount);
			file.writeInt(data.eQ1.getPrecisely());
			file.writeInt(data.eQ2.getPrecisely());
			file.writeInt(data.eQ3.getPrecisely());
			file.writeInt(data.eP1.getPrecisely());
			file.writeInt(data.eP10.getPrecisely());
			file.writeInt(data.eP90.getPrecisely());
			file.writeInt(data.eP95.getPrecisely());
			file.writeInt(data.eP99.getPrecisely());
			file.writeInt(data.eSigma.getPrecisely());
			
		}
		file.writeLine();
	}
	return true;
}

bool CComparativeQTimeReport::LoadReportSummary( ArctermFile& file )
{

	//data
	int nDataSize=0;
	file.getInteger(nDataSize);
	file.getLine();
	for(int i=0;i<nDataSize;i++)
	{
		int nPaxCount =0;
		file.getInteger(nPaxCount);file.getLine();
		std::vector< MultiRunsReport::Summary::SummaryQueueTimeValue> vdata;
		for(int j=0;j<nPaxCount;j++)
		{
				MultiRunsReport::Summary::SummaryQueueTimeValue data;
				TCHAR strbuf[256];
				file.getField(strbuf,256);
				data.strPaxType = strbuf;
				int t=0;
				file.getInteger(t); data.eMinimum.setPrecisely(t);
				file.getInteger(t); data.eAverage.setPrecisely(t);
				file.getInteger(t); data.eMaximum.setPrecisely(t);
				file.getInteger(t); data.nCount = t;
				file.getInteger(t); data.eQ1.setPrecisely(t);
				file.getInteger(t); data.eQ2.setPrecisely(t);
				file.getInteger(t); data.eQ3.setPrecisely(t);
				file.getInteger(t); data.eP1.setPrecisely(t);
				file.getInteger(t); data.eP10.setPrecisely(t);
				file.getInteger(t); data.eP90.setPrecisely(t);
				file.getInteger(t); data.eP95.setPrecisely(t);
				file.getInteger(t); data.eP99.setPrecisely(t);
				file.getInteger(t); data.eSigma.setPrecisely(t);
				vdata.push_back(data);
		}
		file.getLine();
		summaryQTimeList.push_back(vdata);
	}

	

	return true;
}
