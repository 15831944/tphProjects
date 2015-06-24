#include "stdafx.h"

#include "baggage.h"

#include "proclist.h"
#include "pax.h"
#include "barrier.h"
#include "ExceedMaxTimeCheckEvent.h"
#include "terminal.h"
#include "Procq.h"

#include "common\states.h"
#include "common\pollygon.h"
#include "common\line.h"
#include "inputs\miscproc.h"
#include "inputs\procdist.h"
#include "inputs\schedule.h"
#include "inputs\flight.h"
#include "main\ProcDataPage.h"
#include "main\ProcDataSheet.h"
#include "results\out_term.h"
#include "results\baglog.h"
#include "GroupLeaderInfo.h"
#include "ARCportEngine.h"
#include "TerminalMobElementBehavior.h"
#include "PaxTerminalBehavior.h"


#define DEFAULT_MAX_WAIT_SECOND 7200l


BaggageProcessor::BaggageProcessor () :m_lMaxWaitTime( DEFAULT_MAX_WAIT_SECOND )
{
	m_pHostId = NULL;
	bypassProcessors = NULL; 
	m_dSpeed = 100.0;
	m_iBagFirstEntryPosIdx = -1;
	capacity = 1000;

	//m_nRandomIndexOfServicePoint = -1;
}
//clean pointer bypassProcessors.
BaggageProcessor::~BaggageProcessor ()
{
	m_vOwners.clear();
	m_vBaggages.clear();
	m_listApproachBaggage.clear();
	m_vectBagStillOnReclaim.clear();
	
    if (bypassProcessors)
        delete bypassProcessors;	
}

void BaggageProcessor::initServiceLocation (int pathCount, const Point *pointList)
{
	if (pathCount <= 2)
        throw new StringError ("Baggage Processors must have at least 3 point service locations");
	LineProcessor::initServiceLocation( pathCount, pointList );
}

void BaggageProcessor::DoOffset( DistanceUnit _dx, DistanceUnit _dy )
{
	m_barrier.DoOffset( _dx, _dy );
	m_pathBagMovingOn.DoOffset( _dx, _dy ,0.0);
	LineProcessor::DoOffset( _dx, _dy );
}

void BaggageProcessor::Rotate( DistanceUnit _degree )
{

	m_barrier.Rotate( _degree );
	m_pathBagMovingOn.Rotate( _degree );
	LineProcessor::Rotate( _degree );
}

void BaggageProcessor::Mirror( const Path* _p)
{
	m_barrier.Mirror( _p );
	m_pathBagMovingOn.Mirror( _p );
	LineProcessor::Mirror( _p );
}

//Initializes the attributes according to user definition.
void BaggageProcessor::initSpecificMisc (const MiscData *miscData)
{	
	if( miscData == NULL )
	{
		if (bypassProcessors)
			delete bypassProcessors;		
		bypassProcessors = NULL; 
		return;
	}
	
    MiscBaggageData *data = (MiscBaggageData *)miscData;
    capacity = data->getCapacity();


	// bypass
    ProcessorIDList *bypassList = data->getBypassList();
    int count = bypassList->getCount();
//

    if (count)
    {
		double *percents = new double[count];
		for (int i = 0; i < count-1; i++)
			percents[i] = i * (100.0 / count);
		percents[count-1] = 100;
     
		bypassProcessors = new ProcessorDistribution;
        ProcessorID *procIDs = new ProcessorID[count];
        for (i = 0; i < count; i++)
            procIDs[i] = *(bypassList->getID (i));

        bypassProcessors->initDestinations (procIDs, percents, count);
        delete [] procIDs;
		delete [] percents;
		percents = NULL;
    }

    
	// greeting hall
	ProcessorIDList* pProcIDList = (ProcessorIDList*)(data->getHoldAreaList());//matt
	//	if((pProcIDList==NULL) && pProcIDList->getCount()>0)
//    // TRACE("BBBBBB bagpro:\t%s\n",getID()->GetIDString());
	m_pHostId = pProcIDList;
}

