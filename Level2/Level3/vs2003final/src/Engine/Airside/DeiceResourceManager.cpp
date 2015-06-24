#include "StdAfx.h"



#include "DeiceResourceManager.h"

#include <limits>

#include <inputairside\ALTObjectGroup.h>
#include <InputAirside/ALTAirport.h>

#include "TaxiwayResourceManager.h"
#include "VehicleStretchInSim.h"
#include "AirsideFlightInSim.h"

#include "DeicePadGroup.h"
#include "TaxiRouteInSim.h"
#include "SimulationErrorShow.h"


DeicePadInSim::DeicePadInSim( DeicePad* pInput )
{
	m_Input = pInput; m_pEntryNode = NULL; m_pExitNode = NULL;
	mInPath = pInput->GetInContrain();
	mInPath.insertPointAfterAt(pInput->GetServicePoint(), mInPath.getCount()-1);

	mOutPath.init(pInput->GetOutConstrain().getCount()+1);
	mOutPath[0] = pInput->GetServicePoint();
	for(int i=0;i<pInput->GetOutConstrain().getCount();++i)
		mOutPath[i+1] = pInput->GetOutConstrain().getPoint(i);

	m_pGroup = NULL;
	m_pFlightOnto = NULL;
	m_pApproachingRoute = NULL;
}


DeicePadInSim::~DeicePadInSim()
{
	if (m_pApproachingRoute)
	{
		delete m_pApproachingRoute;
		m_pApproachingRoute = NULL;
	}
}

DeiceResourceManager::DeiceResourceManager()
{
	for (std::vector<DeicePadInSim*>::iterator ite = m_vDeicePad.begin();
		ite != m_vDeicePad.end();ite++)
	{
		delete *ite;
	}
	m_vDeicePad.clear();

	for (DeicepadGroupList::iterator ite = m_vDeicepadGroups.begin();
		ite != m_vDeicepadGroups.end();ite++)
	{
		delete *ite;
	}
	m_vDeicepadGroups.clear();

}

DeiceResourceManager::~DeiceResourceManager()
{
	
}

bool DeiceResourceManager::Init( int nPrjID, int nAirportID )
{
	ALTObjectList vDeicePads;
	ALTAirport::GetDeicePadList(nAirportID,vDeicePads);
	for(int i = 0;i< (int)vDeicePads.size(); ++i)
	{
		DeicePad * pPad =(DeicePad*) vDeicePads.at(i).get();

		DeicePadInSim* pDeicepad = new DeicePadInSim(pPad);
		m_vDeicePad.push_back( pDeicepad );

		// to find the DeicepadGroup suited the DeicePad,
		// if not, create a new one,
		// then add pad to the group
		ALTObjectID altID = pPad->GetObjectName();
		int nLen = altID.idLength();
		ASSERT(nLen);
		altID.m_val[nLen - 1] = "";
		DeicepadGroup* pGroup = NULL;
		DeicepadGroupList::iterator ite = std::find_if(m_vDeicepadGroups.begin(), m_vDeicepadGroups.end(), DeicepadGroup::IDEqual(altID));
		if (m_vDeicepadGroups.end() == ite)
		{
			pGroup = new DeicepadGroup(altID);
			m_vDeicepadGroups.push_back(pGroup);
		}
		else
		{
			pGroup = *ite;
		}
		pGroup->AddDeicepad(pDeicepad);
	}

	return true;	
}


// DeicePadInSim* DeiceResourceManager::AssignDeicePad( const ALTObjectGroup& deicGrp, int nMaxQueueLen,AirsideFlightInSim* pFlt )
// {
// 	DeicePadInSim* pLessQPad = NULL;
// 	for(int i=0;i<GetCount();++i)
// 	{
// 		DeicePadInSim* pPad = GetPadByIndex(i);
// 		if( pPad->GetInput()->GetObjectName().idFits(deicGrp.getName()) )
// 		{
// 			if(pPad->m_pEntryNode && pPad->m_pExitNode)
// 			{
// 				if(!pLessQPad)
// 				{
// 					pLessQPad = pPad;
// 					continue;
// 				}
// 
// 				if(pLessQPad)
// 				{
// 					if( pLessQPad->GetQueue().getLength() > pPad->GetQueue().getLength() )
// 					{
// 						pLessQPad = pPad;					
// 					}
// 				}	
// 			}		
// 					
// 		}
// 	}
// 	if(pLessQPad && pLessQPad->GetQueue().getLength() < nMaxQueueLen)
// 		return pLessQPad;
// 	return NULL;
// }


DeicePadInSim* DeiceResourceManager::GetPadByID( int id )
{
	for(int i=0;i<GetCount();i++)
	{
		DeicePadInSim* pPad = GetPadByIndex(i);
		if(pPad->GetInput()->getID() ==id)
			return pPad;
	}
	return NULL;
}

DeicePadInSim* DeiceResourceManager::GetPadByIndex( int idx ) const
{
	return m_vDeicePad.at(idx);
}

