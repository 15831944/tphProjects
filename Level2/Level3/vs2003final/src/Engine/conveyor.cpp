// conveyor.cpp: implementation of the Conveyor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "conveyor.h"
#include "common\line.h"
#include "common\pollygon.h"
#include "inputs\miscproc.h"
#include "inputs\in_term.h"
#include "Pusher.h"
#include "simpleconveyor.h"
#include "Splitter.h"
#include "loader.h"
#include "Scanner.h"
#include "FlowBelt.h"
#include "sorter.h"
#include "engine\person.h"
#include "inputs\procdata.h"
#include "Main\TermPlanDoc.h"
#include <limits>
#define CAPACITY_CONST 67L 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Conveyor::Conveyor():m_pPerformer( NULL )
{
	m_dWidth = 3*SCALE_FACTOR;
	m_enConveyorSubType = SIMPLE_CONVEYOR;
	m_bNeedSyn = false;
}

Conveyor::~Conveyor()
{
	if( m_pPerformer )
	{
		delete m_pPerformer;
		m_pPerformer = NULL;
	}
	else
	{
		m_pTransition=NULL;//because Conveyor and its m_pPerformer use the same property and pointer. please look at ConstructPerformer();
	}
}
void Conveyor::initServiceLocation (int pathCount, const Point *pointList)
{
	if (pathCount < 2)
        throw new StringError ("Conveyor processor must have at least 2 point service locations");

	m_location.init (pathCount, pointList);
	m_vConveyorShape.clear();
	for( int i=0; i<pathCount; ++i )
	{
		PIPEPOINT tempPoint;
		tempPoint.m_point = pointList[ i ];
		tempPoint.m_width = m_dWidth;
		m_vConveyorShape.push_back( tempPoint );
	}
}
void Conveyor::SetWidth( DistanceUnit _dWidth ) 
{
	m_dWidth = _dWidth;
	int iSize = m_vConveyorShape.size();
	for( int i=0; i<iSize; ++i )
	{
		m_vConveyorShape[i].m_width = _dWidth;
	}

}
// fill the bisect line in the m_vConveyorShape
void Conveyor::CalculateTheBisectLine()
{
	Point ptStart;
	Point ptMid;
	Point ptEnd;
	Point point1;
	Point point2;
	DistanceUnit dWidth;

	int nPointCount = m_vConveyorShape.size();
	assert( nPointCount > 1 );

	// calculate the first line
	ptStart = m_vConveyorShape[0].m_point;
	ptEnd = m_vConveyorShape[1].m_point;
	Point centralLineVector( ptStart, ptEnd );
	Point perpVector1 = centralLineVector.perpendicularY();
	dWidth = m_vConveyorShape[0].m_width;
	perpVector1.length( dWidth / 2.0 );
	point1 = perpVector1 + ptStart;
	Point perpVector2 = centralLineVector.perpendicular();
	perpVector2.length( dWidth / 2.0 );
	point2 = perpVector2 + ptStart;
	m_vConveyorShape[0].m_bisectPoint1 = point1;
	m_vConveyorShape[0].m_bisectPoint2 = point2;
	
	DistanceUnit dZ = ptStart.getZ();
	m_vConveyorShape[0].m_bisectPoint1.setZ( dZ );
	m_vConveyorShape[0].m_bisectPoint2.setZ( dZ );

	// calculate the middle line.
	for( int i=1; i<nPointCount-1; i++ )
	{
		ptStart = m_vConveyorShape[i-1].m_point;
		ptMid = m_vConveyorShape[i].m_point;
		dZ = ptMid.getZ();
		ptEnd = m_vConveyorShape[i+1].m_point;
		Point vector1 = Point( ptMid, ptStart );
		Point vector2 = Point( ptMid, ptEnd );
		vector1.length( 1.0 );
		vector2.length( 1.0 );
		Point tempPt = ptMid + vector1;
		tempPt += vector2;
		Point resVector1 = Point( ptMid, tempPt );
		dWidth = m_vConveyorShape[i].m_width;

		// now calculate the width.
		double dCosVal = vector1.GetCosOfTwoVector( vector2 );
		double deg = acos( dCosVal );
		deg /= 2.0;
		dWidth /= sin( deg );

		resVector1.length( dWidth / 2.0 );
		Point resVector2 = Point( tempPt, ptMid );		
		resVector2.length( dWidth / 2.0 );
		point1 = ptMid + resVector1;
		point2 = ptMid + resVector2;
		// CHECK THE ORDER OF POINT 1 and POINT 2
		if( IsCrossOver( m_vConveyorShape[i-1].m_bisectPoint1, m_vConveyorShape[i-1].m_bisectPoint2, point1, point2 ) )
		{
			m_vConveyorShape[i].m_bisectPoint1 = point2;
			m_vConveyorShape[i].m_bisectPoint2 = point1;
		}
		else
		{
			m_vConveyorShape[i].m_bisectPoint1 = point1;
			m_vConveyorShape[i].m_bisectPoint2 = point2;
		}

		m_vConveyorShape[i].m_bisectPoint1.setZ( dZ );
		m_vConveyorShape[i].m_bisectPoint2.setZ( dZ );
	}
		

	// calculate the last line
	ptStart = m_vConveyorShape[nPointCount-2].m_point;
	ptEnd = m_vConveyorShape[nPointCount-1].m_point;
	dZ = ptEnd.getZ();
	centralLineVector = Point( ptStart, ptEnd );
	perpVector1 = centralLineVector.perpendicularY();
	dWidth = m_vConveyorShape[nPointCount-1].m_width;
	perpVector1.length( dWidth / 2.0 );
	point1 = perpVector1 + ptEnd;
	perpVector2 = centralLineVector.perpendicular();
	perpVector2.length( dWidth / 2.0 );
	point2 = perpVector2 + ptEnd;
	m_vConveyorShape[nPointCount-1].m_bisectPoint1 = point1;
	m_vConveyorShape[nPointCount-1].m_bisectPoint2 = point2;

	m_vConveyorShape[nPointCount-1].m_bisectPoint1.setZ( dZ );
	m_vConveyorShape[nPointCount-1].m_bisectPoint2.setZ( dZ );
/*
	for(  i=0; i<nPointCount; ++i )
	{
		// TRACE("\n z1= %.2f; z2= %.2f; z=%.2f\n", m_vConveyorShape[i].m_bisectPoint1.getZ(),m_vConveyorShape[i].m_bisectPoint2.getZ(),m_vConveyorShape[i].m_point.getZ() );
	}
	*/
}
// check if ( _pt11, _pt21 ) X ( _pt12, _pt22 )
bool Conveyor::IsCrossOver( Point _pt11, Point _pt12, Point _pt21, Point _pt22 )
{
	Line line1( _pt11, _pt21 );
	Line line2( _pt12, _pt22 );
	return line1.intersects( line2) == TRUE ? true : false;
}

