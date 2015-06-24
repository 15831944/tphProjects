// ResourceElement.cpp: implementation of the ResourceElement class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceElement.h"
#include "terminal.h"
#include "results\ResourceElementLog.h"
#include "process.h"
#include "common\states.h"
#include "engine\movevent.h"
#include "../Common/ARCTracker.h"
#include "../Main/TermPlanDoc.h"
#include "../Main/Floor2.h"
#include "../Inputs/PipeGraph.h"
#include "../Inputs/PipeDataSet.h"

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
void ResourceElement::handleLeaveProcessor(  const ElapsedTime& _time )
{
	setDestination( m_pOwnPool->getServiceLocation() );
	setState( Resource_Back_To_Base );
	generateEvent( _time + moveTime(),false );
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

void ResourceElement::WalkAlongShortestPath(Point destPoint, const ElapsedTime _curTime)
{
	// if not same floor, need not move in pipe
	int iCurFloor = (int)(location.getZ() / SCALE_FACTOR);
	int iEntryFloor = (int)(destPoint.getZ() / SCALE_FACTOR);
	if (iCurFloor != iEntryFloor)
		return;


//	Point outPoint = location;
//	Point entryPoint;
	// if no pipe , need not to move in pipe
	CPipeGraphMgr* pPipeMgr = m_pTerm->m_pPipeDataSet->m_pPipeMgr;
	if (!pPipeMgr->checkCanMoveByPipe(iEntryFloor))
		return;


	CGraphVertexList shortestPath;
	if (!pPipeMgr->getShortestPathFromLib(location, destPoint, shortestPath))
		return;

	int nVertexCount = shortestPath.getCount();
	if (nVertexCount < 3)
		return;

//	writeLogEntry( _curTime, false );

	setState( WalkOnPipe );

	// get the real path, and write log
	PTONSIDEWALK LogPointList;
	int iPercent = random(100);
	m_pTerm->m_pPipeDataSet->GetPointListForLog( shortestPath, iPercent, LogPointList );
	int tempSize = (int)LogPointList.size();
	for(int i=0; i<tempSize; i++)
	{
		DistanceUnit x = LogPointList[i].getX();
		DistanceUnit y = LogPointList[i].getY();
		DistanceUnit z = LogPointList[i].getZ();
	}
	//Point tempPoint2 = LogPointList.at(0).GetPointOnSideWalk();
	//setDestination(tempPoint2);

	//_curTime += moveTime();
	WritePipeLogs( LogPointList, _curTime);
}

void ResourceElement::WritePipeLogs( PTONSIDEWALK& _vPointList, const ElapsedTime _eventTime,  bool _bNeedCheckEvacuation )
{
	m_vPipePointList.clear();
	for (PTONSIDEWALK::iterator iter = _vPointList.begin(); iter != _vPointList.end(); iter++)
	{
		PipePointInformation pipeInfor;
		if (m_vPipePointList.empty())
		{
			pipeInfor.m_nPrePipe = -1;
		}
		else
		{
			PipePointInformation pipeInfor111 = m_vPipePointList.back();
			pipeInfor.m_nPrePipe = pipeInfor111.m_nCurPipe;//todo
		}

		pipeInfor.pt = iter->GetPointOnSideWalk();
		pipeInfor.m_nCurPipe = iter->GetPipeIdx();
		m_vPipePointList.push_back(pipeInfor);
	}

	setState(WalkOnPipe);
	generateEvent(_eventTime,false);

	//if(!m_bEvacuationWhenInPipe && !m_bUserPipes)
	//	return;

/*	CTimePointOnSideWalk tempPoint, prePoint;
//	ElapsedTime timeFireTime = getEngine()->GetFireEvacuation();

//	if( _bNeedCheckEvacuation && m_bEvacuationWhenInPipe)
//		return;

	int iMovingSideWalkIdx = -1;
	int iPipeIndex = -1;
	prePoint.SetOnSideWalkFlag( false );
	prePoint.SetPoint( location );

	ElapsedTime tTime = _eventTime;
	for( UINT m=0; m<_vPointList.size(); m++ )
	{
		tempPoint = _vPointList[m];

		setDestination( tempPoint);

		if( tempPoint.GetOnSideWalkFlag() )
		{
			if( tempPoint.GetSideWalkIdx() != iMovingSideWalkIdx )
			{
				setDestination( tempPoint);

				//processBillboard(_eventTime);

				_eventTime += moveTime();
				//				writeLogEntry( _eventTime );
				iMovingSideWalkIdx = tempPoint.GetSideWalkIdx();
			}

			_eventTime += tempPoint.GetTime();	
		}
		else
		{
			//if when firing, the person is in pipe.
			if( _bNeedCheckEvacuation && _eventTime + moveTime() >= timeFireTime )
			{
				Point strandPoint;
				if( prePoint.GetOnSideWalkFlag() )
				{
					strandPoint = tempPoint.GetPoint();
				}
				else
				{
					Point vPoint( prePoint.GetPoint(), tempPoint.GetPoint() );
					double detaTime = ( timeFireTime - _eventTime ).asSeconds();
					double dLength = detaTime * m_pPerson->speed;
					vPoint.length( dLength );

					strandPoint= prePoint.GetPoint() + vPoint;
				}

				strandPoint.setZ( prePoint.GetPoint().getZ() );
				setDestination( strandPoint );
				if (getEngine()->IsLandSel())
				{
					bool bCreateNew = (tempPoint.GetPipeIdx() != iPipeIndex ? true : false);
					if (tempPoint.GetPipeIdx() != iPipeIndex)
					{
						CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( tempPoint.GetPipeIdx() );
						pPipe->WritePipePax(getEngine()->GetLandsideSimulation(),getLocation(),strandPoint.GetPoint(),m_pPerson->getID(),_eventTime,timeFireTime + 62l);
					}
				}

				writeLogEntry( timeFireTime, false );
				long lReflectTime = random( 60 );
				writeLogEntry( timeFireTime + lReflectTime + 2l, false );

				setState( EvacuationFire );
				SetFireEvacuateFlag( true );
				generateEvent( timeFireTime + lReflectTime + 2l ,false);


				m_bEvacuationWhenInPipe = true;
				m_bUserPipes = false;
				break;
			}	
			else
			{
				processBillboard(_eventTime);
			}

			_eventTime += moveTime();
		}
		writeLogEntry( _eventTime, false );
		//write landside crosswalk
		{
			if (getEngine()->IsLandSel())
			{
				bool bCreateNew = (tempPoint.GetPipeIdx() != iPipeIndex ? true : false);
				if (tempPoint.GetPipeIdx() != iPipeIndex)
				{
					CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( tempPoint.GetPipeIdx() );
					pPipe->WritePipePax(getEngine()->GetLandsideSimulation(),getLocation(),tempPoint.GetPoint(),m_pPerson->getID(),tTime,_eventTime);
				}
				tTime = _eventTime;
			}

		}

		prePoint = tempPoint;
	}*/
}

void ResourceElement::handleWalkOnPipe( const ElapsedTime& _time )
{
	if(m_vPipePointList.empty())
		return;

	int xxxx = m_vPipePointList.size();
	Point curPoint = location;
	Point tempPoint;
	for(int i=0; i<xxxx; i++)
	{
		tempPoint = m_vPipePointList[i].pt;
	}
	setDestination(m_vPipePointList.front().pt);
	writeLogEntry( _time, false );
	m_vPipePointList.erase(m_vPipePointList.begin());
	if(m_vPipePointList.empty())
	{
		if(getPreState() == Resource_Stay_In_Base)
		{
			setState(Resource_Arrival_Processor);
		}
		else if(getPreState() == Resource_Leave_Processor)
		{
			setState(Resource_Back_To_Base);
		}
	}
	long mTime = moveTime().getPrecisely();
	generateEvent( _time + moveTime(),false );
}

