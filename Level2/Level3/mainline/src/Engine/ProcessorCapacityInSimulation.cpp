#include "StdAfx.h"
#include "ProcessorCapacityInSimulation.h"

#include <algorithm>

#include "Mobile.h"
#include "person.h"

#include "ProcessorCapacity.h"

#include "../inputs/miscproc.h"
#include "../Inputs/CapacityAttributes.h"
#include "terminal.h"

ProcessorCapacityInSimulation::ProcessorCapacityInSimulation()
{
	m_nAllNonPaxCapacity = 0;
}

ProcessorCapacityInSimulation::~ProcessorCapacityInSimulation(void)
{
	for(std::vector<ProcessorCombinationCapactity *>::iterator iter =  m_vCombinationCapacity.begin();
		iter != m_vCombinationCapacity.end(); ++ iter)
		delete *iter;

	m_vCombinationCapacity.clear();
}

void ProcessorCapacityInSimulation::Initialization(Terminal *pTernimal,ProcessorCapacity* pProcCapacity)
{
	Clear();

	for (int i=0; i< pProcCapacity->GetCount(); i++)
	{
		MobileElementCapacityItem mobCapityItem = pProcCapacity->GetItem(i);
		//TY_Normal,TY_AllNO_PAX,TY_Combination
		if(mobCapityItem.m_Type == CapacityAttributes::TY_Normal)
		{
			m_mapMobElementCapacity.insert(std::make_pair(
				mobCapityItem.m_nMobElementTypeIndex,
				mobCapityItem.m_nCapacity
				));
		}
		else if(mobCapityItem.m_Type == CapacityAttributes::TY_AllNO_PAX)
		{
			m_nAllNonPaxCapacity += mobCapityItem.m_nCapacity;
		}
		else if(mobCapityItem.m_Type == CapacityAttributes::TY_Combination)
		{
			if(pTernimal == NULL)
				continue;

			ProcessorCombinationCapactity *pCombCapacity = new ProcessorCombinationCapactity;
			pCombCapacity->m_nCapacity = mobCapityItem.m_nCapacity;
			m_vCombinationCapacity.push_back(pCombCapacity);
			for(int nItem =0; nItem < static_cast<int>(mobCapityItem.m_ContainData.size()); ++ nItem )
			{
				CString strMobType = mobCapityItem.m_ContainData[nItem];
				int nIndex = pTernimal->m_pMobElemTypeStrDB->GetIndex(strMobType);
				if(nIndex >= 0)
				{
					pCombCapacity->m_vMobType.push_back(nIndex);
				}
			}
		}
	}
	//add all non-pax to the ProcessorCombinationCapactity

	if(pTernimal != NULL && m_nAllNonPaxCapacity > 0 )
	{
		ProcessorCombinationCapactity *pCombCapacity = new ProcessorCombinationCapactity;
		pCombCapacity->m_nCapacity = m_nAllNonPaxCapacity;
		m_vCombinationCapacity.push_back(pCombCapacity);
		for(int nItem = 0; nItem < pTernimal->m_pMobElemTypeStrDB->GetCount(); ++ nItem )
		{
			
			//int nIndex = pTernimal->m_pMobElemTypeStrDB->GetIndex(strMobType);
			if(nItem > 0)
			{
				pCombCapacity->m_vMobType.push_back(nItem);
			}
		}
	}


}
/*
processor can serve person when processor can serve person's family

compare occupants state to identify if processor can serve current person:
eg:
Processor's Capacity:          Occupied:			Query person will Increased:
Passenger:	2				   Passenger: 1			Passenger: 1
Visitor:	3				   Visitor:   2			HandBag:   4

*/

