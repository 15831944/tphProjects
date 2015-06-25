#include "StdAfx.h"
#include ".\facilitybehaviorsinsim.h"
#include "..\Landside\FacilityBehavior.h"
#include "LandsideResourceManager.h"
#include "WalkwayInSim.h"
#include "Landside/LandsideWalkway.h"
#include "LandsideBusStationInSim.h"
#include "LandsideTaxiQueueInSim.h"
#include "../Landside/FacilityParkingLotBehavior.h"
#include "Common/ALTObjectID.h"
#include "../Landside/LandsideLayoutObject.h"
#include "CommonData/PROCID.H"
#include "Common/OneToOneProcess.h"
#include "../Landside/FacilityBehaviorLinkage.h"
#include "TERMINAL.H"

CFacilityBehaviorsInSim::CFacilityBehaviorsInSim(void)
:m_pTerminalLinkage(NULL)
,m_resManagerInst(NULL)
,m_pLandsideObjectList(NULL)
{
}

CFacilityBehaviorsInSim::~CFacilityBehaviorsInSim(void)
{
}

void CFacilityBehaviorsInSim::Initialize(LandsideFacilityLayoutObjectList* pObjectList,LandsideResourceManager* resManagerInst)
{
	ASSERT(resManagerInst != NULL);
	m_resManagerInst = resManagerInst;
	m_pLandsideObjectList = pObjectList;

	delete m_pTerminalLinkage;
	m_pTerminalLinkage = new CFacilityBehaviorList;
	m_pTerminalLinkage->ReadData(0);
	m_pTerminalLinkage->SortBehaviorByFaclityName();

}

void CFacilityBehaviorsInSim::GetTerminalLinkageProcName(CFacilityBehaviorLinkageList* pLinkageList,Terminal* pTerminal,const ALTObjectID& objID,std::vector<ALTObjectID>& vProcName)
{
	int nCount = pLinkageList->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CFacilityBehaviorLinkage *pLinkage = pLinkageList->GetItem(nItem);
		if(pLinkage && pLinkage->getType() == CFacilityBehaviorLinkage::enumLinkType_Terminal)
		{
			if (pLinkage->is1To1())
			{
				ProcessorID procID;
				procID.SetStrDict(pTerminal->inStrDict);
				pLinkage->getName(procID);
				GroupIndex groupIndex = pTerminal->procList->getGroupIndex(procID);
				ALTObjectIDList objectList;
				for (int i = groupIndex.start; i <= groupIndex.end; i++)
				{
					Processor *pTermProcConnected =pTerminal->procList->getProcessor(i);
					CString strProc = pTermProcConnected->getIDName();
					ALTObjectID destObject;
					destObject.FromString(strProc.GetBuffer());
					objectList.push_back(destObject);
				}
				OneToOneProcess altProcess(objID,objectList);
				ALTObjectID resultObject;
				if(altProcess.GetOneToOneObject(resultObject,procID.idLength()))
					vProcName.push_back(resultObject);
			}
			else
			{
				ALTObjectID altProc;
				pLinkage->getName(altProc);
				vProcName.push_back(altProc);
			}
		}
	}
}

