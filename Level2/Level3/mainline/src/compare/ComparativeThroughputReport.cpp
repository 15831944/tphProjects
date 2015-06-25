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
	m_vThoughputData.clear();
}


void CComparativeThroughputReport::MergeSample(const ElapsedTime& tInteval)
{
	//clear the old data
	m_vThoughputData.clear();

	ASSERT( m_vSampleRepPaths.size()>0);
	if(m_vSampleRepPaths.size()==0) return;

	ArctermFile file;
	int			nPax=0;
	BOOL		bFound = FALSE;
	CompThroughputData data;

	int			nIndex =0;
	for(std::vector<std::string>::iterator iter=m_vSampleRepPaths.begin(); 
		iter!=m_vSampleRepPaths.end(); iter++, nIndex++)
	{
		try
		{
			if( file.openFile( iter->c_str(), READ) )
			{
				while( file.getLine() )
				{
					bFound = FALSE;
					data.clear();

					//start time
					file.setToField(1);
					file.getTime(data.etStart);

					//end time
					file.setToField(2);
					file.getTime(data.etEnd);

					//q length
					file.setToField(3);
					file.getInteger(nPax);
					data.nPaxServed = nPax;
					
					//PaxServed
					int nSize = (int)m_vThoughputData.size();
					for (unsigned i = 0; i < m_vThoughputData.size(); i++)
					{
						if ((m_vThoughputData[i].etStart == data.etStart) &&
							(m_vThoughputData[i].etEnd == data.etEnd))//if find ,then update the data
						{
							m_vThoughputData[i].nPaxServed += nPax;
							bFound = TRUE;
							break;
						}
					}
					
					if (!bFound)//if not find,then insert the data
					{
						m_vThoughputData.push_back(data);
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
		for(std::vector<CompThroughputData>::const_iterator iterLine=m_vThoughputData.begin(); 
			iterLine != m_vThoughputData.end(); iterLine++)//line
		{
			file.writeTime( iterLine->etStart );//start time
			file.writeTime( iterLine->etEnd );//end time
			file.writeInt(iterLine->nPaxServed);// served passenger count
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
		CompThroughputData data;
		int nQLength = 0;
		while( file.getLine() == 1)
		{
			data.clear();
			file.getTime( data.etStart );//get the start time
			file.getTime( data.etEnd );//get the end time
			for(int n=0; n<nSampleCount; n++)
			{
				file.getInteger( nQLength );
				data.vPaxServed.push_back( nQLength );
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