//get all region the conveyor covered, it consisted of a list of pollygon.
void Conveyor::GetCoveredRegion( POLLYGONVECTOR& _regions ) const
{
	_regions.clear();
	int iPipePointCount = m_vConveyorShape.size();
	if( iPipePointCount <2 )
			return ;

	//POLLYGONVECTOR tempVector;=
	PIPESHAPE::const_iterator iter = m_vConveyorShape.begin();
	PIPESHAPE::const_iterator iterNext = iter;
	++iterNext;
	PIPESHAPE::const_iterator iterEnd = m_vConveyorShape.end();
	for(int i=0; iterNext != iterEnd ; ++iter,++iterNext,++i )
	{
		Pollygon temp;
		Point vertex[4];
		vertex[0] = iter->m_bisectPoint1;
		vertex[1] = iter->m_bisectPoint2;
		vertex[2] = iterNext->m_bisectPoint2;
		vertex[3] = iterNext->m_bisectPoint1;
		temp.init( 4, vertex );
		//GetSiglePollygon( *iter, *iterNext, temp );
		_regions.push_back( temp );
	}
}

int Conveyor::readSpecialField(ArctermFile& procFile)
{
	procFile.getFloat( m_dWidth );
	return TRUE;
}
int Conveyor::writeSpecialField(ArctermFile& procFile)const
{
	procFile.writeFloat((float)m_dWidth );
	return TRUE;
}
void Conveyor::ConstructPerformer( const MiscConveyorData * _pMiscData )
{
	if( m_pPerformer )
	{
		delete m_pPerformer;
		m_pPerformer = NULL;
	}

	if( _pMiscData )
	{
		m_enConveyorSubType = _pMiscData->GetSubType();
		switch( m_enConveyorSubType ) 
		{
			case LOADER:
				m_pPerformer = new CLoader();
				*((Conveyor*)m_pPerformer) = *this;
				break;

			case SIMPLE_CONVEYOR:		
				m_pPerformer = new CSimpleConveyor ();
				*((Conveyor*)m_pPerformer) = *this;
				break;

			case UNIT_BELT:// unit belt is just a simple conveyor with capacity of one
				m_pPerformer = new CSimpleConveyor ();
				*((Conveyor*)m_pPerformer) = *this;
				break;

			case MERGE_BOX:// merge box is just a simple conveyor with capacity of one. the same with unit belt
				m_pPerformer = new CSimpleConveyor ();
				*((Conveyor*)m_pPerformer) = *this;
				break;
				
			case FEEDER:// is just a simple conveyor
				m_pPerformer = new CSimpleConveyor ();
				*((Conveyor*)m_pPerformer) = *this;
				break;
			case SCANNER:
				m_pPerformer = new Scanner ();
				*((Conveyor*)m_pPerformer) = *this;			
				break;

			case SPLITER:
				m_pPerformer = new Splitter();
				*((Conveyor*)m_pPerformer) = *this;			
				break;

			case PUSHER:
				m_pPerformer = new Pusher();
				*((Conveyor*)m_pPerformer) = *this;
				break;		

			case FLOW_BELT:
				m_pPerformer =new CFlowBelt();
				*((Conveyor*)m_pPerformer) = *this;
				break;
			case SORTER:
				m_pPerformer =new CSorter();
				*((Conveyor*)m_pPerformer) = *this;
				break;
		}

	}
	else// if not define behaviour, set it as simple conveyor
	{
		m_pPerformer = new CSimpleConveyor ();
		*((Conveyor*)m_pPerformer) = *this;
	}

	ASSERT( m_pPerformer );
	m_pPerformer->m_pPerformer=NULL;
}

