#include "StdAfx.h"
#include ".\occupiedassignedstandactioninsim.h"
#include "AirsideFlightInSim.h"
#include "AirportResourceManager.h"
#include "IntersectionNodeInSim.h"
#include "TaxiwayResource.h"
#include "AirsideResource.h"

#include "common/AirportDatabase.h"

COccupiedAssignedStandActionInSim::COccupiedAssignedStandActionInSim(void)
{
// 	m_pOccupiedAssignedStandAction = new COccupiedAssignedStandAction;
	m_pOccupiedAssignedStandCriteria=new COccupiedAssignedStandCriteria();
}

COccupiedAssignedStandActionInSim::~COccupiedAssignedStandActionInSim(void)
{
// 	delete m_pOccupiedAssignedStandAction;
// 	m_pOccupiedAssignedStandAction = NULL;


}

void COccupiedAssignedStandActionInSim::Init(int nPrjId,CAirportDatabase* pDatabase)
{
// 	m_pOccupiedAssignedStandAction->SetAirportDB(pDatabase);
// 	m_pOccupiedAssignedStandAction->ReadData(nPrjId);
	m_pOccupiedAssignedStandCriteria->SetAirportDB(pDatabase);
	m_pOccupiedAssignedStandCriteria->SetPrjID(nPrjId);
	m_pOccupiedAssignedStandCriteria->ReadData();
}
int COccupiedAssignedStandActionInSim::calculateMatchValue(int valueArray[],int arraySize)
{
	int matchValue=0;
	for (int i=0;i<arraySize;i++)
	{
		matchValue*=10;
		matchValue+=valueArray[i];
	}
	return matchValue;
}
COccupiedAssignedStandStrategy COccupiedAssignedStandActionInSim::GetStrategyByFlightType(AirsideFlightInSim* pFlight, AirportResourceManager* pResManager)
{

	int maxMatch,curMatch;
	int curLevel;
	const int levelNum=4;
	int levelMatchValue[levelNum];
	COccupiedAssignedStandStrategy _strategy;
	maxMatch=curMatch=0;	
	ExitsList exitsList=m_pOccupiedAssignedStandCriteria->getExitsList();
	ExitsList::iterator exitsIter;
	for (exitsIter=exitsList.begin();exitsIter!=exitsList.end();exitsIter++)
	{
		curLevel=0;
		levelMatchValue[curLevel]=0;
		CAirSideCriteriaExits *curExits=(CAirSideCriteriaExits *)(*exitsIter);	
		if (!curExits->IsAllRunwayExit())		
		{            
			bool bInExitList=false;
			ObjIDList exitList=curExits->getExitList();
			for (int i=0;i<(int)exitList.size();i++)
			{
				if(pFlight->GetRunwayExit()->GetID()==(int)exitList.at(i))
				{
					bInExitList=true;
					break;
				}				
			}
			if (bInExitList==false)
			{
				continue;
			}else
			{
				levelMatchValue[curLevel]=1;
			}
			
		}


		ParkingStandsList::iterator parkingStandIter;
		ParkingStandsList parkingStandList=curExits->getParkingStandsList();
		for (parkingStandIter=parkingStandList.begin();parkingStandIter!=parkingStandList.end();parkingStandIter++)
		{
			curLevel=1;
			levelMatchValue[curLevel]=0;
			CAirSideCriteriaParkingStands *curParkingStand=(CAirSideCriteriaParkingStands *)(*parkingStandIter);
			if (!curParkingStand->isAllParkingStand())
			{				
				int matchValue=curParkingStand->getStandMatch(pFlight->getStand()); 
				if (matchValue==0)
				{
					continue;
				}else
				{
					levelMatchValue[curLevel]=matchValue;
				}
			}

			FlightTypeList flightTypeList=curParkingStand->getFlightTypeList();
			FlightTypeList::iterator flightTypeIter;
			for (flightTypeIter=flightTypeList.begin();flightTypeIter!=flightTypeList.end();flightTypeIter++)
			{
				curLevel=2;
				levelMatchValue[curLevel]=0;
				CAirSideCriteriaFlightType *curFlightType=(CAirSideCriteriaFlightType *)(*flightTypeIter);
				if (!curFlightType->isDefaultFlightType())
				{
					if (pFlight->fits(curFlightType->getFlightType()))
					{
						levelMatchValue[curLevel]=1;
					}
				}

				TimeWindowList timeWindowList=curFlightType->getTimeWindowList();
				TimeWindowList::iterator timeWindowIter;
				for(timeWindowIter=timeWindowList.begin();timeWindowIter!=timeWindowList.end();timeWindowIter++)
				{
					curLevel=3;
					levelMatchValue[curLevel]=0;
					CAirSideCreteriaTimeWin *curTimeWin=(CAirSideCreteriaTimeWin *)(*timeWindowIter);
					if (curTimeWin->inTimeWindow(pFlight->getCurTime()))
					{
						levelMatchValue[curLevel]=1;
						curMatch=calculateMatchValue(levelMatchValue,levelNum);
						if (curMatch>maxMatch)
						{
							maxMatch=curMatch;
							curTimeWin->GetStrategy(_strategy);
						}
					}

				}
			}



		}

	}

	return _strategy;

/*
		COccupiedAssignedStandStrategy _Strategy;
		RunwayExitInSim *runwayExit=pFlight->GetRunwayExit();
		ElapsedTime curTime=pFlight->getCurTime();
		
	
		return _Strategy;
	
		StandStrategy _Strategy;
	
	//--c--should be modified after modified occupied assigned stand action
		int nCount = m_pOccupiedAssignedStandAction->GetFltTypeCount();
	
		COccupiedAssignedStandFltType* pItem = NULL;	
		for (int i =0; i < nCount; i++)
		{
			FlightConstraint fltCnst = m_pOccupiedAssignedStandAction->GetFltTypeItem(i)->GetFltType() ;
			if(	pFlight->fits(fltCnst) )
			{
				pItem = m_pOccupiedAssignedStandAction->GetFltTypeItem(i);
				break;
			}
		}
			
		if (pItem)
		{
			COccupiedAssignedStandPriority* pPriorityItem =NULL;
			int nStrategyID = -1;
			switch(nPriorityID)
			{
				case 1:
					for (int i =0; i < pItem->GetPriorityCount(); i++)
					{
						if (pItem->GetPriorityItem(i)->GetPriority() == Priority1)
						{
							pPriorityItem = pItem->GetPriorityItem(i);
							break;
						}
					}
					break;
	
				case 2:
					for (int i =0; i < pItem->GetPriorityCount(); i++)
					{
						if (pItem->GetPriorityItem(i)->GetPriority() == Priority2)
						{
							pPriorityItem = pItem->GetPriorityItem(i);
							break;
						}
					}
					break;
				case 3:
					for (int i =0; i < pItem->GetPriorityCount(); i++)
					{
						if (pItem->GetPriorityItem(i)->GetPriority() == Priority3)
						{
							pPriorityItem = pItem->GetPriorityItem(i);
							break;
						}
					}
					break;
				case 4:
					for (int i =0; i < pItem->GetPriorityCount(); i++)
					{
						if (pItem->GetPriorityItem(i)->GetPriority() == Priority4)
						{
							pPriorityItem = pItem->GetPriorityItem(i);
							break;
						}
					}
					break;
				case 5:
					for (int i =0; i < pItem->GetPriorityCount(); i++)
					{
						if (pItem->GetPriorityItem(i)->GetPriority() == Priority5)
						{
							pPriorityItem = pItem->GetPriorityItem(i);
							break;
						}
					}
					break;
			default:
				break;
			}
			if (pPriorityItem)
			{				
			
	
							_Strategy.m_Strategy = pPriorityItem->GetStrategyType();
							switch(_Strategy.m_Strategy) 
							{
								case Delaytime:
									{
										long nTime = pPriorityItem->GetDelaytime()*60;		//convert to seconds
										_Strategy.m_tDelayTime = ElapsedTime(nTime);
										break;
									}
								case ToIntersection:
									{
										_Strategy.m_nTempResoure = pPriorityItem->GetIntersectionID();
										break;
									}
								case ToStand:
									{
										_Strategy.m_nTempResoure = pPriorityItem->GetStandID();
										break;
									}
								case ToTemporaryParking:
									{
										_Strategy.m_nTempResoure = pPriorityItem->GetTemporaryParkingID();
										break;
									}
								case ToDynamicalStand:				
										break;
								default:
									break;
							}
				
			}
	
		}
	
	
		return _Strategy;*/
	
}

