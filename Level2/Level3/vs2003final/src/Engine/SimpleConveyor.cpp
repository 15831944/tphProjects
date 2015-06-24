// SimpleConveyor.cpp: implementation of the CSimpleConveyor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SimpleConveyor.h"
#include "ConveyorStepItEvent.h"
#include "common\ARCException.h"
#include "person.h"
#include "inputs\miscproc.h"
#include "engine\terminal.h"
#include "Common\path.h"
#include "Main\TermPlanDoc.h"

#include "Common/ARCTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpleConveyor::CSimpleConveyor() : m_dSpeed( 0.0 ), m_iCapacity( 0 ), 
									 m_state( STOP ), m_StepTime( -1l),
									 m_iOccapuiedCount( 0), m_pSlots( NULL )
{
	
}

CSimpleConveyor::~CSimpleConveyor()
{
	clearData();
}



// clear processor data when destructor
void CSimpleConveyor::clearData( void )
{
	if( m_pSlots )
		delete []m_pSlots;
	m_pSlots = NULL;
}


/****************************************************************
void CSimpleConveyor::initSpecificMisc (const MiscData *miscData)
{
	if( miscData == NULL )
	{
		m_dSpeed	= 1.0;
		m_iCapacity = 3;
		return;
	}
	MiscConveyorData *data = (MiscConveyorData *)miscData;
	//m_dSpeed = data->getSpeed();
	//m_iCapacity = data->getCapacity()
}
****************************************************************/

//	init processor data before engine run
void CSimpleConveyor::InitData( const MiscConveyorData * _pMiscData )
{
	// init misc data
	if ( _pMiscData )
	{
		m_dSpeed = _pMiscData->GetSpeed();
		m_iCapacity = _pMiscData->GetQueueCapacity();		
		if( m_iCapacity == -1 )//is auto define capacity
		{
			m_iCapacity = CaculateConveyorCapacity();
		}
	}
	else
	{
		m_dSpeed = 100;
		m_iCapacity = CaculateConveyorCapacity();
	}
	
	//// TRACE ("\n%s\n", name.GetIDString());
	// init logic data
	clearData();

	m_state	 = STOP;
	m_approaching.clear();
	m_iOccapuiedCount = 0;

	slotsPath.init(*serviceLocationExceedTwoFloorPath(), m_iCapacity );
	m_StepTime = (float)(slotsPath.getSegLen() / m_dSpeed);
	
	m_pSlots = new ElemTimePair[ m_iCapacity ];	
	// init slot
	for( int i=0; i< m_iCapacity; i++  )
	{
		m_pSlots[i].first	= NULL;
		m_pSlots[i].second	= -1l; 
	}

	
}

// add person to processor' approaching list
void CSimpleConveyor::addPersonIntoApproachList( Person *_pPerson )
{
	m_approaching.Add( _pPerson );
}