long Conveyor::CaculateConveyorCapacity()
{
	const Path* path = serviceLocationExceedTwoFloorPath();
	DistanceUnit dLength = path->GetTotalLength();
	if( dLength <= 0.0 )
	{
		return 0L;
	}
	long lCapacity = (long)(dLength / CAPACITY_CONST );
	return lCapacity > 0L ? lCapacity : 1L  ;
}
void Conveyor::InitData( const MiscConveyorData * _pMiscData )
{
	ASSERT( m_pPerformer );
	m_pPerformer->InitData( _pMiscData );
}
void Conveyor::beginService (Person *aPerson, ElapsedTime curTime)
{
	ASSERT( m_pPerformer );
	m_pPerformer->beginService( aPerson, curTime );
}

// Adds Person to the processor's occupants list.
void Conveyor::addPerson (Person *aPerson)
{
	ASSERT( m_pPerformer );
	m_pPerformer->addPersonIntoApproachList( aPerson );
	m_pPerformer->addPersonIntoOccupiedList( aPerson );
}
// Removes Person from the processor's occupants list.
void Conveyor::removePerson (const Person *aPerson)
{
	ASSERT( m_pPerformer );
	m_pPerformer->removePersonFromOccupiedList( aPerson );
	m_pPerformer->removePersonFromApproachList(aPerson);
}

// check if the conveyor is vacant
bool Conveyor::isVacant( Person* pPerson ) const 
{
	ASSERT( m_pPerformer );
	return m_pPerformer->isVacant(pPerson);
}

bool Conveyor::isExceedProcCapacity( void ) const
{
	ASSERT( m_pPerformer );
	return m_pPerformer->isExceedConveyorCapacity();
}
int Conveyor::GetEmptySlotCount( void ) const
{
	ASSERT( m_pPerformer );
	return m_pPerformer->GetEmptySlotCount();
}
// notify conveyor that one of its dest processor is available now
bool Conveyor::destProcAvailable( const ElapsedTime& time )
{
	ASSERT( m_pPerformer );
	return m_pPerformer->destProcAvailable( time );
}

// if need to wake up the processor
bool Conveyor::ifNeedToNotice( void ) const
{
	ASSERT( m_pPerformer );
	return m_pPerformer->_ifNeedToNotice();
}

