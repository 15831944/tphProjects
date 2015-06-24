#include "StdAfx.h"
#include ".\comparativepaxcountreport.h"
#include "..\Common\TERMFILE.H"
#include "..\common\exeption.h"

CComparativePaxCountReport::CComparativePaxCountReport(void)
{
}

CComparativePaxCountReport::~CComparativePaxCountReport(void)
{
}


void CComparativePaxCountReport::MergeSample(const ElapsedTime& tInteval)
{
	m_mapPaxCount.clear();

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
					file.setToField( 1 );
					file.getTime( time );
					//count
					file.setToField( 2 );
					file.getInteger( nLength );

					std::vector<int>& vLengths = m_mapPaxCount[time];
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
			m_mapPaxCount.clear();
			return ;
		}

	}
}

bool CComparativePaxCountReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		//write original sample count
		file.writeField("Comparative Report - Pax Count Report");
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
		for(PaxCountMap::const_iterator iterLine=m_mapPaxCount.begin(); 
			iterLine!=m_mapPaxCount.end(); iterLine++)//line
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

bool CComparativePaxCountReport::LoadReport(const std::string& _sPath)
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapPaxCount.clear();

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
			file.getTime( time );
			std::vector<int>& vLengths = m_mapPaxCount[time];
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
