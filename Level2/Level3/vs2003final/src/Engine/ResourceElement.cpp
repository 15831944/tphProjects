// ResourceElement.cpp: implementation of the ResourceElement class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceElement.h"
#include "terminal.h"
#include "results\ResourceElementLog.h"
#include "process.h"
#include "engine\movevent.h"
#include "../Common/ARCTracker.h"
#include "../Main/TermPlanDoc.h"
#include "../Main/Floor2.h"
#include "../Inputs/PipeGraph.h"
#include "../Inputs/PipeDataSet.h"
#include "../Inputs/ProcToResource.h"
#include "../Inputs/Simparameter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResourceElement::ResourceElement( Terminal* _pTerm, CResourcePool* _pool, int _poolIdx, long _id, double _speed, int _index_in_pool )
			   : MobileElement( _id, 0.0 ), m_pTerm( _pTerm), m_pOwnPool( _pool )
{
	m_strName.Format("%s-%d", _pool->getPoolName(), _index_in_pool );

	m_logEntry.setID( _id );
	m_logEntry.setPoolIdx( _poolIdx );
	m_logEntry.setName( m_strName );
	m_logEntry.setSpeed( (float)_speed );
	m_logEntry.setIndex( _pTerm->resourceLog->getCount() );
	m_logEntry.SetEventLog( _pTerm->m_pResourceEventLog );

	_pTerm->resourceLog->addItem(  m_logEntry );
	
	state = Resource_Stay_In_Base;
	speed = _speed * SCALE_FACTOR;
	
	m_ptDestination = location = _pool->getServiceLocation();

	m_CurrentRequestItem.init();

	m_vPipePointList.clear();
	m_statusBeforeEnterPipe = EVENT_ENUM_MAX;
}

ResourceElement::~ResourceElement()
{

}

// get current service pax
const ResourceRequestItem& ResourceElement::getCurrentRequestItem( void )
{
	return m_CurrentRequestItem;
}

// set current service pax
void ResourceElement::setCurrentRequestItem ( const ResourceRequestItem& _item )
{
	m_CurrentRequestItem = _item;
}

//Event handling routine for all movements.
int ResourceElement::move (ElapsedTime currentTime,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	switch( state )
	{
	case Birth:
		writeLogEntry( currentTime, false );
		handleBirth( currentTime );
		break;

	case Death:
		writeLogEntry( currentTime, false );
		handleDeath( currentTime );
		break;

	case Resource_Stay_In_Base:
		writeLogEntry( currentTime, false );
		handleStayInBase( currentTime );
		break;

	case Resource_Arrival_Processor:
		writeLogEntry( currentTime, false );
		handleArrivalProcessor( currentTime );
		break;

	case Resource_Leave_Processor:
		writeLogEntry( currentTime, false );
		handleLeaveProcessor( currentTime );
		break;
			
	case Resource_Back_To_Base:
		writeLogEntry( currentTime, false );
		handleBackToBase( currentTime );
		break;
	case WalkOnPipe:
		writeLogEntry( currentTime, false );
		handleWalkOnPipe( currentTime );
		break;
	default:
		//assert(0);
		break;
	}

	return TRUE;
}


// process birth
void ResourceElement::handleBirth( const ElapsedTime& _time )
{
	m_logEntry.setStartTime( _time );
	setState( Resource_Stay_In_Base );
	return;
}

// process death
void ResourceElement::handleDeath( const ElapsedTime& _time )
{
	kill( _time );
	return;
}

// processor stay in bas
void ResourceElement::handleStayInBase( const ElapsedTime& _time )
{
	// do nothing
	return;
}

// processor arrival processor
void ResourceElement::handleArrivalProcessor( const ElapsedTime& _time )
{
	// first a random service time
	ElapsedTime serviceTime( m_CurrentRequestItem.request_servicetime );
	
	// then notice processor release pax
	assert( m_CurrentRequestItem.request_proc && m_CurrentRequestItem.request_mob );
	if (m_pOwnPool->getPoolType() == ConcurrentType)
	{
		m_CurrentRequestItem.request_proc->noticeReleasePax( m_CurrentRequestItem.request_mob, _time + serviceTime );
	}
	
	// write log entry
	writeLogEntry( _time + serviceTime, false );

	// then let the resource back to base
// 	m_CurrentRequestItem.init();
// 	setDestination( m_pOwnPool->getServiceLocation() );
	setState( Resource_Leave_Processor );
	generateEvent( _time + serviceTime,false );
}

