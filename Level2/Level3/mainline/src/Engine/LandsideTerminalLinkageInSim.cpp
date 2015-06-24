#include "StdAfx.h"
#include ".\landsideterminallinkageinsim.h"
#include "Landside\FacilityBehavior.h"
#include "CommonData\ProjectCommon.h"
#include "Landside\LandsideLayoutObject.h"
#include "LandsideLayoutObjectInSim.h"
#include "LandsideResourceManager.h"
#include "LandsideBusStationInSim.h"
#include "LandsidePaxTypeLinkageInSimManager.h"
#include "Landside\FacilityParkingLotBehavior.h"
#include "PROCESS.H"
#include "PROCLIST.H"
#include "Common/OneToOneProcess.h"

LandsideTerminalLinkageInSimManager::LandsideTerminalLinkageInSimManager(void)
:m_pProcList(NULL)
{
	m_vLinkage.OwnsElements(1);
}

LandsideTerminalLinkageInSimManager::~LandsideTerminalLinkageInSimManager(void)
{
	m_vLinkage.clear();
}


void LandsideTerminalLinkageInSimManager::Initialize( CFacilityBehaviorList *pFacilityBehaviors, ProjectCommon* pCommon, const LandsideFacilityLayoutObjectList& layoutObjectList,LandsideResourceManager* resManagerInst )
{
	int nCount = pFacilityBehaviors->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		ProcessFacility(pFacilityBehaviors->GetItem(nItem), pCommon, layoutObjectList, resManagerInst);
	}

}

void LandsideTerminalLinkageInSimManager::ProcessFacility( CFacilityBehavior *pFacility, ProjectCommon* pCommon, const LandsideFacilityLayoutObjectList& layoutObjectList, LandsideResourceManager* pResManagerInst )
{
	ASSERT(pFacility != NULL);
	if(pFacility == NULL)
		return;

	if( pFacility->getFacilityType() == ALT_LPARKINGLOT)//parking lot has several level
	{
		CFacilityParkingLotBehavior *pParkingLotBehavior = (CFacilityParkingLotBehavior *)pFacility;
		if(pParkingLotBehavior == NULL)
			return;
		CFacilityParkingLotBehaviorLinkageList* pLinkageList = pParkingLotBehavior->GetParkingLotLinkageList();
		if(pLinkageList == NULL)
			return;

		int nLinkageCount = pLinkageList->GetItemCount();
		for (int nLink = 0; nLink < nLinkageCount; ++ nLink)
		{
			CFacilityParkingLotBehaviorLinkage*  pItem = pLinkageList->GetItem(nLink);
			if(pItem == NULL)
				continue;

			//terminal processor
			if (pItem->is1To1())
			{
				ALTObjectIDList resultList;
				
				if (!layoutObjectList.GetObjectListByName(pFacility->getFacilityName(),resultList))
					continue;

				ALTObjectIDList parkLotList;
				ALTObjectID parkLot;
				parkLot.FromString(pItem->getName().GetBuffer());
				if (!layoutObjectList.GetObjectListByName(parkLot,parkLotList))
					continue;

				int nParkCount = (int)parkLotList.size();
				for (int i = 0; i < nParkCount; i++)
				{
					ALTObjectID destPark = parkLotList.at(i);
					OneToOneProcess altProcess(destPark,resultList);
					ALTObjectID altResult;
					if (altProcess.GetOneToOneObject(altResult,pFacility->getFacilityName().idLength()))
					{
						ProcessorID procID;
						procID.SetStrDict(pCommon->getStringDictionary());
						procID.setID(altResult.GetIDString().GetBuffer());
						std::vector<int> vLevels;
						vLevels.push_back(pItem->GetLevelID());
						AddItem(procID, destPark, vLevels);
					}
				}
			}
			else
			{
				ProcessorID procID;
				procID.SetStrDict(pCommon->getStringDictionary());
				pItem->getName(procID);
				CString strProc = procID.GetIDString();

				std::vector<int> vLevels;
				vLevels.push_back(pItem->GetLevelID());
				AddItem(procID, pFacility->getFacilityName(), vLevels);
			}
		}

	}
	else
	{
		//linkage
		CFacilityBehaviorLinkageList* pLinkageList = pFacility->getLinkageList();

		std::vector<CFacilityBehaviorLinkage *> vLinkage;
		pLinkageList->GetLinkageList(CFacilityBehaviorLinkage::enumLinkType_Terminal,vLinkage);
		int nLinkCount = static_cast<int>(vLinkage.size());
		for (int nItem = 0; nItem < nLinkCount; ++ nItem)
		{
			CFacilityBehaviorLinkage *pLinkage = vLinkage[nItem];
			if(pLinkage)
			{
				//terminal processor
				ProcessorID procID;
				procID.SetStrDict(pCommon->getStringDictionary());
				pLinkage->getName(procID);

				if (pLinkage->is1To1())
				{
					ALTObjectIDList resultList;
					if (!layoutObjectList.GetObjectListByName(pFacility->getFacilityName(),resultList))
						continue;

					
					GroupIndex groupIndex = m_pProcList->getGroupIndex(procID);
					for (int i = groupIndex.start; i <= groupIndex.end; i++)
					{
						Processor* pProc = m_pProcList->getProcessor(i);
						CString strProcID = pProc->getID()->GetIDString();
						ALTObjectID altProc;
						altProc.FromString(strProcID.GetString());

						OneToOneProcess altProcess(altProc,resultList);
						ALTObjectID altResult;
						if (altProcess.GetOneToOneObject(altResult,pFacility->getFacilityName().idLength()))
						{
							ProcessorID id;
							id.SetStrDict(pCommon->getStringDictionary());
							id.setID(altResult.GetIDString().GetBuffer());
							AddItem(id, altProc);
						}
					}
				}
				else
				{

					AddItem(procID, pFacility->getFacilityName());
				}
				

				//lead to ...
				//parking lot
				CLandsideLeadToParkingLotList* vLeadto = pFacility->getParkingLeadToLotList();
				int nLeadToCount = vLeadto->GetItemCount();
				for (int nLead = 0; nLead < nLeadToCount; ++ nLead)
				{
					CLandsideLeadToParkingLot* pLeadTo = vLeadto->GetItem(nLead);
					if(pLeadTo == NULL)
						continue;
					const LandsideFacilityLayoutObject* pObject = layoutObjectList.getObjectByID(pLeadTo->GetparkingLotId());
					if(pObject == NULL)
						continue;
					AddItem(procID, pObject->getName(), pLeadTo->GetLevelList());
				}

				//if bus station, get the parking lot that this bus station could lead to
				LandsideBusStationInSim* pResource = pResManagerInst->getRandomBusStation(pFacility->getFacilityName());
				//if (pLayoutObjectInSim && pLayoutObjectInSim->IsResource())
				//{
				//	LandsideResourceInSim *pResource = pLayoutObjectInSim->IsResource();
					if(pResource && pResource->toBusSation())
					{
						LandsideBusStationInSim *pBusStation = pResource->toBusSation();
						LandsidePaxTypeLinkageInSim *pCanLeadTo = pBusStation->getPaxLeadToLinkage();
						std::vector<LandsideObjectLinkageItemInSim> vAllObject ;
						pCanLeadTo->GetAllObjectCanLeadTo(vAllObject);
						
						int nCount =static_cast<int>(vAllObject.size());
						for (int nObject = 0; nObject < nCount; ++ nObject)
						{
							LandsideObjectLinkageItemInSim& objLinkage = vAllObject[nObject];
							AddItem(procID, objLinkage.getObject());
							
						}
					}
				//}
			}
		}
	}
}

