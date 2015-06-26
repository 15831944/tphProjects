// Sorter.cpp: implementation of the CSorter class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Sorter.h"
#include "person.h"
#include "engine\procq.h"
#include "conveyorstepitevent.h"
#include "inputs\\MiscProc.h"
#include "engine\terminal.h"
#include "TerminalMobElementBehavior.h"
#include "Results\PaxDestDiagnoseInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSorter::CSorter()
{
	m_timeNextStep=ElapsedTime(-1L);
}

CSorter::~CSorter()
{

}

void CSorter::InitData( const MiscConveyorData * _pMiscData )
{

	m_vLinkedExitProcs.clear();

	CSimpleConveyor::InitData(_pMiscData);

	if(_pMiscData!=NULL && _pMiscData->getExitBeltList()!=NULL)
	{
		const ProcessorID* pProcID =NULL;
		Processor* proc=NULL;
		for (int i = 0; i < _pMiscData->getExitBeltList()->getCount (); i++)
		{
			pProcID = _pMiscData->getExitBeltList()->getID( i );
			GroupIndex groupIndex = m_pTerm->procList->getGroupIndex(*pProcID); 

			for (int j = groupIndex.start; j <= groupIndex.end; j++)
			{
				proc=m_pTerm->procList->getProcessor(j);
				m_vLinkedExitProcs.push_back(proc);
			}
		}
	}
}
void CSorter::InitEntryExitSlots()
{
	std::vector<Point> vPoints;
	for(int i=0;i<m_iCapacity;i++)
	{
		vPoints.push_back( slotsPath.getSlotPoint(i) );
	}
	Processor* pProcs=NULL;
	//initialize entry slots
	m_vEntrySlots.clear();
	if(m_vSourceProcs.size()>0)
	{
		int nSlot=0;
		std::vector<Processor*>::iterator iter;
		for(iter=m_vSourceProcs.begin();iter!=m_vSourceProcs.end();iter++)
		{
			pProcs=*iter;
			ASSERT(pProcs!=NULL);
			nSlot=pProcs->getServicePoint( pProcs->serviceLocationLength()-1).getNearest(vPoints);
			m_vEntrySlots.push_back( nSlot );
		}
	}
	
	//initialize exit slots
	m_vExitSlots.clear();
	if(m_vLinkedExitProcs.size())
	{
		int nSlot=0;
		std::vector<Processor*>::iterator iter;
		for(iter=m_vLinkedExitProcs.begin();iter!=m_vLinkedExitProcs.end();iter++)
		{
			pProcs=*iter;
			ASSERT(pProcs!=NULL);
			nSlot=pProcs->getServicePoint( 0 ).getNearest(vPoints);
			m_vExitSlots.push_back( nSlot );
		}
	}
	
	vPoints.clear();
	
	m_vMobileExitSlots.clear();
}

//all slots have pesons or vSlots' size is 0, return false;
//less than one slot have person, return true;
bool CSorter::IfSlotsVacant(const std::vector<int>& vSlots) const
{
	bool bReturn =false;

	std::vector<int>::const_iterator iter;
	for(iter=vSlots.begin();iter!=vSlots.end();iter++)
	{
		ASSERT((*iter) < m_iCapacity);
		if(m_pSlots[(*iter)].first == NULL )
		{
			bReturn=true;
			break;
		}
	}

	return bReturn;
}
bool CSorter::IfAllSlotsOccupy() const
{
	bool bReturn = true;
	for(int i=0; i<m_iCapacity ; i++)
	{
		if(m_pSlots[i].first == NULL )
		{
			bReturn=false;
			break;
		}
	}
	return bReturn;
}

bool CSorter::isVacant( Person* pPerson ) const
{
	// to simple conveyor, only when the last pos is empty and no element is 
	// approaching, it is vacant
	Processor* pProc = m_pTerm->procList->getProcessor(getIndex());
	Conveyor* pNextConveyor=static_cast<Conveyor*>(pProc);
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	int nSorterEntryIndex = pNextConveyor->GetSorterEntryProcs( *spTerminalBehavior->getProcessor()->getID());
	if (nSorterEntryIndex > -1)
	{
		int nEntrySlot=m_vEntrySlots [nSorterEntryIndex];
		if (m_pSlots[ nEntrySlot ].first != NULL )
		{
			return false;
		}
	}
	
	
	if( m_pSlots )
	{
		return IfSlotsVacant(m_vEntrySlots);
	}

	return false;
}

