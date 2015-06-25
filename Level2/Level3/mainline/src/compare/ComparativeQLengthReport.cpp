// ComparativeQLengthReport.cpp: implementation of the CComparativeQLengthReport class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786 4551 4800)

#include "StdAfx.h"
#include "Common\TERMFILE.H"
#include "ComparativeQLengthReport.h"
#include "common\exeption.h"
#define TimeColumn		1
#define LengthColumn	2
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeQLengthReport::CComparativeQLengthReport()
{
	m_mapQLength.clear();
}

CComparativeQLengthReport::~CComparativeQLengthReport()
{
}

void CComparativeQLengthReport::MergeSample(const ElapsedTime& tInteval)
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
bool CComparativeQLengthReport::SaveReport(const std::string& _sPath) const
{
	if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
	{
		return SaveDetailReport(_sPath);
	}

		
	return SaveSummaryReport(_sPath);
	

}


bool CComparativeQLengthReport::LoadReport(const std::string& _sPath)
{
	if(m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
	{
		return LoadDetailReport(_sPath);
	}


	return LoadSummaryReport(_sPath);
}

void CComparativeQLengthReport::MergeDetailSample( const ElapsedTime& tInteval )
{
	//clear the old data
	m_mapQLength.clear();

	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;
	ElapsedTime time(0L);
	int			nLength=0;

	int			nIndex =0;
	for(std::vector<std::string>::iterator iter=m_vSampleRepPaths.begin(); iter!=m_vSampleRepPaths.end(); iter++, nIndex++)
	{
		try
		{
			if( file.openFile( iter->c_str(), READ) )
			{
				while( file.getLine() )
				{
					//time
					file.setToField( TimeColumn);
					file.getTime( time );
					//q length
					file.setToField( LengthColumn );
					file.getInteger( nLength );

					std::vector<int>& vLengths = m_mapQLength[time];
					if( vLengths.size()!= m_vSampleRepPaths.size())
					{
						vLengths.resize( m_vSampleRepPaths.size(), 0);
					}
					vLengths[nIndex] += nLength;
				}
				file.closeIn();
			}
		}
		catch(...)
		{
			m_mapQLength.clear();
			return ;
		}

	}
}

void CComparativeQLengthReport::MergeSummarySample( const ElapsedTime& tInteval )
{
	//clear the old data
	m_mapQLength.clear();

	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;
	ElapsedTime time(0L);
	int			nLength=0;

	int			nIndex =0;
	for(std::vector<std::string>::iterator iter=m_vSampleRepPaths.begin(); iter!=m_vSampleRepPaths.end(); iter++, nIndex++)
	{
		try
		{
			if( file.openFile( iter->c_str(), READ) )
			{
				while( file.getLine() )
				{
					//time
					//QLengthSummary summaryMap;
					//summaryMap.m_strModelName = m_vSimName[nIndex];
					file.setToField( 2);
					file.getTime( time );
					
					file.setToField( 3 );
					std::vector<QueueLengthGroup>& vLengths = m_mapSummaryQLength[time];
					if( vLengths.size()!= m_vSampleRepPaths.size())
					{
						vLengths.resize( m_vSampleRepPaths.size());
					}
					vLengths[nIndex].m_strModelName = m_vSimName[nIndex];
					file.getInteger( vLengths[nIndex].m_nMinValue );
					file.getInteger( vLengths[nIndex].m_nMaxValue );
					file.getInteger( vLengths[nIndex].m_nAvaValue );
					file.getInteger(vLengths[nIndex].m_nTotalValue );
				}
				file.closeIn();
			}
		}
		catch(...)
		{
			m_mapQLength.clear();
			return ;
		}

	}
}

bool CComparativeQLengthReport::SaveDetailReport( const std::string& _sPath ) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		file.writeField("Comparative Report - QLength Report");
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
		for(QLengthMap::const_iterator iterLine=m_mapQLength.begin(); iterLine!=m_mapQLength.end(); iterLine++)//line
		{
			file.writeTime( iterLine->first );//time
			//queue length
			for(std::vector<int>::const_iterator iterLength=iterLine->second.begin(); iterLength!=iterLine->second.end(); iterLength++)//fields of per model
			{
				file.writeInt( *iterLength );
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

bool CComparativeQLengthReport::SaveSummaryReport( const std::string& _sPath )const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		file.writeField("Comparative Report - QLength Report");
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
		for(QLengthSummaryMap::const_iterator iterLine=m_mapSummaryQLength.begin(); iterLine!=m_mapSummaryQLength.end(); iterLine++)//line
		{
			file.writeTime( iterLine->first );//time

		
			//queue length
			for(std::vector<QueueLengthGroup>::const_iterator iterLength=iterLine->second.begin(); iterLength!=iterLine->second.end(); iterLength++)//fields of per model
			{
				file.writeField(iterLength->m_strModelName);
				file.writeInt(iterLength->m_nMinValue);
				file.writeInt(iterLength->m_nMaxValue);
				file.writeInt(iterLength->m_nAvaValue);
				file.writeInt(iterLength->m_nTotalValue);
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

bool CComparativeQLengthReport::LoadDetailReport( const std::string& _sPath )
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapQLength.clear();

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
		//skip a blank line
		file.skipLine();

		//read report data
		ElapsedTime time(0L);
		int nQLength = 0;
		while( file.getLine() == 1)
		{
			//read the time
			file.getTime( time );
			std::vector<int>& vLengths = m_mapQLength[time];
			for(int n=0; n<nSampleCount; n++)
			{
				//read the Q length
				file.getInteger( nQLength );
				vLengths.push_back( nQLength );
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

bool CComparativeQLengthReport::LoadSummaryReport( const std::string& _sPath )
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapQLength.clear();

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
		//skip a blank line
		file.skipLine();

		//read report data
		ElapsedTime time(0L);
		int nQLength = 0;
		while( file.getLine() == 1)
		{
			//read the time
			file.getTime( time );
			std::vector<QueueLengthGroup>& vLengths = m_mapSummaryQLength[time];
			for(int n=0; n<nSampleCount; n++)
			{
				//read the Q length
				QueueLengthGroup lengthGroup;
				file.getField(lengthGroup.m_strModelName.GetBuffer(MAX_PATH),MAX_PATH);
				lengthGroup.m_strModelName.ReleaseBuffer();
				//file.skipField(1);
				file.getInteger(lengthGroup.m_nMinValue );
				file.getInteger( lengthGroup.m_nMaxValue );
				file.getInteger( lengthGroup.m_nAvaValue );
				file.getInteger(lengthGroup.m_nTotalValue );
				vLengths.push_back( lengthGroup );
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

//#pragma warning(default:4786 4551 4800)