// get the release person' time
ElapsedTime Conveyor::getReleasePersonTime( void ) const
{
	ASSERT( m_pPerformer );
	return m_pPerformer->_getReleasePersonTime();
}


//It sets Person's next destination based on state & processor data
void Conveyor::getNextLocation (Person *aPerson)
{
	ASSERT( m_pPerformer );
	m_pPerformer->getDestinationLocation( aPerson );
}

void Conveyor::getDestinationLocation( Person* aPerson )
{
	Processor::getNextLocation( aPerson );
}

void Conveyor::addPersonIntoApproachList( Person *_pPerson )
{
	return;	
}
void Conveyor::addPersonIntoOccupiedList( Person *_pPerson )
{
	Processor::addPerson( _pPerson );
}
void Conveyor::removePersonFromOccupiedList(const Person *_pPerson )
{
	Processor::removePerson( _pPerson );
}

bool Conveyor::isExceedConveyorCapacity( void ) const
{
	return !isVacant();
}
/*
// check to see if there is at least one dest processor is available now
bool Conveyor::TryToSetNextAvailableProcessor( Person * _pPerson )const
{
	static iNextProcIdx = -1;
	int iNextProcCount = m_vDestProcs.size();

	iNextProcIdx = (iNextProcIdx+1) % iNextProcCount;
	for( int j=iNextProcIdx; j<iNextProcCount; ++j )
	{
		Processor* pProc = m_vDestProcs[ j ];
		if (pProc->isAvailable() && pProc->isVacant() && pProc->canServe( _pPerson->getType() ) )
		{
			pProc->setAvail( FALSE );
			_pPerson->SetDirectGoToNextProc( pProc );
			return true;
		}
			
	}
	
	for( j=0; j<iNextProcIdx; ++j )
	{
		Processor* pProc = m_vDestProcs[ j ];
		if (pProc->isAvailable() && pProc->isVacant() && pProc->canServe( _pPerson->getType() ) )
		{
			_pPerson->SetDirectGoToNextProc( pProc );
			pProc->setAvail( FALSE );
			return true;
		}
	}
	
	return false;
}
*/

bool Conveyor::_ifNeedToNotice( void ) const
{
	return false;
}

ElapsedTime Conveyor::_getReleasePersonTime( void ) const
{
	return ElapsedTime( 24*60*60l);
}

long Conveyor::getLoad()const
{
	return 0;
}
long Conveyor::GetStopReason()const
{
	if( m_pPerformer )
	{
		return m_pPerformer->GetStopReason();
	}
	return m_lReason;	
}

void Conveyor::writeLog (void)
{
	if( m_pPerformer )
	{
		m_pPerformer->writeLog();
	}
	else
	{
		Processor::writeLog();
	}
}
void Conveyor::sortLog  ( void )
{
	if( m_pPerformer )
	{
		m_pPerformer->sortLog();
	}
	else
	{
		Processor::sortLog();
	}
}

HoldingArea* Conveyor::GetRandomHoldingArea(void)
{
	if (m_pPerformer)
		return (m_pPerformer->GetRandomHoldingArea());
	else
		return (0);
}

void Conveyor::LogDebugInfo()
{
	if (m_pPerformer)
	{
		m_pPerformer->LogDebugInfo();
	}
}
void Conveyor::flushLog (void)
{
	if( m_pPerformer )
	{
		m_pPerformer->flushLog();
	}
	else
	{
		Processor::flushLog();
	}
}

void Conveyor::flushPersonLog(const ElapsedTime& _time)
{
	if (m_pPerformer)
	{
		m_pPerformer->flushPersonLog(_time);
	}
}

void Conveyor::InitSorter()
{
	if(m_pPerformer==NULL || m_pPerformer->GetSubConveyorType()!=SORTER)return;
	static_cast<CSorter*>(m_pPerformer)->InitEntryExitSlots();
}

Processor* Conveyor::GetSorterExitProcs(int _nIndex)
{
	ASSERT( m_pPerformer!=NULL && m_pPerformer->GetSubConveyorType()== SORTER);
	return static_cast<CSorter*>(m_pPerformer)->GetExitProcessor( _nIndex );
}

