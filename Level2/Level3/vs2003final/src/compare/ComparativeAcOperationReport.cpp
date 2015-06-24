#include "StdAfx.h"
#include ".\comparativeacoperationreport.h"
#include "Common\TERMFILE.H"
#include "common\exeption.h"

CComparativeAcOperationReport::CComparativeAcOperationReport(void)
{
}

CComparativeAcOperationReport::~CComparativeAcOperationReport(void)
{
}



void CComparativeAcOperationReport::MergeSample(const ElapsedTime& tInterval)
{
	
	m_mapAcOperation.clear();
	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	CmpAcOperationVector vAcOperation;//save AcOperation report data of one model when read file
	std::vector<CmpAcOperationVector> vAcOperationReports;//save all models' AcOperation report data when read file
	CmpAcOperation acOperation;
	ArctermFile file;

	ElapsedTime curTime;

	for(std::vector<std::string>::iterator iter=m_vSampleRepPaths.begin(); iter!=m_vSampleRepPaths.end(); iter++)
	{
		try
		{
			if( file.openFile( iter->c_str(), READ) )
			{
				int testco = 0;

				while( file.getLine() )
				{
					testco ++;
					//total time
					curTime.set(0L);
					file.setToField(4);//Schedule Arriving Time
					acOperation.totalTime.set(0,0,0);
					file.getTime( acOperation.totalTime );
					if(acOperation.totalTime == 0L)
					{
						file.setToField(5);//Schedule Departing Time
						file.getTime( acOperation.totalTime );
					}	

					if( acOperation.totalTime >= m_ReportStartTime &&
						acOperation.totalTime <= m_ReportEndTime )
					{
					//processor count
						file.setToField( 1 );
						file.getField( acOperation.procCount, 30);
						vAcOperation.push_back( acOperation );
					}

				}
				file.closeIn();
			}
			vAcOperationReports.push_back( vAcOperation );
			vAcOperation.clear();		
		}
		catch(...)
		{
			vAcOperationReports.clear(); 
			return ;
		}
	}

	// find the max time in all model data
	ElapsedTime t;
	t.set(0, 0, 0);
	for (std::vector<CmpAcOperationVector>::const_iterator iterAcOperation = vAcOperationReports.begin(); 
		iterAcOperation != vAcOperationReports.end(); iterAcOperation++)
	{
		CmpAcOperationVector v = *iterAcOperation;
		for (CmpAcOperationVector::const_iterator iterItem = v.begin(); iterItem != v.end(); iterItem++)
			t = max(t, iterItem->totalTime);
	}

	//calculate the interval number from start time to the max time 
	ElapsedTime timewid = t - m_ReportStartTime;
	int nCount ;
	if( 0 == tInterval.asSeconds() )
	{
		ElapsedTime tmpt;
		tmpt.set(0,0,1);
		nCount = timewid.asSeconds() / tmpt.asSeconds();
		nCount = (timewid.asSeconds() % tmpt.asSeconds()) ? (nCount + 1 ) : nCount;
	}
	else
	{
		nCount = timewid.asSeconds() / tInterval.asSeconds();
		nCount = (timewid.asSeconds() % tInterval.asSeconds()) ? (nCount + 1 ) : nCount;
	}
	if( timewid < ElapsedTime(1l) )
		nCount = 0;

	//	fill data structure
	t = m_ReportStartTime;
	for (int i = 0; i < nCount; i++)
	{
		t += tInterval;
		std::vector<int>& v = m_mapAcOperation[t];
		if (v.size() != m_vSampleRepPaths.size())
			v.resize(m_vSampleRepPaths.size(), 0);
	}

	for (unsigned k = 0; k < vAcOperationReports.size(); k++)
	{
		CmpAcOperationVector v = vAcOperationReports[k];//the k th model's AcOperation report data
		for (CmpAcOperationVector::const_iterator iterItem = v.begin(); iterItem != v.end(); iterItem++)
		{
			std::vector<int>& vi = GetTimePos(iterItem->totalTime, tInterval);
			if(int(vi.size()) > k) vi[k]++;
		}
	}

}

bool CComparativeAcOperationReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		file.writeField("AcOperation Report");
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

		for(AcOperationMap::const_iterator iterLine=m_mapAcOperation.begin(); iterLine!=m_mapAcOperation.end(); iterLine++)//line
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

bool CComparativeAcOperationReport::LoadReport(const std::string& _sPath)
{
	//clear old data
	m_vSampleRepPaths.clear();
	m_mapAcOperation.clear();

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

		//get the AcOperation report data
		ElapsedTime time(0L);
		int nCount = 0;
		while( file.getLine() == 1)
		{
			file.getTime( time, TRUE );
			std::vector<int>& vLengths = m_mapAcOperation[time];
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


std::vector<int>& CComparativeAcOperationReport::GetTimePos(const ElapsedTime &t, const ElapsedTime& tDuration)
{
	AcOperationMap::iterator iter;
	ElapsedTime tPrev;
//	if (t.asSeconds() > tDuration.asSeconds())
//	{
//		int j = 0;
//	}
	int i = 0;
	for (iter = m_mapAcOperation.begin(); iter != m_mapAcOperation.end(); iter++, i++)
	{
		tPrev = iter->first - tDuration;
		if ((t.asSeconds() >= tPrev.asSeconds()) && (t.asSeconds() < iter->first.asSeconds()))
			return iter->second;
	}

	throw new exception("Function GetTime1Pos Error.");
	return iter->second;
}

