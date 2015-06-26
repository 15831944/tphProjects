// EventLogBuffer.cpp: implementation of the CEventLogBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EventLogBuffer.h"
#include "out_term.h"
#include <algorithm>
#include <iterator>
#include "paxentry.h"
#include "paxlog.h"
#include "common\progressbar.h"
#include <process.h>
#include <Common/EventBinarySearch.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventLogBuffer::CEventLogBuffer( OutputTerminal* _pTerminal ) : m_pTerminal( _pTerminal ), m_lSize(0l)
{
	initBuffer();
//	m_hCanUseEvent = CreateEvent( NULL,TRUE,FALSE,NULL );
//	m_bHasClosed = false;
}

CEventLogBuffer::~CEventLogBuffer()
{
	clearData();
//	CloseHandle( m_hCanUseEvent );
}



bool checkIfAlive( const MobLogEntry& entry , CEventLogBuffer* _pBuffer)
{
	long _birthTime = (long) entry.getEntryTime();
	long _deathTime = (long) entry.getExitTime();

// 	if(_birthTime < _pBuffer->getStartTime() && _deathTime < _pBuffer->getStartTime() )
// 		return false;
// 	if(_birthTime > _pBuffer->getEndTime() && _deathTime > _pBuffer->getEndTime() )
// 		return false;
// 	return true;

	if( 
	    ( _birthTime >= _pBuffer->getStartTime() && _birthTime < _pBuffer->getEndTime() ) ||	     
		( _deathTime >= _pBuffer->getStartTime() && _deathTime < _pBuffer->getEndTime() ) ||
		( _birthTime <  _pBuffer->getStartTime()&& _deathTime >=_pBuffer->getEndTime() ) 
	)
		return true;

	return false;
}

void readData( void* _pPara )
{
	CEventLogBuffer* pBuffer = (CEventLogBuffer*)_pPara;	

	pBuffer->OpenEventLogFile();
	if (!pBuffer->hasLogFile())
		return;
	
	// read data
	MobLogEntry element;
	PaxLog* pPaxLog = pBuffer->GetTerminal()->paxLog;
	int nCount = pPaxLog->getCount();

	// progress bar ( shall use multi_thread )
	CProgressBar bar( "Loading Animation Data", 100, nCount, TRUE, 0 );

	part_event_list mobData;
	for(int i=0; i < nCount; i++ )
	{
		pPaxLog->getItem(element, i);

		if( checkIfAlive( element, pBuffer  ) )
		{
			//lLiveMan ++;
			// get the start pos ,end pos( in file) and the number of struct;
			long beginPos	= element.GetMobDesc().startPos;
			long endPos		= element.GetMobDesc().endPos + sizeof( MobEventStruct ) ;
			long eventNum   = ( endPos-beginPos )/sizeof( MobEventStruct );
			
			//////////////////////////////////////////////////////////////////////////
			// read full event
			// push an empty item to data list.
			std::ifstream* pLogFile = pBuffer->GetLogFile(element.GetFileIndex());
			if (pLogFile)
			{
				pBuffer->getDataBuffer().push_back( std::make_pair( i, part_event_list() ) );
				part_event_list&  data_in_vector = pBuffer->getDataBuffer().back().second;
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
					int nStartPos = BinarySearchEvent(mobData,pBuffer->getStartTime());
					int nEndPos = BinarySearchEvent(mobData,pBuffer->getEndTime());
					
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
					memcpy(&data_in_vector[0],&mobData[nStartPos],sizeof(MobEventStruct)*nSize);
					
					int lCount = data_in_vector.size()*sizeof(MobEventStruct);
					long lSize = pBuffer->getSize() + lCount - 1;
					pBuffer->setSize( lSize );
					//m_lSize += (endPos - beginPos);
				}
				catch (...)
				{
					pBuffer->getDataBuffer().pop_back();
				}
				
			}
			
			
		}	
		bar.StepIt();
	}
	pBuffer->CloseEventLogFile();

//	SetEvent( pBuffer->GetEventHandle() );	
}
bool CEventLogBuffer::readDataFromFile( long startTime, long endTime )
{
	assert( m_pTerminal );
	if( startTime > endTime )
		return false;

//	ResetEvent( m_hCanUseEvent );
	m_startTime = startTime;
	m_endTime	= endTime;

	clearData();	//clear buffer;
	//_beginthread( readData,0, this);
	//DWORD dwThreadID;
	//m_hWorkThread = CreateThread( NULL,0, (LPTHREAD_START_ROUTINE)readData ,this,CREATE_SUSPENDED,&dwThreadID);
//	typedef unsigned ( __stdcall *start_address )( void * );
	//m_hWorkThread = (HANDLE )_beginthreadex( NULL,0, (start_address)readData ,this,CREATE_SUSPENDED,(unsigned *)(&dwThreadID));
//	m_hWorkThread = (HANDLE )_beginthreadex( NULL,0, (start_address)readData ,this,0,(unsigned *)(&dwThreadID));
	readData(this);
//	m_bHasClosed = false;
	//SetThreadPriority(m_hWorkThread,THREAD_PRIORITY_BELOW_NORMAL );
	//ResumeThread( m_hWorkThread );
	return true;
}
void CEventLogBuffer::CloseThreadHandle()
{
// 	if( !m_bHasClosed )
// 	{
// 		CloseHandle( m_hWorkThread );
// 		m_bHasClosed = true;
// 	}
}

void CEventLogBuffer::OpenEventLogFile()
{
	FileManager fileMan;
	
	CString sFileName = m_strLogFile;
	int nFileIndex = 0;
	while (fileMan.IsFile ( sFileName ))
	{
		// open log file
		std::ifstream* plogfile = new std::ifstream( sFileName, std::ios::in | std::ios::binary );
		if( plogfile->bad() )
			continue;

		m_vLogFile.push_back(plogfile);
		nFileIndex++;
		CString strLeft;
		long lPos = m_strLogFile.Find('.');
		strLeft = m_strLogFile.Left(lPos);

		sFileName.Format(_T("%s%03d.log"),strLeft,nFileIndex);
	}
}

void CEventLogBuffer::CloseEventLogFile()
{
	for (size_t i = 0; i < m_vLogFile.size(); i++)
	{
		std::ifstream* plogfile = m_vLogFile[i];
		plogfile->close();
		delete plogfile;
	}
	m_vLogFile.clear();
}

bool CEventLogBuffer::hasLogFile() const
{
	return m_vLogFile.empty()?false:true;
}

std::ifstream* CEventLogBuffer::GetLogFile( long idx ) const
{
	long lCount = (long)m_vLogFile.size();
	if (idx >= 0 && idx < lCount)
	{
		return m_vLogFile[idx];
	}
	return NULL;
}