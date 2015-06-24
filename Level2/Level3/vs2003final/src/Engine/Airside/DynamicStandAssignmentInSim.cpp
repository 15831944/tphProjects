#include "StdAfx.h"
#include ".\dynamicstandassignmentinsim.h"
#include "StandResourceManager.h"
#include "StandAssignmentRegisterInSim.h"
#include "Common/elaptime.h"
#include "StandInSim.h"
#include "AirTrafficController.h"
#include "StandBufferInSim.h"
#include "../../Inputs/GateAssignmentConstraints.h"
#include "../../Inputs/AdjacencyConstraintSpec.h"
#include "../../Common/ARCFlight.h"
#include "../../Inputs/FlightPriorityInfo.h"
#include "../../InputAirside/TowOffStandAssignmentDataList.h"
#include "../../InputAirside/TowOffStandAssignmentData.h"

CDynamicStandAssignmentInSim::CDynamicStandAssignmentInSim(void)
:m_pStandResManager(NULL)
,m_pGateAssignConstraints(NULL)
,m_pflightPriorityInfo(NULL)
,m_pTowCriteria(NULL)
{
}

CDynamicStandAssignmentInSim::~CDynamicStandAssignmentInSim(void)
{
}

bool CDynamicStandAssignmentInSim::GetFlightStartAndEndTime(AirsideFlightInSim* pFlight,StandInSim* pTestStand,ElapsedTime& startTime,ElapsedTime& endTime)
{
	
	ElapsedTime tStart = pFlight->GetTime();
	ElapsedTime tEnd = tStart;
	ElapsedTime tDepTime = tStart + (pFlight->GetDepTime() - pFlight->GetArrTime());

	CTowOffStandAssignmentData* pTowData = NULL;

	std::map<CString,CTowOffStandAssignmentData*> mapTowDataList;
	m_pTowCriteria->GetFlightTowoffDataList(pFlight->GetFlightInput(), pTestStand->GetStandInput()->GetObjectName(), mapTowDataList);

	std::map<CString,CTowOffStandAssignmentData*>::iterator iter = mapTowDataList.begin();
	for (; iter != mapTowDataList.end(); iter++)
	{
		pTowData = iter->second;
		if (pTowData == NULL)
			continue;
		
		if ( pTowData->IsTow() == FALSE)		//not tow
		{
			if (pFlight->IsTransfer()&& !(pFlight->getArrStand() == pFlight->getDepStand()))
				tEnd = tStart + (tDepTime - tStart)/2L;
			else
				tEnd = tDepTime;
		}
		else		//tow
		{
			if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
			{
				tEnd = tDepTime;

				if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
					tEnd = tStart + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
				else
					tEnd = tDepTime - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

				if (tEnd < tStart || tEnd > tDepTime)		//error
				{
					continue;
				}
			}
			else	//free up stand
			{
				tEnd = tStart + pTowData->getFltDeplaneTime();
			}		
		}
	}

	startTime = tStart;
	endTime = tEnd;

	return true;
}


bool CDynamicStandAssignmentInSim::StandAssignConflict(AirsideFlightInSim* pFlight,StandInSim* pTestStand)
{
	ASSERT(pTestStand != NULL);
	if (!pTestStand->TryLock(pFlight))
		return true;

	//uncomment these lines when use constraints rules in Dynamic Stand Reassignment
	// test gate assignment constraints conflict
	if( IsGateAssignConstraintsConflict( pTestStand, pFlight))
		return true;

	// test adjacency constraints conflict
	if( IsCurrentOccupiedFlightAdjacencyConflict( pTestStand, pFlight) )
		return true;

	return false;
}
StandInSim* CDynamicStandAssignmentInSim::IsCurrentOccupiedFlightAdjacencyConflict(StandInSim* pStand,AirsideFlightInSim* pFlight)
{
	StandAdjacencyConstraint& standAdjacencyCon = m_pGateAssignConstraints->GetStandAdjConstraint();
	if (standAdjacencyCon.GetUseFlag())
	{
		return StandNameBasedFlightAdjacencyConflict(pStand,pFlight);
	}

	return StandDimensionBasedFlightAdjacencyConflict(pStand,pFlight);
}

