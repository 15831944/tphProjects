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

}

CComparativeQLengthReport::~CComparativeQLengthReport()
{
	m_mapQLength.clear();
}

void CComparativeQLengthReport::MergeSample(const ElapsedTime& tInteval)
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
bool CComparativeQLengthReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		file.writeField("QLength Report");
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


bool CComparativeQLengthReport::LoadReport(const std::string& _sPath)
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

//#pragma warning(default:4786 4551 4800)
