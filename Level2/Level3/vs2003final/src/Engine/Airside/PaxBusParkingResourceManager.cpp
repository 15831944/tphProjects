#include "StdAfx.h"
#include ".\paxbusparkingresourcemanager.h"
#include "../../InputAirside/PaxBusParking.h"

CPaxBusParkingResourceManager::CPaxBusParkingResourceManager(void)
{
}

CPaxBusParkingResourceManager::~CPaxBusParkingResourceManager(void)
{
	//std::vector<CPaxBusParkingInSim *>::iterator iter = m_vParkingInSim.begin();
	//for (;iter != m_vParkingInSim.end(); ++iter)
	//{
	//	delete *iter;
	//}
}

void CPaxBusParkingResourceManager::initialize(int nProjID,int nAirportID)
{
	m_paxParkingList.CleanData();
	m_paxParkingList.ReadData(nAirportID);

	//init

	size_t nCount = m_paxParkingList.GetItemCount();
	for (size_t i =0; i < nCount;++ i)
	{
		CPaxBusParking * pParking = m_paxParkingList.GetItemByIndex(i);

		CPaxBusParkingInSim *paxBusParkingInSim = new CPaxBusParkingInSim(pParking);

		m_vParkingInSim.push_back(paxBusParkingInSim);

	} 
}
size_t CPaxBusParkingResourceManager::GetPaxBusParkingCount()
{
	return m_vParkingInSim.size();
}
CPaxBusParkingInSim *CPaxBusParkingResourceManager::GetPaxBusParkingByIndex(size_t nIndex)
{	
	if(nIndex < m_vParkingInSim.size())
		return m_vParkingInSim.at(nIndex).get();

	return NULL;
}
CPaxBusParkingInSim *CPaxBusParkingResourceManager::GetBestMatch(const CString& strGateID,Gatetype gateType)
{
	ALTObjectID gateID(strGateID);
	size_t nCount =  m_vParkingInSim.size();
	for (size_t i = 0; i < nCount; ++ i )
	{
		 CPaxBusParkingInSim *paxBusParkingInSim = m_vParkingInSim.at(i).get();
		if (paxBusParkingInSim->GetPaxParkingInput()->GetGatetype() == gateType )
		{
			CString strCurGate = paxBusParkingInSim->GetPaxParkingInput()->GetGate();
			
			ALTObjectID paxBusGateID(strCurGate);

			if(gateID.idFits(paxBusGateID) == TRUE || strCurGate.CompareNoCase(_T("DEFAULT")) ==0 )
			{
				return paxBusParkingInSim;
			}
		}
	}

	return NULL;
	

}