//init BaggageProcUtil m_bagUtil 
void BaggageProcessor::InitBaggageDeviceData()
{ 
	m_vOwners.clear();
	m_vBaggages.clear();
	m_listApproachBaggage.clear();
	m_vectBagStillOnReclaim.clear();
	m_iBagFirstEntryPosIdx = -1;

	m_bagUtil.Init( &m_pathBagMovingOn , m_dSpeed );	
}

//a person picks up his bags.
//The input parameter curTime is the time of begin service.
void BaggageProcessor::beginService (Person *person, ElapsedTime curTime)
{
//	TRACE("\npersonID: %d, serviceTime: %s\n",person->getID(),  curTime.printTime());

    writeLogEvent (person, curTime, BeginService);
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)person->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior == NULL)
	{
		return;
	}

	int nBreak;
	nBreak = person->getID();
	
	int iPersonType = person->getType().GetTypeIndex();
	spTerminalBehavior->SetEntryBagDeviceTime( curTime );
	Point pt;
	person->getTerminalPoint(pt);
	if( 0 == iPersonType ) // pax 
	{
		writeLogEvent( person, curTime, PaxArriveAtBaggageProc );
		m_vOwners.push_back( person->getID() );

		spTerminalBehavior->SetPosIdxOnBaggageProc ( m_bagUtil.GetNearestPosIdx( pt) );

		std::vector<PersonID> nMyBaggage = GetMyBaggage( (Passenger*)person );
		std::vector<PersonID>::iterator iter = nMyBaggage.begin();
		for (; iter != nMyBaggage.end(); ++iter)
		{
			GenerateBaggageLogsOnMovingPath( person, *iter, true, curTime );
		}
//		if( nMyBaggage >= 0)//baggage come first
//		{
//			GenerateBaggageLogsOnMovingPath( person, nMyBaggage, true, curTime );
//		}
//		else// pax come first
//		{
			// generate a event to check if he will exceed the max waiting time 1 hour later.
//			ExceedMaxTimeCheckEvent* pCheckEvent = new ExceedMaxTimeCheckEvent( this, person->getID() ,person->getPersonErrorMsg());			
//			pCheckEvent->setTime( curTime + DEFAULT_MAX_WAIT_SECOND );
//			pCheckEvent->addEvent();  
//		}
	}
	else if( 2 == iPersonType )//check bag
	{
		//writeLogEvent( person, curTime, BagArriveAtBaggageProc );

		// write log for baggage device
 		{
 			ProcEventStruct event;
 
 			event.time = (long)curTime;
 			event.type = (unsigned char) BagArriveAtBaggageProc;
 			event.element = (person)? person->getID(): -1;
 			event.lLoad = -1;
 			event.lReason = m_lReason;
 			logEntry.addEvent (event);
 
 		}
		
		m_vBaggages.push_back( person->getID() );

		removeBaggageFromApproachList( person );

		ASSERT( m_iBagFirstEntryPosIdx >= 0 );
		spTerminalBehavior->SetPosIdxOnBaggageProc( m_iBagFirstEntryPosIdx );

		Person* pMyOwner = GetMyOwner( person->getID() );
		if( pMyOwner )//pax come first
		{
			GenerateBaggageLogsOnMovingPath( pMyOwner, person->getID(), false, curTime );
		}
		else// baggage come come first
		{
			// generate a event to check if he will exceed the max waiting time 1 hour later.
//			ExceedMaxTimeCheckEvent* pCheckEvent = new ExceedMaxTimeCheckEvent( this, person->getID() ,person->getPersonErrorMsg());			
//			pCheckEvent->setTime( curTime + DEFAULT_MAX_WAIT_SECOND );
//			pCheckEvent->addEvent();  
		}

		int index = -1;
		if(person->getType().isArrival()||person->getType().isTurround())
			index = person->getLogEntry().getArrFlight();
		if(index>=0) 
		{
			Flight *flight = getEngine()->m_simFlightSchedule.getItem(index);
			if(flight->getFirtTime() == 0l) 
				flight->setFirstTime(curTime);
		}
	}
}