//stand dimension based
StandInSim* CDynamicStandAssignmentInSim::StandDimensionBasedFlightAdjacencyConflict(StandInSim* pStand,AirsideFlightInSim* pFlight)
{
	const Flight* pInputFlight  = pFlight->GetFlightInput();

	StandAdjacencyConstraint& standAdjacencyCon = m_pGateAssignConstraints->GetStandAdjConstraint();
	int nConNum = standAdjacencyCon.GetAdjConstraintSpec().GetCount();
	ALTObjectID standID = pStand->GetStandInput()->GetObjectName();

	std::vector<ALTObjectID> vStandList;
	for (int i = 0; i < nConNum; i++)
	{
		vStandList.clear();
		AdjacencyConstraintSpecDate _Constraint =standAdjacencyCon.GetAdjConstraintSpec().GetItem(i);
		if (!_Constraint.GetAdjacencyStandNameList(standID,vStandList))
		{
			continue;
		}
		for (unsigned j = 0; j < vStandList.size(); j++)
		{
			ALTObjectID adjID = vStandList[j];
			StandInSim* pTestStand = m_pStandResManager->GetStandByName(adjID);

			if(pTestStand == NULL || pTestStand == pStand)
				continue;

			AirsideFlightInSim* pAdjacencyFlightInSim = pTestStand->GetLockedFlight();
			if (pAdjacencyFlightInSim)
			{
				std::vector<ARCFlight*> vAdjFlights;
				vAdjFlights.push_back(pAdjacencyFlightInSim->GetFlightInput());

				InputTerminal* pInTerm = m_pGateAssignConstraints->GetInputTerminal();
				if (!_Constraint.IsFlightFitContraint(pFlight->GetFlightInput(),vAdjFlights,standID,adjID,pInTerm))
					return pTestStand;
			}
		}
	}
	return NULL;
}


//stand name based
StandInSim* CDynamicStandAssignmentInSim::StandNameBasedFlightAdjacencyConflict(StandInSim* pStand,AirsideFlightInSim* pFlight)
{
	const Flight* pInputFlight  = pFlight->GetFlightInput();

	StandAdjacencyConstraint& standAdjacencyCon = m_pGateAssignConstraints->GetStandAdjConstraint();
	int nConNum = standAdjacencyCon.GetAdjConstraints().GetCount();
	ALTObjectID standID = pStand->GetStandInput()->GetObjectName();

	for (int i = 0; i < nConNum; i++)
	{
		AdjacencyGateConstraint _Constraint =standAdjacencyCon.GetAdjConstraints().GetItem(i);

		//find fit first record
		if (standID == _Constraint.GetFirstConstraint()->GetGate())
		{
			StandInSim* pTestStand = m_pStandResManager->GetStandByName(_Constraint.GetSecondConstraint()->GetGate());

			if(pTestStand == NULL || pTestStand == pStand)
				continue;

			AirsideFlightInSim* pAdjacencyFlightInSim = pTestStand->GetLockedFlight();
			if (pAdjacencyFlightInSim)
			{
				InputTerminal* pInTerm = m_pGateAssignConstraints->GetInputTerminal();

				ARCFlight* pAdjFlight = pAdjacencyFlightInSim->GetFlightInput();

				if (_Constraint.GetFirstConstraint()->IsFlightFit(*pInputFlight, pInTerm))
				{
					if (!_Constraint.GetSecondConstraint()->IsFlightFit(*pAdjFlight,pInTerm))
					{
						return pTestStand;
					}
				}
			}
		}
		//find fit second record
		if (standID == _Constraint.GetSecondConstraint()->GetGate())
		{
			StandInSim* pTestStand = m_pStandResManager->GetStandByName(_Constraint.GetFirstConstraint()->GetGate());

			if(pTestStand == NULL || pTestStand == pStand)
				continue;

			AirsideFlightInSim* pAdjacencyFlightInSim = pTestStand->GetLockedFlight();
			if (pAdjacencyFlightInSim)
			{
				InputTerminal* pInTerm = m_pGateAssignConstraints->GetInputTerminal();

				ARCFlight* pAdjFlight = pAdjacencyFlightInSim->GetFlightInput();

				if (_Constraint.GetFirstConstraint()->IsFlightFit(*pAdjFlight, pInTerm))
				{
					if (!_Constraint.GetSecondConstraint()->IsFlightFit(*pInputFlight,pInTerm))
					{
						return pTestStand;

					}
				}
			}
		}
	}
	return NULL;
}

