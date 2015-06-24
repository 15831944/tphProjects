#include "StdAfx.h"
#include ".\carryonstorageprioritiesinsim.h"
#include "../Common/NewPassengerType.h"



//////////////////////////////////////////////////////////////////////////
FlightCarryonPriorityInSim::FlightCarryonPriorityInSim( CFlightCarryonStoragePriorities *pPriority )
{
	m_pPriority = pPriority;
}

FlightCarryonPriorityInSim::~FlightCarryonPriorityInSim()
{

}

int FlightCarryonPriorityInSim::GetDevicePriority(const CPassengerType& paxType, int nTypeIndex ) const
{
	//get fit passenger type
	if(m_pPriority == NULL)
		return STORAGEDEVICE;

	EnumCarryonType cType = GetCarryOnMapType(nTypeIndex);

	int nCount = m_pPriority->getDataCount();
	
	for(int nItem = 0; nItem < nCount; ++nItem)
	{
		CPaxCarryonStoragePriorities *pDevice = m_pPriority->getData(nItem);
		if(pDevice == NULL)
			continue;

		if(pDevice->getNamedPaxType()->Fits(paxType))
		{
			int nDeviceCount = pDevice->getDataCount();
			for (int nDevice = 0; nDevice < nDeviceCount; ++ nDevice)
			{
				CPaxCarryonStoragePriorityItem* pitem = pDevice->getData(nDevice);
				if(pitem == NULL)
					continue;
				if(pitem->GetCarryonType() == cType)
					return pitem->GetPriority(0);
			}
		}
	}
	return STORAGEDEVICE;
}




//////////////////////////////////////////////////////////////////////////
//
CarryonStoragePrioritiesInSim::CarryonStoragePrioritiesInSim(void)
{

}

CarryonStoragePrioritiesInSim::~CarryonStoragePrioritiesInSim(void)
{

}

void CarryonStoragePrioritiesInSim::Initialize( StringDictionary *pStrDict )
{
	m_carryonPriority.ReadData(pStrDict);
}

CFlightCarryonStoragePriorities* CarryonStoragePrioritiesInSim::GetDevicePriority( int nOnboardFltID )
{
	CFlightCarryonStoragePriorities* pPriority = m_carryonPriority.getFlightDataByID(nOnboardFltID);
	return pPriority;
}