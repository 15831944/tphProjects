/*****
*
*   Class       BaseLog
*   Author      Lincoln Fellingham
*   Date        Feb 6, 1996
*   Purpose     Base class for logging all simulation activities.
*               Provides common functionality for different log types
*
*               Note that all functions are defined inline. This a
*               requirement of template based classes.
*
*****/

#ifndef BASE_LOG_H
#define BASE_LOG_H

#include "results\logitem.h"
#include "results\eventlog.h"
#include "common\fsstream.h"
#include "common\fileman.h"
#include "common\exeption.h"
#include "..\common\progressbar.h"
#include <assert.h>
#include <memory>

#define VER_SIZE        (sizeof(short))
template <class LOG_STRUCT, class EVENT_STRUCT, class CLASS> class BaseLog
{
protected:
	std::vector<LOG_STRUCT> m_vLogList;
	short m_nVersion;
	
	EventLog<EVENT_STRUCT> *eventLog;

public:
	std::vector<long> m_vLastTime;
	std::vector<long> m_vLastIdx;
    CString sLastFileName;
	short GetFileVersion() const { return m_nVersion; }
public:
	BaseLog (short _ver = 202)
	{
		m_nVersion = _ver;
		sLastFileName="";
		eventLog = NULL;
	}

	virtual ~BaseLog ()
	{ 
	}

	// called when the simulation engine started.
	virtual void clearLogs()
	{
		m_vLogList.clear();
		m_vLastIdx.clear();
		m_vLastTime.clear();
	}

	virtual void clearEvents()
	{
		if(eventLog)
			eventLog->clearEvents();	// don't do anything for now 
	}


	// event file I/O methods
	void createEventFile (const char *p_filename)
		  { eventLog->createEventFile (p_filename); }
	
	// Description: call by report if the size = 0 load the data
	// Exception:	FileOpenError
	void LoadEventDataIfNecessary(const char *p_filename)
		  { eventLog->openEventFile(p_filename); }

	virtual void SaveEventToFile (const char *p_filename) { eventLog->closeEventFile(); }

	// accessors and modifiers
	virtual long getCount (void)const { return m_vLogList.size(); }
	virtual void addItem (const CLASS &p_item)
	{
		LOG_STRUCT record;
		p_item.initStruct( record );
		addItem( record );
	}

	virtual void addItem (const LOG_STRUCT &p_record)
	{
		m_vLogList.push_back( p_record );
		m_vLastIdx.push_back(0);
		m_vLastTime.push_back(0);
	}

	virtual void getItem (CLASS& p_item, unsigned p_ndx)const
	{
		assert( p_ndx < m_vLogList.size() );
		p_item.initClass( m_vLogList[p_ndx] );

		p_item.setLastTime(m_vLastTime[p_ndx]); //matt
		p_item.setLastIndex(m_vLastIdx[p_ndx]);
	}


	virtual void updateItem (CLASS &p_item, unsigned p_ndx)
	{
		assert( p_ndx < m_vLogList.size() );
		LOG_STRUCT record;
		p_item.initStruct (record);
		m_vLogList[p_ndx] = record;

		m_vLastTime[p_ndx] = p_item.getLastTime(); //matt
		m_vLastIdx[p_ndx]  = p_item.getLastIndex();
	}

	virtual void updateItem (const LOG_STRUCT &p_record, unsigned p_ndx)
	{
		assert( p_ndx < m_vLogList.size() );
		m_vLogList[p_ndx] = p_record;

	}

	// Description: call by report if the size = 0 load the data
	// Exception:	FileOpenError, StringError
	void LoadDataIfNecessary( const char *p_filename )
	{
		
		if( m_vLogList.size() > 0 )
		{
			if(sLastFileName == CString(p_filename))
				return;
			m_vLogList.clear();
			m_vLastIdx.clear();
			m_vLastTime.clear();
		}
		//	return;
        
		fsstream* logFile = new fsstream (p_filename, stdios::binary | stdios::in | stdios::out );
		if (logFile->bad() || !*logFile)
			return;
//			throw new FileOpenError (p_filename);

        sLastFileName = p_filename;

		logFile->clear(); //Jan98
		logFile->seekg (0l, stdios::end);
		long fileSize = static_cast<long>(logFile->tellg());
		int count = (fileSize - (long)VER_SIZE) / (long)sizeof (LOG_STRUCT);

		if(count > 0)
		{
			short ver;
			logFile->clear(); //Jan98
			logFile->seekg (0l, stdios::beg);
			logFile->read ((char *)&ver, VER_SIZE);
			if (ver != m_nVersion )
				throw new StringError ("Log file out of date, " "rerun simulation");

			CProgressBar bar( "Loading Log Data", 100, count, TRUE, 1 );
			for( int i=0; i<count; i++ )
			{
				LOG_STRUCT record;
				logFile->read( (char*)&record, sizeof(LOG_STRUCT) );
				if( logFile->gcount() == sizeof( LOG_STRUCT ) ) {
					m_vLogList.push_back( record );
				}

				bar.StepIt(); 
			}
		}

		delete logFile;

		m_vLastIdx.resize(m_vLogList.size(), 0);
		m_vLastTime.resize(m_vLogList.size(), 0);

	}




	// called when simulation engine finished the work
	virtual void SaveToFile( const char *p_filename )
	{
		FileManager fileMan;
		if( fileMan.IsFile (p_filename) )
			fileMan.DeleteFile( p_filename );

		std::auto_ptr<fsstream> logFile( new fsstream (p_filename, stdios::out|stdios::binary ) );//
		if (logFile->bad() || !*logFile)
			throw new FileOpenError (p_filename);

		logFile->write ((char *)&m_nVersion, VER_SIZE);

		int nCount = m_vLogList.size();
		for( int i=0; i<nCount; i++ )
		{
			logFile->write(   (char*)&m_vLogList[i], sizeof( LOG_STRUCT ) );
		}

		//delete logFile;
	}

	EventLog<EVENT_STRUCT>* getEventLog()const { return eventLog;}
};

#endif
