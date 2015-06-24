// Loader.cpp: implementation of the CLoader class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Loader.h"
#include "person.h"
#include "engine\procq.h"
#include "inputs\miscproc.h"
#include "hold.h"
#include "engine\terminal.h"
#include "TerminalMobElementBehavior.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoader::CLoader()
{

}

CLoader::~CLoader()
{

}

void CLoader::InitData( const MiscConveyorData* _pMiscData )
{
	CSimpleConveyor::InitData( _pMiscData );
	
	// init linkage holding area
	link_holding.clear();
	if( _pMiscData )
	{
		MiscConveyorData* _tMiscData = const_cast<MiscConveyorData*>( _pMiscData);
		for( int i=0; i< _tMiscData->getWaitAreaList()->getCount(); i++ )
		{
			ProcessorID* _pholdingID = _tMiscData->getWaitAreaList()->getItem( i );
			MiscProcessorIDWithOne2OneFlag * pOne2OneHoldingID = (MiscProcessorIDWithOne2OneFlag * )_pholdingID;
			if (pOne2OneHoldingID && pOne2OneHoldingID->getOne2OneFlag()) {
				ProcessorArray tempArray;
				GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( *_pholdingID);
				ASSERT( groupIdx.start >=0 );
				for( int j=groupIdx.start; j<=groupIdx.end; ++j ){
					tempArray.addItem( m_pTerm->procList->getProcessor( j ) );
				}
				//get 1:to:1
				Processor* pProcOneToOne = CFlowChannel::GetOutChannelProc( *this, tempArray, _pholdingID->idLength());
				if( pProcOneToOne )	{ 
					groupIdx = m_pTerm->procList->getGroupIndex ( *pProcOneToOne->getID());
					for( int k=groupIdx.start; k<=groupIdx.end; k++ )
						link_holding.push_back( (HoldingArea*)(m_pTerm->procList->getProcessor(k)) );
				}
			}else{
				GroupIndex group = m_pTerm->procList->getGroupIndex( *_pholdingID );
				for( int g=group.start; g<=group.end; g++ )
					link_holding.push_back( (HoldingArea*)(m_pTerm->procList->getProcessor(g)) );
			}
		}
	}

	//if (link_holding.empty())
	//	throw new StringError ("Load processor must have at least 1 holding area");
}

HoldingArea* CLoader::GetRandomHoldingArea(void)
{
	int nCount = (int)link_holding.size();
	if (nCount < 1)
		return (0);
	else
		return (link_holding.at(random(nCount)) );
}

void CLoader::LogDebugInfo()
{
	ofsstream echoFile ("c:\\conveyor.log", stdios::app);
	echoFile <<"Processor Name: "<<getID()->GetIDString()<<" Capacity: "<<m_iCapacity<<"Occupancy: "<<m_iOccapuiedCount
		<<" Approaching: "<<m_approaching.getCount()<<"\n";

	for( int j=0; j< m_approaching.getCount(); j++ )
	{
		Person* _person = (Person*)( m_approaching.getItem(j) ); 

		echoFile <<"Person ID: "<<_person->getID()<<"\n";
	}

	for( int i = 0; i < m_iCapacity; ++i )
	{
		Person* pPerson = m_pSlots[i].first;
		if( pPerson )		// 
		{
			echoFile <<"Occupancy Person ID: "<<pPerson->getID()<<"\n";
		}		
	}	
	echoFile.close();		
}

// get empty wait location
int CLoader::getNearestEmptyWaitPos( const Person *thePerson ) const
{
	std::vector< int > emptyList;
	for( int i=0; i< m_iCapacity; i++ )
		emptyList.push_back(i);

	TerminalMobElementBehavior* spLoaderPersonBehavior = thePerson->getTerminalBehavior();
	

	// then remove occupied slots from approaching list
	for( int j=0; j< m_approaching.getCount(); j++ )
	{
		Person* _person = (Person*)( m_approaching.getItem(j) );  
		TerminalMobElementBehavior* spTerminalBehavior = _person->getTerminalBehavior();
		int _iDestSlot = spTerminalBehavior->GetDestinationSlot();
		
		std::vector< int >::iterator find_iter = 
			std::find( emptyList.begin(), emptyList.end(),_iDestSlot );
		
		//if( find_iter != emptyList.end() )
		assert( find_iter != emptyList.end() );
		if(find_iter != emptyList.end())
			emptyList.erase( find_iter );
	}
	
	if( emptyList.size() == 0)	// no empty slot
	{	
		return -1;		
	}
	else
	{	
		// get nearest pos
		int iNearestIndex = emptyList[0];
		double dNearestLength = -1.0;
		Point pLocation;
		thePerson->getTerminalPoint(pLocation);
		for( UINT k=0; k<emptyList.size(); k++ )
		{
			Point pDestination = slotsPath.getSlotPoint( emptyList[k] );
			double dDistance =  pLocation.distance3D( pDestination );
			if( dNearestLength<0 || dNearestLength> dDistance )
			{
				dNearestLength = dDistance;
				iNearestIndex = emptyList[k];
			}	
		}
		
		return iNearestIndex;
	}
}



