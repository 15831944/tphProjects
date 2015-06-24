#include "StdAfx.h"
#include ".\temporaryparkinginsim.h"
#include "AirportResourceManager.h"
#include "InputAirside/TemporaryParkingCriteria.h"

#include "TaxiwayResource.h"
#include "AirsideFlightInSim.h"
#include "TempParkingSegmentsInSim.h"


TemporaryParkingInSim::TemporaryParkingInSim(void)
{
	m_pTemporaryParkingList = new CTemporaryParkingCriteriaList;
	m_vTempParkings.clear();
}

TemporaryParkingInSim::~TemporaryParkingInSim(void)
{
	delete m_pTemporaryParkingList;
	m_pTemporaryParkingList = NULL;
}

void TemporaryParkingInSim::Init(int nPrjID,AirportResourceManager* pResManager )
{
	m_pTemporaryParkingList->ReadData(nPrjID);

	int nCount = m_pTemporaryParkingList->GetItemCount();
	for (int i =0; i < nCount; i++)
	{ 
		CTemporaryParkingCriteria* pCritera = m_pTemporaryParkingList->GetItem(i);
		TaxiwayInSim* pTaxiway = pResManager->getTaxiwayResource()->GetTaxiwayByID(pCritera->GetTaxiwayID()) ;
		if(!pTaxiway)
			continue;
		for ( int j =0; j < pCritera->GetItemCount(); j++ )
		{
			CTemporaryParkingCriteriaData* pData = pCritera->GetItem(j);
			int nIdx1 = pData->GetStartID();
			int nIdx2 = pData->GetEndID();
			int nEntryId = pData->GetEntryID();

			TempParkingSegmentsInSim* pTempParking = new TempParkingSegmentsInSim;
			pTempParking->InitTempParking(pTaxiway,nIdx1,nIdx2,nEntryId,pResManager,pData->GetOrientationType() == Angled);
			m_vTempParkings.push_back(pTempParking);

		}
	}
}

TempParkingSegmentsInSim* TemporaryParkingInSim::GetTemporaryParkingTaxiway(TaxiwayInSim* pTaxiway,AirsideFlightInSim* pFlight)
{
	for (int i =0; i < (int)m_vTempParkings.size(); i++)
	{
		if ( m_vTempParkings[i]->GetParkingTaxiway() == pTaxiway)
		{
			if(m_vTempParkings[i]->TryLock(pFlight))
			{
				m_vTempParkings[i]->GetLock(pFlight);
				return m_vTempParkings[i].get();
			}
			else
				return NULL;				
		}
		
	}
	return NULL;
}

TempParkingSegmentsInSim* TemporaryParkingInSim::GetTemporaryParkingTaxiway(AirsideFlightInSim* pFlight)
{
	int nCount = GetParkingCount();
	if (nCount <= 0)
		return NULL;
	for (int i = 0; i < nCount; i++)
	{
		TempParkingSegmentsInSim* pSeg = GetParkingSeg(i);
		if (pSeg->TryLock(pFlight))
		{
			pSeg->GetLock(pFlight);
			return pSeg;
		}
	}
	return NULL;
}