bool CSorter::IsEntryVacant(int _nEntryProcsIndex)
{
	ASSERT( _nEntryProcsIndex >=0 && (UINT)_nEntryProcsIndex < m_vEntrySlots.size());

	int nEntrySlot = m_vEntrySlots[_nEntryProcsIndex];
	return ( m_pSlots[nEntrySlot].first==NULL );
	
}
// let the conveyor step it
void CSorter::stepIt( const ElapsedTime& time )
{
	//preserve the last element in m_pSlots.
	ElemTimePair elemPair;
	elemPair.first=m_pSlots[m_iCapacity-1].first;
	elemPair.second=m_pSlots[m_iCapacity-1].second;
	
	//step all element exclude the last element.
	CSimpleConveyor::stepIt(time);
	
	Person* person = elemPair.first;
	if( person != NULL )		
	{
		person->writeLogEntry( time, false );
		// let person move along the service location
		std::vector< Point > ptListn = slotsPath.getNoFlagPointList( m_iCapacity-1, -1 );
		std::vector< Point > ptList0= slotsPath.getNoFlagPointList( -1, 0);
		ptListn.insert( ptListn.end(), ptList0.begin(), ptList0.end());
		
		ElapsedTime tempTime = time;
		writePersonLog( person, ptListn, tempTime );
	}
	// then update slots			
	m_pSlots[0].first	= elemPair.first;
	m_pSlots[0].second	= time + m_StepTime;

}

// processor conveyorStepItEvent
void CSorter::processorStepItEvent( InputTerminal* _pInTerm, const ElapsedTime& time )
{
	//check entry slot
	//check exit slot
	//IfSlotsVacant(m_vExitSlots) == true
	if(releasePersonInExitSlot(time))
	{
	}

//	if(IfSlotsVacant(m_vExitSlots)==false )
	if( IfAllSlotsOccupy()==true || m_iOccapuiedCount==0 )
	{
		setConveyorState( STOP, time , getLoad(), -1);
		writeAdditionLog( STOP, time, -1);
	}
	else
	{
		if( createStepItEventIfNeed(time) )
		{
			m_timeNextStep = time + m_StepTime;
			stepIt(time);
			noticeEntryConveyor(time + m_StepTime);
		}
	}
}

bool CSorter::releaseHeadPerson( Person* person, ElapsedTime time  )
{
	return true;
}

void CSorter::RemoveHeadPerson(const ElapsedTime& time)
{
	//if (m_vMobileExitSlots.size() > 0)
	//{
	//	m_pSlots[m_vMobileExitSlots[0]].first=NULL;
	//	m_pSlots[m_vMobileExitSlots[0]].second=-1L;

	//	m_vMobileExitSlots.erase(m_vMobileExitSlots.begin());
	//}
	//
	//m_iOccapuiedCount--;

// 	if( createStepItEventIfNeed( time ) )
// 	{
// 		stepIt( time );
// 		noticeSourceConveyor( time );
// 	}
}

