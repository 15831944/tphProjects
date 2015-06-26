#include "stdafx.h"
#include "inputs\in_term.h"
#include "engine\hold.h"
#include "engine\person.h"
#include "engine\process.h"
#include "engine\proclist.h"
#include "common\states.h"
#include "common\path.h"
#include "common\pollygon.h"
#include "inputs\miscproc.h"
#include "common\termfile.h"
#include "common\exeption.h"
//#include "common\ARCImportantException.h"
#include "engine\terminal.h"
#include "Airside\AirTrafficController.h"
#include "Airside\AirsideFlightInSim.h"
#include "Airside\AirsidePaxBusInSim.h"
#include "BoardingCallPolicy.h"
#include "Airside/AirsideSimulation.h"
#include "Airside/FlightServiceReqirementInSim.h"
#include <iostream>
#include "Engine/ARCportEngine.h"
#include "TerminalMobElementBehavior.h"
#include "Procq.h"
#include "Accessq.h"
#include "Inputs\Prochrch.h"
#include "CorralServiceEvent.h"

using namespace std;
HoldingArea::HoldingArea()
{
	radius = 100l; 
	m_iStageID = -1; 
	m_bApplyServiceTimeAfterGreeting = true;
	m_bCorralService	 = false;
	m_eTimeToService	 = ElapsedTime(60.0);
	//m_bCannotHoldNextPax = false;
	m_pCorralServiceEvent = NULL;
}
//Initializes service loacation which is defined as pollygon. 
//It throws an error message if the service m_dArea has no space.
void HoldingArea::initServiceLocation (int _pathCount, const Point* _pointList)
{
    if (_pathCount < 3)
        throw new StringError ("Holding Areas must have at least 3 point service locations");

    m_location.init (_pathCount, _pointList);

    Pollygon serviceArea;
    serviceArea.init (m_location.getCount(), m_location.getPointList());
    m_dArea = serviceArea.calculateArea();
    if (m_dArea == 0.0)
    {
        char str[80];
        strcpy (str, "Holding m_dArea ");
        name.printID (str + strlen (str));
        strcat (str, " has no space");
        throw new StringError (str);
    }

	UpdateMinMax();
}

//The pathCount shoud be zero. 
void HoldingArea::initQueue (int nFixed, int pathCount, const Point *pPointList)
{
//    if (pathCount)
 //       throw new StringError ("Holding Area Processors may not have queues");
	if(IsCorralService())
		Processor::initQueue(nFixed, pathCount, pPointList);
}
 
//Returns a random point as service m_location.   
Point HoldingArea::GetServiceLocation () const
{
    Pollygon serviceArea;
    serviceArea.init (m_location.getCount(), m_location.getPointList());
    return serviceArea.getRandPoint();
}

//For each pax group, set source holdarea and destination holdarea.
void HoldingArea::initSpecificMisc (const MiscData *miscData)
{
	if( miscData == NULL )
	{
		radius = 100l;
		return;
	}

    MiscHoldAreaData *data = (MiscHoldAreaData *)miscData;
    radius = (DistanceUnit) (data->getRadius() * 100.0);
	m_iStageID = data->GetStageNumber();
	
	m_bApplyServiceTimeAfterGreeting = data->getApplyServiceTimeAfterGreeting();
	m_bCorralService = data->IsCorralService();
	m_eTimeToService = ElapsedTime( data->getTimeToService()*1.0);

}   

// Clean all sim engine related data, for the new sim process
void HoldingArea::CleanData()
{
	Processor::CleanData();
	m_waitForLoaderList.clear();
	//m_vPersonWaitService.clear();
	//m_bServiceAfterFull = false;
}
//Returns number of passeners waiting in holdared with given "type".
int HoldingArea::getPaxOfType (const CMobileElemConstraint& aConst) const
{
    int count = 0;
    Person *aPax;
    CMobileElemConstraint type(aConst.GetInputTerminal());
    int occupantCount = occupants.getCount();

    for (int i = 0; i < occupantCount; i++)
    {
        aPax = (Person *)occupants.getItem(i);
        type = aPax->getType();
        if (!type.isArrival() && !type.isVisitor())
            if (aConst.fits (type))
                if (aPax->getState() == WaitInHoldingArea)
                    count++;
    }
    return count;
}