/*
 *
 currently, baggage on reclaim logic updated to:
 all baggage is individual element while moving on reclaim.
 so ignore baggage's group size while caclulate occupied baggage count.
*/
bool BaggageProcessor::isVacant( Person* pPerson) const
{
	ASSERT(pPerson != NULL);
	    
	int nMobElementType = pPerson->getType().GetTypeIndex();


	if(nMobElementType != 2)
		return true;
	
	//checked bag

	//for old version project file 
	if(capacity == 0 || capacity == 1000)
		return true;

	// if queue has baggage on it, other items moving path would be emanative;
	if( m_pQueue != NULL && m_pQueue->getGroupCount() > 0)
		return false;

	int nBagStillOnReclaim = 0;
	for( int i=0; i < (int)m_vBaggages.size(); i++ )
	{
		Person* pBaggage = ( Person* ) findElement( m_vBaggages[ i ] );
		if(pBaggage == NULL)
			continue;

//		nBagStillOnReclaim += pBaggage->GetActiveGroupSize();
		nBagStillOnReclaim += 1;
	}

	for (BagStillOnRecliamConsIter iter = m_vectBagStillOnReclaim.begin(); 
		iter != m_vectBagStillOnReclaim.end(); iter++)
	{
		//nBagStillOnReclaim += iter->nBagCount;
		nBagStillOnReclaim += 1;
	}

	int nApproachBag = 0;
	for (size_t i = 0; i < m_listApproachBaggage.size(); i++)
	{
		Person* pBaggage = ( Person* ) findElement( m_listApproachBaggage.at( i ) );
		if(pBaggage == NULL)
			continue;
//		nApproachBag += pBaggage->GetActiveGroupSize();
		nApproachBag += 1;
	}

	int occupancy = nBagStillOnReclaim + nApproachBag + 1/*pPerson->GetActiveGroupSize()*/;

//	TRACE("\ncapacity:%d         Baggage on reclaim: %d, On Approach: %d, current BaggageGroup size: %d", 
//		capacity, (int)nBagStillOnReclaim,  nApproachBag, pPerson->GetActiveGroupSize());

	return occupancy <= capacity;

}

void BaggageProcessor::addBaggageIntoApproachList(const Person* pBaggage)
{
	ASSERT(pBaggage->getType().GetTypeIndex() == 2);

	std::vector<PersonID>::const_iterator iter = 
		std::find(m_listApproachBaggage.begin(), m_listApproachBaggage.end(), pBaggage->getID() );
	if(iter != m_listApproachBaggage.end())
		return;

	m_listApproachBaggage.push_back( pBaggage->getID() );
}

void BaggageProcessor::removeBaggageFromApproachList(const Person* pBaggage)
{
	ASSERT(pBaggage->getType().GetTypeIndex() == 2);

	std::vector< PersonID >::iterator iter  = 
		m_listApproachBaggage.begin();

	iter = std::find( m_listApproachBaggage.begin(), m_listApproachBaggage.end(), pBaggage->getID() ) ;
	if(iter != m_listApproachBaggage.end())
		m_listApproachBaggage.erase( iter );
}

class BaggageLeave
{
public:
	BaggageLeave(const ElapsedTime time):m_time(time)
	{}
	bool operator()(const BagStillOnReclaim& val)const
	{
		return val.time <= m_time;
	}

private:
	const ElapsedTime m_time;
};

void BaggageProcessor::updateBaggageOnReclaimState(const ElapsedTime& curTime)
{

	if(m_vectBagStillOnReclaim.size() <1)
		return;

	BagStillOnRecliamIter iterLast = 
		std::remove_if( m_vectBagStillOnReclaim.begin(), m_vectBagStillOnReclaim.end(), BaggageLeave( curTime ));
	m_vectBagStillOnReclaim.erase( iterLast, m_vectBagStillOnReclaim.end() );
}

void BaggageProcessor::WakeupWaitingBaggageInPrevProc(  const ElapsedTime& time )
{
	Processor* pStayProcs=NULL;
	Person* pPerson=NULL;

	std::vector<WaitingPair>::iterator iter = m_vWaitingPax.begin();
	while(iter != m_vWaitingPax.end())	
	{
		pStayProcs=m_pTerm->procList->getProcessor( iter->second);
		pPerson = (Person*)pStayProcs->findElement( iter->first);
		if(pPerson==NULL)
		{
			iter=m_vWaitingPax.erase( iter );
			continue;
		}
		else if( this->isVacant(pPerson) )
		{
			//add to approach list;
			addBaggageIntoApproachList( pPerson );

			pPerson->generateEvent( time ,false);
			iter=m_vWaitingPax.erase( iter );
			continue;
		}
		iter++;
	}
}

