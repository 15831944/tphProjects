#include "StdAfx.h"
#include ".\bagcartsparkingspotresourcemanager.h"
#include "BaggageCartParkingLinkageInSimManager.h"


CBagCartsParkingSpotResourceManager::CBagCartsParkingSpotResourceManager(void)
{
	m_pLinkageManager = new BaggageCartParkingLinkageInSimManager;
}

CBagCartsParkingSpotResourceManager::~CBagCartsParkingSpotResourceManager(void)
{
	delete m_pLinkageManager;
	m_pLinkageManager = NULL;
}


void CBagCartsParkingSpotResourceManager::initialize(int nProjID,int nAirportID)
{
	m_lstBagSpot.CleanData();
	m_lstBagSpot.ReadData(nAirportID);

	//init

	size_t nCount = m_lstBagSpot.GetItemCount();
	for (size_t i =0; i < nCount;++ i)
	{
		AirsideBagCartParkSpot * pParking = m_lstBagSpot.GetItemByIndex(i);

		CBagCartsParkingSpotInSim *pBagParkingSpotInSim = new CBagCartsParkingSpotInSim(pParking);

		m_vBagSpotInSim.push_back(pBagParkingSpotInSim);

	} 
	if(m_pLinkageManager)
		m_pLinkageManager->Initialize(nAirportID, m_vBagSpotInSim);
}
size_t CBagCartsParkingSpotResourceManager::GetSpotCount()
{
	return m_vBagSpotInSim.size();
}
CBagCartsParkingSpotInSim *CBagCartsParkingSpotResourceManager::GetSpotByIndex(size_t nIndex)
{	
	if(nIndex < m_vBagSpotInSim.size())
		return m_vBagSpotInSim.at(nIndex).get();

	return NULL;
}
CBagCartsParkingSpotInSim *CBagCartsParkingSpotResourceManager::GetBestMatch()
{
	//ALTObjectID gateID(strGateID);
	//size_t nCount =  m_vBagSpotInSim.size();
	//for (size_t i = 0; i < nCount; ++ i )
	//{
	//	CPaxBusParkingInSim *paxBusParkingInSim = m_vBagSpotInSim.at(i).get();
	//	if (paxBusParkingInSim->GetPaxParkingInput()->GetGatetype() == gateType )
	//	{
	//		CString strCurGate = paxBusParkingInSim->GetPaxParkingInput()->GetGate();

	//		ALTObjectID paxBusGateID(strCurGate);

	//		if(gateID.idFits(paxBusGateID) == TRUE || strCurGate.CompareNoCase(_T("DEFAULT")) ==0 )
	//		{
	//			return paxBusParkingInSim;
	//		}
	//	}
	//}

	return NULL;


}

CBagCartsParkingSpotInSim * CBagCartsParkingSpotResourceManager::GetSpotLinked( ALTObjectID terminalSpot )
{
	return m_pLinkageManager->GetSpotLinked(terminalSpot );

}
