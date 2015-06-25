#include "stdafx.h"
#include <stdlib.h>
#include "common\point.h"
#include "engine\lineproc.h"
#include "engine\lineq.h"
#include "common\states.h"
#include "common\exeption.h"
#include "person.h"
#include "TerminalMobElementBehavior.h"




//Sets m_pQueue flag of logEntry, cleans line (m_pQueue).
//If path is not empty, creates a new line (m_pQueue).
void LineProcessor::initQueue (int, int pathCount, const Point *pointList)
{
    logEntry.setQueue (pathCount > 0);
    if (m_pQueue)
        delete m_pQueue;
    m_pQueue = NULL;

	if( pointList== NULL || pathCount <= 0 )
		return;
    
	// for sequential queues only, test which end of m_pQueue is the head
	Path tempPath;
	tempPath.init (pathCount, pointList);
	Point pnt (m_location.getPoint(0));
	if (pnt.distance (pointList[0]) > pnt.distance (pointList[pathCount-1]))
		tempPath.invertList();

    m_pQueue = new LinearQueue (tempPath);
}

//Initializes line service m_location and allocates occupancy flags.
// EXCEPTION: StringError
void LineProcessor::initServiceLocation (int pathCount, const Point *pointList)
{
    if (pathCount <= 1)
        throw new StringError ("Line Processors must have at least 2 point service locations");

    m_location.init (pathCount, pointList);

    Path points;
    points.distribute (pathCount, pointList, 80l);
    m_serviceCoords.init (points);

	int nServicePointCount = points.getCount();

	for( int i=0; i<nServicePointCount; i++ )
	{
		m_vOccupiedID.push_back( -1 );
	}

	UpdateMinMax();
}

 
// return a random available service point index
int LineProcessor::getRandomPoint () const
{
	std::vector<int> vAvailableIndex;
	
    for (unsigned int i = 0; i < m_vOccupiedID.size(); i++ )
        if( m_vOccupiedID[i] == -1 )
			vAvailableIndex.push_back( i );

	int optionCount = vAvailableIndex.size();
	if( optionCount > 0 )
	{
		int randomIndex = random( optionCount );
		return vAvailableIndex[randomIndex];
	}
	return -1;
}

 
Point LineProcessor::GetServiceLocation () const
{
    int ndx = getRandomPoint();
	if( ndx < 0 )
		ndx = 0;
    return m_serviceCoords.getPoint (ndx);
}


// Acquire service m_location.
// If no service point is available, a random point returns.
Point LineProcessor::AcquireServiceLocation( Person* _pPerson  )
{
    Point aPoint;
	_pPerson->getTerminalPoint(aPoint);	// get current location

    double closest = 999999999.9;
    //double distance;
    int ndx = -1;

    if (!aPoint)
	{
		// 
		ndx = getRandomPoint();
	}
    else
    {
        for (unsigned int i = 0; i < m_vOccupiedID.size(); i++)
        {
            if( m_vOccupiedID[i] != -1 )
                continue;
            double distance = aPoint.distance (m_serviceCoords.getPoint(i));
            if (distance < closest)
            {
                closest = distance;
                ndx = i;
            }
        }
    }
    if (ndx == -1)
        ndx = random( m_vOccupiedID.size() );
	else
		m_vOccupiedID[ndx] = _pPerson->getID();

    return m_serviceCoords.getPoint (ndx);
}


int LineProcessor::isAvailable (void) const
{
    if (!isOpen && !isClosing)
        return FALSE;

	for (unsigned int i = 0; i < m_vOccupiedID.size(); i++)
        if ( m_vOccupiedID[i] == -1 )
            return TRUE;

    return FALSE;
}

//Return if m_pQueue is empty.
//Or releases all unoccupied elements from line m_pQueue.
void LineProcessor::openLineProc (ElapsedTime p_time)
{
    if (!m_pQueue)
        return;

    for (unsigned int i = 0; i < m_vOccupiedID.size() && m_pQueue->hasWait(); i++)
        if ( m_vOccupiedID[i] == -1)
            ((LinearQueue *)m_pQueue)->release (m_serviceCoords.getPoint(i), p_time + ElapsedTime(DELAY));

}

