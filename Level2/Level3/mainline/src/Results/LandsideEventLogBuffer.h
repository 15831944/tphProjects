// EventLogBuffer.h: interface for the CEventLogBuffer class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <list>
#include <map>
#include "../common/LANDSIDE_BIN.h"

class MobLogEntry;
class OutputLandside;
class LandsideVehicleLogEntry;
class LandsideVehicleLog;
class IFloorDataSet;
class CTermPlanDoc; 

class CLandsideEventLogBuffer  
{
public:
	typedef std::vector< LandsideVehicleEventStruct > part_event_list;
	//typedef std::pair< long, part_event_list > index_eventlist_pair;
	typedef std::map< long, part_event_list > VehicleEventBuffer;

	CLandsideEventLogBuffer(LandsideVehicleLog* log);
	virtual ~CLandsideEventLogBuffer();


	bool checkIfAlive( const LandsideVehicleLogEntry& entry)const;
 
	part_event_list& getEventList(int idx){ return m_DataList[idx]; }
private:
	long m_startTime;
	long m_endTime;
	VehicleEventBuffer m_DataList;
	
	long m_lSize;
	
	LandsideVehicleLog* m_log;
	
public:

	bool readDataFromFile( long startTime, long endTime,CTermPlanDoc* pDoc );
	
	// init buffer
	void initBuffer( void )
	{
		m_startTime = -1l;
		m_endTime = -1l;
		clearData();
	}

	// clear data
	void clearData( void )
	{
		m_DataList.clear();
		m_lSize = 0l;
	}

	// get & set
	//void setStartTime( long _time ) { m_startTime = _time; }
	//void setEndTime( long _time ) { m_endTime = _time;	}
	//void setFileName( const CString& _strFile ) { m_strLogFile = _strFile;	}
	long getStartTime( void ) const { return m_startTime;	}
	long getEndTime( void ) const { return m_endTime;	}
	//CString getFileName( void ) const { return m_strLogFile; }
	VehicleEventBuffer& getDataBuffer( void ) { return m_DataList; }
	long getSize( void ) const { return m_lSize; }
	void setSize( long _lSize ){ m_lSize = _lSize;	}

	
	//need convert alt to visual alt
	void readData(IFloorDataSet& floorlist);
	//void OpenEventLogFile();
	//void CloseEventLogFile();

	

//	std::ifstream* GetLogFile(long idx)const;
//private:
//	std::vector<std::ifstream*> m_vLogFile;
//	OutputLandside* m_pOutLandside;

};