//It is called by person::selectBestHoldArea() to detemine which hold m_dArea is the best option.
double HoldingArea::getDensity (void)
{
    return (occupants.getCount())? (double) occupants.getCount() / m_dArea : 0.0;
}

//Releases passengers from holdares.
void HoldingArea::releasePax (const CMultiMobConstraint *multiConst, float proportion, ElapsedTime releaseTime)
{
    for (int i = 0; i < multiConst->getCount(); i++)
        releasePax (*multiConst->getConstraint(i),
            (occupants.getCount() * proportion), releaseTime);
}

//Releases a type of passenger from holdarea.
void HoldingArea::releasePax (const CMobileElemConstraint& aConst, float proportion, ElapsedTime releaseTime)
{
    if (proportion == 1.0 || getServiceTime (aConst,releaseTime))
        releasePax (aConst, releaseTime);
    else
        releasePax (aConst, (int)(getPaxOfType (aConst) * proportion), releaseTime);
}
int HoldingArea::releasePaxToAirside(const CMobileElemConstraint& aConst ,int cout ,ElapsedTime time)
{
	int remainpax = getPaxOfType(aConst) ;
	int releasecout = remainpax < cout ? remainpax : cout ;
	int allcout = occupants.getCount() ;
	Passenger* aPerson = NULL ; 
	CAirsidePaxBusInSim* _bus = NULL;
	
	int nReleaseCount = 0;
	for (int nCurPerson = 0; nCurPerson < allcout && nReleaseCount < releasecout; ++ nCurPerson)
	{
		aPerson = (Passenger *)occupants.getItem (nCurPerson);

		if(((aPerson->getType()).isArrival() ||
			(aPerson->getType()).GetTypeIndex() > 0  ||
			!aConst.fits (aPerson->getType()) ||
			aPerson->getState() != WaitInHoldingArea))
			continue;

		_bus = GetCanServerBus(aPerson) ;
		if(_bus == NULL)
			break ;
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
		if (spTerminalBehavior)
		{
			if (!spTerminalBehavior->HasBusForThisFlight(aPerson->GetCurFlightIndex(),this))
				break;

			spTerminalBehavior->SetAirsideBus(_bus) ;
			_bus->AddPassenger(aPerson) ;
			if( m_iStageID > 0 )
			{
				spTerminalBehavior->release( m_iStageID );
			}
			else
			{
				spTerminalBehavior->release();
			} 
			spTerminalBehavior->HasBusServer(TRUE);
			spTerminalBehavior->leaveHoldArea (time);
		}


		nReleaseCount += 1;

	}


	/*for (int i = 0; i < releasecout ; i++)
	{

		do 
		{
			aPerson = (Passenger *)occupants.getItem (random (allcout));
			strLog.Format(_T("Pax Address, %d, state, %d, arrival, %d, typeindex, %d, fit,%d"),
				aPerson->getID(), aPerson->getState(), (int)(aPerson->getType()).isArrival(), (aPerson->getType()).GetTypeIndex(), (int)!aConst.fits (aPerson->getType()) );
			TmpLog3(time, strLog);

		}
		while ((aPerson->getType()).isArrival() ||
			(aPerson->getType()).GetTypeIndex() > 0  ||
			!aConst.fits (aPerson->getType()) ||
			aPerson->getState() != WaitInHoldingArea);


		_bus = GetCanServerBus(aPerson) ;
		if(_bus == NULL)
			break ;
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
		if (spTerminalBehavior)
		{
			if (!spTerminalBehavior->HasBusForThisFlight(aPerson->GetCurFlightIndex(),this))
				break;

			spTerminalBehavior->SetAirsideBus(_bus) ;
			_bus->AddPassenger(aPerson) ;
			if( m_iStageID > 0 )
			{
				spTerminalBehavior->release( m_iStageID );
			}
			else
			{
				spTerminalBehavior->release();
			} 
			spTerminalBehavior->HasBusServer(TRUE);
			spTerminalBehavior->leaveHoldArea (time);
		}
      */



	return nReleaseCount ;
}
//Release a certain type passenger who is not an arrival or waiting person.
void HoldingArea::releasePax (const CMobileElemConstraint& aConst, int count, ElapsedTime releaseTime)
{
    if (count > occupants.getCount())
        throw new StringError ("HoldingArea cannot release more pax than it holds");

    int occupantCount;
    Person *aPerson;
    for (int i = 0; i < count; i++)
    {
        occupantCount = occupants.getCount();
        do {
            aPerson = (Person *)occupants.getItem (random (occupantCount));
        } while ((aPerson->getType()).isArrival() ||
                 (aPerson->getType()).isVisitor() ||
                !aConst.fits (aPerson->getType()) ||
            aPerson->getState() != WaitInHoldingArea);

		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();

		if (spTerminalBehavior)
		{
			if( m_iStageID > 0 )
			{
				spTerminalBehavior->release( m_iStageID );
			}
			else
			{
				spTerminalBehavior->release();
			}
			
			if (m_iStageID == 1 && spTerminalBehavior->HasBusForThisFlight(aPerson->GetCurFlightIndex(),this))
			{
				CAirsidePaxBusInSim* pArrivalBus = GetCanServerBus(aPerson) ;
				if(pArrivalBus == NULL)
					continue;
				pArrivalBus->AddPassenger(aPerson);
				spTerminalBehavior->SetAirsideBus(pArrivalBus);
				spTerminalBehavior->HasBusServer(TRUE);
			}
			spTerminalBehavior->leaveHoldArea (releaseTime);
		}
		
    }
}   