LandsideResourceInSim* CFacilityBehaviorsInSim::GetLandsideTerminalLinkage( const ProcessorID& procID )
{
	if(!m_pTerminalLinkage)
		return NULL;

	std::vector<CFacilityBehavior*> blist;
	m_pTerminalLinkage->getBehaviorList(ALT_LCURBSIDE,blist);
	for(size_t i=0;i<blist.size();i++)
	{
		CFacilityBehavior* pLinkage = blist.at(i);
		CFacilityBehaviorLinkage* pBehaviorLink = pLinkage->GetFacilityBehavior(procID);
		if (pBehaviorLink)
		{
			if (pBehaviorLink->is1To1())
			{
				ALTObjectID objID;
				objID.FromString(procID.GetIDString().GetBuffer());
				ALTObjectIDList objectList;
				if(m_pLandsideObjectList->GetObjectListByName(pLinkage->getFacilityName(),objectList))
				{
					OneToOneProcess altProcess(objID,objectList);
					ALTObjectID resultObject;
					if(altProcess.GetOneToOneObject(resultObject,pLinkage->getFacilityName().idLength()))
						return m_resManagerInst->getRandomCurb(resultObject);
				}
				
			}
			else
			{
				return m_resManagerInst->getRandomCurb(pLinkage->getFacilityName());
			}
		}
	
	/*	if( pLinkage->containProcID(procID) )
		{
			return m_resManagerInst->getRandomCurb(pLinkage->getFacilityName());
		}*/
	}

	blist.clear();
	m_pTerminalLinkage->getBehaviorList(ALT_LBUSSTATION,blist);
	for(size_t i=0;i<blist.size();i++)
	{
		CFacilityBehavior* pLinkage = blist.at(i);
		CFacilityBehaviorLinkage* pBehaviorLink = pLinkage->GetFacilityBehavior(procID);
		if (pBehaviorLink)
		{
			if (pBehaviorLink->is1To1())
			{
				ALTObjectID objID;
				objID.FromString(procID.GetIDString().GetBuffer());
				ALTObjectIDList objectList;
				if(m_pLandsideObjectList->GetObjectListByName(pLinkage->getFacilityName(),objectList))
				{
					OneToOneProcess altProcess(objID,objectList);
					ALTObjectID resultObject;
					if(altProcess.GetOneToOneObject(resultObject,pLinkage->getFacilityName().idLength()))
						return m_resManagerInst->getRandomCurb(resultObject);
				}

			}
			else
			{
				return m_resManagerInst->getRandomBusStation(pLinkage->getFacilityName());
			}
		}

	/*	if( pLinkage->containProcID(procID) )
		{
			return m_resManagerInst->getRandomBusStation(pLinkage->getFacilityName());
		}*/
	}

	blist.clear();
	m_pTerminalLinkage->getBehaviorList(ALT_LTAXIQUEUE,blist);
	for(size_t i=0;i<blist.size();i++)
	{
		CFacilityBehavior* pLinkage = blist.at(i);
		CFacilityBehaviorLinkage* pBehaviorLink = pLinkage->GetFacilityBehavior(procID);
		if (pBehaviorLink)
		{
			if (pBehaviorLink->is1To1())
			{
				ALTObjectID objID;
				objID.FromString(procID.GetIDString().GetBuffer());
				ALTObjectIDList objectList;
				if(m_pLandsideObjectList->GetObjectListByName(pLinkage->getFacilityName(),objectList))
				{
					OneToOneProcess altProcess(objID,objectList);
					ALTObjectID resultObject;
					if(altProcess.GetOneToOneObject(resultObject,pLinkage->getFacilityName().idLength()))
						return m_resManagerInst->getRandomCurb(resultObject);
				}

			}
			else
			{
				return m_resManagerInst->getRandomTaxiQueue(pLinkage->getFacilityName());
			}
		}
	/*	if( pLinkage->containProcID(procID) )
		{
			return m_resManagerInst->getRandomTaxiQueue(pLinkage->getFacilityName());
		}*/
	}


	return NULL;
}

LandsideResourceInSim* CFacilityBehaviorsInSim::GetLandsideTerminalLinkage( ALTOBJECT_TYPE resType, const ProcessorID& procID )
{
	if(!m_pTerminalLinkage)
		return NULL;

	std::vector<CFacilityBehavior*> blist;
	if(resType == ALT_LCURBSIDE)
	{
		m_pTerminalLinkage->getBehaviorList(ALT_LCURBSIDE,blist);
		for(size_t i=0;i<blist.size();i++)
		{
			CFacilityBehavior* pLinkage = blist.at(i);
			CFacilityBehaviorLinkage* pBehaviorLink = pLinkage->GetFacilityBehavior(procID);
			if (pBehaviorLink)
			{
				if (pBehaviorLink->is1To1())
				{
					ALTObjectID objID;
					objID.FromString(procID.GetIDString().GetBuffer());
					ALTObjectIDList objectList;
					if(m_pLandsideObjectList->GetObjectListByName(pLinkage->getFacilityName(),objectList))
					{
						OneToOneProcess altProcess(objID,objectList);
						ALTObjectID resultObject;
						if(altProcess.GetOneToOneObject(resultObject,pLinkage->getFacilityName().idLength()))
							return m_resManagerInst->getRandomCurb(resultObject);
					}
					
				}
				else
				{
					return m_resManagerInst->getRandomCurb(pLinkage->getFacilityName());
				}
			}
		
		/*	if( pLinkage->containProcID(procID) )
			{
				return m_resManagerInst->getRandomCurb(pLinkage->getFacilityName());
			}*/
		}
	}
	else if(resType == ALT_LBUSSTATION)
	{
		blist.clear();
		m_pTerminalLinkage->getBehaviorList(ALT_LBUSSTATION,blist);
		for(size_t i=0;i<blist.size();i++)
		{
			CFacilityBehavior* pLinkage = blist.at(i);
			CFacilityBehaviorLinkage* pBehaviorLink = pLinkage->GetFacilityBehavior(procID);
			if (pBehaviorLink)
			{
				if (pBehaviorLink->is1To1())
				{
					ALTObjectID objID;
					objID.FromString(procID.GetIDString().GetBuffer());
					ALTObjectIDList objectList;
					if(m_pLandsideObjectList->GetObjectListByName(pLinkage->getFacilityName(),objectList))
					{
						OneToOneProcess altProcess(objID,objectList);
						ALTObjectID resultObject;
						if(altProcess.GetOneToOneObject(resultObject,pLinkage->getFacilityName().idLength()))
							return m_resManagerInst->getRandomCurb(resultObject);
					}

				}
				else
				{
					return m_resManagerInst->getRandomBusStation(pLinkage->getFacilityName());
				}
			}

		/*	if( pLinkage->containProcID(procID) )
			{
				return m_resManagerInst->getRandomBusStation(pLinkage->getFacilityName());
			}*/
		}
	}
	else if(resType == ALT_LTAXIQUEUE)
	{
		blist.clear();
		m_pTerminalLinkage->getBehaviorList(ALT_LTAXIQUEUE,blist);
		for(size_t i=0;i<blist.size();i++)
		{
			CFacilityBehavior* pLinkage = blist.at(i);
			CFacilityBehaviorLinkage* pBehaviorLink = pLinkage->GetFacilityBehavior(procID);
			if (pBehaviorLink)
			{
				if (pBehaviorLink->is1To1())
				{
					ALTObjectID objID;
					objID.FromString(procID.GetIDString().GetBuffer());
					ALTObjectIDList objectList;
					if(m_pLandsideObjectList->GetObjectListByName(pLinkage->getFacilityName(),objectList))
					{
						OneToOneProcess altProcess(objID,objectList);
						ALTObjectID resultObject;
						if(altProcess.GetOneToOneObject(resultObject,pLinkage->getFacilityName().idLength()))
							return m_resManagerInst->getRandomCurb(resultObject);
					}

				}
				else
				{
					return m_resManagerInst->getRandomTaxiQueue(pLinkage->getFacilityName());
				}
			}
		/*	if( pLinkage->containProcID(procID) )
			{
				return m_resManagerInst->getRandomTaxiQueue(pLinkage->getFacilityName());
			}*/
		}
	}
	else
	{
		//this type has not been implement, please do it yourself.
		ASSERT(0);
	}






	return NULL;

}

