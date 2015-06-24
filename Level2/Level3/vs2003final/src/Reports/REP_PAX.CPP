#include "stdafx.h"
#include "common\point.h"
#include "common\states.h"
#include "reports\rep_pax.h"
#include "inputs\pax_cnst.h"
#include "results\out_term.h"
#include "results\fltlog.h"


//&CPrintLog *ReportPaxEntry::report = NULL;
CBaseReport* ReportPaxEntry::report = NULL;
void ReportPaxEntry::writeElement (ArctermFile& p_file)
{
	p_file.writeInt (getID());
	p_file.writeTime (getEntryTime(), TRUE);
	p_file.writeTime (getExitTime(), TRUE);
	p_file.writeInt (getGroupSize());
	p_file.writeInt ((int)(getBagCount() / getGroupSize()));
	p_file.writeInt ((int)(getCartCount() / getGroupSize()));
	float fSpeed = (float)(getSpeed() * SCALE_FACTOR);
	p_file.writeFloat( fSpeed );
/*

	int nTrackCount = getCount();

	setToFirst();
    ReportPaxEvent track;
	track.init( getNextEvent() );
	Point prevPt = track.getPoint();
	ElapsedTime ePrevTime = track.getTime();

	float fTotalSpeed = 0.0;
	int nCount = 0;
    for( int i = 1; i < nTrackCount; i++ )
	{
		track.init( getNextEvent() );
		Point pt = track.getPoint();
		ElapsedTime eTime = track.getTime();
		float fSpeed = 0.0;
		if( eTime != ePrevTime )
		{
			fSpeed = ( pt - prevPt ).length() / ( eTime - ePrevTime ).asSeconds();
			if( fSpeed < 1000 )
			{
				nCount++;
				fTotalSpeed += fSpeed;
			}
			prevPt = pt;
			ePrevTime = eTime;
		}
	}
	
	p_file.writeFloat( nCount > 0 ? fTotalSpeed / nCount : 0.0 );
*/

	char str[MAX_PAX_TYPE_LEN];
	printFullType (str);
	p_file.writeField (str);
}
/*
// indicates whether the pax uses a selected processor in the passed
// time range. Requires that the pax's events are buffered
int ReportPaxEntry::usesSelectedProc (ElapsedTime p_start, ElapsedTime p_end)
{
    if (!eventList)
        throw new StringError ("All tracks must be buffered");

    PaxEvent track;
    int trackCount = getCount();
    for (int i = 1; i < trackCount; i++)
    {
        track.init (getEvent(i));

        if (track.getTime() > p_end)
            break;

        if (track.getTime() < p_start)
            continue;

        if (isValidProc (track.getProc()))
            return 1;
    }
    return 0;
}
*/
// get the coordinate at time
// return true if succeed
bool ReportPaxEntry::getPointAtTime (ElapsedTime p_time, Point& p_coord )
{

    setToFirst();
    PaxEvent prevTrack, nextTrack;
    int trackCount = getCount();
	if( trackCount == 0 )
		return false;

    nextTrack.init (getNextEvent());

    for (int i = 1; i < trackCount; i++)
    {
        prevTrack = nextTrack;
        nextTrack.init (getNextEvent());

        if (p_time == prevTrack.getTime())
		{
			p_coord = prevTrack.getPoint();
			return true;
		}

        if (p_time > prevTrack.getTime() && p_time < nextTrack.getTime())
        {
            p_coord.initVector (prevTrack.getPoint(), nextTrack.getPoint());
            double scale = (p_time - prevTrack.getTime()) /
                (nextTrack.getTime() - prevTrack.getTime());
            p_coord *= scale;
			p_coord += prevTrack.getPoint();
			p_coord.setZ(prevTrack.getPoint().getZ());
			return true;
        }
    }
    return false;
}

/*****
*
*   Report Track methods
*
*****/

void ReportPaxEvent::writeTrack (ArctermFile& p_file) const
{
    p_file.writeTime (getTime(), TRUE);
    p_file.writePoint (getPoint());
    p_file.writeField (STATE_NAMES[getState()]);
    p_file.writeInt (getProc());
}


int ReportPaxEntry::usesSelectedProc(ElapsedTime p_start, ElapsedTime p_end)
{

    PaxEvent track;
    int trackCount = getCount();
    for (int i = 1; i < trackCount; i++)
    {
        track.init (getEvent(i));

        if (track.getTime() > p_end)
            break;

        if (track.getTime() < p_start)
            continue;

        if (isValidProc (track.getProc()))
            return 1;
    }
    return 0;
}

void ReportPaxEntry::loadifNecessary()
{
	if( m_vEventList.empty())
		loadEvents();
}

ElapsedTime ReportPaxEntry::getEntryTime()
{
	return MobLogEntry::getTerminalEntryTime();
}

ElapsedTime ReportPaxEntry::getExitTime()
{
	return MobLogEntry::getTerminalEndTime();
 }

MobEventStruct ReportPaxEntry::getEvent(long p_ndx)
{
	loadifNecessary();

	ASSERT(p_ndx >= 0 && p_ndx < (int)m_vEventList.size());
	return m_vEventList[p_ndx];
}

MobEventStruct ReportPaxEntry::getNextEvent(void)
{
	return getEvent(m_curPos++);
}

long ReportPaxEntry::getCount()
{
  	loadifNecessary();
  
  	return (long)m_vEventList.size();
}

ReportPaxEntry::ReportPaxEntry()
{
	m_curPos = 0;
}

void ReportPaxEntry::loadEvents()
{
	if (!m_vEventList.empty())
		return;

	if (item.startPos == -1 || item.endPos == -1)
		return;
	
	assert( m_pEventLog );
	EventLog<MobEventStruct>* pSingleEventlog = NULL;
	if (item.fileID != -1)
	{
		pSingleEventlog = m_pEventLog->GetEventLog(item.fileID);
	}
	else
	{
		pSingleEventlog = m_pEventLog->GetEventLog();
	}
	assert(pSingleEventlog);
	int nSize = 1+ ( item.endPos - item.startPos )/ sizeof( MobEventStruct );	
	pSingleEventlog->setPosition (item.startPos);
	//setToFirst();
	MobEventStruct* pEventList = new MobEventStruct[nSize];
	pSingleEventlog->getEvents ( pEventList, nSize);

  	int nStartPos = -1;
  	int nEndPos = -1;
 
  	//get terminal start event position
  	for(int i = 0; i < nSize; i++)
  	{
  		if (pEventList[i].state == TerminalBegin)
  		{
  			nStartPos = i;//when pax state is terminal begin means he generate next event that move to the first processor
  							  // the event should be ignore
  			break;
  		}
  	}
  
  	//get terminal end position
  	for (i = nSize - 1; i >= 0; i--)
  	{
  		if (pEventList[i].state == TerminalEnd)
  		{
  			nEndPos = i;
  			break;
  		}
  	}
  
  	//not find reset start position
  	if (nStartPos == -1)
  	{
  		nStartPos = 0;
  	}
  
  	//not find set end position
  	if (nEndPos == -1)
  	{
  		nEndPos = nSize - 1;
  	}
  
  	if (nStartPos > nEndPos)
  	{
  		std::swap(nStartPos,nEndPos);
  	}
  
   	for( i = nStartPos; i<=nEndPos; i++ )
   	{
   		m_vEventList.push_back( pEventList[i] );
   	}

	delete [] pEventList;
}