bool CSorter::releasePersonInExitSlot(ElapsedTime time)
{
	ASSERT(m_vLinkedExitProcs.size() == m_vExitSlots.size());
	bool bReturn=false;

	int nPos=0;
	std::vector<int>::const_iterator iter;
	for(iter=m_vExitSlots.begin();iter!=m_vExitSlots.end();iter++, nPos++)
	{
		if(m_vLinkedExitProcs[nPos]->isVacant() && m_pSlots[*iter].first!=NULL)
		{
			Person* person=m_pSlots[*iter].first;
			TerminalMobElementBehavior* spTerminalBehavior = person->getTerminalBehavior();
			//// TRACE("Sorter exit procs: %s\n" , m_vLinkedExitProcs[nPos]->getID()->GetIDString());
			//// TRACE("Sorter release pax: %s\n\n" , person->getPersonErrorMsg());



			if(spTerminalBehavior&&spTerminalBehavior->GetSorterEntryExitProcs()== nPos/* && m_vLinkedExitProcs[nPos]->canServe(person->getType(),NULL)*/)

			{
				bReturn=true;
				
 				m_iOccapuiedCount--;
 				assert( m_iOccapuiedCount >=0 );
 				
 				ASSERT(person!=NULL);
				m_pSlots[*iter].first=NULL;
				m_pSlots[*iter].second=-1L;
				//m_vMobileExitSlots.push_back(*iter);
				////////////////////////////////////////////////////////////////////////
				// leave the conveyor
				person->writeLogEntry( time, false ); 
				std::vector< Point >& ptList = slotsPath.getNoFlagPointList( *iter -1, *iter );
				writePersonLog( person, ptList, time );
				////////////////////////////////////////////////////////////////////////
				
				//set exit processor index;
				spTerminalBehavior->SetSorterEntryExitProcs( nPos );

				// move to next processor
				person->setState( LeaveServer );
			//	person->move( time,false );	
				person->generateEvent( time,false);
				
			}
			
		}
	}
	return bReturn;
}

// It is called by Person::processServerArrival
// The different processor types serve clients in different ways.
void CSorter::beginService (Person *aPerson, ElapsedTime curTime)
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	ASSERT( spTerminalBehavior&&spTerminalBehavior->GetSorterEntryExitProcs()>=0 && (UINT)spTerminalBehavior->GetSorterEntryExitProcs()< m_vEntrySlots.size());

	//get entry slot;
	int nEntrySlot=m_vEntrySlots [ spTerminalBehavior->GetSorterEntryExitProcs() ];
	spTerminalBehavior->SetSorterEntryExitProcs( -1 );//clear its value after used.
	
	if( m_pSlots[ nEntrySlot ].first != NULL )
	{
		//// TRACE("Error Person:%s\n",(LPCSTR)aPerson->getPersonErrorMsg());
		//// TRACE("Error Time:%s\n",(LPCSTR)curTime.printTime());

		writeLogEvent (aPerson, curTime, BeginService);
		
		aPerson->flushLog (curTime);
		
		throw new ARCConveyorSimultaneousServiceError( aPerson->getPersonErrorMsg(), 
			getID()->GetIDString(),_T(""), curTime.printTime() );

	}

	//select person's exit belt.
	ProcessorDistribution* pNextFlow=getNextDestinations( aPerson->getType(), spTerminalBehavior->GetInGateDetailMode());
	if(spTerminalBehavior->GetPaxDestDiagnoseInfo())
		spTerminalBehavior->GetPaxDestDiagnoseInfo()->clear();
	const Processor* pExitConveyor=spTerminalBehavior->SelectNextProcessor( pNextFlow, curTime );//do like this way because of percent and roster.
	//const Processor* pExitConveyor=aPerson->TryToSelectNextProcessor(curTime);
	if(pExitConveyor)
	{
		CString strProcessor = pExitConveyor->getID()->GetIDString();
		if (GetExitProcessor(pExitConveyor) == -1)
		{
			int index = m_approaching.findItem (aPerson);
			assert( index != INT_MAX );
			m_approaching.removeItem (index);

			CString szMobType = aPerson->getPersonErrorMsg();
			CString strProcName = spTerminalBehavior->getProcessor() ? spTerminalBehavior->getProcessor()->getID()->GetIDString() : "";
			//				delete pPerson;
			CString strMsg;
			strMsg.Format(_T("The eligible exit conveyor processor %s does not exist in Exit Belt list of Sorter in Behavior setting."), strProcessor);
			ARCDestProcessorUnavailableError *pDestUnavaiableException = new ARCDestProcessorUnavailableError( szMobType, strProcName,"", curTime.printTime());
			if(spTerminalBehavior->GetPaxDestDiagnoseInfo())
				spTerminalBehavior->GetPaxDestDiagnoseInfo()->flushDiagnoseInfo();
			pDestUnavaiableException->setDiagType( PAXDEST_DIAGNOS );
			pDestUnavaiableException->setDiagData( aPerson->getID() );
			
			
			spTerminalBehavior->kill(curTime);
			MobileElementMovementEvent::RemoveAllMobileElementEvent( aPerson );

			throw pDestUnavaiableException;
		}
		spTerminalBehavior->SetSorterEntryExitProcs( GetExitProcessor(pExitConveyor));
	}
	else
	{
		int index = m_approaching.findItem (aPerson);
		assert( index != INT_MAX );
		m_approaching.removeItem (index);

		makeAvailable(aPerson,curTime,false);

		CString szMobType = aPerson->getPersonErrorMsg();
		CString strProcName = spTerminalBehavior->getProcessor() ? spTerminalBehavior->getProcessor()->getID()->GetIDString() : "";

		if(spTerminalBehavior->GetPaxDestDiagnoseInfo())
			spTerminalBehavior->GetPaxDestDiagnoseInfo()->flushDiagnoseInfo();
		
		
		ARCDestProcessorUnavailableError *pDestUnavaiableException = new ARCDestProcessorUnavailableError( szMobType, strProcName,"Cannot find available Exit Belt in Sorter processor.", curTime.printTime());
		pDestUnavaiableException->setDiagType( PAXDEST_DIAGNOS );
		pDestUnavaiableException->setDiagData( aPerson->getID() );
	
		spTerminalBehavior->kill(curTime);
		MobileElementMovementEvent::RemoveAllMobileElementEvent( aPerson );

		throw pDestUnavaiableException;

	}



	// first write processor's log
	writeLogEvent (aPerson, curTime, BeginService);
	// the write person's log
	std::vector< Point > ptList = slotsPath.getNoFlagPointList( nEntrySlot , nEntrySlot+1 );
	writePersonLog( aPerson, ptList, curTime );

	aPerson->setState( MovingInConveyor );
	
	// start up the conveyor if need
	startUpIfNeed( curTime );
	
	// remove from approaching list
	int index = m_approaching.findItem (aPerson);
	assert( index != INT_MAX );
    m_approaching.removeItem (index);
	
	// add the element to the processor
	m_iOccapuiedCount++;
	m_pSlots[nEntrySlot].first = aPerson;
	m_pSlots[nEntrySlot].second = curTime;

}

