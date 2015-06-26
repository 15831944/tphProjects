// EventLogBuffer.cpp: implementation of the CEventLogBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <algorithm>
#include <iterator>
#include "LandsideEventLogBuffer.h"

#include "common\progressbar.h"
#include <process.h>
#include <Common/EventBinarySearch.h>
#include "Landside\LandsideVehicleLogEntry.h"
#include "Landside\LandsideVehicleLog.h"
#include "Landside\LandsideVehicleLogEntry.h"
#include "OutputLandside.h"
#include "common/IFloorDataSet.h"
#include "Main/TermPlanDoc.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLandsideEventLogBuffer::CLandsideEventLogBuffer(LandsideVehicleLog* log ) 
{
	m_lSize = 0;
	m_log = log;
	initBuffer();
}

CLandsideEventLogBuffer::~CLandsideEventLogBuffer()
{
	clearData();
}



bool CLandsideEventLogBuffer::checkIfAlive( const LandsideVehicleLogEntry& entry )const
{
	long _birthTime = (long) entry.getEntryTime();
	long _deathTime = (long) entry.getExitTime();

	if( 
	    ( _birthTime >= getStartTime() && _birthTime < getEndTime() ) ||	     
		( _deathTime >= getStartTime() && _deathTime < getEndTime() ) ||
		( _birthTime <  getStartTime()&& _deathTime >=getEndTime() ) 
	)
		return true;

	return false;
}

void CLandsideEventLogBuffer::readData(IFloorDataSet& floorlist)
{
	if(!m_log)return;
	// read data
	LandsideVehicleLogEntry element;
	LandsideVehicleLog& vehicleLog = *m_log;
	int nCount = vehicleLog.getCount();

	// progress bar ( shall use multi_thread )
	CProgressBar bar( "Loading Animation Data", 100, nCount, TRUE, 0 );

	part_event_list mobData;
	for(int i=0; i < nCount; i++ )
	{
		vehicleLog.getItem(element, i);

		if( checkIfAlive( element ) )
		{
			//lLiveMan ++;
			// get the start pos ,end pos( in file) and the number of struct;
			long beginPos	= element.GetVehicleDesc().startPos;
			long endPos		= element.GetVehicleDesc().endPos + sizeof( LandsideVehicleEventStruct ) ;
			long eventNum   = ( endPos-beginPos )/sizeof( LandsideVehicleEventStruct );


			
			//////////////////////////////////////////////////////////////////////////
			// read full event
			// push an empty item to data list.
			fsstream* pLogFile = vehicleLog.getEventLog()->GetEventFile();
			if (pLogFile)
			{				
				part_event_list&  data_in_vector = m_DataList[i];
				try
				{
					//data_in_vector.resize(eventNum);
					mobData.resize(eventNum);
					// read all event to memory			
					if( pLogFile->fail() && !pLogFile->bad() )
					{				
						pLogFile->clear();//all error bits are cleared			
					}

					pLogFile->seekg( beginPos, std::ios::beg );

					//long lPos = logfile.tellg();
					pLogFile->read( (char*)&( mobData[0] ), endPos - beginPos );
					//int iCount = logfile.gcount();
					int nStartPos = BinarySearchEvent(mobData,getStartTime());
					int nEndPos = BinarySearchEvent(mobData,getEndTime());
					
					if (nStartPos == -1 && nEndPos == -1)
					{
						bar.StepIt();
						continue;
					}

					int eventCount = (int)mobData.size();

					nStartPos -= 5;
					nStartPos = (nStartPos < 0 ? 0 : nStartPos);

					if (nEndPos < 0)
					{
						nEndPos = eventCount;
					}
					else
					{
						nEndPos += 5;
						nEndPos =  (nEndPos >=  eventCount? eventCount  : nEndPos);
					}
					
					int nSize = nEndPos - nStartPos;
					if (nSize <= 0)
					{
						bar.StepIt();
						continue;
					}
					data_in_vector.resize(nSize);
					memcpy(&data_in_vector[0],&mobData[nStartPos],sizeof(LandsideVehicleEventStruct)*nSize);
					
					int lCount = data_in_vector.size()*sizeof(LandsideVehicleEventStruct);
					long lSize = getSize() + lCount - 1;
					setSize( lSize );
					//m_lSize += (endPos - beginPos);
					for(size_t i=0;i<data_in_vector.size();i++){
						LandsideVehicleEventStruct& evt = data_in_vector[i];
						evt.z = (float)floorlist.GetVisualHeight(evt.z);
					}
				}
				catch (...)
				{
					//getDataBuffer().pop_back();
				}
				
			}
			
			
		}	
		bar.StepIt();
	}

}


bool CLandsideEventLogBuffer::readDataFromFile( long startTime, long endTime ,CTermPlanDoc* pDoc)
{
	//assert( m_pTerminal );
	if( startTime > endTime )
		return false;

	m_startTime = startTime;
	m_endTime	= endTime;

	clearData();	//clear buffer;
	readData(pDoc->GetFloorByMode(EnvMode_LandSide));
	return true;
}