// generate baggage moving logs on baggage device 
void BaggageProcessor::GenerateBaggageLogsOnMovingPath( Person* _pOwner, PersonID _nBaggage , bool _bBaggageComeFirst, const ElapsedTime& _curTime)
{
	Person* pBaggage = (Person*)findElement( _nBaggage );
	

	if(_pOwner==NULL || pBaggage==NULL )return;

	TerminalMobElementBehavior* spBagTerminalBehavior = (TerminalMobElementBehavior*)pBaggage->getBehavior(MobElementBehavior::TerminalBehavior);
	TerminalMobElementBehavior* spOwnerTerminalBehavior = (TerminalMobElementBehavior*)_pOwner->getBehavior(MobElementBehavior::TerminalBehavior);

	Point baggagePt,ownerPurePt;
	pBaggage->getTerminalPoint(baggagePt);
	_pOwner->getTerminalPureLocation(ownerPurePt);

	if (spBagTerminalBehavior == NULL || spOwnerTerminalBehavior == NULL)
		return;
	int iBaggageStandPos = spBagTerminalBehavior->GetPosIdxOnBaggageProc();
	int iOwnerStandPos = spOwnerTerminalBehavior->GetPosIdxOnBaggageProc();

	ElapsedTime eventTime;
	int nPointCount = m_bagUtil.GetPosCount();	
	if( _bBaggageComeFirst )
	{
		ElapsedTime bagEntryTime = spBagTerminalBehavior->GetEntryBagDeviceTime();
		ElapsedTime paxEntryTime =spOwnerTerminalBehavior->GetEntryBagDeviceTime();
		ElapsedTime circleTime = m_bagUtil.GetCircleTime();

		// iCircleCount means the count that baggage has already moved circle arourd on baggage device when its owner came.
		int iCircleCount = (paxEntryTime - bagEntryTime).asSeconds() / circleTime.asSeconds();

		// write the midst point which bag moved on device circlely.
		eventTime = bagEntryTime;
		for( int i=0; i<iCircleCount; ++i )
		{
			int iBagPos = iBaggageStandPos;
			do 
			{
				pBaggage->setTerminalDestination( m_bagUtil.GetNextPos( iBagPos ) );
				eventTime += m_bagUtil.GetItemAt( iBagPos ).GetCostTime();
				((PaxVisitor*)pBaggage)->writeLogEntry( eventTime, false );
				++iBagPos;
				iBagPos = iBagPos % nPointCount;
			} while( iBagPos != iBaggageStandPos );
		}
		//write the midst points in order to let baggage move to its owner on bag moving path
		int iBagPos = iBaggageStandPos;
		while( eventTime < _curTime )
		{
			eventTime += m_bagUtil.GetItemAt( iBagPos ).GetCostTime();
			if( eventTime < _curTime )
			{
				pBaggage->setTerminalDestination( m_bagUtil.GetNextPos( iBagPos ) );			
				((PaxVisitor*)pBaggage)->writeLogEntry( eventTime, false );
				++iBagPos;
				iBagPos = iBagPos % nPointCount;
			}				
		}
		while( iBagPos != iOwnerStandPos )
		{
			pBaggage->setTerminalDestination( m_bagUtil.GetNextPos( iBagPos ) );
			eventTime += m_bagUtil.GetItemAt( iBagPos ).GetCostTime();
			((PaxVisitor*)pBaggage)->writeLogEntry( eventTime, false );
			++iBagPos;
			iBagPos = iBagPos % nPointCount;
		}
		int nBaggageCount = (int)((Passenger*)_pOwner)->GetBags().size();
		Point pDestination = spOwnerTerminalBehavior->getPureLocation();
		float fDir = (nBaggageCount> (MAX_GROUP-1)) ? (float)0.0 : _pOwner->m_pRotation[nBaggageCount+1];
		pDestination.offsetCoords( baggagePt, (double)fDir, GROUP_OFFSET );
		pBaggage->setTerminalDestination( pDestination);
		eventTime += pBaggage->moveTime();
		((PaxVisitor*)pBaggage)->writeLogEntry( eventTime, false );
	}
	else// pax come first , then baggage move to the point which its owner stand .
	{
		// write the midst points in order to let baggage move to its owner on bag moving path
		eventTime = _curTime;
		while( iBaggageStandPos != iOwnerStandPos )
		{
			pBaggage->setTerminalDestination( m_bagUtil.GetNextPos( iBaggageStandPos ) );
			eventTime += m_bagUtil.GetItemAt( iBaggageStandPos ).GetCostTime();
			((PaxVisitor*)pBaggage)->writeLogEntry( eventTime, false );
			++iBaggageStandPos;
			iBaggageStandPos = iBaggageStandPos % nPointCount;
		}
		int nBaggageCount = (int)((Passenger*)_pOwner)->GetBags().size();
		Point pDestination = ownerPurePt;
		float fDir = (nBaggageCount> (MAX_GROUP-1)) ? (float)0.0 : _pOwner->m_pRotation[nBaggageCount+1];
		pDestination.offsetCoords( baggagePt, (double)fDir, GROUP_OFFSET );
		pBaggage->setTerminalDestination( pDestination);
		eventTime += pBaggage->moveTime();
		((PaxVisitor*)pBaggage)->writeLogEntry( eventTime, false );
	}

//	TRACE("\nEnd BagLog Write:   %s, %s\n",_curTime.printTime(), eventTime.printTime() );
	
	((PaxTerminalBehavior*)spOwnerTerminalBehavior)->SetMaxTime(eventTime);
	// clear 
	m_vBaggages.erase( std::find( m_vBaggages.begin(), m_vBaggages.end(), _nBaggage ) );


	// write log for baggage device
 	{
 		ProcEventStruct event;
 		event.time = (long)eventTime;
 		event.type = (unsigned char) BagLeaveBaggageProc;
 		event.element =  pBaggage->getID();
 		event.lLoad = -1;
 		event.lReason = m_lReason;
 		logEntry.addEvent (event);
 	}

//	std::vector<Person*> vBaggages;
	((Passenger*)_pOwner)->GetBags().push_back(pBaggage);
	m_vectBagStillOnReclaim.push_back( BagStillOnReclaim(_nBaggage, eventTime, 1)/*pBaggage->GetActiveGroupSize()) */);
	// owner to pick bag up
	if (_pOwner->getLogEntry().getBagCount()== (int)((Passenger*)_pOwner)->GetBags().size())
	{
		m_vOwners.erase( std::find( m_vOwners.begin(), m_vOwners.end(), _pOwner->getID() ) );
		spOwnerTerminalBehavior->pickupBag( ((Passenger*)_pOwner)->GetBags(), ((PaxTerminalBehavior*)spOwnerTerminalBehavior)->GetMaxTime() );	
		((Passenger*)_pOwner)->GetBags().clear();

//		m_vectBagStillOnReclaim.push_back( BagStillOnReclaim(_nBaggage, ((Passenger*)_pOwner)->GetMaxTime(), pBaggage->GetActiveGroupSize()) );
		WakeupWaitingBaggageInPrevProc(((PaxTerminalBehavior*)spOwnerTerminalBehavior)->GetMaxTime());


		// if user defined dependent processor in behavior,
		// need wake up mobile elements waiting in dependent processors.
		WakeupWaitingPaxInDependentProc( ((PaxTerminalBehavior*)spOwnerTerminalBehavior)->GetMaxTime() );
	}
}