StandInSim* CDynamicStandAssignmentInSim::AssignFlightStandWithStandList( AirsideFlightInSim* pFlight,FLIGHT_PARKINGOP_TYPE _type,const std::vector<StandInSim*>& vStandList,
																		   const ElapsedTime& tStart, const ElapsedTime& tEnd)
{
	size_t nCount = vStandList.size();
	StandInSim* pTestStand = NULL;

	for ( size_t i = 0; i < nCount; i++ )
	{
		pTestStand = vStandList.at(i);

		ASSERT(pTestStand != NULL);

		if (!pTestStand->TryLock(pFlight))
			continue;
		

		if (IsCurrentOccupiedFlightAdjacencyConflict(pTestStand,pFlight))
			continue;
		
		
		CStandAssignmentRegisterInSim* pRegister = pTestStand->GetStandAssignmentRegister();

		// test stand occupy time conflict
	//	if( pRegister->IsStandIdle( tStart, tEnd ))
		{
			//uncomment these lines when use constraints rules in Dynamic Stand Reassignment
			// test gate assignment constraints conflict
			if( IsGateAssignConstraintsConflict( pTestStand, pFlight))
				continue;
			
			// test adjacency constraints conflict
		/*	if( IsAdjacencyConstraintsConflict( pTestStand, pFlight,tStart, tEnd) )
			{
				continue;
			}*/

			//all test pass, assign stand to flight
			StandAssignRecord _record;
			_record.m_pFlight = pFlight;
			_record.m_tStart = tStart;
			_record.m_tEnd = tEnd;
			_record.m_OpType = _type;

			pRegister->AddRecord(_record);

			//clear old stand assignment
			ALTObjectID standOldID = pFlight->getStand();
			StandInSim * pOldStand = m_pStandResManager->GetStandByName(standOldID);
			if (pOldStand)
			{
				int oldIdx = pOldStand->GetStandAssignmentRegister()->FindRecord(pFlight,_type);
				pOldStand->GetStandAssignmentRegister()->DeleteRecord(oldIdx);
			}

			//lock flight
			pTestStand->GetLock(pFlight);

			return pTestStand;
		}
	}

	return NULL;
}

