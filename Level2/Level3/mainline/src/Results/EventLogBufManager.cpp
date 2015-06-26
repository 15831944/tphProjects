// EventLogBufManager.cpp: implementation of the CEventLogBufManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventLogBufManager.h"
#include "out_term.h"
#include <iostream>
#include <fstream>
#include "main\TermPlanDoc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ONE_HOUR			(3600*100)			//1  hour
#define DEFAULT_DETA_TIME	(20*60*100)		//2  hours
//#define MAX_TIME			(24*3600*100)		//24 hours
#define BUFFER_TIME			(600*100)			//10 mins
#define HUNDRED_M			(100*1024*1024)		//100 M

CEventLogBufManager::CEventLogBufManager( OutputTerminal* _pTerminal ) 
					:play_mode( step_by_step ), m_pTerminal( _pTerminal ), m_detaTime( DEFAULT_DETA_TIME )
{
	m_pFirstBuffer = /*new CEventLogBuffer( m_pTerminal )*/NULL;		
	m_pSecondBuffer = /*new CEventLogBuffer( m_pTerminal )*/NULL;
	m_pCurrentLogBuffer = /*m_pFirstBuffer*/new CEventLogBuffer( m_pTerminal );
	m_bFirstTimeToLoadData = true;
	m_bUseFirstBuffer = true;
}

CEventLogBufManager::~CEventLogBufManager()
{
	if( m_pFirstBuffer )
		delete m_pFirstBuffer;
	if( m_pSecondBuffer )
		delete m_pSecondBuffer;

	if (m_pCurrentLogBuffer)
	{
		delete m_pCurrentLogBuffer;
		m_pCurrentLogBuffer = NULL;
	}
}
void CEventLogBufManager::InitBuffer()
{
	//m_pCurrentLogBuffer = m_pFirstBuffer;
	m_bFirstTimeToLoadData = true;
	m_bUseFirstBuffer = true;
	clearLogBuffer();

}
void CEventLogBufManager::setPaxEventLogFilePath( const CString& _strFile ) 
{
/*	m_pFirstBuffer->setFileName( _strFile );	
	m_pSecondBuffer->setFileName( _strFile );*/	
	m_pCurrentLogBuffer->setFileName(_strFile);
}
// just if need to read data.
bool CEventLogBufManager::loadDataIfNecessary( long _lCurTime, ANIMATION_MODE _animMode,CTermPlanDoc* pTermDoc )
{
	long startTime	= m_pCurrentLogBuffer->getStartTime();
	long endTime	= m_pCurrentLogBuffer->getEndTime();

	if( _lCurTime >= startTime && _lCurTime <= endTime )	// need not to read data
		return false;

	if (_animMode == ANIMATION_FORWARD)
	{
		startTime = max( _lCurTime , 0l );
		endTime = _lCurTime + m_detaTime;
	}
	else
	{
		startTime = max( _lCurTime - m_detaTime, 0l );
		endTime = _lCurTime;
	}

	CTermPlanDoc::ANIMATIONSTATE eState = pTermDoc->m_eAnimState;
	pTermDoc->m_eAnimState = CTermPlanDoc::anim_pause;
	pTermDoc->AnimTimerCallback();
	m_pCurrentLogBuffer->readDataFromFile(  startTime, endTime );
	pTermDoc->AnimTimerCallback();
	pTermDoc->m_eAnimState = eState;
	return true;
// 	long lScondBufferStartTime ;
// 	long lScondBufferEndTime ;
// 	if( _animMode == ANIMATION_FORWARD )
// 	{
// 		//startTime = max( _lCurTime - BUFFER_TIME, 0l );
// 		startTime = max( _lCurTime , 0l );
// 		endTime = _lCurTime + m_detaTime;
// 		if( m_bFirstTimeToLoadData )
// 		{
// 			lScondBufferStartTime = endTime;
// 			lScondBufferEndTime =  lScondBufferStartTime + m_detaTime;
// 		}
// 		else
// 		{
// 			if( m_bUseFirstBuffer )
// 			{
// 				lScondBufferStartTime = m_pSecondBuffer->getEndTime();
// 				lScondBufferEndTime = lScondBufferStartTime + m_detaTime;
// 			}
// 			else
// 			{
// 				lScondBufferStartTime = m_pFirstBuffer->getEndTime();
// 				lScondBufferEndTime = lScondBufferStartTime + m_detaTime;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		startTime = max( _lCurTime - m_detaTime, 0l );
// 		//endTime = min( _lCurTime + BUFFER_TIME, MAX_TIME );
// 		endTime = _lCurTime;
// 
// 		if( m_bFirstTimeToLoadData )
// 		{
// 			lScondBufferStartTime = max( startTime - m_detaTime,  0l );
// 			lScondBufferEndTime =  startTime;//min ( lScondBufferStartTime + m_detaTime, MAX_TIME );
// 		}
// 		else
// 		{
// 			if( m_bUseFirstBuffer )
// 			{
// 				lScondBufferEndTime = m_pSecondBuffer->getStartTime();				
// 				lScondBufferStartTime = max( lScondBufferEndTime - m_detaTime, 0l );
// 			}
// 			else
// 			{
// 				lScondBufferEndTime = m_pFirstBuffer->getStartTime();				
// 				lScondBufferStartTime = max( lScondBufferEndTime - m_detaTime, 0l );
// 			}
// 		}
// 	}
// 
// 		/*
// 		ElapsedTime start1( startTime /100l );
// 		ElapsedTime start2( lScondBufferStartTime /100l );
// 		ElapsedTime end1( endTime /100l );
// 		ElapsedTime end2( lScondBufferEndTime /100l );
// 		ElapsedTime currTime( _lCurTime /100l );
// 		char timeData[32];
// 		*/
// 
// 	if( m_bFirstTimeToLoadData )
// 	{
// 		CWaitCursor tempCursor;
// 		m_pFirstBuffer->readDataFromFile(  startTime, endTime );
// 		WaitForSingleObject( m_pFirstBuffer->GetEventHandle(), INFINITE );
// 		m_pFirstBuffer->CloseThreadHandle();
// 
// 		m_pSecondBuffer->readDataFromFile(lScondBufferStartTime, lScondBufferEndTime );
// 		//WaitForSingleObject( m_pSecondBuffer->GetEventHandle(), INFINITE );
// 		//m_pSecondBuffer->CloseThreadHandle();
// 
// 		m_pCurrentLogBuffer = m_pFirstBuffer;
// 		m_bUseFirstBuffer = true;
// 		m_bFirstTimeToLoadData = false;
// 
// /*		
// 		ofsstream echoFile ("loaddata.log", stdios::app);
//         echoFile <<" ********first time *********"<<'\n';
// 			echoFile <<" first buffer load : "<<'\n';
// 
// 			start1.printTime( timeData );
// 			echoFile <<" start time : "<< timeData  ;
// 
// 			end1.printTime( timeData );
// 			echoFile << " end time: "<< timeData <<'\n';
// 
// 			echoFile <<" second buffer load : "<<'\n';
// 
// 			start2.printTime( timeData );
// 			echoFile <<" start time : "<< timeData  ;
// 
// 			end2.printTime( timeData );
// 			echoFile << " end time: "<< timeData <<'\n';
// 			
//         echoFile.close();
// */		
// 		return true;
// 	}
// 	
// 	if( m_bUseFirstBuffer )
// 	{	
// 		WaitForSingleObject( m_pSecondBuffer->GetEventHandle() ,INFINITE );
// 		m_pSecondBuffer->CloseThreadHandle();
// 		m_pFirstBuffer->readDataFromFile( lScondBufferStartTime,lScondBufferEndTime );
// 		m_pCurrentLogBuffer = m_pSecondBuffer;	
// 		m_bUseFirstBuffer = !m_bUseFirstBuffer;
// /*
// 		ofsstream echoFile ("loaddata.log", stdios::app);  
// 			echoFile <<" ******************************* : "<<'\n';
// 
// 			currTime.printTime( timeData );
// 			echoFile <<" current time  : "<<timeData<<'\n';
// 			echoFile <<" first buffer load : "<<'\n';
// 
// 			start2.printTime( timeData );
// 			echoFile <<" start time : "<< timeData  ;
// 
// 			end2.printTime( timeData );
// 			echoFile << " end time: "<< timeData <<'\n';
//         echoFile.close();
// */
// 		return true;
// 	}
// 	else
// 	{		
// 		WaitForSingleObject( m_pFirstBuffer->GetEventHandle() ,INFINITE );
// 		m_pFirstBuffer->CloseThreadHandle();
// 		m_pSecondBuffer->readDataFromFile( lScondBufferStartTime,lScondBufferEndTime );
// 		m_pCurrentLogBuffer = m_pFirstBuffer;	
// 		m_bUseFirstBuffer = !m_bUseFirstBuffer;
// /*
// 		ofsstream echoFile ("loaddata.log", stdios::app);        
// 			echoFile <<" ******************************* : "<<'\n';
// 			currTime.printTime( timeData );
// 			echoFile <<" current time  : "<<timeData<<'\n';
// 			echoFile <<" scond buffer load : "<<'\n';
// 			
// 			start2.printTime( timeData );
// 			echoFile <<" start time : "<< timeData  ;
// 
// 			end2.printTime( timeData );
// 			echoFile << " end time: "<< timeData <<'\n';
//         echoFile.close();
// */
// 		return true;
// 	}
// 	//return m_pLogBuffer->readDataFromFile( startTime, endTime );
}