ElapsedTime ResourceElement::moveTime(void)const
{
	ElapsedTime t;
	//	if (location.getZ() != destination.getZ() || !location)
	if (!location)
		t = 0l;
	else
	{

		double dLxy = m_ptDestination.distance(location);
		double dLz = 0.0;
		int nFloorFrom = (int)(location.getZ() / SCALE_FACTOR);
		int nFloorTo = (int)(m_ptDestination.getZ() / SCALE_FACTOR);
		double dL = dLxy;
		if( nFloorFrom != nFloorTo )
		{
			CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
			assert( pDoc );
			dLz = pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloorFrom]->RealAltitude() - pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloorTo]->RealAltitude();
			if( dLz < 0 )
				dLz = -dLz;
			Point pt(dLxy, dLz, 0.0);
			dL = pt.length();
		}

		//		double time = destination.distance(location);
		double time = dL;
		t = (float) (time / (double)getSpeed());
	}
	return t;
}

// processor leave processor
void ResourceElement::handleLeaveProcessor( const ElapsedTime& _time )
{
	processPipe(Resource_Leave_Processor, m_pOwnPool->getServiceLocation(), _time);
	if (m_pOwnPool->getPoolType() == PostServiceType)
	{
		m_CurrentRequestItem.request_proc->makeAvailable(NULL,_time,false);
	}
	m_CurrentRequestItem.init();
}

// processor back to bas
void ResourceElement::handleBackToBase( const ElapsedTime& _time )
{
	// now the the resource back to base
	setState( Resource_Stay_In_Base );
	m_pOwnPool->resourceBackToBase( this, _time );
}

// process walking on pipe
void ResourceElement::handleWalkOnPipe( const ElapsedTime& _time )
{
	if(!m_vPipePointList.empty())
	{
		setDestination(m_vPipePointList.front().pt);
		m_vPipePointList.erase(m_vPipePointList.begin());
	}
	else
	{
		if(m_statusBeforeEnterPipe == Resource_Stay_In_Base)
			setState(Resource_Arrival_Processor);
		else if(m_statusBeforeEnterPipe == Resource_Leave_Processor)
			setState(Resource_Back_To_Base);
		else
			return;
		setDestination(m_ptDestOfPipe);
	}
	generateEvent( _time + moveTime(), false );
	writeLogEntry( _time + moveTime(), false );
}

//Writes the element's current location, state to the log file.
// in:
// _bBackup: if the log is backwards.
void ResourceElement::writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset/* = true*/ )
{
	ResEventStruct track;
	track.time = (long) time;
	track.state = LOBYTE( LOWORD(state) );
	
	if( location != m_ptDestination )
	{
		Point vector( location, m_ptDestination );
		Point latPoint( vector.perpendicular() );
		latPoint.length( 0.8 * SCALE_FACTOR );
		if(bOffset)
			location = m_ptDestination + latPoint;
		else
			location = m_ptDestination;

		m_ptDestination = location;
	}
	location.getPoint( track.x, track.y, track.z );
	m_logEntry.addEvent( track );
}

//Performs all required functions before element's destruction
void ResourceElement::kill (ElapsedTime killTime)
{
	if (getState() == Death)
	{
		return;
	}
	m_logEntry.setEndTime( killTime );
	flushLog( killTime );
	return;
}

// Dumps all tracks to the log and updates ElementDescription record.
void ResourceElement::flushLog (ElapsedTime currentTime)
{
	setState( Death );
	writeLogEntry( currentTime, false );

	long trackCount = m_logEntry.getCurrentCount();
	
    ResEventStruct *log = NULL;
    m_logEntry.getLog (log);
	
    m_logEntry.setEventList (log, trackCount);
	
	delete []log;
	log = NULL;
	
	assert( m_pTerm );
	m_pTerm->resourceLog->updateItem( m_logEntry,m_logEntry.getIndex() );
    m_logEntry.clearLog();
}

//Generates the next movement event for the receiver based on its current state and adds it to the event list.
void ResourceElement::generateEvent (ElapsedTime eventTime,bool bNoLog)
{
	static short testID = -1;
    MobileElementMovementEvent *aMove = new ResourceElementMovementEvent;
    ElapsedTime destTime = aMove->removeMoveEvent (this);
	
//    if (id == testID)
//        cout << '\r';
	
    aMove->init (this, eventTime,bNoLog);
    aMove->addEvent();
	m_prevEventTime = aMove->getTime();
}