//Releases a certain pax who is not arrival and waiting in holdarea.
void HoldingArea::releasePax (const CMobileElemConstraint& aConst, ElapsedTime p_time)
{   
    Person *aPerson;
    for (int i = occupants.getCount() - 1; i >= 0; i--)
    {
        aPerson = (Person *)occupants.getItem (i);
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
        if (spTerminalBehavior&&aConst.fits (aPerson->getType()) && !(aPerson->getType()).isArrival() && aPerson->getState() ==  WaitInHoldingArea)
        {
            if( m_iStageID > 0 )
			{
				spTerminalBehavior->release( m_iStageID );
			}
			else
			{
				spTerminalBehavior->release();
			}

			if (m_iStageID == 1 && spTerminalBehavior->HasBusForThisFlight(aPerson->GetCurFlightIndex(),this))
			{
				CAirsidePaxBusInSim* pArrivalBus = GetCanServerBus(aPerson) ;
				if(pArrivalBus == NULL)
					continue;
				pArrivalBus->AddPassenger(aPerson);
				spTerminalBehavior->SetAirsideBus(pArrivalBus);
				spTerminalBehavior->HasBusServer(TRUE);
			}
            spTerminalBehavior->leaveHoldArea (p_time);
        }
    }
}

//It detemines service time based on the input person's type.
void HoldingArea::beginService (Person *person, ElapsedTime curTime)
{
///	// TRACE ("\n%s\n", name.GetIDString());
	//if this holding area is the wait area of the person, then the person should wait here for some proc opening
	TerminalMobElementBehavior* spTerminalBehavior = person->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if( spTerminalBehavior->IfWaitInLinkedArea() )
	{
		return;
	}
	if(IsCorralService())
	{
		CorralService(person, curTime);
		return;
	}


	if( PersonShouldWaitOnStage( person ,curTime) )
	{
		person->setState( WaitInHoldingArea );
		return;
	}
	// determine service time, based on the Person's type
	person->regetAttatchedNopax();
    ElapsedTime serviceTime;
    serviceTime = getServiceTime (person,curTime);

	/*
	if( m_iStageID <= 0 && serviceTime <= 0l )
	{
		throw new ARCImportantException( Engine_Error_HoldingArea_No_StageInfo, " HoldingArea processor name : "+ name.GetIDString() );
	}
*/
    if (spTerminalBehavior->isStranded())
    {
        person->setState (WaitInHoldingArea);
        spTerminalBehavior->leaveHoldArea (curTime);
        return;
    }


    // may be scaled by some value in the future
    //if (person->getGroupSize() > 1)
    //    serviceTime *= (float)(0.9 * (float)(person->getGroupSize()));

    person->generateEvent (curTime+serviceTime,false);
}