// clear log  buffer
void CEventLogBufManager::clearLogBuffer( void )
{
// 	if( m_pFirstBuffer )
// 		m_pFirstBuffer->initBuffer();
// 	if( m_pSecondBuffer )
// 		m_pSecondBuffer->initBuffer();

	if (m_pCurrentLogBuffer)
	{
		m_pCurrentLogBuffer->initBuffer();
	}
}


void CEventLogBufManager::setDetaTimeBaseFileSize( const CString& _strFile )
{
	// set log file path
	setPaxEventLogFilePath( _strFile );

	/*
	// get file size
	std::ifstream log_file( _strFile, std::ios::in | std::ios::binary );
	if( log_file.bad() )
		throw new FileOpenError( _strFile );
	log_file.seekg( 0, std::ios::end );
	long filesize = log_file.tellg();

	// set deta time base size
	int iM = filesize / HUNDRED_M;
	if( iM <= 3 )	// less than 300M, detaTime = 24h
	{ 
		m_detaTime = 24 * ONE_HOUR;	
	}
	else if( iM <= 5)	// less than 500M, detaTime = 12h
	{
		m_detaTime = 12 * ONE_HOUR;
	}
	else if( iM <= 8)	// less than 800M, detaTime = 6h
	{ 
		m_detaTime = 6 * ONE_HOUR;
	}
	else if( iM <= 10 )	// less than 1000M, detaTime = 3h				
	{
		m_detaTime = 3 * ONE_HOUR;
	}
	else
	{
		m_detaTime = 2 * ONE_HOUR;
	}
*/
	m_detaTime = 15*60*100;// 2h     
	// close file
	//log_file.close();
}