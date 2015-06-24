// ComparativeSpaceDensityReport.cpp: implementation of the CComparativeSpaceDensityReport class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeSpaceDensityReport.h"
#include "Common\TERMFILE.H"
#include "common\exeption.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeSpaceDensityReport::CComparativeSpaceDensityReport()
{

}

CComparativeSpaceDensityReport::~CComparativeSpaceDensityReport()
{
	m_mapPaxDens.clear();
}

void CComparativeSpaceDensityReport::MergeSample(const ElapsedTime& tInteval)
{
	//clear the old data
	m_mapPaxDens.clear();

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

					std::vector<int>& vLengths = m_mapPaxDens[time];
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
			m_mapPaxDens.clear();
			return ;
		}

	}
}

bool CComparativeSpaceDensityReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;
		
		int nSampleCount = m_vSampleRepPaths.size();

		//write original sample count
		file.writeField("Comparative Report");
		file.writeLine();
		file.writeInt( nSampleCount );
		file.writeLine();

		//write original sample path
		for( std::vector<std::string>::const_iterator iterPath=m_vSampleRepPaths.begin(); iterPath!=m_vSampleRepPaths.end(); iterPath++)
		{
			file.writeField( iterPath->c_str() );
			file.writeLine();
		}
		file.writeLine();

		//write data lines
		for(PaxDensMap::const_iterator iterLine=m_mapPaxDens.begin(); 
			iterLine!=m_mapPaxDens.end(); iterLine++)//line
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

bool CComparativeSpaceDensityReport::LoadReport(const std::string& _sPath)
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapPaxDens.clear();

	//check if file exist
	/*
	HANDLE hFileFind = ::FindFirstFile( _sPath.c_str(), NULL );
	if( hFileFind == INVALID_HANDLE_VALUE )
	{
		throw FileNotFoundError( _sPath.c_str() );
	}
	::FindClose( hFileFind );
	*/

	try
	{
		ArctermFile file;
		file.openFile( _sPath.c_str(), READ);
		//get model number
		int nSampleCount =0;

		if (file.getInteger( nSampleCount )==false || nSampleCount<=0)
			return false;


		char sSamplePath[MAX_PATH]="";
		//get sample file name
		for(int i=0; i<nSampleCount; i++)
		{
			file.getLine();
			file.getField(sSamplePath, MAX_PATH);
			m_vSampleRepPaths.push_back(std::string(sSamplePath));
		}
		//skip a blank line
		file.skipLine();

		//read report data
		ElapsedTime time(0L);
		int nQLength = 0;
		while( file.getLine() == 1)
		{
			file.getTime( time );
			std::vector<int>& vLengths = m_mapPaxDens[time];
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