void LandsideTerminalLinkageInSimManager::AddItem(const ProcessorID& procID,const ALTObjectID& objID )
{
	std::vector<int> vLevels;
	AddItem(procID, objID, vLevels);

}

void LandsideTerminalLinkageInSimManager::AddItem( const ProcessorID& procID,const ALTObjectID& objID, const std::vector<int>& vLevels )
{
	TerminalProcLinkageInSim *pEqualLinkage = NULL;

	int nCount = GetCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		TerminalProcLinkageInSim *pInsim = GetItem(nItem);
		if(pInsim == NULL)
			continue;

		ProcessorID* pCurProcID =  pInsim->getProcID();

		if(procID == *pCurProcID)
		{
			pEqualLinkage = pInsim;
			pEqualLinkage->AddObject(objID, vLevels);

		}
		else if(procID.idFits(*pCurProcID))
		{
			pInsim->AddObject(objID, vLevels);
		}
	}

	if(pEqualLinkage == NULL)
	{
		pEqualLinkage = new TerminalProcLinkageInSim(procID);
		pEqualLinkage->AddObject(objID, vLevels);
		m_vLinkage.addItem(pEqualLinkage);
	}
}

int LandsideTerminalLinkageInSimManager::GetCount() const
{
	return static_cast<int>(m_vLinkage.size());
}

TerminalProcLinkageInSim* LandsideTerminalLinkageInSimManager::GetItem( int nIndex )
{
	ASSERT(nIndex >= 0 && nIndex < GetCount());
	if(nIndex < 0 && nIndex >= GetCount())
		return NULL;
	return m_vLinkage[nIndex];

}

TerminalProcLinkageInSim * LandsideTerminalLinkageInSimManager::FindItem( const ProcessorID& procID )
{
	int nCount = GetCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		TerminalProcLinkageInSim *pInsim = GetItem(nItem);
		if(pInsim == NULL)
			continue;
		if(pInsim->getProcID()->idFits(procID))
			return pInsim;
	}

	return NULL;
}

