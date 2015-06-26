// EventLogBuffer.h: interface for the CEventLogBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLOGBUFFER_H__619266C4_348C_4B9E_917F_DADB194C1F14__INCLUDED_)
#define AFX_EVENTLOGBUFFER_H__619266C4_348C_4B9E_917F_DADB194C1F14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <list>
#include "common\term_bin.h"

typedef std::vector< MobEventStruct > part_event_list;
typedef std::pair< long, part_event_list > index_eventlist_pair;
typedef std::list< index_eventlist_pair > PaxEventBuffer;

class MobLogEntry;
class OutputTerminal;

class CEventLogBuffer  
{
public:
	CEventLogBuffer( OutputTerminal* _pTerminal );
	virtual ~CEventLogBuffer();

private:
	CString m_strLogFile;
	OutputTerminal* m_pTerminal;
	long m_startTime;
	long m_endTime;
	PaxEventBuffer m_DataList;
	
	long m_lSize;

	HANDLE m_hCanUseEvent;
	HANDLE m_hWorkThread;
	bool m_bHasClosed;
public:
	HANDLE GetEventHandle(){ return m_hCanUseEvent;}
	OutputTerminal* GetTerminal() { return m_pTerminal ;}


	bool readDataFromFile( long startTime, long endTime );
	
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
	void setFileName( const CString& _strFile ) { m_strLogFile = _strFile;	}
	long getStartTime( void ) const { return m_startTime;	}
	long getEndTime( void ) const { return m_endTime;	}
	CString getFileName( void ) const { return m_strLogFile; }
	PaxEventBuffer& getDataBuffer( void ) { return m_DataList; }
	long getSize( void ) const { return m_lSize; }
	void setSize( long _lSize ){ m_lSize = _lSize;	}
	void CloseThreadHandle();


	void OpenEventLogFile();
	void CloseEventLogFile();

	bool hasLogFile()const;

	std::ifstream* GetLogFile(long idx)const;
private:
	std::vector<std::ifstream*> m_vLogFile;
};

#endif // !defined(AFX_EVENTLOGBUFFER_H__619266C4_348C_4B9E_917F_DADB194C1F14__INCLUDED_)
