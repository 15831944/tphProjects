#include "StdAfx.h"
#include ".\mobileelementgenerator.h"
#include "Airside/AirsideFlightInSim.h"
#include "engine/TurnaroundVisitor.h"
#include "engine/GroupLeaderInfo.h"
#include "engine/turnarnd.h"
#include "Airside/AirsideSimulation.h"
#include "terminal.h"
#include "AirsideMobElementBehavior.h"
#include "ARCportEngine.h"
#include "OnboardSimulation.h"
#include "OnboardFlightInSim.h"
#include "OnboardSeatInSim.h"
#include "Common\OnBoardException.h"
#include "PaxDeplaneBehavior.h"
#include "PaxLandsideBehavior.h"
#include "LogContainer.h"
#include "LandsideSimulation.h"
#include "LandsideVehicleInSim.h"

#include "ArrivalPaxLandsideBehavior.h"
#include "DeparturePaxLandsideBehavior.h"
#include "VisitorLandsideBehavior.h"
#include "Landside/CLandSidePublicVehicleType.h"
#include "Landside/VehicleItemDetail.h"

#include "LandsideBusStationInSim.h"
#include "LandsideCurbsideInSim.h"
#include "LandsideResourceManager.h"
#include "Landside/LandsideCurbSide.h"


CMobileElementGenerator::CMobileElementGenerator()
{

}
CMobileElementGenerator::~CMobileElementGenerator()
{

}
//CPaxGenerator::CPaxGenerator():CMobileElementGenerator()
//{
//
//}
CPaxGenerator::CPaxGenerator(CARCportEngine* _pEngine) :m_pEngine(_pEngine),CMobileElementGenerator()
{

}
CPaxGenerator::~CPaxGenerator()
{

}

