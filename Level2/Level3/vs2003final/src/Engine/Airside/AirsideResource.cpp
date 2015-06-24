#include "StdAfx.h"
#include ".\airsideresource.h"
#include "AirsideFlightInSim.h"
#include "AirsideVehicleInSim.h"

#include <algorithm>
#include <limits>


OccupancyInstance& AirsideResource::GetAddOccupyInstance( CAirsideMobileElement* pFlight )
{
	for(OccupancyTable::iterator itr = m_vOccupancyTable.begin();itr!=m_vOccupancyTable.end();itr++)
	{
		if(itr->m_pMobileElement == pFlight) return *itr;
	}	
	m_vOccupancyTable.push_back(OccupancyInstance(pFlight));
	return *m_vOccupancyTable.rbegin();
}

AirsideFlightInSim * AirsideResource::GetLastInResourceFlight(void)
{

	for(OccupancyTable::reverse_iterator itr = m_vOccupancyTable.rbegin();itr!=m_vOccupancyTable.rend();itr++)
	{
		if(  itr->IsValid() && !itr->IsExitTimeValid() )
		{
			return reinterpret_cast<AirsideFlightInSim *>(itr->m_pMobileElement);
		}
	}
	return NULL;
}

AirsideVehicleInSim * AirsideResource::GetLastInResourceVehicle(void)
{

	for(OccupancyTable::reverse_iterator itr = m_vOccupancyTable.rbegin();itr!=m_vOccupancyTable.rend();itr++)
	{
		if(  itr->IsValid() && !itr->IsExitTimeValid() )
		{
			return reinterpret_cast<AirsideVehicleInSim *>(itr->m_pMobileElement);
		}
	}
	return NULL;
}
void AirsideResource::SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode)
{
	OccupancyInstance& ocyInstance  = GetAddOccupyInstance(pFlight);
	ocyInstance.m_tEntry = enterT;
	ocyInstance.m_tExit = ElapsedTime(-1L);
	ocyInstance.m_ocyType = (AirsideMobileElementMode)fltMode;
	std::sort(m_vOccupancyTable.begin(),m_vOccupancyTable.end());
}

void AirsideResource::SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode,double dSpd)
{
	OccupancyInstance& ocyInstance  = GetAddOccupyInstance(pFlight);
	ocyInstance.m_tEntry = enterT;
	ocyInstance.m_tExit = ElapsedTime(-1L);
	ocyInstance.m_ocyType = (AirsideMobileElementMode)fltMode;
	ocyInstance.m_dSpd = dSpd;
	std::sort(m_vOccupancyTable.begin(),m_vOccupancyTable.end());
}
void AirsideResource::SetExitTime( CAirsideMobileElement * pFlight, const ElapsedTime& exitT )
{
	OccupancyInstance& ocyInstance  = GetAddOccupyInstance(pFlight);
	ocyInstance.m_tExit = exitT;
	if(!ocyInstance.IsEnterTimeValid())
		ocyInstance.m_tEntry = exitT;
}

OccupancyInstance AirsideResource::GetLastOccupyInstance() const
{
	if(!m_vOccupancyTable.empty()) return m_vOccupancyTable.back();
	OccupancyInstance ret;
	return ret;
}
OccupancyInstance AirsideResource::GetFirstOccupyInstance() const
{
	if(!m_vOccupancyTable.empty()) 
		return *m_vOccupancyTable.begin();
	
	OccupancyInstance ret;
	return ret;
}
OccupancyInstance AirsideResource::GetPreviousMobileElmentOccupancy( CAirsideMobileElement* pFlight ) const
{
	OccupancyTable::const_iterator preFlightItr = m_vOccupancyTable.begin();
	OccupancyTable::const_iterator thisFlightItr = preFlightItr;

	for( thisFlightItr = m_vOccupancyTable.begin();thisFlightItr!=m_vOccupancyTable.end();thisFlightItr++)
	{
		if(thisFlightItr->m_pMobileElement == pFlight ) break;
		preFlightItr = thisFlightItr;
	}

	if( (preFlightItr != thisFlightItr)&&(preFlightItr!= m_vOccupancyTable.end()) )
		return *preFlightItr;
	
	return OccupancyInstance(NULL);
}
std::vector<CAirsideMobileElement *> AirsideResource::GetInResouceMobileElement()
{
	std::vector<CAirsideMobileElement *> vOccupancyElement;

	OccupancyTable::const_iterator const_iter = m_vOccupancyTable.begin();

	for (;const_iter != m_vOccupancyTable.end(); ++const_iter)
	{
		if ((*const_iter).IsExitTimeValid() == false)
		{
			vOccupancyElement.push_back((*const_iter).GetElement());
		}

	}
	return	vOccupancyElement;

}