std::vector<PersonID> BaggageProcessor::GetMyBaggage( Passenger* _pOwner )
{
	std::vector<PersonID> vBaggages;
	vBaggages.clear();
	int iSize = m_vBaggages.size();
	for( int i=0; i<iSize; ++i )
	{
		if( _pOwner->IsMyVisitor( m_vBaggages[i] ) )
		{
			vBaggages.push_back(m_vBaggages[i]);
		}
	}

	return vBaggages;
}
Person* BaggageProcessor::GetMyOwner( PersonID _nBaggage )
{
	int iSize = m_vOwners.size();
	for( int i=0; i<iSize; ++i )
	{
		Passenger* pOwner = ( Passenger* ) findElement( m_vOwners[ i ] );
		if( pOwner!=NULL && pOwner->IsMyVisitor( _nBaggage ) )
		{
			return pOwner;
		}
	}
	return NULL;
}
bool BaggageProcessor::IfPersonStillExsit( long _lPersonToCheck )
{
	int iCount = m_vOwners.size();
	for( int i=0; i<iCount; ++i )
	{
		if( m_vOwners[i] == _lPersonToCheck  )
		{
			return true;
		}
	}

	iCount = m_vBaggages.size();
	for( i=0; i<iCount; ++i )
	{
		if( m_vBaggages[i] == _lPersonToCheck  )
		{
			return true;
		}
	}

	return false;
}
// get next index and next point in BaggageProcUtil m_bagUtil according to current index
int BaggageProcessor::GetNextPosOnBaggage( int _iCurrentPosIdx , Point& _ptNextPos ) const 
{
	int iPosCount = m_bagUtil.GetPosCount();
	_ptNextPos = m_bagUtil.GetNextPos( _iCurrentPosIdx );
	_iCurrentPosIdx = (_iCurrentPosIdx + 1) % iPosCount;

	return _iCurrentPosIdx;	
}