int CSorter::GetEntryProcessor(const ProcessorID& _sourceProcsID)
{
	ASSERT(m_vSourceProcs.size()==m_vEntrySlots.size());

	int nReturn=-1;//if not find, return -1;
	int nPos=0;
	std::vector<Processor*>::iterator iter;
	for( iter=m_vSourceProcs.begin();iter!=m_vSourceProcs.end();iter++ , nPos++)
	{
		if( _sourceProcsID== *( (*iter)->getID() ) )
		{
			nReturn=nPos;
			break;
		}
	}
	return nReturn;
}

Processor* CSorter::GetExitProcessor(int _nIndex)
{
	ASSERT( _nIndex>=0 && (UINT)_nIndex < m_vLinkedExitProcs.size());
	return  m_vLinkedExitProcs[_nIndex];
}

int CSorter::GetExitProcessor(const Processor* _pExitProcessor) const
{
	int nResult=-1;
	int nPos=0;
	std::vector<Processor*>::const_iterator iter;
	for(iter=m_vLinkedExitProcs.begin(); iter!=m_vLinkedExitProcs.end();iter++, nPos++)
	{
		if((*iter)==_pExitProcessor)
		{
			nResult = nPos;
		}
	}
	return nResult;
}

// update approaching person
void CSorter::updateApproachingPerson( const ElapsedTime& time )
{
/*
	Person* _person = NULL;
	for( int i=0; i< m_approaching.getCount(); i++ )
	{
		_person = ( Person* )( m_approaching.getItem(i));
		assert( _person );
		int nEntryPos = _person->GetSorterEntryExitProcs();
		assert( nEntryPos!= -1 && nEntryPos<m_vEntrySlots.size() );

		int nEntrySlot=m_vEntrySlots[nEntryPos];

		// if the person is waiting empty slot and the destination slot is empty
		if( _person->getState() == WaitForEmptySlot && 
			m_pSlots[  nEntrySlot ].first == NULL )
		{
			_person->setState(MovingInConveyor);
			beginService( _person , time );
		}
	}*/

}

