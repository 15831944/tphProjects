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

//void FltOperatingDoorSpecInSim::getFlightOpDoors( const FlightConstraint& fltcont,const ALTObjectID& standName,std::vector<ACTypeDoor*>& vOpenDoors )
//{
//	//std::vector<int> vDoorIds;
//	int nCount = m_pOperatingDoorSpec->GetFltDataCount();
//	for (int i =0; i < nCount; i++)
//	{
//		FltOperatingDoorData* pDoorData = m_pOperatingDoorSpec->GetFltDoorData(i);
//		if(!pDoorData->GetFltType().fits(fltcont))
//			continue;
//
//		int nStandCount = pDoorData->GetStandDoorCount();
//		for (int j =0; j < nStandCount; j++)
//		{
//			StandOperatingDoorData* pStandData = pDoorData->GetStandDoorData(j);
//			if (standName.idFits(pStandData->GetStandName()))
//			{
//		//		pStandData->GetOpDoors(vDoorIds);
//
//				char szbuffer[1024] = {0};
//				fltcont.getACType(szbuffer);
//
//				ACTYPEDOORLIST* pDoors =m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(szbuffer);
//				if (pDoors == NULL)	
//					return;
//
//				int nSize = pDoors->size();
//				for (int idx =0; idx < nSize; idx++)
//				{
//					ACTypeDoor* pDoor = pDoors->at(idx);
//					if (std::find(vDoorIds.begin(),vDoorIds.end(),pDoor->GetID()) != vDoorIds.end())
//						vOpenDoors.push_back(pDoor);
//				}
//				return;
//			
//			}
//		}
//	}
//
//	char szbuffer[1024] = {0};
//	fltcont.getACType(szbuffer);
//
//	ACTYPEDOORLIST* pDoors =m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(szbuffer);
//	if (pDoors == NULL ||(pDoors && pDoors->empty()))	
//		return;
//	
//	int nSize = pDoors->size();
//	for (int idx =0; idx < nSize; idx++)
//	{
//		ACTypeDoor* pDoor = pDoors->at(idx);
//		if (pDoor->m_enumDoorDir == ACTypeDoor::RightHand)
//			continue;
//
//		vOpenDoors.push_back(pDoor);
//	}
//}

std::vector<ACTypeDoorOpen> FltOperatingDoorSpecInSim::getFlightOpenDoors( const FlightConstraint& fltcont,const ALTObjectID& standName )const
{
	std::vector<ACTypeDoorOpen> vDefaultOpenDoors = getAllFlightDoors(fltcont);

	StandOperatingDoorData* pStandData = _getFitData(fltcont,standName);
	if(!pStandData)
		return vDefaultOpenDoors;

	//find open entry 
	std::vector<ACTypeDoorOpen> vOpenDoorsFilterd;
    std::vector<StandOperatingDoorData::OperationDoor>* pDoorOpList = pStandData->GetDoorOpList();
	for(size_t i=0;i<pDoorOpList->size();i++)
	{
		StandOperatingDoorData::OperationDoor& doorOp = pDoorOpList->at(i);
		for(size_t j=0;j<vDefaultOpenDoors.size();++j)
		{
			ACTypeDoorOpen& defaultDoor = vDefaultOpenDoors[j];
			if(defaultDoor.pDoor->GetDoorName()==doorOp.GetDoorName() && 
                defaultDoor.openSide==doorOp.GetHandType())
			{
				vOpenDoorsFilterd.push_back(defaultDoor);
			}
		}
	}
	return vOpenDoorsFilterd;
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

std::vector<ACTypeDoorOpen> FltOperatingDoorSpecInSim::getAllFlightDoors( const FlightConstraint& fltcont ) const
{
	std::vector<ACTypeDoorOpen> vDefaultOpenDoors;
	char szbuffer[1024] = {0};
	fltcont.getACType(szbuffer);

	ACTYPEDOORLIST* pDoors =m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(szbuffer);
	if (pDoors == NULL ||(pDoors && pDoors->empty()))	
		return vDefaultOpenDoors;

	ACTYPEDOORLIST sortDorrs = *pDoors;
	std::sort(sortDorrs.begin(),sortDorrs.end(), OpenDoorsOrder());

	//default open all doors
	int RIndex =0 ;
	int LIndex =0;
	int nSize = sortDorrs.size();
	for (int idx =0; idx < nSize; idx++)
	{
		ACTypeDoor* pDoor = sortDorrs.at(idx);
		ACTypeDoor::DoorDir doortype = pDoor->m_enumDoorDir;

		if(doortype == ACTypeDoor::BothSide || doortype == ACTypeDoor::RightHand)
		{
			ACTypeDoorOpen door;
			door.pDoor = pDoor; 
			door.openSide = ACTypeDoor::RightHand;
			door.index = ++RIndex;
			vDefaultOpenDoors.push_back(door);
		}

		if(doortype == ACTypeDoor::BothSide || doortype == ACTypeDoor::LeftHand)
		{
			ACTypeDoorOpen door;
			door.pDoor = pDoor; 
			door.openSide = ACTypeDoor::LeftHand;
			door.index = ++LIndex;
			vDefaultOpenDoors.push_back(door);
		}		
	}
	std::sort(vDefaultOpenDoors.begin(),vDefaultOpenDoors.end());
	return vDefaultOpenDoors;
}

bool OpenDoorsOrder::operator()( ACTypeDoor* door1, ACTypeDoor*door2 )
{
	return door1->m_dNoseDist < door2->m_dNoseDist;
}

bool ACTypeDoorOpen::operator<( const ACTypeDoorOpen& other ) const
{
	if(other.openSide==openSide)
	{
		return index<other.index;
	}
	return ( openSide==ACTypeDoor::LeftHand);
}