// add person into approaching list
void CLoader::addPersonIntoApproachList( Person *_pPerson )
{
	
	int iSeg = getNearestEmptyWaitPos( _pPerson );
	assert( iSeg != -1 );
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	spTerminalBehavior->SetDestinationSlot( iSeg );
	
	CSimpleConveyor::addPersonIntoApproachList( _pPerson );
}

//release head person and wait area
void CLoader::RemoveHeadPerson(const ElapsedTime& time)
{
	CSimpleConveyor::RemoveHeadPerson( time );
	releaseLinkHolding(time);
	WakeupWaitingPaxInDependentProc(time);
}

// check if the conveyor is vacant
bool CLoader::isVacant( Person* pPerson ) const
{
	return true;
}


// It is called by Person::processServerArrival
// The different processor types serve clients in different ways.
void CLoader::beginService (Person *aPerson, ElapsedTime curTime)
{
	// when the person arrival the wait location
	// first it check the location if is empty
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	int _iDestSlot = spTerminalBehavior->GetDestinationSlot();
	assert( _iDestSlot!= -1 );
	// if is empty, put the bag to slot
	if( m_pSlots[_iDestSlot].first == NULL )	
	{
		putBagToSlot( aPerson, curTime );
	}
	// else wait at this location 
	else	
	{
		aPerson->setState( WaitForEmptySlot );
//		aPerson->writeLogEntry( curTime, false );
	}
}


// update approaching person
void CLoader::updateApproachingPerson( const ElapsedTime& time )
{
	Person* _person = NULL;
	for( int i=0; i< m_approaching.getCount(); i++ )
	{
		_person = ( Person* )( m_approaching.getItem(i));
		assert( _person );
		TerminalMobElementBehavior* spTerminalBehavior = _person->getTerminalBehavior();
		//when group pass through the processor,they will be separated,
		//so,if the loader's next processor is END,then the leader death first .it will flush the log,
		//the operation also flush its follower's log,then the follower is death.
		//if the follower is death,remove it from the approaching list.
		if (_person->getState() == Death)
		{
//			_person->setState(WaitForEmptySlot);
			int index = m_approaching.findItem (_person);
			assert( index != INT_MAX );
			m_approaching.removeItem (index);
			spTerminalBehavior->SetDestinationSlot( -1 );
		}
		else
		{
			int _iDestSlot = spTerminalBehavior->GetDestinationSlot();
// 			if (_person->getID() == 50849)
// 			{
// 				ofsstream echoFile ("c:\\conveyor.log", stdios::app);
// 				if( m_pSlots[_iDestSlot].first == NULL )	
// 				{
// 					echoFile <<"Dest Slot: " << _iDestSlot <<" not Wait for empty slot"<< "\n";
// 				}
// 				else	
// 				{
// 					echoFile <<"Dest Slot: " << _iDestSlot <<" Wait for empty slot"<< "\n";
// 				}
// 
// 				echoFile.close();			
// 			}
			//assert( _iDestSlot!= -1 );
			// if the person is waiting empty slot and the destination slot is empty
			if( _person->getState() == WaitForEmptySlot && _iDestSlot>=0 &&
				m_pSlots[ _iDestSlot].first == NULL )
			{ 
				putBagToSlot( _person, time );
			}
		}
	}
}

void CLoader::getDestinationLocation( Person* aPerson )
{
	switch (aPerson->getState())
    {
	case MoveAlongInConstraint:
		{
			aPerson->setTerminalDestination(In_Constr.getPoint(0));
			break;
		}
	case MoveToQueue:
		{
			m_pQueue->getTailPoint (aPerson);
			break;
		}
	case ArriveAtServer:
		{
			setPersonWaitLocationNearByLoader( aPerson );
			break;
		}
    }
}

