#include "StdAfx.h"
#include "ComparativSpaceThroughputReport.h"
#include "Common/exeption.h"

CComparativSpaceThroughputReport::CComparativSpaceThroughputReport(void)
{
}


CComparativSpaceThroughputReport::~CComparativSpaceThroughputReport(void)
{
}

void CComparativSpaceThroughputReport::MergeSample(const ElapsedTime& tInteval)
{
	//clear the old data
	m_mapPaxThroughput.clear();

	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;
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
					file.setToField( 1 );
					TimeRange timeRange;
					ElapsedTime eStartTime;
					ElapsedTime eEndTime;
					file.getTime( eStartTime );
					file.getTime(eEndTime);
					timeRange.SetStartTime(eStartTime);
					timeRange.SetEndTime(eEndTime);
					//count
					file.setToField( 3 );
					file.getInteger( nLength );

					std::vector<int>& vLengths = m_mapPaxThroughput[timeRange];
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
			m_mapPaxThroughput.clear();
			return ;
		}

	}
}

bool CComparativSpaceThroughputReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		//write original sample count
		file.writeField("Comparative Report - Space Density Report");
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
		for(PaxThroughputMap::const_iterator iterLine=m_mapPaxThroughput.begin(); 
			iterLine!=m_mapPaxThroughput.end(); iterLine++)//line
		{
			file.writeTime( iterLine->first.GetStartTime() );//time
			file.writeTime(iterLine->first.GetEndTime());
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

bool CComparativSpaceThroughputReport::LoadReport(const std::string& _sPath)
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapPaxThroughput.clear();

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
		int nQLength = 0;
		while( file.getLine() == 1)
		{
			ElapsedTime eStartTime;
			ElapsedTime eEndTime;
			file.getTime( eStartTime );
			file.getTime(eEndTime);
			std::vector<int>& vLengths = m_mapPaxThroughput[TimeRange(eStartTime,eEndTime)];
			for(int n=0; n<nSampleCount; n++)
			{
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

CString CComparativSpaceThroughputReport::GetFooter( int iSubType ) const
{
	CString strFooter;
	strFooter.Format(_T("Comparative Report Density Throughput(%s) %s%s"),GetModelName(),m_cmpParam.GetStartTime().printTime(),m_cmpParam.GetEndTime().printTime());
	return strFooter;
}