CWalkwayInSim * CFacilityBehaviorsInSim::getLandsideTerminalWalkwayLinkage( const ProcessorID& procID )
{
	std::vector<CFacilityBehavior*> blist;
	m_pTerminalLinkage->getBehaviorList(ALT_LWALKWAY,blist);
	for(size_t i=0;i<blist.size();i++)
	{
		CFacilityBehavior* pLinkage = blist.at(i);
		if( pLinkage->containProcID(procID) )
		{
			return m_resManagerInst->GetWalkwayInSim(pLinkage->getFacilityName());
		}
	}
	return NULL;
}
CWalkwayInSim* CFacilityBehaviorsInSim::GetWalkwayTerminalLinkage(std::vector<CWalkwayInSim *>& vWalkwayInSim, std::vector<ALTObjectID>& vLinkTermProcID )
{
	if(!m_pTerminalLinkage)
		return NULL;

	std::vector<CFacilityBehavior*> blist;
	m_pTerminalLinkage->getBehaviorList(ALT_LWALKWAY,blist);
	int nWalkwayCount = static_cast<int>(vWalkwayInSim.size());
	for(int iWalkway=0;iWalkway<nWalkwayCount;iWalkway++)
	{

		CWalkwayInSim *pWay = vWalkwayInSim[iWalkway];
		for(size_t j=0;j<blist.size();j++)
		{
			CFacilityBehavior* pFacilityBehavior = blist.at(j);
		
			std::vector<ALTObjectID> vTermProc;
			pFacilityBehavior->getLinkageList()->GetTerminalLinkageProcName(vTermProc);
			if (vTermProc.size())//link to terminal processor
			{				
				if(pFacilityBehavior->getFacilityName().idFits(pWay->GetInputWalkway()->getName()))//find the first one matched, not the best matched behavior
				{
					vLinkTermProcID = vTermProc;
					return pWay;
				}
			}
		}
	}

	return NULL;
}
void CFacilityBehaviorsInSim::GetTerminalProcLinkedWithParkingLot( ALTObjectID objParkingLot, int nLevel, std::vector<ALTObjectID>& vLinkTermProcID )
{
	CFacilityBehavior *pFacilityBehavior = m_pTerminalLinkage->FindFacilityBehavior(objParkingLot);
	if(pFacilityBehavior == NULL)
		return;
	if(pFacilityBehavior->getFacilityType() != ALT_LPARKINGLOT)
		return;

	CFacilityParkingLotBehavior *pParkingLotBehavior = (CFacilityParkingLotBehavior *)pFacilityBehavior;
	if(pParkingLotBehavior == NULL)
		return;
	CFacilityParkingLotBehaviorLinkageList* pLinkageList = pParkingLotBehavior->GetParkingLotLinkageList();
	if(pLinkageList == NULL)
		return;

	int nCount = pLinkageList->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CFacilityParkingLotBehaviorLinkage* pLinkage = pLinkageList->GetItem(nItem);
		if(pLinkage == NULL)
			continue;
	//	if(nItem == nLevel)
		if(pLinkage->GetLevelID() == nLevel)
		{
			ALTObjectID procID;
			pLinkage->getName(procID);
			if(!procID.IsBlank())
				vLinkTermProcID.push_back(procID);
		}
	}
}





