void CLoader::setPersonWaitLocationNearByLoader( Person* _person )
{
	// get dest slot location position
	TerminalMobElementBehavior* spTerminalBehavior = _person->getTerminalBehavior();
	int _iDestSlot = spTerminalBehavior->GetDestinationSlot();
	Point servicePoint;
	if( _iDestSlot!= -1 )
	{
		servicePoint = slotsPath.getSlotPoint(_iDestSlot);
	}
	else
		servicePoint = GetServiceLocation();
	
	// offset the position
	Point pVector;
	Point pLocation;
	_person->getTerminalPoint(pLocation);
	pVector.initVector3D( servicePoint, pLocation);
	pVector.length3D( GetWidth() );
	servicePoint.plus( pVector );
	
	// set person's destination
	_person->setTerminalDestination( servicePoint );
}

//called by updateApproachingPerson()
// put a person from wait location to it's destination slot
void CLoader::putBagToSlot( Person *aPerson, ElapsedTime curTime)
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	int _iDestSlot = spTerminalBehavior->GetDestinationSlot();
	assert( _iDestSlot!= -1 );
	if( m_pSlots[ _iDestSlot ].first != NULL )
	{
		writeLogEvent (aPerson, curTime, BeginService);
	
		aPerson->kill(curTime);

		throw new ARCConveyorSimultaneousServiceError( aPerson->getPersonErrorMsg(), 
			getID()->GetIDString(),_T(""), curTime.printTime() );
	}
	
	startUpIfNeed( curTime+1l );
	
	// remove from approaching list
	int index = m_approaching.findItem (aPerson);
	assert( index != INT_MAX );
	m_approaching.removeItem (index);
	spTerminalBehavior->SetDestinationSlot( -1 );
	
	// add the element to the processor
	m_iOccapuiedCount++;
	m_pSlots[_iDestSlot].first = aPerson;
	m_pSlots[_iDestSlot].second = curTime+1l;
	
	// write the element's log
	
	aPerson->setTerminalDestination( slotsPath.getSlotPoint( _iDestSlot ) );
 	
	aPerson->setState( MovingInConveyor );
	aPerson->writeLogEntry( curTime+1l, false );


	writeLogEvent (aPerson, curTime+1l, BeginService);	
}

// let the conveyor step it
void CLoader::stepIt( const ElapsedTime& time )
{
	CSimpleConveyor::stepIt( time );

	// release like hold area if need
	releaseLinkHolding( time );
	WakeupWaitingPaxInDependentProc(time);
}

//if the slot is not full,release the person from the linked holdarea
//and push to slot
// release linkage holding area
bool CLoader::releaseLinkHolding( const ElapsedTime& time )
{
	// if no empty slot, then need not to release
	if( isExceedConveyorCapacity() )	
		return false;
	
	int link_size = link_holding.size();
	if( link_size == 0 )
		return false;

	int radan_indx = random( link_size );
	for( int i=0; i< link_size; i++)
	{
		HoldingArea* pHolding = link_holding[radan_indx];
		assert( pHolding );

		if( pHolding->TryToReleaseBagToLoader( time, *getID() ) )
			return true;

		radan_indx = (radan_indx + 1)% link_size;
	}

	return false;
}

bool CLoader::isExceedConveyorCapacity( void ) const
{
	int _iEmptySlotCount = m_iCapacity - m_iOccapuiedCount - m_approaching.getCount();
	assert( _iEmptySlotCount>=0 );
	
	return _iEmptySlotCount== 0;
}
//get the empty slot count
int CLoader::GetEmptySlotCount() const
{
	int _iEmptySlotCount = m_iCapacity - m_iOccapuiedCount - m_approaching.getCount();
	return _iEmptySlotCount;
}
long CLoader::getLoad()const
{
	long lLoad =0;
	int iHoldAreaCount = link_holding.size();
	for( int i=0; i<iHoldAreaCount ; ++i )
	{
		HoldingArea* pHold = link_holding[ i ];
		lLoad += pHold->GetOccupants()->getCount();
	}
	
	return lLoad + CSimpleConveyor::getLoad();
}