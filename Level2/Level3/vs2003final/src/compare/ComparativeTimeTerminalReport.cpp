#include "StdAfx.h"
#include ".\comparativetimeterminalreport.h"
#include "Common\TERMFILE.H"
#include "Common\exeption.h"


#define TotalTimeCol 1
#define ProcCountCol 3
CComparativeTimeTerminalReport::CComparativeTimeTerminalReport(void)
{
}

CComparativeTimeTerminalReport::~CComparativeTimeTerminalReport(void)
{
}
void CComparativeTimeTerminalReport::MergeSample(const ElapsedTime& tInterval)
{	
	//clear the old data
	m_mapTimeTerminal.clear();
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
		std::vector<int>& v = m_mapTimeTerminal[t];
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
bool CComparativeTimeTerminalReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		file.writeField("TimeTerminal Report");
		file.writeLine();

		int nSampleCount = m_vSampleRepPaths.size();

		//write original sample count
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
		for(QTimeMap::const_iterator iterLine=m_mapTimeTerminal.begin(); iterLine!=m_mapTimeTerminal.end(); iterLine++)//line
		{
			file.writeTime( iterLine->first, TRUE );//time
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
bool CComparativeTimeTerminalReport::LoadReport(const std::string& _sPath)
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapTimeTerminal.clear();

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
		int nCount = 0;
		while( file.getLine() == 1)
		{
			file.getTime( time, TRUE );
			std::vector<int>& vLengths = m_mapTimeTerminal[time];
			for(int n=0; n<nSampleCount; n++)
			{
				file.getInteger( nCount );
				vLengths.push_back( nCount );
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

std::vector<int>& CComparativeTimeTerminalReport::GetTimePos(const ElapsedTime &t, const ElapsedTime& tDuration)
{
	QTimeMap::iterator iter;
	ElapsedTime tPrev;
	if (t.asSeconds() > tDuration.asSeconds())
	{
		int j = 0;
	}
	for (iter = m_mapTimeTerminal.begin(); iter != m_mapTimeTerminal.end(); iter++)
	{
		tPrev = iter->first - tDuration;
		if ((t.asSeconds() >= tPrev.asSeconds()) && (t.asSeconds() < iter->first.asSeconds()))
			return iter->second;
	}

	throw new exception("Function GetTimePos Error.");
	return iter->second;
}