//erase person from current index ,and add person in next index
void BaggageProcessor::MovePersonToNextPos( int _iCurrentPosIdx, PersonID _nPerson )
{
	m_bagUtil.ErasePerson( _iCurrentPosIdx, _nPerson );

	int iPosCount = m_bagUtil.GetPosCount();
	_iCurrentPosIdx = (_iCurrentPosIdx + 1) % iPosCount;
	m_bagUtil.AddPerson( _iCurrentPosIdx, _nPerson );
}

// the owner stand on currentpos to pickup its bags
void BaggageProcessor::PickupBags( Passenger* _pOwner ,int _iCurrentPos, BAGS& _vBags )
{
	ASSERT( _iCurrentPos >= 0 );
	m_bagUtil.PickUpBags( _pOwner, _iCurrentPos, _vBags );	
}


Point BaggageProcessor::AcquireServiceLocation (Person * _pPerson )
{
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	Point pt;
	_pPerson->getTerminalPoint(pt);
	if( _pPerson->getType().GetTypeIndex() != 2  )//not check bag
	{
		return LineProcessor::AcquireServiceLocation( _pPerson );
	}
	else
	{
		if( m_iBagFirstEntryPosIdx == -1 )
		{
			m_bagUtil.GetNearestPoint( pt , ((BaggageProcessor*)this)->m_ptBagFirstEntryPos );
			((BaggageProcessor*)this)->m_iBagFirstEntryPosIdx =  m_bagUtil.GetNearestPosIdx( m_ptBagFirstEntryPos );			
		}

		return m_ptBagFirstEntryPos;
	}	
}

Point BaggageProcessor::GetServiceLocation()const
{
	
 	int ndx = getRandomPoint();
 	if( ndx < 0 )
 		ndx = random( m_vOccupiedID.size() );
 	return m_serviceCoords.getPoint (ndx);
}

int BaggageProcessor::intersects (const Point& p1, const Point& p2)
{
	Pollygon temp;
	temp.init( m_location.getCount(), m_location.getPointList() );
	return temp.intersects( p1, p2 );
}

int BaggageProcessor::contains (const Point& pnt)
{
	Pollygon temp;
	temp.init( m_location.getCount(), m_location.getPointList() );
	return temp.contains( pnt );
}
void BaggageProcessor::EraseBagAt( int _iBagPosIdx, PersonID _nBag )
{
	m_bagUtil.ErasePerson( _iBagPosIdx, _nBag );
}