StandInSim* CDynamicStandAssignmentInSim::StandReassignment( AirsideFlightInSim* pFlight, FLIGHT_PARKINGOP_TYPE _type)
{
	if (m_pTowCriteria == NULL)
		return NULL;

	if (_type == DEP_PARKING)
	{
		return AssignFlightDepartureStand(pFlight);
	}

	std::vector<FlightGate> vFlightGate;
	vFlightGate.assign(m_pflightPriorityInfo->m_vectFlightGate.begin(),m_pflightPriorityInfo->m_vectFlightGate.end());
	int nPriorityCount = vFlightGate.size();

	if (nPriorityCount == 0)		//without define priority, give a default to all priority
	{
		ALTObjectID allStand;
		AirsideFlightType fltType;

		FlightGate _priority;
		_priority.flight = fltType;
		_priority.procID = allStand;
		vFlightGate.push_back(_priority);

		nPriorityCount = 1;
	}


	ElapsedTime tStart = pFlight->GetTime();
	ElapsedTime tEnd = tStart;
	ElapsedTime tDepTime = tStart + (pFlight->GetDepTime() - pFlight->GetArrTime());

	StandInSim* pTestStand = NULL;

	CTowOffStandAssignmentData* pTowData = NULL;

	std::vector<StandInSim*> vFitStands;

	for (int m =0; m < nPriorityCount; m++)
	{
		FlightGate _priority = vFlightGate.at(m) ;

		if (!_priority.flight.IsFit(pFlight->GetFlightInput()))	
			continue;

		if (!_priority.flight.IsArrToDep() && !_priority.flight.IsFit(pFlight->GetFlightInput(),'A'))	//if not arr --> dep
			continue;

		bool bArrAndDepSamePriority = false;
		if (_priority.flight.IsArrToDep() ||  !pFlight->IsDeparture())//arr --> dep, only arr
			bArrAndDepSamePriority = true;
		else
		{
			if (_priority.flight.IsFit(pFlight->GetFlightInput(),'D'))
				bArrAndDepSamePriority = true;
		}

		std::vector<StandInSim*> vPriorityStands;
		m_pStandResManager->GetStandListByStandFamilyName(_priority.procID , vPriorityStands);

		std::map<CString,CTowOffStandAssignmentData*> mapTowDataList;
		m_pTowCriteria->GetFlightTowoffDataList(pFlight->GetFlightInput(), _priority.procID, mapTowDataList);

		std::map<CString,CTowOffStandAssignmentData*>::iterator iter = mapTowDataList.begin();
		for (; iter != mapTowDataList.end(); iter++)
		{
			pTowData = iter->second;
			if (pTowData == NULL)
				continue;

			ALTObjectID standName(iter->first);
			m_pStandResManager->GetStandListByStandFamilyName(standName, vFitStands);

			//erase the stand list from priority stand list
			if (standName == _priority.procID)
				vPriorityStands.clear();
			else
			{
				size_t nCount = vFitStands.size();
				for (size_t idx = 0; idx < nCount; idx++)
				{
					std::remove(vPriorityStands.begin(),vPriorityStands.end(),vFitStands.at(idx));
				}
			}

			if (bArrAndDepSamePriority)		
			{
				if ( pTowData->IsTow() == FALSE)		//not tow
				{
					tEnd = /*pFlight->GetDepTime()*/tDepTime;
					pTestStand = AssignFlightStandWithStandList(pFlight,_type,vFitStands,tStart,tEnd);

					if (pTestStand != NULL)
					{
						if (pFlight->IsDeparture())
							pFlight->SetParkingStand(pTestStand, DEP_PARKING);

						return pTestStand;
					}
				}
				else			//tow
				{
					if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
					{
						tEnd = /*pFlight->GetDepTime()*/tDepTime;
						pTestStand = AssignFlightStandWithStandList(pFlight,_type,vFitStands,tStart,tEnd);

						if (pTestStand != NULL)
						{
							if (pFlight->IsDeparture())
								pFlight->SetParkingStand(pTestStand, DEP_PARKING);

							return pTestStand;
						}
						
						if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
							tEnd = tStart + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
						else
							tEnd = /*pFlight->GetDepTime()*/tDepTime - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

						if (tEnd < tStart || tEnd > /*pFlight->GetDepTime()*/tDepTime)		//error
						{
							ASSERT(0);
							continue;
						}
						
					}
					else			//free up stand
					{
						tEnd = tStart + pTowData->getFltDeplaneTime();

						pTestStand = AssignFlightStandWithStandList(pFlight,_type,vFitStands,tStart,tEnd);

						if (pTestStand != NULL)
						{
							if (pFlight->IsDeparture() && pTowData->GetBackToArrivalStand())
							{
								ElapsedTime tDepStart = pFlight->GetDepTime() - pTowData->getFltEnplaneTime();
								CStandAssignmentRegisterInSim* pRegister = pTestStand->GetStandAssignmentRegister();
								
								if( pRegister->IsStandIdle( tDepStart, pFlight->GetDepTime()))
								{
									StandAssignRecord _record;
									_record.m_pFlight = pFlight;
									_record.m_tStart = tDepStart;
									_record.m_tEnd = pFlight->GetDepTime();
									_record.m_OpType = DEP_PARKING;

									pRegister->AddRecord(_record);
									pFlight->SetParkingStand(pTestStand, DEP_PARKING);
									return pTestStand;
								}
							}
						}
					}
				}				
			}
			else			// not arr --> dep, and arr priority different with dep priority
			{
				if(pTowData->IsTow() == TRUE && pTowData->GetRepositionOrFreeup() == FALSE)		// free up stand
				{
					tEnd = tStart+ ElapsedTime(pTowData->GetDeplanementTime()*60L);
				}
				else			//not tow or reposition for dep
				{
					if (pTowData->IsTow() == TRUE)		//reposition for dep
					{
						if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
							tEnd = tStart + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
						else
							tEnd = /*pFlight->GetDepTime()*/tDepTime - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

						if (tEnd < tStart || tEnd > /*pFlight->GetDepTime()*/tDepTime)		//error
						{
							ASSERT(0);
							continue;
						}
					}
					else				//not tow
						tEnd = tStart + (/*pFlight->GetDepTime()*/tDepTime - tStart)/2L;
				}
			}
			pTestStand = AssignFlightStandWithStandList(pFlight,_type,vFitStands,tStart,tEnd);

			if (pTestStand != NULL)
				return pTestStand;
		}

		if (!vPriorityStands.empty())		//left priority stand assign
		{
			if (bArrAndDepSamePriority)		
				tEnd = /*pFlight->GetDepTime()*/tDepTime;
			else
				tEnd = tStart + (/*pFlight->GetDepTime()*/tDepTime - tStart)/2L;

			pTestStand = AssignFlightStandWithStandList(pFlight,_type,vFitStands,tStart,tEnd);

			if (pTestStand != NULL)
			{
				if (bArrAndDepSamePriority && pFlight->IsDeparture())
					pFlight->SetParkingStand(pTestStand, DEP_PARKING);

				return pTestStand;	
			}
		}
	}

	//bool bPriority = false;
	//int nCount = 0;

	//if (_type == ARR_PARKING)
	//{
	//	bPriority = GetPriorityGate(pFlight, vStandGroup, false);
	//}
	//else
	//{
	//	CTowOffStandAssignmentData* pTowData = pFlight->GetTowOffData();
	//	if(pTowData == NULL || (pTowData && !pTowData->IsTow()))
	//		bPriority = GetPriorityGate(pFlight, vStandGroup, false);
	//	else
	//	{
	//		if (pTowData->GetRepositionOrFreeup())		//reposition for departure, need consider flight current state
	//			bPriority = GetPriorityGate(pFlight, vStandGroup, true);
	//		else								// free up stand 
	//		{
	//			if ((pTowData->GetBackToArrivalStand()))	 //back to arrival stand,need not consider flight current state
	//				bPriority = GetPriorityGate(pFlight, vStandGroup, false);
	//			else								// to different return stands
	//			{
	//				//bPriority = GetPriorityDepartureStands(pFlight, vStandGroup);
	//				bPriority = GetPriorityGate(pFlight, vStandGroup, true);
	//			}
	//		}


	//	}
	//}
	//
	//pTestStand = AssignFlightStandWithStandList(pFlight,_type, vStandGroup);

	//if (pTestStand)
	//	return pTestStand;

	//nCount = vStandGroup.size();

	//for ( int i = 0; i < nCount; i++ )
	//{
	//	if (bPriority)
	//		pTestStand = vStandGroup.at(i);
	//	else
	//		pTestStand = m_pStandResManager->GetStandByIdx(i);

	//	ASSERT(pTestStand != NULL);

	//	if (!pTestStand->TryLock(pFlight))
	//		continue;

	//	ElapsedTime tStart = -1L, tEnd = -1L;
	//	GetFlightPartParkingTime(pTestStand, pFlight, _type, tStart, tEnd);
	//
	//	CStandAssignmentRegisterInSim* pRegister = pTestStand->GetStandAssignmentRegister();

	//	// test stand occupy time conflict
	//	if( pRegister->IsStandIdle( tStart, tEnd ))
	//	{
	//		//uncomment these lines when use constraints rules in Dynamic Stand Reassignment
	//		// test gate assignment constraints conflict
	//		if( IsGateAssignConstraintsConflict( pTestStand, pFlight))
	//			continue;

	//		// test adjacency constraints conflict
	//		if( IsAdjacencyConstraintsConflict( pTestStand, pFlight,tStart, tEnd) )
	//			continue;

	//		//all test pass, assign stand to flight
	//		StandAssignRecord _record;
	//		_record.m_pFlight = pFlight;
	//		_record.m_tStart = tStart;
	//		_record.m_tEnd = tEnd;
	//		_record.m_OpType = _type;

	//		pRegister->AddRecord(_record);

	//		//clear old stand assignment
	//		ALTObjectID standOldID = pFlight->getStand();
	//		StandInSim * pOldStand = m_pStandResManager->GetStandByName(standOldID);
	//		if (pOldStand)
	//		{
	//			int oldIdx = pOldStand->GetStandAssignmentRegister()->FindRecord(pFlight,_type);
	//			pOldStand->GetStandAssignmentRegister()->DeleteRecord(oldIdx);
	//		}

	//		//lock flight
	//		pTestStand->GetLock(pFlight);

	//		return pTestStand;

	//	}



	//}

	return NULL;
}