void ResourceElement::processPipe(EntityEvents _status, Point _destPoint, const ElapsedTime _curTime)
{
	m_ptDestOfPipe = _destPoint;
	m_statusBeforeEnterPipe = _status;
	// if not same floor, need not move in pipe
	int iCurFloor = (int)(location.getZ() / SCALE_FACTOR);
	int iEntryFloor = (int)(_destPoint.getZ() / SCALE_FACTOR);
	if (iCurFloor != iEntryFloor)
	{
		walkStraightly(_destPoint, _curTime);
		return;
	}
	CPipeGraphMgr* pPipeMgr = m_pTerm->m_pPipeDataSet->m_pPipeMgr;
	if (!pPipeMgr->checkCanMoveByPipe(iEntryFloor))
	{
		walkStraightly(_destPoint, _curTime);
		return;
	}

	PROC2RESSET& proc2ResList = m_pTerm->m_pProcToResourceDB->getProc2ResList();
	PROC2RESSET::iterator iter;
	CProcToResource procToRes;
	for( iter = proc2ResList.begin(); iter!=proc2ResList.end(); ++iter )
	{
		if(strcmp(iter->getResourcePoolName(), m_pOwnPool->getPoolName()) != 0)
			continue;
		ProcessorID process = iter->getProcessorID();
		ProcessorID reqProc = *(m_CurrentRequestItem.request_proc->getID());
		if(process == reqProc)
		{
			procToRes = *iter;
			break;
		}
		if(process.idFits(reqProc))
		{
			procToRes = *iter;
		}
	}

	if(procToRes.GetTypeOfUsingPipe() == USE_NOTHING)
	{
		walkStraightly(_destPoint, _curTime);
	}
	else if(procToRes.GetTypeOfUsingPipe() == USE_PIPE_SYSTEM)
	{
		walkAlongShortestPath(_destPoint, _curTime);
	}
	else if(procToRes.GetTypeOfUsingPipe() == USE_USER_SELECTED_PIPES)
	{
		walkAlongUserDefinePath(_destPoint, procToRes.GetPipeVector(), _curTime);
	}
}

void ResourceElement::walkStraightly(Point _destPoint, const ElapsedTime _curTime)
{
	if(m_statusBeforeEnterPipe == Resource_Stay_In_Base)
		setState(Resource_Arrival_Processor);
	else if(m_statusBeforeEnterPipe == Resource_Leave_Processor)
		setState(Resource_Back_To_Base);
	setDestination(_destPoint);
	generateEvent( _curTime + moveTime(), false );
	writeLogEntry( _curTime + moveTime(), false );
}

void ResourceElement::walkAlongShortestPath(Point _destPoint, const ElapsedTime _curTime)
{
	CGraphVertexList shortestPath;
	CPipeGraphMgr* pPipeMgr = m_pTerm->m_pPipeDataSet->m_pPipeMgr;
	if (!pPipeMgr->getShortestPathFromLib(location, _destPoint, shortestPath))
	{
		walkStraightly(_destPoint, _curTime);
		return;
	}

	int nVertexCount = shortestPath.getCount();
	if (nVertexCount < 3)
	{
		walkStraightly(_destPoint, _curTime);
		return;
	}

	PTONSIDEWALK logPointList;
	int iPercent = random(100);
	m_pTerm->m_pPipeDataSet->GetPointListForLog( shortestPath, iPercent, logPointList );

	m_vPipePointList.clear();
	for (PTONSIDEWALK::iterator iter = logPointList.begin(); iter != logPointList.end(); iter++)
	{
		PipePointInformation pipeInfor;
		if (m_vPipePointList.empty())
		{
			pipeInfor.m_nPrePipe = -1;
		}
		else
		{
			pipeInfor.m_nPrePipe = m_vPipePointList.back().m_nCurPipe;
		}

		pipeInfor.pt = iter->GetPointOnSideWalk();
		pipeInfor.m_nCurPipe = iter->GetPipeIdx();
		m_vPipePointList.push_back(pipeInfor);
	}
	setState(WalkOnPipe);
	generateEvent(_curTime,false);
}

