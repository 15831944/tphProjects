/*****
*
*   Class       LogItem
*   Author      Lincoln Fellingham
*   Date        Feb 13, 1996
*   Purpose     Base class for all log items that track events. Defines
*               a common interface for basic I/O functions
*
*               Note that all functions are defined inline. This a
*               requirement of template based classes.
*
*****/

#ifndef LOG_ITEM_H
#define LOG_ITEM_H

#include "..\common\fsstream.h"
#include "..\common\fileman.h"
#include "..\common\exeption.h"
#include "..\common\util.h"
#include "..\common\template.h"
#include "..\common\projectmanager.h"
#include "assert.h"
#include "eventlog.h"

extern int echoFlag;
template <class LOG_STRUCT, class EVENT_STRUCT> class LogItem
{ 
protected:
    LOG_STRUCT item;
	std::vector<EVENT_STRUCT> m_vEventList;		// used for this item only
    EventLog<EVENT_STRUCT>* m_pEventLog;       //event log file
	long m_vLastTime;
	long m_vLastIdx;
	
public:
	//set m_pEventLog for this item
	void SetEventLog( EventLog<EVENT_STRUCT>* _pEventLog ){ m_pEventLog = _pEventLog; }
	LOG_STRUCT& getStruct(){ return item; }
	const LOG_STRUCT& getStruct()const{ return item; }

    LogItem ()
    {
        item.endPos = item.startPos = -1l;		        
		m_pEventLog = NULL;
		m_vLastTime = 0;   //matt modify 20040429
		m_vLastIdx = 0;    //matt modify 20040429
    }                           
	
  //  LogItem (const LogItem& p_item)
  //  {
  //      item = p_item.item;
  //      m_vEventList = p_item.m_vEventList;
		//m_pOutTerm = p_item.m_pOutTerm;

		//m_vLastIdx = p_item.m_vLastIdx;   //matt modify 20040429
		//m_vLastTime = p_item.m_vLastTime;  //matt modify 20040429
		//
  //  }

    virtual ~LogItem () { }

    // accessors and modifiers

	// get event count from Eventlog files 
    virtual long getCount (void) const
    {
        return (item.startPos < 0l || item.endPos < 0l) ? 0l:
            1 + ((item.endPos - item.startPos) / (long)sizeof (EVENT_STRUCT));
    }

	//get event count from m_vEventList of this item
    virtual long getCurrentCount (void) const { return (long)m_vEventList.size(); }

	//using current item to init the p_item 
    virtual void initStruct (LOG_STRUCT& p_item) const
    {
        p_item = item;
    }

	//init current item
    virtual void initClass (const LOG_STRUCT& p_item)
    {
        item = p_item;
    }

    virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const = 0;

    /*****
    *
    *   Event I/O routines
    *
    *****/

    //write events list of p_item to m_pEventLog file
	virtual void setEventList (const EVENT_STRUCT *p_item, int p_count)
    {
		assert( m_pEventLog );
		if(m_pEventLog)
		{
			item.startPos = m_pEventLog->getPositionForWrite();
			m_pEventLog->setEventList(p_item, p_count);
			item.endPos = m_pEventLog->getPositionForWrite() - sizeof( EVENT_STRUCT );
		}
    }
	 //write events list of p_item to EventLog file
	virtual void setEventList( const std::vector<EVENT_STRUCT>& p_item )
	{
		assert( m_pEventLog );
		if(m_pEventLog)
		{
			item.startPos = m_pEventLog->getPositionForWrite();
			m_pEventLog->setEventList( p_item );
			item.endPos = m_pEventLog->getPositionForWrite() - sizeof( EVENT_STRUCT );
		}	
	}
	//save events to log file
	virtual void saveEventsToLog()
	{
		assert(m_pEventLog);
		if(m_pEventLog)
		{
			item.startPos = m_pEventLog->getPositionForWrite();
			m_pEventLog->setEventList( m_vEventList );
			item.endPos = m_pEventLog->getPositionForWrite() - sizeof( EVENT_STRUCT );
		}
	}

	//set current m_pEvntLog position to its start
    virtual void setToFirst (void)
    {
		assert( m_pEventLog );
		if(m_pEventLog)
			m_pEventLog->setPosition ((item.startPos != -1)? item.startPos: 0);
    }


	//get next event from the m_pEventLog
    virtual EVENT_STRUCT getNextEvent (void)
    {
		assert( m_pEventLog );
		if(!m_pEventLog)
			throw std::exception("Invalid Log Data");
		
		return m_pEventLog->getNextEvent();
    }


	//get the p_ndex event from the m_pEventLog
    virtual EVENT_STRUCT getEvent (long p_ndx)
    {
		if( m_vEventList.size() > 0 )
			return m_vEventList[p_ndx];

		assert( m_pEventLog );
			
		if(!m_pEventLog)
			throw std::exception("Invalid Log Data");

        m_pEventLog->setPosition (item.startPos + p_ndx * sizeof( EVENT_STRUCT ) );
        return m_pEventLog->getNextEvent();
    }

	//update the _lIdex event for the _newEventData
	virtual void updateEvent( EVENT_STRUCT& _newEventData, long _lIdx )
	{	
		if( m_vEventList.size() > 0 )
		{
			m_vEventList[_lIdx] = _newEventData;
		}		
		assert( m_pEventLog );

		if(!m_pEventLog)
			throw std::exception("Invalid Log Data");

        m_pEventLog->setPosition (item.startPos + _lIdx * sizeof( EVENT_STRUCT ) );
        m_pEventLog->updateNextEvent( _newEventData );
	}



    /*****
    *
    *   Log manipulation routines
    *
    *****/

	//get the current m_vEventList
	virtual const std::vector<EVENT_STRUCT>& getEventList( void ) const
	{
		return m_vEventList;
	}

	//get the current m_vEventList
	virtual std::vector<EVENT_STRUCT>& getEventList( void )
	{
		return m_vEventList;
	}
	
	//init the m_vEventList for blank
    virtual void initLog (void)
    {
		m_vEventList.clear();
    }

	//add event to m_vEventList for p_item
    virtual void addEvent (const EVENT_STRUCT &p_item)
    {
		m_vEventList.push_back( p_item );
    }

	//get the current m_vEventList
    virtual int getLog (EVENT_STRUCT* &p_log)
    {
        int retVal = 1;
        // no file buffer used, copy memory buffer
		int nSize = (int)m_vEventList.size();
		if( nSize > 0 )
		{
			if (!p_log)
				p_log = new EVENT_STRUCT[nSize];
			for( int i=0; i<nSize; i++ )
				p_log[i] = m_vEventList[i];
		}

        return retVal;
    }

	//loadEvents from logfiles and set value of m_pEventLog
    virtual void loadEvents (void)
    {
		//int nSize = m_vEventList.size();
		int nSize = 1+ ( item.endPos - item.startPos )/ sizeof( EVENT_STRUCT );		
		m_vEventList.clear();
		m_pEventLog->setPosition (item.startPos);
        //setToFirst();
		assert( m_pEventLog );
		if(!m_pEventLog)
			throw std::exception("Invalid Log Data");

        EVENT_STRUCT* pEventList = new EVENT_STRUCT[nSize];
        m_pEventLog->getEvents ( pEventList, nSize);
		for( int i=0; i<nSize; i++ )
		{
			m_vEventList.push_back( pEventList[i] );
		}
		delete [] pEventList;
    }

	// clear m_vEventList
    virtual void clearLog (void)
    {
        // if file buffer has been used, close and delete the file
		m_vEventList.clear();
    }

    //get m-vlastTime value
	virtual long getLastTime()
	{
		return m_vLastTime;
	}
	virtual long getLastIndex()
	{
		return m_vLastIdx;
	}
	//set m_vLastTime value
	virtual void setLastTime(long value)
	{
		m_vLastTime = value;
	}

	virtual void setLastIndex(long value)
	{
		m_vLastIdx = value; 
	}
	//Get two event at this time of t      /////////////add function 2004,4,29
	virtual bool getEventsAtTime(int time,EVENT_STRUCT& event1,EVENT_STRUCT& event2)
    {
		
		
		long nextIdx = -1;
		long nEventCount = LogItem::getCount();
		
		if(time >= m_vLastTime)
		{
			//search forwards for event at t
			int c = 0;
			for(long jj=m_vLastIdx; jj<nEventCount&&jj>=0; jj++) 
			{
				c++;

				if(jj == 0)
				{
					if(getEvent(jj).time >= time)
					{
						nextIdx = m_vLastIdx = jj;
						m_vLastTime = time;
						//break;
					}				
				}
				else
				{
					if(getEvent(jj).time >= time &&getEvent(jj-1).time <= time)
					{
						nextIdx = m_vLastIdx = jj;
						m_vLastTime = time;
						break;
					}
				}
			}
			
//			// TRACE("Forward scan %d\n", c);
			if(nextIdx < 0)
				m_vLastIdx = 0;	
		}
		else { //t < lastTime
			//search backwards for event at t
					       int c = 0;
						   for(long jj = m_vLastIdx; jj>=1; jj--) {
							   c++;
							   if(getEvent(jj).time >= time &&getEvent(jj-1).time <= time)
							   {
								   nextIdx = m_vLastIdx = jj;
								   m_vLastTime = time;
								   break;
							   }
						   }
//						   // TRACE("Backward scan %d\n", c);
						   if(nextIdx < 0)
							    m_vLastIdx = 0;
		}
		
		if(nextIdx > 0) 
		{
			event1 = getEvent(nextIdx-1);
			event2 = getEvent(nextIdx);
			return true;
		}
		else { return false; }   	
	}
};

class OutputTerminal;
template <class LOG_STRUCT, class EVENT_STRUCT> class TerminalLogItem : public LogItem<LOG_STRUCT, EVENT_STRUCT>
{
public:
	TerminalLogItem()
	{
		m_pOutTerm = NULL;         
	}
	void SetOutputTerminal(OutputTerminal* pOutterm){ m_pOutTerm = pOutterm; }
protected:
	OutputTerminal* m_pOutTerm;
};

#endif;





