//It wipes occupants from holdarea according to time.
int HoldingArea::wipe (ElapsedTime currentTime)
{
    Person *aPerson;
    int stranded = occupants.getCount();

    while (occupants.getCount() != 0)
    {
        aPerson = (Person *)occupants.getItem(0);
        occupants.removeItem (0);

#ifdef DEBUG
	    char str[128];
		name.printID (str);
		//char str1[128];
		CString str1;
		aPerson->getType().screenPrint( str1,0,128 ); //MATT
	    stdcerr << "\nStranded: " << str1.GetBuffer(0) << " at " << str;
#endif //DEBUG

        aPerson->flushLog (currentTime);
//        delete aPerson;
    }
    return stranded;
}

//set holdarea available.
void HoldingArea::makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay)
{
    available = TRUE;
    writeLogEvent (aPerson, availTime, CompleteService);
	

	//// accept the next waiting Person, if any
	//if (m_pQueue && IsServiceAfterFull())
	//{
	//	if (m_pQueue->hasWait())
	//	{
	//	
	//		//check the capacity
	//		Person *pHeadPerson = GetHeadPersonInQueue();
	//		if(GetPersonWaitServiceGroupSize() + pHeadPerson->GetActiveGroupSize() <= m_nTimeToService)
	//		{
	//			AddPersonToWaitServiceList(pHeadPerson);

	//			TRACE("\r\n PaxID: %d,	GroupSize: %d, HoldSize: %d \r\n",pHeadPerson->getID(),pHeadPerson->GetActiveGroupSize(),
	//				GetPersonWaitServiceGroupSize());

	//			if( _bNoDelay )
	//				m_pQueue->releaseNext ( availTime );
	//			else
	//				m_pQueue->releaseNext ( availTime + ElapsedTime(DELAY));

	//			if (m_pQueue->getGroupCount() == 1)
	//				balanceQueues (availTime);
	//		}
	//	}
	//	else
	//	{
	//		balanceQueues (availTime);
	//		if( isClosing && m_pQueue->getTotalQueueLength() <= 0 )
	//		{
	//			if (isClosing)
	//			{
	//				isClosing = FALSE;
	//				isOpen = FALSE;
	//				writeLogEvent (NULL, availTime, CloseForService);
	//			}
	//		}
	//	}		
	//}
	//else if (isClosing) // close processor if isClosing == TRUE && m_pQueue empty
	//{
	//	isClosing = FALSE;
	//	isOpen = FALSE;
	//	writeLogEvent (NULL, availTime, CloseForService);
	//}


	WakeupWaitingPaxInPrevProc( availTime );	
	WakeupWaitingPaxInDependentProc(availTime);

	noticeSourceConveyor( availTime );
}
bool HoldingArea::PersonShouldWaitOnStage( Person *_pPerson  , const ElapsedTime& _currentTime )const
{
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	if( _pPerson->getType().isDeparture() || _pPerson->getType().isTurround() )
	{

		if( m_iStageID > 0 )
		{

			if(_pPerson->getEngine()->IsAirsideSel() && m_iStageID == 1 && spTerminalBehavior->HasBusForThisFlight(_pPerson->GetCurFlightIndex(),const_cast<HoldingArea*>(this)))
			{
				if(const_cast<HoldingArea*>(this)->GetCanServerBus(_pPerson) != NULL )
				{
					spTerminalBehavior->release();
					return FALSE ;
				}
				else
					return TRUE;
			}
			if( spTerminalBehavior->PersonShouldWaitOnThisStage( m_iStageID,_currentTime ) )
			{
				return true;
			}
			else
			{
				//_pPerson->release();
				return false;
			}
		}
		else
		{
			//_pPerson->release();
			return false;	
		}
	}
	else
	{
		//_pPerson->release();
		return false;
	}
	
	
}