void ResourceElement::walkAlongUserDefinePath(Point _destPoint, std::vector<int> pipeList, const ElapsedTime _curTime)
{
	int nPipeCount = pipeList.size();
	if( nPipeCount == 0 )
	{
		walkStraightly(_destPoint, _curTime);
		return;
	}

	std::vector<int> workingPipeList;
	for( int i=0; i<nPipeCount; i++ )
	{
		CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( pipeList[i] );
		if( pPipe->GetZ() == location.getZ() )
		{
			workingPipeList.push_back( pipeList[i] );
		}
	}

	nPipeCount = workingPipeList.size();
	if( nPipeCount == 0 )
	{
		walkStraightly(_destPoint, _curTime);
		return;
	}

	writeLogEntry( _curTime, false );

	CPointToPipeXPoint entryPoint;
	CPointToPipeXPoint exitPoint;
	std::vector<CMobPipeToPipeXPoint> vMidPoint;	// num count should be nPipeCount - 1

	CPipe* pPipe1 = NULL;
	CPipe* pPipe2 = NULL;
	for( i=0; i<nPipeCount; i++ )
	{
		if( i == 0 )
		{
			pPipe1 = m_pTerm->m_pPipeDataSet->GetPipeAt( workingPipeList[0] );
			entryPoint = pPipe1->GetIntersectionPoint( location );

			if( nPipeCount == 1 )
			{
				exitPoint = pPipe1->GetIntersectionPoint( _destPoint );
			}
			else
			{
				pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt( workingPipeList[1] );
				CMobPipeToPipeXPoint midPt;
				if( pPipe1->GetIntersectionPoint( pPipe2, entryPoint, midPt ) )
				{
					vMidPoint.push_back( midPt );
				}
				else
				{
					kill(_curTime);
					throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(),pPipe2->GetPipeName(),"", ClacTimeString(_curTime));
				}
			}
		}
		else if( i == nPipeCount - 1 )
		{
			exitPoint = pPipe1->GetIntersectionPoint( _destPoint );
			vMidPoint[vMidPoint.size()-1].SetOutInc( exitPoint );
		}
		else
		{
			pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt( workingPipeList[i+1] );
			CMobPipeToPipeXPoint midPt;
			if( pPipe1->GetIntersectionPoint( pPipe2, vMidPoint[vMidPoint.size()-1], midPt ) )
			{
				vMidPoint[vMidPoint.size()-1].SetOutInc( midPt );
				vMidPoint.push_back( midPt );
			}
			else
			{
				kill(_curTime);
				throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(),pPipe2->GetPipeName(),"", ClacTimeString(_curTime));
			}
		}
		pPipe1 = pPipe2;
	}

	ElapsedTime eventTime = _curTime;

	// process entry point
	CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( workingPipeList[0] );

	PTONSIDEWALK allPtList, partialPtList;
	int nPercent = random( 100 );
	int nMidPoint = vMidPoint.size();

	if( nMidPoint == 0 )
	{	
		// add entry point and exit point only.
		pPipe->GetPointListForLog( workingPipeList[0],entryPoint, exitPoint, nPercent, allPtList );
	}
	else
	{
		// add entry point
		pPipe->GetPointListForLog( workingPipeList[0],entryPoint, vMidPoint[0], nPercent, allPtList );
		// add mid point
		for( int i=1; i<nMidPoint; i++ )
		{
			pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( workingPipeList[i] );
			if( vMidPoint[i-1].OrderChanged() )
				nPercent = 100 - nPercent;

			pPipe->GetPointListForLog( workingPipeList[0],vMidPoint[i-1], vMidPoint[i], nPercent ,partialPtList );
			copy(partialPtList.begin(), partialPtList.end(), back_inserter(allPtList));
		}
		// add exit point
		pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( workingPipeList[nPipeCount-1] );
		if( vMidPoint[i-1].OrderChanged() )
			nPercent = 100 - nPercent;
		pPipe->GetPointListForLog( workingPipeList[0],vMidPoint[nMidPoint-1], exitPoint, nPercent,partialPtList );
		copy(partialPtList.begin(), partialPtList.end(), back_inserter(allPtList));
	}

	m_vPipePointList.clear();
	for (PTONSIDEWALK::iterator iter = allPtList.begin(); iter != allPtList.end(); iter++)
	{
		PipePointInformation pipeInfor;
		if (m_vPipePointList.empty())
		{
			pipeInfor.m_nPrePipe = -1;
		}
		else
		{
			pipeInfor.m_nPrePipe = m_vPipePointList.back().m_nCurPipe;
		}

		pipeInfor.pt = iter->GetPointOnSideWalk();
		pipeInfor.m_nCurPipe = iter->GetPipeIdx();
		m_vPipePointList.push_back(pipeInfor);
	}
	setState(WalkOnPipe);
	generateEvent(_curTime,false);
}

CString ResourceElement::ClacTimeString(const ElapsedTime& _curTime)
{
	char str[64];
	ElapsedTime tmptime(_curTime.asSeconds() % WholeDay);
	tmptime.printTime ( str );

	CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
	bool bAbsDate;
	COleDateTime date;
	int nDtIdx;
	COleDateTime time;
	CString sDateTimeStr;
	sDate.GetDateTime( _curTime, bAbsDate, date, nDtIdx, time );
	if( bAbsDate )
	{
		sDateTimeStr = date.Format(_T("%Y-%m-%d"));
	}
	else
	{
		sDateTimeStr.Format("Day %d", nDtIdx + 1 );
	}
	sDateTimeStr += " ";
	sDateTimeStr += str;

	return sDateTimeStr;
}

