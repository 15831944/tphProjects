#include "StdAfx.h"
#include ".\fltoperatingdoorspecinsim.h"
#include "../Inputs/OperatingDoorSpec.h"
#include "../Common/AirportDatabase.h"
#include "../Common/ALTObjectID.h"
#include "../Common/ACTypeDoor.h"
#include "../Common/FLT_CNST.H"
#include <algorithm>

FltOperatingDoorSpecInSim::FltOperatingDoorSpecInSim(CAirportDatabase* pAirportDB)
:m_pAirportDB(pAirportDB)
{
	m_pOperatingDoorSpec = new OperatingDoorSpec;
	m_pOperatingDoorSpec->SetAirportDatabase(pAirportDB);
	m_pOperatingDoorSpec->ReadData();
}

FltOperatingDoorSpecInSim::~FltOperatingDoorSpecInSim(void)
{
}

void FltOperatingDoorSpecInSim::getFlightOpDoors( const FlightConstraint& fltcont,const ALTObjectID& standName,std::vector<ACTypeDoor*>& vOpenDoors )
{
	std::vector<int> vDoorIds;
	int nCount = m_pOperatingDoorSpec->GetFltDataCount();
	for (int i =0; i < nCount; i++)
	{
		FltOperatingDoorData* pDoorData = m_pOperatingDoorSpec->GetFltDoorData(i);
		if(!pDoorData->GetFltType().fits(fltcont))
			continue;

		int nStandCount = pDoorData->GetStandDoorCount();
		for (int j =0; j < nStandCount; j++)
		{
			StandOperatingDoorData* pStandData = pDoorData->GetStandDoorData(j);
			if (standName.idFits(pStandData->GetStandName()))
			{
				pStandData->GetOpDoors(vDoorIds);

				char szbuffer[1024] = {0};
				fltcont.getACType(szbuffer);

				ACTYPEDOORLIST* pDoors =m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(szbuffer);
				if (pDoors == NULL)	
					return;

				int nSize = pDoors->size();
				for (int idx =0; idx < nSize; idx++)
				{
					ACTypeDoor* pDoor = pDoors->at(idx);
					if (std::find(vDoorIds.begin(),vDoorIds.end(),pDoor->GetID()) != vDoorIds.end())
						vOpenDoors.push_back(pDoor);
				}
				return;
			
			}
		}
	}

	char szbuffer[1024] = {0};
	fltcont.getACType(szbuffer);

	ACTYPEDOORLIST* pDoors =m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(szbuffer);
	if (pDoors == NULL ||(pDoors && pDoors->empty()))	
		return;
	
	int nSize = pDoors->size();
	for (int idx =0; idx < nSize; idx++)
	{
		ACTypeDoor* pDoor = pDoors->at(idx);
		if (pDoor->m_enumDoorDir == ACTypeDoor::RightHand)
			continue;

		vOpenDoors.push_back(pDoor);
	}
}

std::vector<ACTypeDoorOpen> FltOperatingDoorSpecInSim::getFlightDoors( const FlightConstraint& fltcont,const ALTObjectID& standName )const
{
	std::vector<ACTypeDoorOpen> vOpenDoors;
	
	char szbuffer[1024] = {0};
	fltcont.getACType(szbuffer);

	ACTYPEDOORLIST* pDoors =m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(szbuffer);
	if (pDoors == NULL ||(pDoors && pDoors->empty()))	
		return vOpenDoors;
	
	ACTYPEDOORLIST sortDorrs = *pDoors;
	std::sort(sortDorrs.begin(),sortDorrs.end(), OpenDoorsOrder());

	int nSize = sortDorrs.size();
	for (int idx =0; idx < nSize; idx++)
	{
		ACTypeDoor* pDoor = sortDorrs.at(idx);

		ACTypeDoorOpen door;
		door.pDoor = pDoor;
		door.bOpen = TRUE;
		vOpenDoors.push_back(door);
	}
	//
	StandOperatingDoorData* pStandData = _getFitData(fltcont,standName);
	if(!pStandData)
		return vOpenDoors;
	std::vector<int> vDoorIds;
	pStandData->GetOpDoors(vDoorIds);
	
	for(size_t i=0;i<vOpenDoors.size();i++)
	{
		ACTypeDoorOpen& door = vOpenDoors[i];
		if(std::find(vDoorIds.begin(),vDoorIds.end(),door.pDoor->GetID()) == vDoorIds.end())
		{
			door.bOpen  = FALSE;
		}		
	}

	return vOpenDoors;
}

StandOperatingDoorData* FltOperatingDoorSpecInSim::_getFitData( const FlightConstraint& fltcont,const ALTObjectID& standName )const
{
	int nCount = m_pOperatingDoorSpec->GetFltDataCount();
	for (int i =0; i < nCount; i++)
	{
		FltOperatingDoorData* pDoorData = m_pOperatingDoorSpec->GetFltDoorData(i);
		if(!pDoorData->GetFltType().fits(fltcont))
			continue;

		int nStandCount = pDoorData->GetStandDoorCount();
		for (int j =0; j < nStandCount; j++)
		{
			StandOperatingDoorData* pStandData = pDoorData->GetStandDoorData(j);
			if (standName.idFits(pStandData->GetStandName()))
			{
				return pStandData;
			}
		}
	}
	return NULL;
}

bool OpenDoorsOrder::operator()( ACTypeDoor* door1, ACTypeDoor*door2 )
{
	return door1->m_dNoseDist < door2->m_dNoseDist;
}