StandInSim* CDynamicStandAssignmentInSim::AssignFlightDepartureStand( AirsideFlightInSim* pFlight )
{
	ElapsedTime tStart = pFlight->GetTime();
	ElapsedTime tEnd = pFlight->GetDepTime();

	std::vector<FlightGate> vFlightGate;
	vFlightGate.assign(m_pflightPriorityInfo->m_vectFlightGate.begin(),m_pflightPriorityInfo->m_vectFlightGate.end());
	int nPriorityCount = vFlightGate.size();

	if (nPriorityCount == 0)		//without define priority, give a default to all priority
	{
		ALTObjectID allStand;
		AirsideFlightType fltType;

		FlightGate _priority;
		_priority.flight = fltType;
		_priority.procID = allStand;
		vFlightGate.push_back(_priority);

		nPriorityCount = 1;
	}

	for (int m =0; m < nPriorityCount; m++)
	{
		FlightGate _priority = vFlightGate.at(m) ;

		if ( !_priority.flight.IsFit(pFlight->GetFlightInput()))	//if dep priority
			continue;

		std::vector<StandInSim*> vPriorityStands;
		m_pStandResManager->GetStandListByStandFamilyName(_priority.procID , vPriorityStands);

		StandInSim* pTestStand = AssignFlightStandWithStandList(pFlight,DEP_PARKING,vPriorityStands,tStart,tEnd);

		if (pTestStand)
			return pTestStand;
	}

	return NULL;

}