//Doesn't use any more
BOOL HoldingArea::HasBusServer(int FlightID)
{
	AirsideFlightInSim* Airflight = getEngine()->GetAirsideSimulation()->GetAirsideFlight(FlightID) ;
	//ASSERT(Airflight) ;
	if(Airflight == NULL)
		return FALSE ;
	//return	Airflight->hasPaxBusService();
	return TRUE;
}
// release person who wait in area because of the roster reason in Person::SelectProc
void HoldingArea::releaseWaitInAreaPax( const ElapsedTime& releaseTime, Processor* _pBackToProc )
{
    Person *aPerson;
    for (int i = occupants.getCount() - 1; i >= 0; i--)
    {
        aPerson = (Person *)occupants.getItem (i);
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
		if( spTerminalBehavior&&_pBackToProc->canServe( aPerson->getType() ) && spTerminalBehavior->IfWaitInLinkedArea() && _pBackToProc == spTerminalBehavior->GetBackToProc() )
		{
			// set the proc which the person will lead to when he leave this hold area
			//aPerson->SetBackToProc( _pBackToProc );
			spTerminalBehavior->release();		
            spTerminalBehavior->leaveHoldArea ( releaseTime );
		}
    }
}

// let the bag wait in holding till the destination loader is ok
void HoldingArea::storeBagtoHolding( Person* _person, const ProcessorID& _destProcID )
{
	assert( _person );

	m_waitForLoaderList.push_back( std::make_pair( _person, _destProcID) );
	TerminalMobElementBehavior* spTerminalBehavior = _person->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->SetWaitInLinkedAreaFlag( true );
	}
	
}


// release bag to loader
bool HoldingArea::TryToReleaseBagToLoader( const ElapsedTime& releaseTime, const ProcessorID& _backToProcID )
{
	//// TRACE(" back to processor id:%s\r\n", _backToProcID.GetIDString() );
	std::list< PersonDestidPair >::iterator iter;
	for( iter = m_waitForLoaderList.begin(); iter != m_waitForLoaderList.end(); ++iter )
	{
		Person* _person = iter->first;
		//// TRACE(" want to processor id: %s\r\n", iter->second.GetIDString() );
		TerminalMobElementBehavior* spTerminalBehavior = _person->getTerminalBehavior();
		if(spTerminalBehavior && iter->second == _backToProcID )	
		{
			// release the person
			GroupIndex group = m_pTerm->procList->getGroupIndex( _backToProcID );
			assert( group.start == group.end );

			Processor* pDestProc = m_pTerm->procList->getProcessor( group.start );
			assert( pDestProc );
			
			// release from this holding area
			spTerminalBehavior->SetWaitInLinkedAreaFlag( false );
			removePerson( _person );
			_person->writeLogEntry( releaseTime, false );

			// move to next proc
			spTerminalBehavior->setProcessor( pDestProc );
			_person->setState( ArriveAtServer );
			pDestProc->addPerson( _person );
			pDestProc->getNextLocation( _person );
			_person->generateEvent( releaseTime + _person->moveTime(),false);
			//set person's next proc to NULL
			_person->m_pGroupInfo->SetNextProc(NULL);

			// remove from m_waitForLoaderList
			m_waitForLoaderList.erase(iter);

			return true;
		}
	}

	return false;
}

Processor* HoldingArea::CreateNewProc()
{
	Processor* pProc = new HoldingArea;
	return pProc;
}