bool DeiceResourceManager::InitRelations( IntersectionNodeInSimList& nodeList,TaxiwayResourceManager * pTaxiwayRes )
{
	for(int i=0;i<(int)m_vDeicePad.size();i++)
	{
		DeicePadInSim* pPad =  m_vDeicePad.at(i);
		pPad->InitRelateNodes(nodeList);		
	}

	std::for_each(m_vDeicepadGroups.begin(), m_vDeicepadGroups.end(), std::mem_fun(&DeicepadGroup::InitEntryPoint));
	return true;
}

DeicepadGroup* DeiceResourceManager::AssignGroup( const ALTObjectGroup& deicGrp, int nMaxQueueLen, AirsideFlightInSim* pFlt )
{
	DeicepadGroupList suitableGroups;
	int nApproachingFlightNum = (std::numeric_limits<int>::max)();
	for (DeicepadGroupList::iterator ite = m_vDeicepadGroups.begin();ite!=m_vDeicepadGroups.end();ite++)
	{
		DeicepadGroup* pGroup = *ite;
		if (pGroup->GetGroupID().idFits(deicGrp.getName()))
		{
			if (pGroup->GetApproachingFlightNum()<nApproachingFlightNum)
			{
				nApproachingFlightNum = pGroup->GetApproachingFlightNum();
				suitableGroups.clear();
				suitableGroups.push_back(pGroup);
			}
			else if (pGroup->GetApproachingFlightNum()==nApproachingFlightNum)
			{
				suitableGroups.push_back(pGroup);
			}
		}
	}
	if (nApproachingFlightNum<nMaxQueueLen)
	{
		size_t nSize = suitableGroups.size();
		if (nSize)
		{
			size_t nIndex = 0;
			if (nIndex>1)
				nIndex = rand()%nSize;
	
			return suitableGroups[nIndex];
		}
	}
	else
	{
		TRACE(_T("Queue length limited.\n"));
	}
	return NULL;
}

DeicePad* DeicePadInSim::GetInput()
{
	return m_Input.get();
}

CString DeicePadInSim::PrintResource() const
{
	return m_Input->GetObjNameString();
}

CPoint2008 DeicePadInSim::GetDistancePoint( double dist ) const
{
	double dInLen = GetInPath().GetTotalLength();
	if(dist < dInLen )
		return GetInPath().GetDistPoint(dist);
	else
		return GetOutPath().GetDistPoint(dist - dInLen);
}


ElapsedTime DeicePadInSim::getAvaileWithinTime()
{
	return avgHandleTime* (long)GetQueue().getLength() ;
}

void DeicePadInSim::InitRelateNodes( IntersectionNodeInSimList& nodeList )
{
	for(int i=0;i<nodeList.GetNodeCount();i++)
	{
		IntersectionNodeInSim* pNode = nodeList.GetNodeByIndex(i);
		if( pNode->GetNodeInput().HasDeiceInContrain(m_Input->getID()) )
		{
			m_pEntryNode = pNode;
		}

		if( pNode->GetNodeInput().HasDeiceOutContrain(m_Input->getID()) )
		{
			m_pExitNode = pNode;
		}

		if(m_pEntryNode && m_pExitNode )
			return;
	}

	// invalid entry or exit
	AirsideSimErrorShown::SimpleSimWarning(
		m_Input->GetObjectName().GetIDString(),
		_T("The deicepad has no entry node or exit node. Please adapt the layout."),
		_T("Deicepad Floating")
		);

}

void DeicePadInSim::SetExitTime( CAirsideMobileElement * pFlight, const ElapsedTime& exitT )
{
// 	GetQueue().RemoveFlight( (AirsideFlightInSim*)pFlight,exitT );
	if(pFlight->GetType() == CAirsideMobileElement::AIRSIDE_FLIGHT)
	{
		OnFlightLeaving((AirsideFlightInSim*)pFlight, exitT);
	}
	AirsideResource::SetExitTime(pFlight,exitT);
}

#include "..\..\Results\AirsideFlightEventLog.h"
void DeicePadInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetInput()->getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

void DeicePadInSim::AddOutLane(VehicleLaneEntry* laneEntry )
{
	if (std::find(m_vOutLanes.begin(),m_vOutLanes.end(),laneEntry) != m_vOutLanes.end())
		return;

	m_vOutLanes.push_back(laneEntry);
}

AirsideFlightInSim* DeicePadInSim::OnFlightLeaving(AirsideFlightInSim* pFlight, const ElapsedTime& exitT)
{
	if (m_pGroup)
	{
		ASSERT(m_pFlightOnto == pFlight);
		m_pFlightOnto = NULL;
		m_pGroup->RemoveApproachingFlight(pFlight);
		return m_pGroup->WakeFlight(this, exitT);
	}
	return NULL;
}

void DeicePadInSim::SetApproachingRoute( TaxiRouteInSim* pRoute )
{
	if (m_pApproachingRoute)
	{
		delete m_pApproachingRoute;
		m_pApproachingRoute = NULL;
	}
	m_pApproachingRoute = pRoute;
}