bool CDynamicStandAssignmentInSim::IsGateAssignConstraintsConflict(StandInSim* pStand, AirsideFlightInSim* pFlight)
{
	ALTObjectID nObjID  = pStand->GetStandInput()->GetObjectName();
	const Flight* pInputFlight = pFlight->GetFlightInput();
	
	return !m_pGateAssignConstraints->IsFlightAndGateFit( pInputFlight, nObjID);
}

/* Sky modified for adjacency constrain does not work. use the new idea*/
// use pStand to find record that fit second constrain stand
// if not find, no conflict.
// if find, check the record first constrain
// if first constrain idle: no conflict
// if first constrain occupied by flight: check first constrain whether fit
// fit: check second constrain,fit:no conflicts,or conflict 
// don't fit: conflict

bool CDynamicStandAssignmentInSim::IsAdjacencyConstraintsConflict(StandInSim* pStand, AirsideFlightInSim* pFlight, ElapsedTime tStart, ElapsedTime tEnd)
{
	const Flight* pInputFlight  = pFlight->GetFlightInput();

	int nConNum = m_pGateAssignConstraints->GetStandAdjConstraint().GetAdjConstraintSpec().GetCount();
	ALTObjectID standID = pStand->GetStandInput()->GetObjectName();

	std::vector<ALTObjectID> vStandList;
	for (int i = 0; i < nConNum; i++)
	{
		AdjacencyConstraintSpecDate _Constraint =m_pGateAssignConstraints->GetStandAdjConstraint().GetAdjConstraintSpec().GetItem(i);
		if (!_Constraint.GetAdjacencyStandNameList(standID,vStandList))
		{
			continue;
		}
		for (unsigned j = 0; j < vStandList.size(); j++)
		{
			ALTObjectID adjID = vStandList[j];
			StandInSim* pTestStand = m_pStandResManager->GetStandByName(adjID);

			if(pTestStand == NULL || pTestStand == pStand)
				continue;

			AirsideFlightInSim* pAdjacencyFlightInSim = pTestStand->GetLockedFlight();
			if (pAdjacencyFlightInSim)
			{
				std::vector<ARCFlight*> vAdjFlights;
				vAdjFlights.push_back(pAdjacencyFlightInSim->GetFlightInput());

				InputTerminal* pInTerm = m_pGateAssignConstraints->GetInputTerminal();
				if (!_Constraint.IsFlightFitContraint(pFlight->GetFlightInput(),vAdjFlights,standID,adjID,pInTerm))
					return true;
			}
		}
	}
	return false;
}