void CSorter::getDestinationLocation( Person* aPerson )
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	switch (aPerson->getState())
    {
	case ArriveAtServer:
		{
			ASSERT( spTerminalBehavior->GetSorterEntryExitProcs()>=0 && (UINT)spTerminalBehavior->GetSorterEntryExitProcs()< m_vEntrySlots.size());
			
			int nEntrySlot=m_vEntrySlots [ spTerminalBehavior->GetSorterEntryExitProcs() ];

			Point servicePoint=slotsPath.getSlotPoint(nEntrySlot);
			
			// set person's destination
			aPerson->setTerminalDestination( servicePoint );
			
			break;
		}
	case MoveAlongInConstraint:
		{
			aPerson->setTerminalDestination (In_Constr.getPoint(0));
			break;
		}
	case MoveToQueue:
		{
			m_pQueue->getTailPoint (aPerson);
			break;
		}
    }
}


// get the release person' time
ElapsedTime CSorter::_getReleasePersonTime( void ) const
{
	ElapsedTime earlyTime( 24*60*60l );//24*60*60l

	ASSERT(m_vLinkedExitProcs.size() == m_vExitSlots.size());
	
	int nPos=0;
	std::vector<int>::const_iterator iter;
	for(iter=m_vExitSlots.begin();iter!=m_vExitSlots.end();iter++, nPos++)
	{
		if(m_vLinkedExitProcs[nPos]->isVacant() && m_pSlots[*iter].first!=NULL)
		{
			Person* person=m_pSlots[*iter].first;
			//// TRACE("get release pax: %s\n" , person->getPersonErrorMsg());
			
		//	if(m_vLinkedExitProcs[nPos]->canServe( person->getType() , NULL) && earlyTime>m_pSlots[*iter].second)
			if(m_vLinkedExitProcs[nPos]->canServe( person->getType() , NULL) )
			{
				earlyTime = m_pSlots[*iter].second;
				break;
			}
			
		}
	}



	return earlyTime;
}


void CSorter::noticeEntryConveyor(const ElapsedTime& time)
{
	int nEntryPos=0;
	Conveyor* pEntryConveyor=NULL;
	std::vector<int>::iterator iter;
	for(iter=m_vEntrySlots.begin();iter!=m_vEntrySlots.end(); iter++ ,nEntryPos++)
	{
		pEntryConveyor=static_cast<Conveyor*>(m_vSourceProcs[nEntryPos]);
		if(pEntryConveyor->ifNeedToNotice() && m_pSlots[*iter].first==NULL)
		{
			pEntryConveyor->destProcAvailable( time );
		}
	}
}

// if need to wake up the processor
bool CSorter::_ifNeedToNotice( void ) const
{
/*
	bool bReturn =false;
	std::vector<int>::const_iterator iter;
	for(iter=m_vEntrySlots.begin();iter!=m_vEntrySlots.end(); iter++ )
	{
		if( m_pSlots[*iter].first==NULL)
		{
			bReturn=true;
		}
	}
	return bReturn;
*/
	return CSimpleConveyor::_ifNeedToNotice();
}

bool CSorter::CanAcceptPerson(const ProcessorID& _entryProcsID ,const ElapsedTime& _time )const
{
	CString strEntryID=_entryProcsID.GetIDString();

	int nEntryProcs=-1;
	int nEntryPos=0;
	std::vector<Processor*>::const_iterator iter;
	for(iter=m_vSourceProcs.begin() ; iter!=m_vSourceProcs.end(); iter++ , nEntryPos++)
	{
		if( *(*iter)->getID()== _entryProcsID)
		{
			nEntryProcs=nEntryPos;
			break;
		}
	}

	if(nEntryProcs<0) return false;// _entryProcsID is not Sorter's Entry;

	if( m_timeNextStep <0L ) return true;

	//calculate acception slot position.
	long nPace=(long)(( m_timeNextStep - _time) / m_StepTime) ;
	if(m_timeNextStep - _time > m_StepTime* nPace)
	{
		nPace++;
	}
	int nPos=( m_vEntrySlots[ nEntryProcs ] - nPace  + m_iCapacity)%m_iCapacity ;

	return (m_pSlots[nPos].first==NULL);


}
