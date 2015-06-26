#include "stdafx.h"
#include "StandResourceManager.h"
#include "TaxiwayResourceManager.h"
#include "../../InputAirside/ALTAirport.h"
#include "../../Common/GetIntersection.h"
#include <inputAirside\ALTObjectGroup.h>
#include "../../Results/AirsideFlightLogItem.h"
#include "../../Results/OutputAirside.h"

bool StandResourceManager::Init( int nPrjID, int nAirportID,OutputAirside *pOutput )
{
	ALTObjectList vStands;
	ALTAirport::GetStandList(nAirportID,vStands);

	for(int i = 0;i< (int)vStands.size(); ++i)
	{
		Stand * pStand =(Stand*) vStands.at(i).get();
		
		if (pStand->GetType() == ALT_STAND)
		{
			AirsideALTObjectLogItem objectItem;
			objectItem.m_altObject = pStand->getName();
			objectItem.m_emType = ALT_STAND;
			pOutput->m_reportLog.AddAltObjectLogItem(objectItem);
			m_vStands.push_back( new StandInSim(pStand) );
		}
		else
		{
			m_vStands.push_back( new DeiceStandInSim(pStand) );
		}
	}
	return true;
}
bool StandResourceManager::InitStandCriteriaAssignment( int nPrjID,CAirportDatabase* pAirPortdb)
{
	m_StandCriteriaAssignmentList.SetAirportDatabase(pAirPortdb);
	m_StandCriteriaAssignmentList.ReadData(nPrjID);

	for(int i = 0;i< (int)m_vStands.size(); ++i)
	{
		StandInSim* pStandInput =(StandInSim*) m_vStands.at(i).get();
		StandCriteriaAssignment* pStandCriteriaAssignment = NULL;
		pStandCriteriaAssignment = m_StandCriteriaAssignmentList.fit(pStandInput->GetStandInput());
		if (pStandCriteriaAssignment)
		{
			pStandInput->SetFlightTypeOnEachStand(pStandCriteriaAssignment);
		}
	}
	return true;
}


StandInSim * StandResourceManager::GetStandByID( int id )
{
	for(int i=0;i<GetStandCount();i++)
	{
		StandInSim * pStand = GetStandByIdx(i);
		if(pStand->GetID() == id)return pStand;
	}
	return NULL;
}

StandInSim * StandResourceManager::GetStandByName( const ALTObjectID& nameID )
{
	for(int i=0;i<GetStandCount();i++)
	{
		StandInSim * pStand = GetStandByIdx(i);
		if(pStand->GetStandInput()->GetObjectName().GetIDString() == nameID.GetIDString())return pStand;
	}
	return NULL;
}

bool StandResourceManager::InitRelations( IntersectionNodeInSimList& nodeList,TaxiwayResourceManager * pTaxiRes )
{	
	
	for(int i =0 ;i< GetStandCount();i++)
	{
		StandInSim * pStand = GetStandByIdx(i);
		pStand->Init(nodeList);		
	}
	return true;
}

ElapsedTime StandResourceManager::GetNextNeedTime( StandInSim* pStand, const ElapsedTime& curTime )
{
	return ElapsedTime::Max();
}

StandInSim* StandResourceManager::GetStandNeedNotWithin( const ALTObjectGroup& standGrp, const ElapsedTime& curTime, const ElapsedTime& withInTime )
{
	int nCount = GetStandCount();
	for(int i=0;i< nCount;i++)
	{
		StandInSim* pStand = GetStandByIdx(i);
		if( pStand->GetStandInput()->GetObjectName().idFits( standGrp.getName()) )
		{
			ElapsedTime stand_avlwithinTime = GetNextNeedTime(pStand, curTime) - curTime;  //stand avaible within time
			if(stand_avlwithinTime < withInTime )
				continue;
			else
			{
				return pStand;
			}
		}
	}
	return NULL;
}

void StandResourceManager::GetStandListByStandFamilyName( const ALTObjectID& nameID,std::vector<StandInSim*>& vStands )
{
	int nCount = GetStandCount();
	for(int i=0;i<nCount;i++)
	{
		StandInSim* pStand = m_vStands.at(i).get();
		if(pStand->GetStandInput()->GetObjectName().idFits(nameID))
		{
			vStands.push_back(pStand);
		}
			
	}
}