int CPaxGenerator::GenerateDelayMobileBag(int Fli_ID ,ElapsedTime& Time, std::vector<Person*>& _paxlist, bool bHasCartService)
{
	CFlightPaxLogContainer* p_contaner = CFlightPaxLogContainer::GetInstance() ;
	CFlightLogPair* _pair = p_contaner->GetDelayPaxLogByFlightID(Fli_ID) ;
	if(_pair == NULL)
		return 0 ;
	Flight* fli = m_pEngine->GetAirsideSimulation()->GetAirsideFlight(Fli_ID)->GetFlightInput();
	ElapsedTime _delaytime ;
	if(fli != NULL)
	{
		_delaytime = Time - fli->getArrTime() ;
	}
	PaxLog* p_PaxLog = _pair->p_Log ;
	for (int i = 0 ; i < p_PaxLog->getCount() ;i++)
	{
		Person* p_pax = NULL ;
		MobLogEntry bagentry ;
		p_PaxLog->getItem(bagentry,i) ;
		if(bagentry.GetMobileType() == 2 && bagentry.getOwnStart())
		{
			if(bagentry.isTurnaround())
			{
				if(bagentry.GetMobileType()>0)
				{
					bagentry.setEntryTime(Time);
					p_contaner->updateItem(bagentry);
					p_pax = new TurnaroundVisitor(bagentry, m_pEngine);
				}
			}
			else
			{
				if(bagentry.GetMobileType()>0)
				{
					bagentry.setEntryTime(Time);
					p_contaner->updateItem(bagentry);
					p_pax = new PaxVisitor ( bagentry, m_pEngine );
				}
			} 
		}
		if(p_pax != NULL)
		{
          ((CGroupLeaderInfo*)p_pax->m_pGroupInfo)->SetGroupLeader(p_pax);
		   m_pEngine->m_simBobileelemList.Register( p_pax );
		 //  bagentry.setEntryTime(bagentry.getEntryTime() + _delaytime) ;
		 //  if(bagentry.getOwnStart())
		//   {
			   if(!bHasCartService)
				   p_pax->generateEvent(bagentry.getEntryTime(), false);
		//   }
		   _paxlist.push_back(p_pax);
		}
	}
	return p_PaxLog->getCount() ;
}
int CPaxGenerator::GenerateDelayMobileElement(int nFlightID, ElapsedTime& Time, std::vector<Person*>& _paxlist,bool bGenerateBaggage, int _PaxNum)
{
    CFlightPaxLogContainer* p_contaner = CFlightPaxLogContainer::GetInstance() ;
	CFlightLogPair* pLogPair = p_contaner->GetDelayPaxLogByFlightID(nFlightID) ;

	if(pLogPair == NULL)
		return 0 ;

	PaxLog* p_PaxLog = pLogPair->p_Log ;
	int nCount = p_PaxLog->getCount() ;

	Person* p_pax = NULL ;
	MobLogEntry logentry ; 
    
	if(_PaxNum == -1 || _PaxNum > nCount)
		_PaxNum = nCount ;

	int currentIndex = p_contaner->CurrentIndexOfPaxlogByFlightID(nFlightID) ;
	int oldindex = currentIndex ;
	int actual = currentIndex + _PaxNum ;
	Flight* pFlight = m_pEngine->GetAirsideSimulation()->GetAirsideFlight(nFlightID)->GetFlightInput();

	ElapsedTime _delaytime =0L;

	PaxLog paxlog(new EventLog<MobEventStruct>()) ;
    pLogPair->GetNoActivePaxlog(paxlog,_PaxNum) ;

	int nPaxCount = paxlog.getCount();
	for (int i = 0 ; i < nPaxCount;i++)
	{
		paxlog.getItem(logentry,i) ;

		if(!logentry.getOwnStart())
		{
			i = i + (logentry.getGroupSize() - 1) ;
			continue ;
			
		}

		if (bGenerateBaggage == true)//check baggage already generate
		{
			if (logentry.GetMobileType() == 2)
			{
				continue;
			}
		}

		if(logentry.isTurnaround())
		{
			if(logentry.GetMobileType()>0)
				p_pax = new TurnaroundVisitor(logentry, m_pEngine);
			else
				p_pax = new TurnaroundPassenger ( logentry, m_pEngine );
		}
		else
		{
			if(logentry.GetMobileType()>0)
				p_pax = new PaxVisitor ( logentry, m_pEngine );
			else
				p_pax = new Passenger (logentry, m_pEngine );
		} 
		m_pEngine->m_simBobileelemList.Register( p_pax );

		if (logentry.GetMobileType() != 2)
		{
			p_pax->newGroupFollower();
			i = i + logentry.getGroupSize() - 1;
		}
		else
		{
			((CGroupLeaderInfo*)p_pax->m_pGroupInfo)->SetGroupLeader(p_pax);
		}

		if(logentry.GetMobileType() == 0)
		{
			if (m_pEngine->IsOnboardSel() && m_pEngine->GetOnboardSimulation())
			{
				if(pFlight != NULL)
					_delaytime = Time - logentry.getEntryTime() ;

				OnboardFlightInSim* pOnboardFlight = m_pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pEngine->GetAirsideSimulation()->GetAirsideFlight(nFlightID), true);
				if (pOnboardFlight)
				{
					ElapsedTime tTime = logentry.getEntryTime() + _delaytime;

					PaxDeplaneBehavior* pBehavior = new PaxDeplaneBehavior( p_pax);
					pBehavior->setFlight(pOnboardFlight);
					p_pax->setBehavior(pBehavior);
					p_pax->setState(EntryOnboard);

					CString strMessage;
					OnboardSeatInSim* pSeat = pOnboardFlight->GetSeat(p_pax,strMessage);
					if (pSeat)
					{

						CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)p_pax->m_pGroupInfo;
						if(pGroupLeaderInfo->isInGroup() && !pGroupLeaderInfo->IsFollower())
						{
							MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
							int nFollowerCount = elemList.getCount();

							if (!pOnboardFlight->IsSeatsAvailable(nFollowerCount))
							{
								pSeat->PersonLeave();
								p_pax->flushLog(tTime, false);
								//throw new OnBoardSimEngineException( new OnBoardDiagnose(p_pax->getID(),pOnboardFlight->getFlightID(),tTime,strMessage));	
								continue;
							}

							pSeat->AssignToPerson(p_pax);
							pBehavior->setSeat(pSeat);

							for( int i=0; i< nFollowerCount; i++ )
							{
								Person* _pFollower = (Person*) elemList.getItem( i );
								if(_pFollower == NULL)
									continue;  

								PaxDeplaneBehavior* pBehavior = new PaxDeplaneBehavior(_pFollower);
								pBehavior->setFlight(pOnboardFlight);
								_pFollower->setBehavior(pBehavior);
								_pFollower->setState(EntryOnboard);
								
								CString strMessage;
								OnboardSeatInSim* pSeat = pOnboardFlight->GetSeat(_pFollower,strMessage);
								if (pSeat == NULL)
								{
									p_pax->flushLog(tTime, false);
									//throw new OnBoardSimEngineException( new OnBoardDiagnose(p_pax->getID(),pOnboardFlight->getFlightID(),tTime,strMessage));	
									continue;
								}
								pSeat->AssignToPerson(_pFollower);
								pBehavior->setSeat(pSeat);
							}

							int nCount =((Passenger*)p_pax)->m_pVisitorList.size();
							for( int i=nCount-1; i>=0; i-- )
							{
								PaxVisitor* pVis = ((Passenger*)p_pax)->m_pVisitorList[i];
								if (pVis == NULL)
									continue;

								PaxDeplaneBehavior* pBehavior = new PaxDeplaneBehavior(pVis);
								pVis->setBehavior(pBehavior);
								pBehavior->SetGroupBehavior();
								pVis->setState(EntryOnboard);
							}
						}
					}
					else
					{
						p_pax->flushLog(tTime, false);
						//throw new OnBoardSimEngineException( new OnBoardDiagnose(p_pax->getID(),pOnboardFlight->getFlightID(),tTime,strMessage));		
						continue;
					}
					p_pax->generateEvent(tTime, false);
					_paxlist.push_back(p_pax) ;

					continue;
				}
			}
			//else
			{
				/*if(pFlight != NULL)
					_delaytime = Time - pFlight->getArrTime() ;*/

				//p_pax->getLogEntry().setEntryTime(logentry.getEntryTime() + _delaytime);
				p_pax->setBehavior( new AirsidePassengerBehavior( p_pax,TAKE_OFF_FLIGHT ));
				_paxlist.push_back(p_pax) ;
			}

		}

		if(pFlight != NULL)
			_delaytime = Time - pFlight->getArrTime();
		p_pax->getLogEntry().setEntryTime(logentry.getEntryTime() + _delaytime);//check baggage doesn't show up in airside. But need update checkbaggage entry time
		p_pax->generateEvent(logentry.getEntryTime() + _delaytime, false);
	}

	return _paxlist.size() ;
}

