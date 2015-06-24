#include "StdAfx.h"
#include ".\comparativedistancetravelreport.h"
#include "Common\TERMFILE.H"
#include "Common\exeption.h"

#define TotalDistanceCol 1
#define ProcCountCol 3


CComparativeDistanceTravelReport::CComparativeDistanceTravelReport(void)
{
}

CComparativeDistanceTravelReport::~CComparativeDistanceTravelReport(void)
{
}
void CComparativeDistanceTravelReport::MergeSample(const ElapsedTime& tInteval)
{
	ASSERT(0);
}
void CComparativeDistanceTravelReport::MergeSample(const long& tInterval)
{

	//clear the old data
	m_mapDistance.clear();
	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	CmpDistanceVector vDistance;
	std::vector<CmpDistanceVector> vMultiDistanceReports;
	CmpDistance qDistance;
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
					file.setToField( TotalDistanceCol);
					file.getInteger( qDistance.totalDistance );
					//processor count
					//file.setToField( ProcCountCol );
					//file.getInteger( qTime.procCount );
					vDistance.push_back( qDistance );

				}
				file.closeIn();
			}
			vMultiDistanceReports.push_back( vDistance );
			vDistance.clear();		
		}
		catch(...)
		{
			vMultiDistanceReports.clear();
			return ;
		}
	}

	// find max distance
	long t = 0;

	for (std::vector<CmpDistanceVector>::const_iterator iterDistance = vMultiDistanceReports.begin(); 
		iterDistance != vMultiDistanceReports.end(); iterDistance++)
	{
		CmpDistanceVector v = *iterDistance;
		for (CmpDistanceVector::const_iterator iterItem = v.begin(); iterItem != v.end(); iterItem++)
			t = max(t, iterItem->totalDistance);
	}

	//get the distance interval number
	int nCount = t / tInterval;
	nCount = (t % tInterval) ? (nCount + 1 ) : nCount;

	//	fill data structure
	t = 0;
	for (int i = 0; i < nCount; i++)
	{
		t += tInterval;
		std::vector<int>& v = m_mapDistance[t];
		if (v.size() != m_vSampleRepPaths.size())
			v.resize(m_vSampleRepPaths.size(), 0);
	}

	for (unsigned k = 0; k < vMultiDistanceReports.size(); k++)
	{
		CmpDistanceVector v = vMultiDistanceReports[k];
		for (CmpDistanceVector::const_iterator iterItem = v.begin(); iterItem != v.end(); iterItem++)
		{
			std::vector<int>& vi = GetDistancePos(iterItem->totalDistance  , tInterval);
			if(int(vi.size()) > k)
				vi[k]++;
		}
	}

}
bool CComparativeDistanceTravelReport::SaveReport(const std::string& _sPath) const
{
	try
	{
		ArctermFile file;
		if(file.openFile( _sPath.c_str(),WRITE)==false) return false;

		file.writeField("Comparative Report - Distance Report");
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
		for(DistanceMap::const_iterator iterLine=m_mapDistance.begin(); iterLine!=m_mapDistance.end(); iterLine++)//line
		{
			file.writeInt( iterLine->first );//distance
			//count
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
bool CComparativeDistanceTravelReport::LoadReport(const std::string& _sPath)
{
	m_vSampleRepPaths.clear();
	m_mapDistance.clear();

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
		long distance(0L);
		int nCount = 0;
		while( file.getLine() == 1)
		{
			file.getInteger( distance );
			std::vector<int>& vLengths = m_mapDistance[distance];
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

std::vector<int>& CComparativeDistanceTravelReport::GetDistancePos(const long& t, const long& tDistance)
{
	DistanceMap::iterator iter;
	long tPrev;
	if (t > tDistance)
	{
		int j = 0;
	}
	for (iter = m_mapDistance.begin(); iter != m_mapDistance.end(); iter++)
	{
		tPrev = iter->first - tDistance;
		if ((t >= tPrev) && (t <= iter->first))
			return iter->second;
	}

	throw new Exception("Function GetTimePos Error.");
	return iter->second;

}
