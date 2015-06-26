// EventLogBufManager.h: interface for the CEventLogBufManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLOGBUFMANAGER_H__388108E0_0266_4C75_BAF0_BFD74CFDABE5__INCLUDED_)
#define AFX_EVENTLOGBUFMANAGER_H__388108E0_0266_4C75_BAF0_BFD74CFDABE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "assert.h"
#include "EventLogBuffer.h"
class OutputTerminal;
class CTermPlanDoc;
class CEventLogBufManager  
{
public:
	CEventLogBufManager( OutputTerminal* _pTerminal );
	virtual ~CEventLogBufManager();
	
	enum animation_play_mode { step_by_step, pick_time };
	
	enum ANIMATION_MODE { ANIMATION_FORWARD, ANIMATION_BACKWARD };

private:
	CEventLogBuffer* m_pCurrentLogBuffer;

	CEventLogBuffer* m_pFirstBuffer;
	CEventLogBuffer* m_pSecondBuffer;
	bool m_bUseFirstBuffer;
	
	long m_detaTime;
	animation_play_mode	 play_mode;
	OutputTerminal* m_pTerminal;

	bool m_bFirstTimeToLoadData;
	
	
public:
	// get data buffer
	const PaxEventBuffer& getLogDataBuffer( void ) const { return m_pCurrentLogBuffer->getDataBuffer();	}
	
	// just if need to read data.
	bool loadDataIfNecessary( long _lCurTime, ANIMATION_MODE _animMode,CTermPlanDoc* pTermDoc );

	// clear log  buffer
	void clearLogBuffer( void );

	//set & get
		void setDetaTime( long _time ) { assert( _time >0); m_detaTime = _time; }
	long getDetaTime( void ) const { return m_detaTime; } 
	void setPlayMode( animation_play_mode _mode ) { play_mode = _mode; }
	animation_play_mode getPlayMode( void ) const { return play_mode; }
	void setDetaTimeBaseFileSize( const CString& _strFile );
	void InitBuffer();  
private:
	void setPaxEventLogFilePath( const CString& _strFile ) ;
};

#endif // !defined(AFX_EVENTLOGBUFMANAGER_H__388108E0_0266_4C75_BAF0_BFD74CFDABE5__INCLUDED_)