int CPaxGenerator::GenerateNodelayMobileElement(ElapsedTime& Time ,std::vector<Passenger*>& _paxlist,TerminalEntryEvent& _event)
{
	CFlightPaxLogContainer* p_contaner = CFlightPaxLogContainer::GetInstance() ;
	 PaxLog* p_paxDatas = p_contaner->GetNoDelayPaxLog() ;
	_event.initList(p_paxDatas,p_paxDatas->getCount()) ;
	return p_paxDatas->getCount() ;
}
void CPaxGenerator::FindNearestStation(LandsideCurbSideInSim *assCurside,LandSidePublicVehicleType *operation,LandsideBusStationInSim * &pGetOffStation)
{
	//find the nearest bus station to assCurbside
	double minDis=-1;
	
	CPoint2008 curbsidePos;
	assCurside->getCurbInput()->get3DCenter(curbsidePos);

	for (int j=0;j<operation->getElementCount();j++)					
	{
		LandsideResourceManager *resManager=m_pEngine->GetLandsideSimulation()->GetResourceMannager();
		VehicleTypeStation *pStation=operation->getStationItem(j);
		for (int k=0;k<(int)resManager->m_BusStation.size();k++)
		{
			LandsideBusStationInSim *curStationInSim=(LandsideBusStationInSim *)resManager->m_BusStation.at(k);
			if (curStationInSim->getBusStation()->getID()==pStation->GetBusStationID())
			{							
				CPoint2008 sationPos=curStationInSim->getBusStation()->get2DCenter();
				double curDis=sationPos.getDistanceTo(curbsidePos);
				if (minDis<0 || curDis<minDis)
				{
					minDis=curDis;
					pGetOffStation=curStationInSim;
				}
			}
		}
	}
}
int CPaxGenerator::GenerateDepMobileElementToLandside( int nFlightID, const ElapsedTime& t )
{
	CFlightPaxLogContainer* p_contaner = CFlightPaxLogContainer::GetInstance() ;
	CFlightLogPair* pLogPair = p_contaner->GetDepDelayPaxLogByFlightID(nFlightID) ;

	if(pLogPair == NULL)
		return 0 ;

	PaxLog* p_PaxLog = pLogPair->p_Log ;
	int nCount = p_PaxLog->getCount() ;

	Person* p_pax = NULL ;
	MobLogEntry logentry ; 



	int currentIndex = p_contaner->CurrentIndexOfPaxlogByFlightID(nFlightID) ;
	int oldindex = currentIndex ;
	int actual = currentIndex + nCount ;
	//Flight* pFlight = m_pEngine->GetAirsideSimulation()->GetAirsideFlight(nFlightID)->GetFlightInput();

	ElapsedTime _delaytime =0L;

	PaxLog paxlog(new EventLog<MobEventStruct>()) ;
	pLogPair->GetNoActivePaxlog(paxlog,nCount) ;

	int nPaxCount = paxlog.getCount();
	for (int i = 0 ; i < nPaxCount;i++)
	{		
		paxlog.getItem(logentry,i) ;

		if(!logentry.getOwnStart())
		{
			i = i + (logentry.getGroupSize() - 1) ;
			continue ;

		}

		LandsideVehicleInSim* pVehicle = m_pEngine->GetLandsideSimulation()->FindPaxVehicle(logentry.getID());

		//if (pVehicle && pVehicle->getVehicleType()==VehicleType_Public)
		//{
		//	LandSidePublicVehicleType *operation=(LandSidePublicVehicleType *)pVehicle->GetVehicleItemDetail()->getOperation(VehicleType_Public);
		//	ASSERT(operation);
		//	if (operation->getElementCount()==0)
		//	{
		//		//no pax will be on the bus that don't stop at any station
		//		continue;
		//	}

		//}

		if(logentry.isTurnaround())
		{
			if(logentry.GetMobileType()>0)
				p_pax = new TurnaroundVisitor(logentry, m_pEngine);
			else
				p_pax = new TurnaroundPassenger ( logentry, m_pEngine );
		}
		else
		{
			if(logentry.GetMobileType()>0)
				p_pax = new PaxVisitor ( logentry, m_pEngine );
			else
				p_pax = new Passenger (logentry, m_pEngine );
		} 
		m_pEngine->m_simBobileelemList.Register( p_pax );

		if (logentry.GetMobileType() != 2)
		{
			p_pax->newGroupFollower();
			i = i + logentry.getGroupSize() - 1;
		}
		else
		{
			((CGroupLeaderInfo*)p_pax->m_pGroupInfo)->SetGroupLeader(p_pax);
		}

		if(logentry.GetMobileType() == 0)
		{		

			DeparturePaxLandsideBehavior* pBehavior = new DeparturePaxLandsideBehavior( p_pax);			
			p_pax->setBehavior(pBehavior);
			p_pax->setState(EntryLandside);	
			pBehavior->InitializeBehavior();


			//p_pax->generateEvent(t, false);
			//LandsideVehicleInSim* pVehicle = m_pEngine->GetLandsideSimulation()->FindPaxVehicle(p_pax->getID());
			if(pVehicle)
			{
				p_pax->getLandsideBehavior()->setVehicle(pVehicle);
				//pVehicle->AddBelongVehiclePax(p_pax->getLandsideBehavior());
				//pBehavior->SetUsePrivateVehicle();
//                if(pVehicle->getVehicleType()==VehicleType_Public)
//				{
//					LandsideCurbSideInSim *assCurside=m_pEngine->GetLandsideSimulation()->AssignCurbside(logentry);
//					ASSERT(assCurside);
//					pBehavior->SetUsePublicVehicle();
//
//					LandsideBusStationInSim *pGetOffStation=NULL;
//
//					LandSidePublicVehicleType *operation=(LandSidePublicVehicleType *)pVehicle->GetVehicleItemDetail()->getOperation(VehicleType_Public);
//
//					FindNearestStation(assCurside,operation,pGetOffStation);
//
//// 					ASSERT(pGetOffStation);
//					pBehavior->SetAssCurbside(assCurside);
//					pBehavior->SetUseBusStation(pGetOffStation);
//				}
			}
			else
			{
				p_pax->generateEvent(logentry.getEntryTime(), false);
			}
			//vPaxList.push_back(p_pax) ;

		}
	}
	return 1;
}



