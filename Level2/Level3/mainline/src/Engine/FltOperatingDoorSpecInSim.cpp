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