int BaggageProcessor::readSpecialField(ArctermFile& procFile)
{
	int iCount = -1;
	procFile.getInteger( iCount );
	if( iCount > 0 )
	{
		Point* ptList = new Point[ iCount ];
		for( int i=0; i<iCount; ++i )
		{
			procFile.getPoint( ptList[ i ] );
		}

		m_barrier.initServiceLocation( iCount, ptList );
		delete ptList;
	}

	procFile.getInteger( iCount );
	if( iCount > 0 )
	{
		Point* ptList = new Point[ iCount ];
		for( int i=0; i<iCount; ++i )
		{
			procFile.getPoint( ptList[ i ] );
		}

		m_pathBagMovingOn.init( iCount, ptList );
		delete ptList;
	}
	return TRUE;

}
int BaggageProcessor::writeSpecialField(ArctermFile& procFile)const
{
	BaggageProcessor* pBag =  const_cast<BaggageProcessor*>( this );
	int iCount = m_barrier.serviceLocationLength();
	procFile.writeInt( iCount );
	for( int i=0; i<iCount; ++i )
	{
		procFile.writePoint(  pBag->m_barrier.serviceLocationPath()->getPoint( i ) );	
	}

	iCount = m_pathBagMovingOn.getCount();
	procFile.writeInt( iCount );
	for( i=0; i<iCount; ++i )
	{
		procFile.writePoint( pBag->m_pathBagMovingOn.getPoint( i ) );	
	}

	return FALSE;
}

void BaggageProcessor::SetHostIdList(ProcessorIDList* _ProcsID)
{
	m_pHostId = _ProcsID;
}

const ProcessorIDList* BaggageProcessor::GetHostIdList() const
{
	return m_pHostId;
}

// Get Entry Point of the processor,
// might be inconstraint, or service m_location.
//Point BaggageProcessor::GetEntryPoint(int _iCurFloor)
//{
//	if(getServicePoint( 0 ).getZ()/ SCALE_FACTOR !=_iCurFloor) 
//	{
//		throw new ARCFlowError ("not the same Floor Problem!", this->getID()->GetIDString(),"not the same Floor Problem! ","");
//		return getServicePoint( 0 );
//	}
//	//AfxMessageBox("the Same Floor Problem !");
//	if( In_Constr.getCount() )
//		return In_Constr.getPoint( 0 );
//
//	if( m_pQueue != NULL )
//		return	m_pQueue->corner( m_pQueue->cornerCount() - 1 );
//
//	int nServicePointCount = serviceLocationLength();
//	int nRandServicePoint = random(nServicePointCount);
//	return getServicePoint(nRandServicePoint);
//}

bool BaggageProcessor::CopyOtherData(Processor* _pDestProc)
{
	BaggageProcessor* pBP = (BaggageProcessor*)_pDestProc;

	int nBMOCount = GetPathBagMovingOn()->getCount();
	if(nBMOCount> 0)
	{
		Point* pPointBMO = new Point[nBMOCount];
		memcpy(pPointBMO, GetPathBagMovingOn()->getPointList(), sizeof(Point)*nBMOCount);
		pBP->GetPathBagMovingOn()->init(nBMOCount, pPointBMO);
		delete[] pPointBMO;
	}

	int nBSLCount = GetBarrier()->serviceLocationPath()->getCount();
	if(nBSLCount> 0)
	{
		Point* pPointBSL=new Point[nBSLCount];
		memcpy(pPointBSL, GetBarrier()->serviceLocationPath()->getPointList(),sizeof(Point)*nBSLCount);
		pBP->initQueue( FALSE, nBSLCount, pPointBSL);
		pBP->GetBarrier()->initServiceLocation(nBSLCount, pPointBSL);
		delete[] pPointBSL;
	}

	return true;
}

Processor* BaggageProcessor::CreateNewProc()
{
	Processor* pProc = new BaggageProcessor;
	return pProc;
}

//Point BaggageProcessor::GetPipeExitPoint(int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob)
//{
//	Point pt;
//	if (inConstraintLength()>0)
//	{
//		return inConstraint(0);
//	}
//	else
//	{
//		pt=GetServiceLocation();
//		terminalMob->SetNextHoldAreaValue(true,pt);
//		return pt;
//	}
//}














