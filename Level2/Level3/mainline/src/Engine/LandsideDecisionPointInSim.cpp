#include "stdafx.h"
#include "LandsideDecisionPointInSim.h"
#include "..\Landside\LandsideControlDevice.h"
#include "LandsideStretchInSim.h"
#include "LandsideResourceManager.h"

LandsideDecisionPointInSim::LandsideDecisionPointInSim( LandsideDecisionPoint*pInput )
:LandsideLayoutObjectInSim(pInput)
{

}


CString LandsideDecisionPointInSim::print() const
{
	return getInput()->getName().GetIDString();
}

void LandsideDecisionPointInSim::InitRelateWithOtherObject( LandsideResourceManager* pRes )
{

	LandsideDecisionPoint* pDP= (LandsideDecisionPoint*)getInput();
	if(!pDP->getStretch())
		return;

	LandsideStretchInSim*pStretch = pRes->getStretchByID(pDP->getStretch()->getID());
	if(pStretch)
	{
		CPoint2008 ptFrom = pStretch->m_Path.GetDistPoint(pDP->GetPosOnStretch());

		for(int i=0;i<pStretch->GetLaneCount();i++)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(i);
			if(pLane)
			{
				LandsideLaneExit* pLaneExit = new LandsideLaneExit();
				DistanceUnit distF = pLane->GetPointDist(ptFrom);
				pLaneExit->SetPosition(pLane,distF);			
				pLaneExit->SetToRes(this);			
				
				pLane->AddLaneNode(pLaneExit);

				LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
				pLaneEntry->SetPosition(pLane,distF);
				pLaneEntry->SetFromRes(this);				
				pLane->AddLaneNode(pLaneEntry);			
			}
		}
		
	}
}

void LandsideDecisionPointInSim::InitGraphNodes( LandsideRouteGraph* pGraph ) const
{

}

void LandsideDecisionPointInSim::InitGraphLink( LandsideRouteGraph* pGraph ) const
{

}

LandsideLayoutObjectInSim* LandsideDecisionPointInSim::getLayoutObject() const
{
	return const_cast<LandsideDecisionPointInSim*>(this);
}