int Conveyor::GetSorterEntryProcs(const ProcessorID& _procsID)
{
	ASSERT( m_pPerformer!=NULL && m_pPerformer->GetSubConveyorType()== SORTER);
	return static_cast<CSorter*>(m_pPerformer)->GetEntryProcessor( _procsID );
}
bool Conveyor::IsSorterEntryVacant(int _nEntryProcsIndex)
{
	ASSERT( m_pPerformer!=NULL && m_pPerformer->GetSubConveyorType()== SORTER);

	return static_cast<CSorter*>(m_pPerformer)->IsEntryVacant( _nEntryProcsIndex );
}


bool Conveyor::CanAcceptPersonToSorter(const ProcessorID& _entryProcsID , const ElapsedTime& _time ) const
{
	ASSERT( m_pPerformer!=NULL && m_pPerformer->GetSubConveyorType()== SORTER);
	
	return static_cast<CSorter*>(m_pPerformer)->CanAcceptPerson( _entryProcsID, _time );

}

Processor* Conveyor::CreateNewProc()
{
	Processor* pProc = new Conveyor;
	return pProc;
}


bool Conveyor::CopyOtherData(Processor* _pDestProc)
{
	((Conveyor*)_pDestProc)->SetWidth( GetWidth() );

	return true;
}

void Conveyor::RemoveHeadPerson(const ElapsedTime& time)
{
	if (m_pPerformer)
	{
		m_pPerformer->RemoveHeadPerson(time);
	}
}

const Path* Conveyor::serviceLocationExceedTwoFloorPath()
{
	//check the path whether exceed two floor
	if (m_bNeedSyn)
	{
		return &m_realPath;
	}

	m_realPath.clear();

	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	if (pDoc == NULL)
		return &m_location;

	Path realPath = pDoc->GetFloorByMode(EnvMode_Terminal).getRealAttitudePath(m_location);

	//first point floor
	Point prePoint = m_location.getPoint(0);
	Point preRealPt = realPath.getPoint(0);

	int nPreFloor = int(prePoint.getZ()/SCALE_FACTOR);
	Point pointList[1];
	pointList[0] = prePoint;
	m_realPath.init(1,pointList);

	for (int i = 1; i < m_location.getCount(); i++)
	{
		const Point& point = m_location.getPoint(i);
		const Point& realPoint = realPath.getPoint(i);

		////T244, made the change
		//int nCurrentFloor = (int)(point.getZ()/SCALE_FACTOR);

		////exceed two floor, need insert point to the floor where between the two floor
		//while (nCurrentFloor != nPreFloor)
		//{
		//	Point thisVector;
		//	thisVector.initVector3D(preRealPt,realPoint);
		//	double dTotalLength = thisVector.length3D();

		//	if (nCurrentFloor > nPreFloor)
		//	{
		//		nPreFloor++;
		//	}
		//	else
		//	{
		//		nPreFloor--;
		//	}

		//	if (nCurrentFloor == nPreFloor)
		//		break;		

		//	double dDist = pDoc->GetFloorByMode(EnvMode_Terminal).getLeavlRealAttitude(nPreFloor);
		//	double dRate = 0.0; 
		//	double dDeta = realPoint.getZ() - preRealPt.getZ();
		//	if (dDeta > (std::numeric_limits<double>::min)())
		//	{
		//		dRate = ((dDist - preRealPt.getZ())/(realPoint.getZ() - preRealPt.getZ()));
		//	}
		//	

		//	double dLength = dTotalLength*dRate;
		//	thisVector.length3D(dLength);
		//	thisVector.plus(preRealPt);

		//	prePoint = point;
		//	preRealPt = thisVector;

		//	thisVector.setZ(nPreFloor*SCALE_FACTOR);

		//	//m_realPath.insertPointAfterAt(thisVector,m_realPath.getCount()-1);
		//}

		prePoint = point;
		preRealPt = realPoint;

		m_realPath.insertPointAfterAt(point,m_realPath.getCount()-1);
	}

	return &m_realPath;
}

void Conveyor::synFlag()
{
	m_bNeedSyn = false;

	serviceLocationExceedTwoFloorPath();
	
	m_bNeedSyn = true;
}

int Conveyor::clearPerson( const ElapsedTime& _time )
{
	if(m_pPerformer)
		m_pPerformer->Processor::clearPerson(_time);

	return 0;
}

void Conveyor::removePersonFromApproachList( const Person* _pPerson )
{

}