BOOL HoldingArea::IsCanTakeOnFlight(Passenger* _pax) 
{
 	int _busCount = (int)m_serverBus.size() ;
 	AirsideFlightInSim* _flightSim =  getEngine()->GetAirsideSimulation()->GetAirsideFlight(_pax->GetCurFlightIndex()) ;
 	if(_flightSim == NULL)
 		return TRUE ;
 	AirTrafficController* controller = _flightSim->GetAirTrafficController() ;
 //	CBoardingCallPolicy* _policy = controller->GetBoardingCall() ;
// 	Flight* _fli = _flightSim->GetFlightInput() ;
//  	if(!_policy->FlightIsArrived(_fli,false))
//  		return FALSE ;
 	if(!getEngine()->IsAirsideSel())
 		return TRUE ;
 	else
 	{
		TerminalMobElementBehavior* spTerminalBehavior = _pax->getTerminalBehavior();
		if (spTerminalBehavior && spTerminalBehavior->HasBusForThisFlight(_pax->GetCurFlightIndex(),this))
		{
			CAirsidePaxBusInSim* _bus = NULL;
		//	CPolicyData* Flightpolicy = _policy->FindPolicyData(_fli,false) ;
		//	_bus = Flightpolicy->GetCanServerBus(_pax) ;
			_bus = _flightSim->GetAvailablePassengerBus(_pax,false);
			if(_bus != NULL)
			{
				spTerminalBehavior->SetAirsideBus(_bus);
				_bus->AddPassenger(_pax) ;
			}
			return TRUE ;
		}
 		
 	}
		return FALSE ;
	
}
CAirsidePaxBusInSim* HoldingArea::GetCanServerBus(Person* _pax)
{
	if(_pax == NULL)
		return NULL;
	AirsideFlightInSim* _flightSim =  getEngine()->GetAirsideSimulation()->GetAirsideFlight(_pax->GetCurFlightIndex()) ;
	if(_flightSim == NULL)
		return NULL ;
// 	Flight* _fli = _flightSim->GetFlightInput() ;
// 	AirTrafficController* controller = _flightSim->GetAirTrafficController() ;
// 	CBoardingCallPolicy* _policy = controller->GetBoardingCall() ;
// 
// 	CAirsidePaxBusInSim* _bus = NULL ;
//     CPolicyData* Flightpolicy = _policy->FindPolicyData(_fli,false) ;
// 	 return Flightpolicy->GetCanServerBus(_pax) ;
	return _flightSim->GetAvailablePassengerBus(_pax,false);

}
void HoldingArea::RegisterServerBus(CAirsidePaxBusInSim* _bus)
{
	if(_bus == NULL)
		return ;
	std::vector<CAirsidePaxBusInSim*>::iterator iter = std::find(m_serverBus.begin() ,m_serverBus.end() ,_bus) ;
	if(iter == m_serverBus.end())
	   m_serverBus.push_back(_bus) ;
}
void HoldingArea::LogoutServerBus(CAirsidePaxBusInSim* _bus)
{
	if(_bus == NULL)
		return ;
	std::vector<CAirsidePaxBusInSim*>::iterator iter = std::find(m_serverBus.begin() ,m_serverBus.end() ,_bus) ;
	if(iter != m_serverBus.end())
	     m_serverBus.erase(iter) ;
}

//service after full feature

bool HoldingArea::IsCorralService() const
{
	return m_bCorralService;
}