void CDynamicStandAssignmentInSim::Init(StandResourceManager* pStandResManager,CTowOffStandAssignmentDataList* pTowCriteria)
{
	m_pStandResManager = pStandResManager;
	m_pTowCriteria = pTowCriteria;

}

void CDynamicStandAssignmentInSim::SetGateAssignmentConstraints(GateAssignmentConstraintList* pConstraints)
{
	ASSERT(pConstraints != NULL);
	m_pGateAssignConstraints = pConstraints;
}

void CDynamicStandAssignmentInSim::SetStandAssignmentPriority(FlightPriorityInfo *flightPriorityInfo)
{
	ASSERT(flightPriorityInfo != NULL);
	m_pflightPriorityInfo = flightPriorityInfo;
}

bool CDynamicStandAssignmentInSim::GetPriorityGate(AirsideFlightInSim* pFlight, std::vector<StandInSim*>& vStandGroup, bool bCheckState)
{
	vStandGroup.clear();
	std::vector<FlightGate> vectFlightGate = m_pflightPriorityInfo->m_vectFlightGate;
	int nFlightCount = vectFlightGate.size();
	if (nFlightCount == 0)
		return false;

	//for (int i =0; i < nFlightCount; i++)
	//{
	//	FlightGate flightGate = vectFlightGate[i];
	//	if (flightGate.flight.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP)
	//	{
	//		if ((bCheckState && !pFlight->IsDepartingOperation()))
	//			continue;

	//		if ( !flightGate.flight.fits(pFlight->GetFlightInput()->getOpLogEntry('D')))
	//			continue;
	//	}
	//	else
	//	{
	//		if (flightGate.flight.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR)
	//		{
	//			if ((bCheckState && !pFlight->IsArrivingOperation()))
	//				continue;

	//			if ( !flightGate.flight.fits(pFlight->GetFlightInput()->getOpLogEntry('A')))
	//				continue;
	//		}

	//		if (!flightGate.flight.fits(pFlight->GetFlightInput()->getOpLogEntry('A')) && !flightGate.flight.fits(pFlight->GetFlightInput()->getOpLogEntry('D')))
	//			continue;
	//	}


	//	ALTObjectID StandFamilyID = flightGate.procID;
	//	int nCount = m_pStandResManager->GetStandCount();
	//	for(int k = 0; k < nCount; k++)
	//	{
	//		StandInSim* pStand = m_pStandResManager->GetStandByIdx(k);
	//		if(pStand->GetStandInput()->GetObjectName().idFits(StandFamilyID))
	//			vStandGroup.push_back(pStand);
	//	}
	//
	//}
	return true;
}


//bool CDynamicStandAssignmentInSim::GetPriorityDepartureStands( AirsideFlightInSim* pFlight, std::vector<StandInSim*>& vStandGroup )
//{
//	CTowOffStandAssignmentData* pTowData = pFlight->GetTowOffData();
//	if (pTowData == NULL || (pTowData && !pTowData->IsTow() || (pTowData->IsTow() && pTowData->GetRepositionOrFreeup())))
//		return false;
//
//	StandInSim* pIntStand = pFlight->GetIntermediateParking();
//	if (pIntStand == NULL)
//		return false;
//
//	ALTObjectID IntName = pIntStand->GetStandInput()->GetObjectName();
//	int nCount = pTowData->GetPriorityCount();
//	for (int i =0; i < nCount; i++)
//	{
//		CTowOffPriorityEntry* pPriority = pTowData->GetPriorityItem(i);
//		if (IntName.idFits(pPriority->GetStandName()))
//		{
//			ALTObjectID StandFamilyID = pPriority->get;
//			int nCount = m_pStandResManager->GetStandCount();
//			for(int k = 0; k < nCount; k++)
//			{
//				StandInSim* pStand = m_pStandResManager->GetStandByIdx(k);
//				if(pStand->GetStandInput()->GetObjectName().idFits(StandFamilyID))
//					vStandGroup.push_back(pStand);
//			}
//		}
//	}
//
//	return true;
//}