bool LandsideTerminalLinkageInSimManager::GetRandomObject( const std::vector<LandsideObjectLinkageItemInSim>& vAllObject,LandsideObjectLinkageItemInSim& linkObject )
{
	std::vector<LandsideObjectLinkageItemInSim> vFitObject;
	int nSize = (int)vAllObject.size();
	int nCount = GetCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		TerminalProcLinkageInSim *pInsim = GetItem(nItem);
		if(pInsim == NULL)
			continue;

		for (int i = 0; i < nSize; i++)
		{
			LandsideObjectLinkageItemInSim ObjItem = vAllObject[i];
			if(pInsim->CanLeadToAltobject(ObjItem.getObject()))
			{
				if (std::find(vFitObject.begin(),vFitObject.end(),ObjItem) == vFitObject.end())
				{
					vFitObject.push_back(ObjItem);
				}
			}
		}
	}

	if (vFitObject.empty())
	{
		return false;
	}

	size_t nFitCount = vFitObject.size();
	int nRandom = random(nFitCount);
	linkObject = vFitObject[nRandom];
	return true;
}

void LandsideTerminalLinkageInSimManager::SetProcessorList( ProcessorList* pProcList )
{
	m_pProcList = pProcList;
}

//////////////////////////////////////////////////////////////////////////
//TerminalProcLinkageInSim
TerminalProcLinkageInSim::TerminalProcLinkageInSim(const ProcessorID& procID)
:m_procID(procID)
{

}

TerminalProcLinkageInSim::~TerminalProcLinkageInSim()
{

}

void TerminalProcLinkageInSim::AddObject( const ALTObjectID& objID )
{
	LandsideObjectLinkageItemInSim newItem(objID);
	newItem.setOrigin(m_procID.GetIDString());
	m_vLandsideObject.push_back(newItem);

}

void TerminalProcLinkageInSim::AddObject( const ALTObjectID& objID,const std::vector<int>& vLevels )
{

	LandsideObjectLinkageItemInSim newItem(objID,vLevels);
	newItem.setOrigin(m_procID.GetIDString());
	m_vLandsideObject.push_back(newItem);

}

ProcessorID * TerminalProcLinkageInSim::getProcID()
{
	return &m_procID;
}

bool TerminalProcLinkageInSim::operator<( const TerminalProcLinkageInSim& pItem )
{
	if(m_procID < pItem.m_procID)
		return false;
	else
		return true;
}

const std::vector<LandsideObjectLinkageItemInSim>& TerminalProcLinkageInSim::GetLandsideObject() const
{
	return m_vLandsideObject;
}

bool TerminalProcLinkageInSim::CanLeadToAltobject( const ALTObjectID& objID ) const
{
	if (m_vLandsideObject.empty())
		return true;
	
	for (UINT i = 0; i < m_vLandsideObject.size(); i++)
	{
		const LandsideObjectLinkageItemInSim& linkage = m_vLandsideObject.at(i);
		if (objID.idFits(linkage.getObject()))
		{
			return true;
		}
	}
	return false;
}

LandsideObjectLinkageItemInSim::LandsideObjectLinkageItemInSim( const ALTObjectID& objID )
{
	m_altObj = objID;
}

LandsideObjectLinkageItemInSim::LandsideObjectLinkageItemInSim( const ALTObjectID& objID,const std::vector<int>& vLevels )
{
	m_altObj = objID;
	m_vLevels = vLevels;
}

LandsideObjectLinkageItemInSim::LandsideObjectLinkageItemInSim()
{

}

bool LandsideObjectLinkageItemInSim::operator==( const LandsideObjectLinkageItemInSim& pID ) const
{
	if(m_altObj == pID.m_altObj && m_vLevels == pID.m_vLevels)
		return true;

	return false;

}

bool LandsideObjectLinkageItemInSim::CanLeadTo( const ALTObjectID& altDestination ) const
{	
	if(m_altObj.idFits(altDestination))
		return true;
	return false;
}

bool LandsideObjectLinkageItemInSim::CanLeadTo( const ALTObjectID& altDestination, int nLevel ) const
{
	if(CanLeadTo(m_altObj))
	{
		if(m_vLevels.size() == 0)
			return true;

		if(std::find(m_vLevels.begin(), m_vLevels.end(), nLevel)!= m_vLevels.end())
		{
			return true;
		}
	}

	return false;
}

bool LandsideObjectLinkageItemInSim::CanLeadTo(const LandsideObjectLinkageItemInSim& destObject ) const
{
	if(CanLeadTo(destObject.getObject()))
	{
		if(m_vLevels.size() == 0)
			return true;
		const std::vector<int>& vDestLevels = destObject.getLevels();

		int nDstLevelCount = static_cast<int>(vDestLevels.size());
		if(nDstLevelCount == 0)
			return true;

		for (int nLevel = 0; nLevel < nDstLevelCount; ++ nLevel)
		{
			if(std::find(m_vLevels.begin(), m_vLevels.end(), vDestLevels[nLevel])!= m_vLevels.end())
			{
				return true;
			}
		}
	}

	return false;
}

CString LandsideObjectLinkageItemInSim::getOrigin() const
{
	return m_strOrigin;
}

void LandsideObjectLinkageItemInSim::setOrigin( const CString& strOrigin )
{
	m_strOrigin = strOrigin;
}