bool HoldingArea::CorralService(Person *person, ElapsedTime curTime)
{
	//check the capacity
	//m_ProcessorCapacity.GetCount()
	//int nCount = occupants.getCount();
	//if(isVacant(person))

	//isVacant()

	person->setState(WaitInHoldingArea);

	if(m_pCorralServiceEvent)
	{
		m_pCorralServiceEvent->SetAvailable(false);
	}

	TRACE("\r\n 1---------------- PaxID: %d, %s,	GroupSize: %d \r\n",
			person->getID(),
			STATE_NAMES[person->getState()],
			person->GetActiveGroupSize());

	if(!isVacant(person))
	{
		//TRACE("\r\n ---------------- PaxID: %d,	GroupSize: %d, HoldSize: %d, Queue Len: %d \r\n",
		//	person->getID(),
		//	person->GetActiveGroupSize(),
		//	GetPersonWaitServiceGroupSize(), 
		//	m_pQueue->getQueueLength());

		TRACE("\r\n 2---------------- PaxID: %d, %s,	GroupSize: %d \r\n",
			person->getID(),
			STATE_NAMES[person->getState()],
			person->GetActiveGroupSize());
		available = FALSE;
		BeginCorralService(curTime);
	}
	else//wait the next person
	{
		//person->writeLogEntry(curTime,false);

		m_pCorralServiceEvent = new CorralServiceEvent(this, curTime + m_eTimeToService);
		m_pCorralServiceEvent->addEvent();
		TRACE("\r\n 3---------------- PaxID: %d, %s,	GroupSize: %d \r\n",
			person->getID(),
			STATE_NAMES[person->getState()],
			person->GetActiveGroupSize());
	}
	

	return false;
}

ElapsedTime HoldingArea::GetCorralServiceTime(ElapsedTime curTime)
{
	//get all person in the hold
	long srvTime = 0l;
	long serviceTime = 0l;

	std::vector< long > vlServiceTime ;
	float fTotalTime=0.0;

	int nPersonCount = static_cast<int>(occupants.size());

	for (int nPerson = 0; nPerson < nPersonCount; ++ nPerson)
	{	
		Person *pPerson = (Person *)occupants[nPerson];

		int nGroupSize=pPerson->GetActiveGroupSize();	
	
		for(int i=0 ; i<nGroupSize; i++)
		{
			//long srvTime = 0l;
			if (!service)
				srvTime= 0l;
			else
			{
				const ProbabilityDistribution* prob = service->hierarchyLookupEx (pPerson->getType(), curTime );
				if (!prob)
					srvTime= 0l;
				else
					srvTime = (long)prob->getRandomValue();
			}
			if(srvTime<0l) srvTime = 0l;

			//srvTime = getServiceTime(person->getType(),curTime);
			vlServiceTime.push_back(srvTime);
		}

	}
	int nTotalGroupSize=static_cast<int>(vlServiceTime.size());	
	ASSERT(nTotalGroupSize);
	std::sort(vlServiceTime.begin(),vlServiceTime.end());

	if(m_nGroupSerTimeModIndex < 0 ||m_nGroupSerTimeModIndex>4)
		(int)m_nGroupSerTimeModIndex = 0;
	if(m_nGroupSerTimeModSum < 0 || m_nGroupSerTimeModSum >100 )
		(int)m_nGroupSerTimeModSum = 100;

	int n;
	switch ( m_nGroupSerTimeModIndex ) 
	{
	case 0://Sum
		for( n=0; n<nTotalGroupSize; n++)
		{
			fTotalTime = fTotalTime + vlServiceTime[n];
		}
		fTotalTime = (float)(fTotalTime * (m_nGroupSerTimeModSum /100.0)) ;
		break;
	case 1://Average
		for( n=0; n<nTotalGroupSize; n++)
		{
			fTotalTime = fTotalTime + vlServiceTime[n];
		}
		fTotalTime =fTotalTime/ nTotalGroupSize;
		break;
	case 2: //Minimum
		fTotalTime =  (float)vlServiceTime[0];
		break;
	case 3://Maximum
		fTotalTime = (float)vlServiceTime[nTotalGroupSize-1];
		break;
	case 4://Random for group
		fTotalTime = (float)vlServiceTime[random(nTotalGroupSize)];
		break;
	default:
		for( n=0; n<nTotalGroupSize; n++)
		{
			fTotalTime = fTotalTime + vlServiceTime[n];
		}
		fTotalTime = fTotalTime * (float)( m_nGroupSerTimeModSum / 100);
		break;		
	}

	serviceTime = (long)(fTotalTime+0.5);
	return serviceTime;



	return ElapsedTime(100L);
}
void HoldingArea::leaveQueue (const Person *aPerson, ElapsedTime p_time, bool _bNoDelay )
{
	//available = FALSE;
	writeLogEvent (aPerson, p_time, DecreaseQueue);
	if( _bNoDelay )
		m_pQueue->leaveQueue ( aPerson, p_time );
	else
		m_pQueue->leaveQueue ( aPerson, p_time + ElapsedTime( DELAY ) );
}