// check if the conveyor is vacant
bool CSimpleConveyor::isVacant( Person* pPerson ) const
{
	// to simple conveyor, only when the last pos is empty and no element is 
	// approaching, it is vacant
	if( m_pSlots )
	{
		if( m_pSlots[0].first == NULL && m_approaching.getCount() == 0)
			return true;
	}

	return false;
}
void CSimpleConveyor::LogDebugInfo()
{
	ofsstream echoFile(_T("C:\\path.log"),stdios::app);
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

void CSimpleConveyor::flushPersonLog(const ElapsedTime& _time)
{
	for( int i=0; i< m_iCapacity; i++  )
	{
		Person* pPerson = m_pSlots[i].first;
		if (pPerson)
		{
			pPerson->flushLog(_time);

			m_pSlots[i].first = NULL;
			m_pSlots[i].second = -1L;
		}
	}

	for (i = 0; i < m_approaching.getCount(); i++)
	{
		Person* pPerson = (Person*)m_approaching.getItem(i);
		pPerson->flushLog(_time);
	}
	m_approaching.clear();
}
// It is called by Person::processServerArrival
// The different processor types serve clients in different ways.
void CSimpleConveyor::beginService (Person *aPerson, ElapsedTime curTime)
{
	if( m_pSlots[0].first != NULL )
	{
		writeLogEvent (aPerson, curTime, BeginService);

		aPerson->kill(curTime);

		int index = m_approaching.findItem (aPerson);
		//assert( index != INT_MAX );
		if (index != INT_MAX)
			m_approaching.removeItem (index);
		
		throw new ARCConveyorSimultaneousServiceError( aPerson->getPersonErrorMsg(), 
		getID()->GetIDString(),_T(""), curTime.printTime() );
	}

	// first write processor's log
	writeLogEvent (aPerson, curTime, BeginService);
	// the write person's log
	std::vector< Point > ptList = slotsPath.getNoFlagPointList( -1, 0 );
	writePersonLog( aPerson, ptList, curTime );
	
	// start up the conveyor if need
	startUpIfNeed( curTime );
	
	// remove from approaching list
	int index = m_approaching.findItem (aPerson);
	if(index != INT_MAX )
		m_approaching.removeItem (index);

	// add the element to the processor
	m_iOccapuiedCount++;
	m_pSlots[0].first = aPerson;
	m_pSlots[0].second = curTime;
}

// start up the conveyor if need
bool CSimpleConveyor::startUpIfNeed( const ElapsedTime& time )
{
	// if no pax on it, need to stat up it
	if( m_iOccapuiedCount == 0 && m_state == STOP )
	{
		CConveyorStepItEvent* pStepIt = new CConveyorStepItEvent( this );
		pStepIt->setTime( time );
		pStepIt->addEvent();		
		setConveyorState( RUN, time ,getLoad(), -1 );
		writeAdditionLog( RUN, time, -1 );
		return true;
	}

	return false;
}
	

// create StepIt event if need
bool CSimpleConveyor::createStepItEventIfNeed( const ElapsedTime& time )
{
	// if m_iOccapuiedCount >0 , need to create new event
	// else need not
	if( m_iOccapuiedCount >0 || m_approaching.getCount()>0 )
	{
		//if (getID()->GetIDString().CompareNoCase("LOADER-T1_CURB-UA_D3_NORTH-1") == 0)
		//{
		//	ofsstream echoFile ("d:\\conveyorUAD3.log", stdios::app);
		//	echoFile << getID()->GetIDString().GetBuffer(2)<< m_iOccapuiedCount <<"  "  
		//		<<  "  " <<time.printTime().GetBuffer( 2 ) <<" "
		//		<< getLoad() <<" " << m_approaching.getCount() <<"     ";

		//	for (int i =0; i < m_approaching.getCount(); i++)
		//	{
		//		echoFile <<"approaching Pax"<<"      ";
		//		echoFile <<m_approaching.getItem(i)->getID()<<"      ";
		//	}
		//	echoFile<<"\n";
		//	echoFile.close();

		//}
		//else if (getID()->GetIDString().CompareNoCase("LOADER-T1_TERMINAL-UA_GBLS+F+BUS-1") == 0)
		//{
		//	ofsstream echoFile ("d:\\conveyor1111.log", stdios::app);
		//	echoFile << getID()->GetIDString().GetBuffer(2)<<  m_iOccapuiedCount <<"  " 
		//		<<  "  " <<time.printTime().GetBuffer( 2 ) <<" "
		//		<< getLoad() <<" " << m_approaching.getCount() <<"     ";
		//	for (int i =0; i < m_approaching.getCount(); i++)
		//	{
		//		echoFile <<"approaching Pax"<<"      ";
		//		echoFile <<m_approaching.getItem(i)->getID()<<"      ";
		//	}
		//	echoFile<<"\n";

		//	echoFile.close();
		//}
		PLACE_TRACK_STRING("2010223-11:16:46");
			CConveyorStepItEvent* pStepIt = new CConveyorStepItEvent( this );
			pStepIt->setTime( time + m_StepTime );
			pStepIt->addEvent();
			setConveyorState( RUN, time ,getLoad(), -1 );
			writeAdditionLog( RUN, time, -1 );
			return true;
	}
	else
	{			
		//setConveyorState( STOP, time );
		PLACE_TRACK_STRING("2010223-11:16:59");
		setConveyorState( STOP, time ,getLoad(), -1 );
		writeAdditionLog( STOP, time, -1 );
		return false;
	}	
}


// processor conveyorStepItEvent
void CSimpleConveyor::processorStepItEvent( InputTerminal* _pInTerm, const ElapsedTime& time )
{
	Person* pReleasePerson = m_pSlots[m_iCapacity-1].first;

	if( pReleasePerson == NULL  )					// if the head pos is empty
	{
		if( createStepItEventIfNeed( time ) )
		{
			stepIt( time );
			noticeSourceConveyor( time );
		}
		return;
	}

	else											// need to release the head element
	{
 		if( IsDestProcExceedCapacity( pReleasePerson, time ) )	// all probability destination all not vacant
 		{
 			// stop the processor, need not to create next event
 			//m_state = STOP;
 			PLACE_TRACK_STRING("2010223-11:17:03");
 			Processor* pDestProc = GetRandomDestProc( pReleasePerson );
 			if( pDestProc )
 			{
 				if( pDestProc->GetStopReason() >=0 )
 				{
 					setConveyorState( STOP, time , getLoad(), pDestProc->GetStopReason() );
 					writeAdditionLog( STOP, time, pDestProc->GetStopReason() );
 				}
 				else
 				{
 					setConveyorState( STOP, time , getLoad(), pDestProc->getIndex() );
 					writeAdditionLog( STOP, time,  pDestProc->getIndex() );
 				}				
 			}
 			else
 			{
 				setConveyorState( STOP, time , getLoad(), -1 );
 				writeAdditionLog( STOP, time,  -1 );
 			}
 			
 			return;
 		}
		else													// release the person
		{
			PLACE_TRACK_STRING("2010223-11:17:08");
			//if dest processor is sorter, check sorter can be used.
			Processor* pDestProc = GetRandomDestProc( pReleasePerson );
			if(pDestProc && pDestProc->getProcessorType()==ConveyorProc && static_cast<Conveyor*>(pDestProc)->GetSubConveyorType()==SORTER)
			{
				Conveyor* pDestSorter=static_cast<Conveyor*>(pDestProc);
				if( pDestSorter->IsSorterEntryVacant( pDestSorter->GetSorterEntryProcs( *getID()))==false )
				{
					if( pDestProc->GetStopReason() >=0 )
					{
						setConveyorState( STOP, time , getLoad(), pDestProc->GetStopReason() );
						writeAdditionLog( STOP, time, pDestProc->GetStopReason() );
					}
					else
					{
						setConveyorState( STOP, time , getLoad(), pDestProc->getIndex() );
						writeAdditionLog( STOP, time,  pDestProc->getIndex() );
					}
					return;
				}
					
			}
			PLACE_TRACK_STRING("2010223-11:17:12");
			if( releaseHeadPerson( pReleasePerson, time ) )
			{
				createStepItEventIfNeed( time );
//			if( createStepItEventIfNeed( time ) )
// 				{
// 					stepIt( time );
// 					noticeSourceConveyor( time );
// 				}
			}
			else
			{
				PLACE_TRACK_STRING("2010223-11:17:17");
				//m_state = STOP;
				Processor* pDestProc = GetRandomDestProc( pReleasePerson );
				if( pDestProc->GetStopReason() >=0 )
				{
					setConveyorState( STOP, time , getLoad(), pDestProc->GetStopReason() );
					writeAdditionLog( STOP, time, pDestProc->GetStopReason() );
				}
				else
				{
					setConveyorState( STOP, time , getLoad(), pDestProc->getIndex() );
					writeAdditionLog( STOP, time,  pDestProc->getIndex() );
				}								
			}
			PLACE_TRACK_STRING("2010223-11:17:21");
		}
	}
}

void CSimpleConveyor::RemoveHeadPerson(const ElapsedTime& time)
{
	if(m_pSlots[ m_iCapacity -1 ].first != NULL)
	{
		m_pSlots[ m_iCapacity -1 ].first = NULL;
		m_pSlots[ m_iCapacity -1 ].second = -1l;

		m_iOccapuiedCount--;
	}

// 	if( createStepItEventIfNeed( time ) )
// 	{
// 	 	stepIt( time );
// 	 	noticeSourceConveyor( time );
// 	}
}

// release the head person, and remove from this processor
bool CSimpleConveyor::releaseHeadPerson( Person* person, ElapsedTime time  )
{
	assert( person );

	std::vector< Point > ptList = slotsPath.getNoFlagPointList( m_iCapacity-1, -1 );
	writePersonLog( person, ptList, time );
	////////////////////////////////////////////////////////////////////////

	// move to next processor
	person->setState( LeaveServer );

	person->move( time,false );	

	return true;
}



// let the conveyor step it
void CSimpleConveyor::stepIt( const ElapsedTime& time )
{
	//// TRACE("simple conveyor: %s\r\n", this->getID()->GetIDString() );

	// let all person on it step it, and write their log
	for( int i = m_iCapacity-1; i>0; i-- )
	{
		Person* person = m_pSlots[i-1].first;
		if( person != NULL )		// 
		{
			
		//	person->writeLogEntry( time, false );
			// let person move along the service location
			std::vector< Point > ptList = slotsPath.getNoFlagPointList( i-1, i );
			ElapsedTime tempTime = time;
			writePersonLog( person, ptList, tempTime );
		}
		// then update slots			
		m_pSlots[i].first	= m_pSlots[i-1].first;
		m_pSlots[i].second	= time + m_StepTime; 
	}
	
	// clear the last pos
	m_pSlots[0].first	= NULL;
	m_pSlots[0].second	= -1l;

	updateApproachingPerson( time );
}

// update approaching person
void CSimpleConveyor::updateApproachingPerson( const ElapsedTime& time )
{
// 	Person* _person = NULL;
// 
// 	Processor* pProc = m_pTerm->procList->getProcessor(getIndex());
// 	if (pProc)
// 	{
// 		Processor* pStayProcs=NULL;
// 		Person* pPerson=NULL;
// 		std::vector<WaitingPair>& vWaitList = pProc->getWaitList();
// 		std::vector<WaitingPair>::iterator iter = vWaitList.begin();
// 		std::vector<MobileElement*> vMobileList;
// 		vMobileList.clear();
// 		
// 		while(iter!=vWaitList.end())
// 		{
// 			pStayProcs=m_pTerm->procList->getProcessor( iter->second);
// 			pPerson = (Person*)pStayProcs->findElement( iter->first);
// 			if(pPerson && (m_approaching.findItem(pPerson) == INT_MAX))
// 			{
// 				vWaitList.erase( vWaitList.begin());
// 				vMobileList.push_back(pPerson);
// 				break;
// 			}
// 			iter++;
// 		}
// 
// 		if(vMobileList.size() > 0)
// 		{
// 			_person = ( Person* )( vMobileList[0]);
// 			if (_person && m_pSlots[0].first == NULL)
// 			{
// 				_person->setState( LeaveServer );
// 				_person->move( time,false );	
// 			}
// 		}
// 	}
}

// write the person's log by processor
void CSimpleConveyor::writePersonLog( Person* _person, std::vector< Point>& ptList, ElapsedTime& time )
{
	assert( _person );
	assert( ptList.size() >1 );

	//double dOldSpeed = _person->getSpeed();
	//_person->setSpeed( m_dSpeed );
	_person->setState( MovingInConveyor );
	
	for( UINT i= 1; i< ptList.size(); i++ )
	{
		_person->setTerminalDestination( ptList[i] );
		Point locationPt;
		_person->getTerminalPoint(locationPt);
		ElapsedTime moveTime = (float) ( locationPt.distance3D( ptList[i] ) / m_dSpeed );
		time += moveTime;

	/*	if (getID()->GetIDString().CompareNoCase("19^FLOWBELT-II-27>38+39>50>+51>60-SORTER>MANUAL>BADTAGS") == 0)
		{
			if (_person->getID() == 66327)
			{
				ofsstream echoFile ("c:\\conveyor.log", stdios::app);
				echoFile <<"Check baggage ID: "<<_person->getID()<<", "<<"Position: "<<ptList[i].getX()<<", "<<ptList[i].getY()<<", " 
					<< ptList[i].getZ() <<", From: "<<m_FloorGroupIndex.nFloorFrom<<"To: "<<m_FloorGroupIndex.nFloorTo<<"Time: "<<time.printTime()<<"\n";
				echoFile.close();
			}

			if (_person->getID() == 66356)
			{
				ofsstream echoFile ("c:\\Nomalconveyor.log", stdios::app);
				echoFile <<"Check baggage ID: "<<_person->getID()<<", "<<"Position: "<<ptList[i].getX()<<", "<<ptList[i].getY()<<", " 
					<< ptList[i].getZ() <<", From: "<<m_FloorGroupIndex.nFloorFrom<<"To: "<<m_FloorGroupIndex.nFloorTo<<"Time: "<<time.printTime()<<"\n";
				echoFile.close();
			}
		}*/
		_person->writeLogEntry( time, false );
	}

	//_person->setSpeed( dOldSpeed );
}

void CSimpleConveyor::writePersonLog( Person* _person, const Path& _path, ElapsedTime& time )
{
	assert( _person );
	//double dOldSpeed = _person->getSpeed();
	//_person->setSpeed( m_dSpeed );
	_person->setState( MovingInConveyor );
	_person->writeLogEntry( time, false );
	for( int i= 1; i< _path.getCount(); i++ )
	{
		_person->setTerminalDestination( _path.getPoint(i) );
		Point locationPt;
		_person->getTerminalPoint(locationPt);
		ElapsedTime moveTime = (float) ( locationPt.distance3D( _path.getPoint(i) ) / m_dSpeed );
		time += moveTime;
		_person->writeLogEntry( time, false );
	}
	
	//_person->setSpeed( dOldSpeed );
}


// if need to wake up the processor
bool CSimpleConveyor::_ifNeedToNotice( void ) const
{
	if( m_state == STOP && m_iOccapuiedCount > 0 )
		return true;
	else
		return false; 
}


// get the release person' time
ElapsedTime CSimpleConveyor::_getReleasePersonTime( void ) const
{
	Person* _person = m_pSlots[m_iCapacity-1].first;

	assert( _person );

	return m_pSlots[m_iCapacity-1].second;
}

// wake up the processor 
bool CSimpleConveyor::destProcAvailable( const ElapsedTime& time )
{
	// // TRACE("simple conveyor: %s\r\n", this->getID()->GetIDString() );
	//if( ifNeedToWakeUp() )
	//{
		CConveyorStepItEvent* pStepIt = new CConveyorStepItEvent( this );
		pStepIt->setTime( time );
		pStepIt->addEvent();
		
		//m_state = RUN;
		setConveyorState( RUN, time ,getLoad(), -1 );
		writeAdditionLog( RUN, time,  -1 );
		return true;
	//}

	//return false;
}

// set conveyor state
void CSimpleConveyor::setConveyorState( CONVEYORSTATE _eState , const ElapsedTime& _time ,long _lLoad, long _lReason )
{	
#ifdef _DEBUG
	static long lReason =-1;
	
	if( _lReason >= 0 )
	{
		if( lReason ==-1 )//first time
		{
			Processor* pProc = m_pTerm->procList->getProcessor( _lReason );
			ofsstream echoFile ("d:\\conveyor.log", stdios::app);
			echoFile 
				<< name.GetIDString().GetBuffer(2 ) << "  " << pProc->getID()->GetIDString().GetBuffer(2) 
				<<  "  " <<_time.printTime().GetBuffer( 2 ) << " "
				<< _lLoad <<" " << _lReason << "\n";

			echoFile.close();			
			
			lReason = _lReason;
		}
		else
		{
			if( lReason != _lReason )
			{
				Processor* pProc = m_pTerm->procList->getProcessor( _lReason );
				ofsstream echoFile ("d:\\conveyor.log", stdios::app);
				echoFile << name.GetIDString().GetBuffer(2 ) <<"  " << pProc->getID()->GetIDString().GetBuffer(2) 
					<<  "  " <<_time.printTime().GetBuffer( 2 ) <<" "
					<< _lLoad <<" " << _lReason <<"\n";
					echoFile.close();

			}
			
			lReason = _lReason;
		}
	}	
#endif
	
	m_state = _eState;
	m_lReason = _lReason;
	switch( _eState )
	{
	case RUN:		
		writeLogEvent( NULL, _time, ProcessorStateRun ,_lLoad );
		break;
		
	case STOP:
		writeLogEvent( NULL, _time, ProcessorStateStop ,_lLoad);
		break;

	default:
		break;
	}	
}

long CSimpleConveyor::getLoad()const
{
	long lLoad = 0;
	for( int i = 0; i < m_iCapacity; ++i )
	{		
		if( m_pSlots[i].first )		// 
		{
			++lLoad;
		}		
	}

	return lLoad + m_approaching.getCount();
}

void CSimpleConveyor::writeAdditionLog( CONVEYORSTATE _eState, const ElapsedTime& _time, long _lReason )
{
	EntityEvents logState;
	if( _eState == RUN )
	{
		logState = ProcessorStateRun;
	}
	else
	{
		logState = ProcessorStateStop;
	}
	for( int i = 0; i < m_iCapacity; ++i )
	{
		Person* pPerson = m_pSlots[i].first;
		if( pPerson )		// 
		{
			TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
			if (spTerminalBehavior)
			{
				spTerminalBehavior->SetStopReason( _lReason );
			}
		
			pPerson->writeLogEntry( _time, false );	
		}		
	}	
}

void CSimpleConveyor::removePersonFromOccupiedList(const Person *_pPerson )
{
	Processor::removePerson( _pPerson );

	for( int i = m_iCapacity-1; i>= 0; i-- )
	{
		Person* person = m_pSlots[i].first;
		if( person != NULL  && _pPerson == person)
		{
			m_pSlots[i].first = NULL;
			m_pSlots[i].second = -1l;

			m_iOccapuiedCount--;
			break;
		}	
	}
}

void CSimpleConveyor::removePersonFromApproachList( const Person* _pPerson )
{
	int index = m_approaching.findItem (_pPerson);
	if(index != INT_MAX )
		m_approaching.removeItem (index);
}