int CPaxGenerator::GenerateArrMobileElementToLandside( int nFlightID )
{
	CFlightPaxLogContainer* p_contaner = CFlightPaxLogContainer::GetInstance() ;
	CFlightLogPair* pLogPair = p_contaner->GetDelayPaxLogByFlightID(nFlightID) ;

	if(pLogPair == NULL)
		return 0 ;

	PaxLog* p_PaxLog = pLogPair->p_Log ;
	int nCount = p_PaxLog->getCount() ;

	Person* p_pax = NULL ;
	MobLogEntry logentry ; 	

	int currentIndex = p_contaner->CurrentIndexOfPaxlogByFlightID(nFlightID) ;
	int oldindex = currentIndex ;
	int actual = currentIndex + nCount ;
	Flight* pFlight = m_pEngine->GetAirsideSimulation()->GetAirsideFlight(nFlightID)->GetFlightInput();

	ElapsedTime _delaytime =0L;

	PaxLog paxlog(new EventLog<MobEventStruct>()) ;
	pLogPair->GetNoActivePaxlog(paxlog,nCount) ;

	int nPaxCount = paxlog.getCount();
	for (int i = 0 ; i < nPaxCount;i++)
	{
		paxlog.getItem(logentry,i) ;

		//const LandsideSimulation::PaxVehicleMap &paxVehicleMap=m_pEngine->GetLandsideSimulation()->GetPaxVehicleMap();
		//LandsideSimulation::PaxVehicleMap::const_iterator iter=paxVehicleMap.find(logentry.getID());

		//if(iter!=paxVehicleMap.end())
		//{
		//	CVehicleItemDetail *pVehicleType=(CVehicleItemDetail*)iter->second;

		//	if (pVehicleType &&pVehicleType->getLandSideBaseType()==VehicleType_Public)
		//	{
		//		LandSidePublicVehicleType *operation=(LandSidePublicVehicleType *)pVehicleType->getOperation(VehicleType_Public);
		//		if (operation->getElementCount()==0)
		//		{
		//			//no pax will be on the bus that don't stop at any station
		//			continue;
		//		}
		//	}
		//}

		if(!logentry.getOwnStart())
		{
			i = i + (logentry.getGroupSize() - 1) ;
			continue ;

		}

		if(logentry.isTurnaround())
		{
			if(logentry.GetMobileType()>0)
				p_pax = new TurnaroundVisitor(logentry, m_pEngine);
			else
				p_pax = new TurnaroundPassenger ( logentry, m_pEngine );
		}
		else
		{
			if(logentry.GetMobileType()>0)
				p_pax = new PaxVisitor ( logentry, m_pEngine );
			else
				p_pax = new Passenger (logentry, m_pEngine );
		} 
		m_pEngine->m_simBobileelemList.Register( p_pax );

		if (logentry.GetMobileType() != 2)
		{
			p_pax->newGroupFollower();
			i = i + logentry.getGroupSize() - 1;
		}
		else
		{
			((CGroupLeaderInfo*)p_pax->m_pGroupInfo)->SetGroupLeader(p_pax);
		}

		if(logentry.GetMobileType() == 0)
		{		
			ArrivalPaxLandsideBehavior* pBehavior = new ArrivalPaxLandsideBehavior( p_pax);	

			LandsideBusStationInSim *pGetOffStation=NULL;

		/*	iter=paxVehicleMap.find(logentry.getID());

			if (iter!=paxVehicleMap.end())
			{				
				CVehicleItemDetail *pVehicleType=(CVehicleItemDetail*)iter->second;

				
			}		*/						
			
			p_pax->setBehavior(pBehavior);
			p_pax->setState(EntryLandside);	
			//pBehavior->InitializeBehavior();
						
			p_pax->generateEvent(logentry.getEntryTime(), false);

		}
		
	}

	return 1;
}