//It sets current Person unoccupied and accepts the next waiting Person if any.
//If no more person, calls openLineProc().
//If isClose is True and line m_pQueue is empty, the line service is closed.
void LineProcessor::makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay)
{
    writeLogEvent (aPerson, availTime, CompleteService);

	if (!aPerson)
    {
        openLineProc (availTime);
		
		noticeSourceConveyor( availTime );
        return;
    }

	for( unsigned int i=0; i<m_vOccupiedID.size(); i++ )
	{
		if( m_vOccupiedID[i] == aPerson->getID() )
			m_vOccupiedID[i] = -1;
	}

	Point locationPt;
	aPerson->getTerminalPoint(locationPt);
    // accept the next waiting Person, if any
	if (m_pQueue && m_pQueue->hasWait())
	{
		// make the queue release one person to the available point.
		((LinearQueue *)m_pQueue)->release (locationPt, availTime + ElapsedTime(DELAY));
		//////////////////////////////////////////////////////////////////////////
		// discarded code
		//
		//	((LinearQueue *)m_pQueue)->release (m_serviceCoords.getPoint
		//		(aPerson->getCurrentIndex()), availTime + ElapsedTime(DELAY));
		//////////////////////////////////////////////////////////////////////////
	}
    else if (isClosing && occupants.IsEmpty())
    {
		// close processor if isClosing == TRUE && m_pQueue empty
        isClosing = FALSE;
        writeLogEvent (NULL, availTime, CloseForService);
    }
		
	WakeupWaitingPaxInPrevProc( availTime );
	WakeupWaitingPaxInDependentProc(availTime);

	noticeSourceConveyor( availTime );
}

//Serves a person if the line processor can serve a client.
void LineProcessor::processPerson (Person *aPerson, ElapsedTime arriveTime, bool _bNoDelay)
{
    if (isAvailable())
    {
        ((LinearQueue *)m_pQueue)->removePerson (aPerson);
        aPerson->setState (LeaveQueue);
        aPerson->generateEvent (arriveTime + ElapsedTime(DELAY),false);
    }
}


// do offset for all the position related data. be careful with the derived class.
void LineProcessor::DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset )
{
	Processor::DoOffset( _xOffset, _yOffset );
	m_serviceCoords.DoOffset( _xOffset, _yOffset, 0.0 );
}
void LineProcessor::Rotate( DistanceUnit _degree )
{
	Processor::Rotate( _degree );
	m_serviceCoords.Rotate( _degree );
}

// release person who wait in area because of the roster reason in Person::SelectProc
void LineProcessor::releaseWaitInAreaPax( const ElapsedTime& releaseTime, Processor* _pBackToProc )
{
    Person *aPerson;
    for (int i = occupants.getCount() - 1; i >= 0; i--)
    {
        aPerson = (Person *)occupants.getItem (i);
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
		if( spTerminalBehavior&&spTerminalBehavior->IfWaitInLinkedArea() && _pBackToProc == spTerminalBehavior->GetBackToProc() )
		{
			// set the proc which the person will lead to when he leave this hold area
			//aPerson->SetBackToProc( _pBackToProc );
			spTerminalBehavior->release();		
            spTerminalBehavior->leaveLineProc ( releaseTime );
		}
    }
}

void LineProcessor::beginService (Person *person, ElapsedTime curTime)
{
	//if this lineproc is the wait area of the person, then the person should wait here for some proc opening
	TerminalMobElementBehavior* spTerminalBehavior = person->getTerminalBehavior();
	if(spTerminalBehavior&&spTerminalBehavior->IfWaitInLinkedArea() )
	{
		return;
	}

	Processor::beginService( person, curTime );
}

int LineProcessor::readSpecialField(ArctermFile& procFile)
{
	char chFlag;
	procFile.getChar( chFlag );

	m_bEmergentFlag = ( chFlag == 'Y' ? TRUE : FALSE );

	return TRUE;
}

int LineProcessor::writeSpecialField(ArctermFile& procFile) const
{
	procFile.writeChar( m_bEmergentFlag ? 'Y' : 'N' );

	return TRUE;
}

Processor* LineProcessor::CreateNewProc()
{
	Processor* pProc = new LineProcessor;
	return pProc;
}

void LineProcessor::UpdateMinMax()
{
	Processor::UpdateMinMax();
	GetMinMax(m_min,m_max,m_serviceCoords);
}

void LineProcessor::UpdateFloorIndex(const FloorChangeMap& changMap)
{
	__super::UpdateFloorIndex(changMap);
	changMap.ChangePathFloor(m_serviceCoords);
}
// Protected

// give a person id, found index of service location, return -1 for not found.
int LineProcessor::GetIndexOfServiceLocation( int _nIdPerson ) const
{
	int nServiceLocationCount = m_vOccupiedID.size();
	for( int i=0; i<nServiceLocationCount; i++ )
	{
		if( m_vOccupiedID[i] == _nIdPerson )
			return i;
	}
	return -1;
}

//Point LineProcessor::GetPipeExitPoint(int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob)
//{
//	Point pt;
//	if (inConstraintLength()>0)
//	{
//		
//		return inConstraint(0);
//	}
//	else 
//	{
//
//		pt=GetServiceLocation();
//		terminalMob->SetNextHoldAreaValue(true,pt);
//		return pt;
//		
//	}
//}