bool HoldingArea::isVacant( Person* pPerson /*= NULL*/ ) const
{
	bool bVacant = Processor::isVacant(pPerson);

	if(IsCorralService() && bVacant)
	{

	}

	return bVacant;
}

//int HoldingArea::GetPersonWaitServiceGroupSize()
//{
//	int nGroupSize = 0;
//	std::vector<Person *>::iterator iter =  m_vPersonWaitService.begin();
//	for (;iter != m_vPersonWaitService.end(); ++iter)
//	{
//		if(*iter)
//			nGroupSize += (*iter)->GetActiveGroupSize();
//	}
//	return nGroupSize;
//}

//Person * HoldingArea::GetHeadPersonInQueue()
//{
//	Person* pHeadPerson = NULL;
//	MobileElementList *pWaitPersonList = m_pQueue->GetWaitList();
//	ASSERT(pWaitPersonList != NULL);
//	if(pWaitPersonList && pWaitPersonList->getCount())
//		pHeadPerson =(Person *)pWaitPersonList->getItem(0);
//
//	return pHeadPerson;
//}

bool HoldingArea::BeginCorralService( ElapsedTime curTime )
{
	m_pCorralServiceEvent = NULL;

	if(!IsAllPersonsReadyForService())
		return false;


	available = FALSE;

	ElapsedTime eServiceTime =GetCorralServiceTime(curTime);
	//generate leave service event.
	int nPersonCount = static_cast<int>(occupants.size());
	for (int nPerson = 0; nPerson < nPersonCount; ++nPerson)
	{	
		Person *pTmp = (Person *)occupants[nPerson];
		
		//pTmp->writeLogEntry(curTime,false);

		//pTmp->setState(WaitInHoldingArea);
		//pTmp->writeLogEntry(curTime,false);


		pTmp->setState(LeaveServer);
		pTmp->generateEvent (curTime + eServiceTime,false);
	}
	TRACE("\r\n Clear \r\n");




	return true;
}
void HoldingArea::processPerson (Person *aPerson, ElapsedTime p_time, bool _bStation )
{
	return Processor::processPerson (aPerson, p_time, _bStation );
}

//void HoldingArea::AddPersonToWaitServiceList( Person *pPerson )
//{
//	if(pPerson && std::find(m_vPersonWaitService.begin(), m_vPersonWaitService.end(), pPerson) == m_vPersonWaitService.end())
//		m_vPersonWaitService.push_back(pPerson);
//}

bool HoldingArea::IsAllPersonsReadyForService()
{
	int nPersonCount = static_cast<int>(occupants.size());

	for (int nPerson = 0; nPerson < nPersonCount; ++ nPerson)
	{	
		Person *pPerson = (Person *)occupants[nPerson];
		if(pPerson->getState() != WaitInHoldingArea)
			return false;
	}
	return true;
}

void HoldingArea::ClearCorralEvent( CorralServiceEvent *pEvent )
{
	if(m_pCorralServiceEvent == pEvent)
		m_pCorralServiceEvent = NULL;

}

//Point HoldingArea::GetPipeExitPoint(int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob)
//{
//	Point pt;
//	if (inConstraintLength()>0)
//	{
//		return inConstraint(0);
//	}
//	else 
//	{
//		pt= GetServiceLocation();
//		terminalMob->SetNextHoldAreaValue(true,pt);
//		return pt;
//	}
//}