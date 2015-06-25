/*****
*
*   Class       EventLog
*   Author      Lincoln Fellingham
*   Date        Feb 13, 1996
*   Purpose     Base class
*
*               Note that all functions are defined inline. This a
*               requirement of template based classes.
*
*****/

#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include "../common/fsstream.h"
#include "../common/fileman.h"
#include "../common/exeption.h"
#include <iostream>
#include <vector>
#include "resultsdll.h"


template <class EVENT_STRUCT> class RESULTS_TRANSFER EventLog
{
protected:
    fsstream *eventFile;
	//std::vector<EVENT_STRUCT> m_eventBuffer;
	//int m_nCurrentPos;
	//int m_nCurrentStartPosInBuffer;
	//int m_nCurrentEndPosInBuffer;
	//int m_nTotalEventCount;

	//const int MAXBUFFERSIZE;//= 1e+7; // 10 million

	/*
	virtual void FillBuffer( int newlocation )
	{
		ASSERT(newlocation >= 0 && newlocation < m_nTotalEventCount);
		ASSERT(eventFile!=NULL);

		m_nCurrentStartPosInBuffer = max(0, newlocation-MAXBUFFERSIZE/2);
		m_nCurrentEndPosInBuffer = min(m_nCurrentStartPosInBuffer+MAXBUFFERSIZE, m_nTotalEventCount-1);

		int nEventsToRead = m_nCurrentEndPosInBuffer-m_nCurrentStartPosInBuffer+1;
		m_eventBuffer.resize(nEventsToRead);

		eventFile->seekg (m_nCurrentStartPosInBuffer*sizeof(EVENT_STRUCT), stdios::beg);
		eventFile->read ((char *)&(m_eventBuffer[0]), nEventsToRead * sizeof (EVENT_STRUCT));
		if (eventFile->gcount() != nEventsToRead * sizeof (EVENT_STRUCT))
		{
			stdcerr<<"Out of eventFile"<<endl;
			_exit(1);
		}
		// TRACE("**Event buffer fill for event %d\n", newlocation);
	}
	*/

public:
    EventLog () //: MAXBUFFERSIZE(4e+7) 
	{
		eventFile = NULL;
	//	m_nCurrentStartPosInBuffer=m_nCurrentEndPosInBuffer=-1;
	//	m_nCurrentPos=0;
	//	m_nTotalEventCount=0;
	}

    virtual ~EventLog () { closeEventFile(); }

	
	void clearEvents()
	{
		/*
		m_nCurrentStartPosInBuffer=m_nCurrentEndPosInBuffer=-1;
		m_nCurrentPos=0;
		m_nTotalEventCount=0;
		m_eventBuffer.clear();
		*/
		closeEventFile();
	}
		

    // basic log file I/O methods
    virtual void createEventFile (const char *p_filename)
	{
		if (eventFile)
			closeEventFile();

		FileManager fileMan;
		if (fileMan.IsFile (p_filename))
			fileMan.DeleteFile (p_filename);

		eventFile = new fsstream (p_filename, stdios::out|stdios::binary );//
		if (eventFile->bad() || !*eventFile)
			throw new FileOpenError (p_filename);

//		eventFile->write ((char *)&version, VER_SIZE);
		closeEventFile();
		openEventFile (p_filename);
	}

    virtual void openEventFile (const char *p_filename)
	{
		if (eventFile)
			closeEventFile();

		eventFile = new fsstream (p_filename, stdios::binary | stdios::in | stdios::out);
		if (eventFile->bad() || !*eventFile)
			return ;
//			throw new FileOpenError (p_filename);

		//eventFile->seekg(0, stdios::end);
		//m_nTotalEventCount = eventFile->tellg() / sizeof(EVENT_STRUCT);
		//eventFile->seekg(0, stdios::beg);
	}

    virtual void closeEventFile (void)
	{
		if (eventFile)
		{
			delete eventFile;
			eventFile = NULL;
		}
	}

	//save event to file immediately
    virtual void addEvent (const EVENT_STRUCT &p_item)
	{
		eventFile->seekg (0l, stdios::end);
		eventFile->write ((char *)&p_item, sizeof (EVENT_STRUCT));
	}

	//save events to file immediately
    virtual void setEventList (const EVENT_STRUCT *p_item, int p_count)
	{
		eventFile->seekg (0l, stdios::end);
		int nsize = sizeof(EVENT_STRUCT);
		eventFile->write ((char *)p_item, p_count * nsize);
		if (eventFile->bad())
		{
//			stdcerr<<"cannot write to eventFile"<<endl;
			_exit(1);
		}
	}
	
	virtual void setEventList( const std::vector< EVENT_STRUCT>& p_item )
	{
		eventFile->seekg( 0l, stdios::end );
		//eventFile->write( (const char *)(&p_item[0]), p_count * sizeof (EVENT_STRUCT));
		for( unsigned i=0; i< p_item.size(); i++ )
		{
			int nsize = sizeof(EVENT_STRUCT);
			eventFile->write( (const char*)( &p_item[i] ), nsize );
		}

		if (eventFile->bad())
		{
//			stdcerr<<"cannot write to eventFile"<<endl;
			_exit(1);
		}
	}

	virtual void setEventList( const std::vector< EVENT_STRUCT* >& p_item )
	{
		eventFile->seekg( 0l, stdios::end );
		for (unsigned i = 0; i < p_item.size(); i++)
		{
			int nsize = sizeof(EVENT_STRUCT);
			eventFile->write((const char*)(p_item[i]), nsize);
		}

		if (eventFile->bad())
		{
			_exit(1);
		}
	}


    virtual void setPosition (long p_pos)
	{
		eventFile->seekg (p_pos, stdios::beg);
		/*
		int nPos = p_pos / sizeof( EVENT_STRUCT );

		if( nPos < m_nCurrentStartPosInBuffer || nPos > m_nCurrentEndPosInBuffer) {
			FillBuffer( nPos);
		}
		m_nCurrentPos = nPos;
		*/
	}

    virtual long getPosition (void) const
	{
		return static_cast<long>(eventFile->tellg());
		//return m_nCurrentPos * sizeof(EVENT_STRUCT);
	}

	virtual long getPositionForWrite (void) const
	{
		return static_cast<long>(eventFile->tellg());
	}


    virtual EVENT_STRUCT getNextEvent (void)
	{
		/*
		if( m_nCurrentPos > m_nCurrentEndPosInBuffer )
			FillBuffer( m_nCurrentPos );
		EVENT_STRUCT ret = m_eventBuffer[m_nCurrentPos-m_nCurrentStartPosInBuffer];
		m_nCurrentPos++;
		return ret;
		*/

		EVENT_STRUCT record;
		eventFile->read ((char *)&record, sizeof (EVENT_STRUCT));
		if (eventFile->gcount() != sizeof (EVENT_STRUCT))
		{
//			stdcerr<<"Out of eventFile"<<endl;
			throw new StringError ("The log file is corrupted, please run simulation." "simulation");
			_exit(1);
		}
		return record;
	}

	virtual void updateNextEvent(const EVENT_STRUCT& _newData )
	{
		eventFile->write( (char *)&_newData, sizeof (EVENT_STRUCT) );
	}
	virtual void getEvents (EVENT_STRUCT *p_eventList, int p_count)
	{
		/*
		if (m_nCurrentPos+p_count-1 > m_nCurrentEndPosInBuffer) {

			FillBuffer(m_nCurrentPos);
		}

		memcpy(p_eventList, &(m_eventBuffer[m_nCurrentPos-m_nCurrentStartPosInBuffer]), p_count * sizeof (EVENT_STRUCT));
		m_nCurrentPos += p_count;
		*/
		eventFile->read ((char *)p_eventList, p_count * sizeof (EVENT_STRUCT));
		if (eventFile->gcount() != p_count * sizeof (EVENT_STRUCT))
		{
//			stdcerr<<"Out of eventFile"<<endl;
			throw new StringError ("The log file is corrupted, please run simulation." "simulation");
			_exit(1);
		}
	}

	long getTotalEventCount()
	{
		eventFile->seekg(0, stdios::end);
		long nTotalEventCount = static_cast<long>(eventFile->tellg()) / sizeof(EVENT_STRUCT);
		eventFile->seekg(0, stdios::beg);//set to begin
		return nTotalEventCount;
	}

	virtual bool needCreateFile(long lCount)const 
	{
		return false;
	}

	virtual long GetLastFileID()const
	{
		return 0;
	}

	virtual EventLog<EVENT_STRUCT>* GetEventLog(long lFile = 0)
	{
		return this;
	}
	fsstream* GetEventFile()const
	{
		return eventFile;
	}
	void SetEventFile(fsstream* pEventFile)
	{
		eventFile = pEventFile;
	}
};

#endif