OccupancyInstance AirsideResource::GetOccupyInstance( CAirsideMobileElement * pMobile ) const
{
	for(OccupancyTable::const_iterator itr = m_vOccupancyTable.begin();itr!=m_vOccupancyTable.end();itr++)
	{
		if(itr->m_pMobileElement == pMobile) return *itr;
	}	
	return OccupancyInstance(NULL);
}

AirsideFlightInSim * AirsideResource::GetFlightAhead( AirsideFlightInSim *pFlight )
{
	std::vector<AirsideFlightInSim * > vInResourceFlight;
	for(OccupancyTable::const_iterator itr = m_vOccupancyTable.begin();itr!=m_vOccupancyTable.end();itr++)
	{
		if( itr->m_pMobileElement->GetType() == CAirsideMobileElement::AIRSIDE_FLIGHT && itr->IsValid() && !itr->IsExitTimeValid() )
		{
			vInResourceFlight.push_back((AirsideFlightInSim*)itr->m_pMobileElement);
		}
	}

	DistanceUnit pFlightDistInRes;
	if( vInResourceFlight.end() != std::find(vInResourceFlight.begin(),vInResourceFlight.end(),pFlight) )// no in resource
	{
		pFlightDistInRes = pFlight->GetDistInResource();
	}else
	{
		pFlightDistInRes = -1;
	}

	DistanceUnit minDist = (std::numeric_limits<DistanceUnit>::max)() * 0.5;
	AirsideFlightInSim * pPreFlight = NULL;
	for(int i=0;i<(int)vInResourceFlight.size();i++)
	{
		DistanceUnit  sepDist = vInResourceFlight.at(i)->GetDistInResource() - pFlightDistInRes;
		if(sepDist > 0 && sepDist <= minDist)
		{
			minDist = sepDist;
			pPreFlight = vInResourceFlight.at(i);
		}

	}
	return pPreFlight;	
}
//remove occupancy information of the given mobile element
void AirsideResource::RemoveElementOccupancyInfo( CAirsideMobileElement * pMobile )
{
	for(OccupancyTable::iterator iter = m_vOccupancyTable.begin();iter!=m_vOccupancyTable.end();iter++)
	{
		if(iter->m_pMobileElement == pMobile )
		{
			m_vOccupancyTable.erase(iter);
			break;
		}
	}
}

//get the flights after given flight
std::vector<AirsideFlightInSim *> AirsideResource::GetFlightsAfterFlight( AirsideFlightInSim *pFlight )
{	
	std::vector<AirsideFlightInSim *> vFlights;
	if (pFlight == NULL)
		return vFlights;
	

	BOOL bAfter = FALSE;
	for(OccupancyTable::iterator iter = m_vOccupancyTable.begin();iter!=m_vOccupancyTable.end();iter++)
	{
		if(bAfter)
		{
			vFlights.push_back((AirsideFlightInSim *)(iter->m_pMobileElement));
		}

		if(iter->m_pMobileElement == pFlight )
		{
			bAfter = TRUE;
		}


	}

	return vFlights;
}

bool AirsideResource::IsStandResource() const
{
	return GetType()==ResType_Stand||GetType()==ResType_StandLeadInLine||GetType()==ResType_StandLeadOutLine;
}































