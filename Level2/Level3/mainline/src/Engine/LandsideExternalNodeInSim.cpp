#include "StdAfx.h"
#include ".\landsideexternalnodeinsim.h"
//////////////////////////////////////////////////////////////////////////
#include "Landside/LandsideExternalNode.h"
#include "LandsideResourceManager.h"

void LandsideExternalNodeInSim::InitRelateWithOtherObject( LandsideResourceManager* allRes )
{
	LandsideExternalNode* mpInput = getExtNode();
	if(!mpInput->getStretch())
		return;

	int nID = mpInput->getStretch()->getID();
	if(LandsideStretchInSim* pStretch = allRes->getStretchByID(nID))
	{
		for(int i=0;i<pStretch->GetLaneCount();i++)
		{
			LandsideStretchLaneInSim* plane = pStretch->GetLane(i);
			if(!plane)
				continue;

			if( plane->IsPositiveDir() == mpInput->IsLinkStretchPort1() )
			{				
				LandsideLaneEntry* pEntry= plane->GetFirstLaneEntry();
				if(pEntry && pEntry->getFromRes()==NULL)
				{
					pEntry->SetFromRes(this);
					m_vLaneEntries.push_back(pEntry);
				}				
			}
			else
			{
				LandsideLaneExit* pExit = plane->GetLastLaneExit();
				if(pExit && pExit->getToRes() == NULL)
				{
					pExit->SetToRes(this);
					m_vLaneExits.push_back(pExit);
				}
			}
		}
	}
}

LandsideExternalNodeInSim::LandsideExternalNodeInSim( LandsideExternalNode* pNode )
:LandsideLayoutObjectInSim(pNode)
{
	//mpInput = pNode;	
	if(pNode->GetPosition(mPos))
	{

	}	
}

bool LandsideExternalNodeInSim::IsOrign() const
{
	return !m_vLaneEntries.empty();		
}

bool LandsideExternalNodeInSim::IsDest() const
{
	return !m_vLaneExits.empty();
}

CString LandsideExternalNodeInSim::print() const
{
	return getInput()->getName().GetIDString();
}

LandsideLayoutObjectInSim* LandsideExternalNodeInSim::getLayoutObject() const
{
	return const_cast<LandsideExternalNodeInSim*>(this);
}