bool ProcessorCapacityInSimulation::IsVacant(MobileElement* pMobElement,
											 ProcessorOccupantsState& currentProcessorOccupants, 
											 ProcessorOccupantsState& queristIncreasedOccupants)const
{
	ProcessorOccupantsState::Occupants& queristOcc = queristIncreasedOccupants.GetOccupants();

	bool bRetDirectly = true;
	//if processor does not has capacity attributes matched 
	//querist's each family member's type, return directly
	for (ProcessorOccupantsState::OccupantsIter iter = queristOcc.begin(); 
		iter != queristOcc.end(); iter++)
	{
		CapacitiesMapIter it = m_mapMobElementCapacity.find(iter->first);
		if (it != m_mapMobElementCapacity.end())
		{
			bRetDirectly = false;
			break;
		}
	}
	//has combination setting
	if(m_vCombinationCapacity.size() > 0)
		bRetDirectly = false;

	////has all non-pax setting
	//if(m_nAllNonPaxCapacity > 0)
	//	bRetDirectly = false;

	if (true == bRetDirectly)
		return true;

	//convert mobile element to Person
	Person *pPerson = (Person *)pMobElement;
	if(pPerson == NULL)
		return true;
	
	//if the 
	int nCurPersonTypeIndex = pPerson->getType().GetTypeIndex();
	
	//check each mobileElement Types moving with current querist,such as followers, visitorse.
	//processor can serve querist when it can serve querist's each family member.


	if(m_vCombinationCapacity.size() == 0) //old ways
	{
		ProcessorOccupantsState::Occupants& OccupiedOcc = currentProcessorOccupants.GetOccupants();
		for (ProcessorOccupantsState::OccupantsIter occIter = queristOcc.begin(); 
			occIter != queristOcc.end(); occIter++)
		{
			// if processor does not has capacity attributes matched current family member type
			// means that processor can serve current family member
			CapacitiesMapIter capcacityIt = m_mapMobElementCapacity.find(occIter->first);
			if( capcacityIt == m_mapMobElementCapacity.end() )
				continue;
			
			//find current mobElement type, which has occupied current processor
			//occIter->first: current mobElement type which moving with querist
			ProcessorOccupantsState::OccupantsIter OccupiedIter = OccupiedOcc.find(occIter->first);

			int nOccupiedCount = 0;
			if (OccupiedIter != OccupiedOcc.end())
				nOccupiedCount = OccupiedIter->second;

			//if(nOccupiedCount > 0 && nCurPersonTypeIndex != occIter->first)
			//	return false;

			//compare with processor's current mobileElement's capacity attribute
			//capcacityIt->second: processor's capacity item
			//occIter->second: the count of MobElement Type which querist will add to processor 
			if (capcacityIt->second < (nOccupiedCount + occIter->second))
				return false;
		}
	}
	else
	{
		ProcessorOccupantsState::Occupants& OccupiedOcc = currentProcessorOccupants.GetOccupants();
		//int nOccupanyNonPaxCount = 0;
		//if(m_nAllNonPaxCapacity > 0 && nCurPersonTypeIndex > 0)
		//{
		//	for (ProcessorOccupantsState::OccupantsIter occIter = OccupiedOcc.begin(); 
		//		occIter != OccupiedOcc.end(); occIter++)
		//	{

		//		CapacitiesMapIter capcacityIt = m_mapMobElementCapacity.find(occIter->first);
		//		if( capcacityIt == m_mapMobElementCapacity.end() )
		//			nOccupanyNonPaxCount += capcacityIt->second;

		//		if((*occIter).first > 0)
		//			nOccupanyNonPaxCount -= (*occIter).second;
		//	}
		//}

		for (ProcessorOccupantsState::OccupantsIter occIter = queristOcc.begin(); 
			occIter != queristOcc.end(); occIter++)
		{

			int nQueryMobType = occIter->first;
			int nQueryCount = 0;
			nQueryCount = occIter->second;

			// if processor does not has capacity attributes matched current family member type
			// means that processor can serve current family member
			int nTypeCapacity = -1;//no limitation
			CapacitiesMapIter capcacityIt = m_mapMobElementCapacity.find(occIter->first);
			if( capcacityIt != m_mapMobElementCapacity.end() )
				nTypeCapacity = capcacityIt->second;

			//find current mobElement type, which has occupied current processor
			//occIter->first: current mobElement type which moving with querist
			ProcessorOccupantsState::OccupantsIter OccupiedIter = OccupiedOcc.find(occIter->first);

			int nOccupiedCount = 0;
			if (OccupiedIter != OccupiedOcc.end())
				nOccupiedCount = OccupiedIter->second;

			//compare with processor's current mobileElement's capacity attribute
			//capcacityIt->second: processor's capacity item
			//occIter->second: the count of MobElement Type which querist will add to processor 

			int nTotalTypeCapacity = 0;
			if(nTypeCapacity != -1)//this type has no obviously limitation
				nTotalTypeCapacity = nTypeCapacity;
			
			
			if (nTotalTypeCapacity >= (nOccupiedCount + occIter->second) && nTotalTypeCapacity > 0 )
			{
				continue;
				//if(nCurPersonTypeIndex  > 0) //non-pax
				//{

				//	if(capcacityIt->second + m_nAllNonPaxCapacity >  nOccupiedCount)

				//}
				//return false;
			}

			bool bHasCombCapacityLimitation =  false;
			//add the combination
			int nCombCapCount = static_cast<int>(m_vCombinationCapacity.size());
			for (int nCombCap = 0; nCombCap < nCombCapCount; ++ nCombCap)
			{
				if(m_vCombinationCapacity[nCombCap])
				{
					//this combination capacity
					int nThisCombinationCapacity = m_vCombinationCapacity[nCombCap]->m_nCapacity;

					std::vector<int>& vMobType = m_vCombinationCapacity[nCombCap]->m_vMobType;
					if(std::find(vMobType.begin(),vMobType.end(),nQueryMobType) != vMobType.end())//find the type
					{
						int nMobTypeCount = static_cast<int>(vMobType.size());
						for(int nMobType = 0; nMobType < nMobTypeCount; ++ nMobType)
						{
							if(vMobType[nMobType] != nQueryMobType)
							{
								//get the type capacity
								int nTheTypeCapacity = -1;
								CapacitiesMapIter theTypeCapIter =  m_mapMobElementCapacity.find(vMobType[nMobType]);
								if(theTypeCapIter != m_mapMobElementCapacity.end())
									nTheTypeCapacity = theTypeCapIter->second;

								//get the type occupancy
								int nTheTypeOccupancy = 0;
								ProcessorOccupantsState::OccupantsIter theTypeOccIter =  OccupiedOcc.find(vMobType[nMobType]);
								if(theTypeOccIter != OccupiedOcc.end())
									nTheTypeOccupancy = theTypeOccIter->second;

								if(nTheTypeCapacity == -1)
									nThisCombinationCapacity = nThisCombinationCapacity - nTheTypeOccupancy;
								else if(nTheTypeOccupancy > nTheTypeCapacity)
									nThisCombinationCapacity = nThisCombinationCapacity - (nTheTypeOccupancy - nTheTypeCapacity );
								
							}
						}
						bHasCombCapacityLimitation = true;
						if(nThisCombinationCapacity > 0)
							nTotalTypeCapacity += nThisCombinationCapacity;
					}
				}
			}
			//have no limitation to this type
			if(nTypeCapacity == -1 && !bHasCombCapacityLimitation)
				continue;

			if(nTotalTypeCapacity >= nOccupiedCount + nQueryCount)
			{
				if(OccupiedIter != OccupiedOcc.end())
					OccupiedIter->second  += nQueryCount;

				continue;
			}

			////check all non-pax 
			//if(m_nAllNonPaxCapacity > 0 && nCurPersonTypeIndex > 0)
			//{
			//	if(m_nAllNonPaxCapacity > nQueryCount)
			//		continue;

			//	if(m_nAllNonPaxCapacity + nTotalTypeCapacity > nQueryCount + nOccupiedCount)
			//	{
			//		if(OccupiedIter != OccupiedOcc.end())//increase the occupancy count
			//			OccupiedIter->second += nTotalTypeCapacity;

   //                   //nOccupanyNonPaxCount += nQueryCount - nTotalTypeCapacity;// increase the non-pax count
			//		continue;
			//	}
			//}
			return false;
		}
	}

	////all non-pax count
	//if(m_nAllNonPaxCapacity > 0)
	//{
	//	int nOccupanyNonPaxCount = 0;
	//	for (ProcessorOccupantsState::OccupantsIter occIter = OccupiedOcc.begin(); 
	//		occIter != OccupiedOcc.end(); occIter++)
	//	{
	//		if((*occIter).first > 0)
	//			nOccupanyNonPaxCount += (*occIter).second;
	//	}

	//	if(nOccupanyNonPaxCount > m_nAllNonPaxCapacity)
	//		return false;

	//	int nQueryNonPaxCount = 0;
	//	for (ProcessorOccupantsState::OccupantsIter queryIter = queristOcc.begin(); 
	//		queryIter != queristOcc.end(); queryIter++)
	//	{
	//		if((*iter).first > 0)
	//			nQueryNonPaxCount += (*queryIter).second;
	//	}
	//	
	//	if(m_nAllNonPaxCapacity < nQueryNonPaxCount + nOccupanyNonPaxCount)
	//		return false;
	//}
	//
	////combination capacity
	//if(m_vCombinationCapacity.size() > 0)
	//{





	//}



	////in the processor capacity settings, we can define the capacity like this
	////LAPTOP CASE: 1
	////SHOES		:  2
	////COAT		:  1
	////It means the processor could only service 1 LAPTOP CASE or 2 SHOES or 1 COAT at one time.
	////So, if the LAPROP CASE in servicing, the SHOES,and COAT cannot entry the processor.
	////

	//for (ProcessorOccupantsState::OccupantsIter OccupiedIter = OccupiedOcc.begin(); 
	//	OccupiedIter != OccupiedOcc.end(); OccupiedIter++)
	//{
	//	// if processor does not has capacity attributes matched current family member type
	//	// means that processor can serve current family member
	//	CapacitiesMapIter capcacityIt = m_mapMobElementCapacity.find(OccupiedIter->first);
	//	if( capcacityIt == m_mapMobElementCapacity.end() )
	//		continue;

	//	int nOccupancyCount = 0;
	//	if (OccupiedIter != OccupiedOcc.end())
	//		nOccupancyCount = OccupiedIter->second;
	//	
	//	//if the current Mobile element is not a PASSENGER, and has other non-pax in the processor
	//	//return false, for at a moment, the processor could only apply one setting
	//	//if the current Mobile Element is pax, need to check all settings allow the passenger entry
	//	if(nOccupancyCount > 0 && nCurPersonTypeIndex != OccupiedIter->first && nCurPersonTypeIndex != 0)
	//		return false;


	//	//find current mobElement type, which has occupied current processor
	//	//occIter->first: current mobElement type which moving with querist
	//	ProcessorOccupantsState::OccupantsIter occIter = queristOcc.find(OccupiedIter->first);

	//	int nQueryCount = 0;
	//	if (occIter != queristOcc.end())
	//	{	
	//		nQueryCount = occIter->second;

	//		//remove the one checked 
	//		queristOcc.erase(occIter);
	//	}
	//	//compare with processor's current mobileElement's capacity attribute
	//	//capcacityIt->second: processor's capacity item
	//	//occIter->second: the count of MobElement Type which querist will add to processor 
	//	if (capcacityIt->second < (nOccupancyCount + nQueryCount))
	//		return false;
	//}
	//
	//if(queristOcc.size() > 0)
	//{
	//	ProcessorOccupantsState::OccupantsIter occIter = queristOcc.begin();

	//	for (;occIter != queristOcc.end(); ++ occIter)
	//	{
	//		// if processor does not has capacity attributes matched current family member type
	//		// means that processor can serve current family member
	//		CapacitiesMapIter capcacityIt = m_mapMobElementCapacity.find(occIter->first);
	//		if( capcacityIt == m_mapMobElementCapacity.end() )
	//			continue;

	//		int nQueryCount = 0;
	//		nQueryCount = occIter->second;
	//		//compare with processor's current mobileElement's capacity attribute
	//		//capcacityIt->second: processor's capacity item
	//		//occIter->second: the count of MobElement Type which querist will add to processor 
	//		if (capcacityIt->second < nQueryCount)
	//			return false;
	//	}
	//}
	return true;
}


void ProcessorCapacityInSimulation::Clear()
{
	m_mapMobElementCapacity.clear();
	m_nAllNonPaxCapacity = 0;

	for(std::vector<ProcessorCombinationCapactity *>::iterator iter =  m_vCombinationCapacity.begin();
		iter != m_vCombinationCapacity.end(); ++ iter)
		delete *iter;

	m_vCombinationCapacity.clear();
}



