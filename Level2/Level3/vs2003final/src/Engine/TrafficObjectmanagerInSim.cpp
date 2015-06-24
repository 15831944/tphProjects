#include "StdAfx.h"
#include ".\trafficobjectmanagerinsim.h"
#include "LandsideStretchInSim.h"
#include "LandsideResourceManager.h"
#include "CrossWalkInSim.h"

CTrafficObjectmanagerInSim::CTrafficObjectmanagerInSim(void)
{
}

CTrafficObjectmanagerInSim::~CTrafficObjectmanagerInSim(void)
{
}
//----------------------------------------------------------------------------
//Summary:
//		calculate every single object intersection node with same type object
//----------------------------------------------------------------------------
void CTrafficObjectmanagerInSim::CalculateIntersectionNode()
{
	for (int i = 0; i < getCount(); i++)
	{
		CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim = getItem(i);
		//calculate intersection node with other walkway
		for (int j = 0; j < getCount(); j++)
		{
			CLandsideWalkTrafficObjectInSim* pInterSectTrafficObjectInSim = getItem(j);
			pTrafficObjectInSim->CalculateIntersectionNode(pInterSectTrafficObjectInSim);
		}
	}
}

//----------------------------------------------------------------------------
//Summary:
//		calculate every single object intersection node with different object
//----------------------------------------------------------------------------
void CTrafficObjectmanagerInSim::CalculateIntersectionNode( CTrafficObjectmanagerInSim* pTrafficManagerInSim )
{
	for (int i = 0; i < getCount(); i++)
	{
		CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim = getItem(i);
		//calculate intersection node with other walkway
		for (int j = 0; j < pTrafficManagerInSim->getCount(); j++)
		{
			CLandsideWalkTrafficObjectInSim* pInterSectTrafficObjectInSim = pTrafficManagerInSim->getItem(j);
			pTrafficObjectInSim->CalculateIntersectionNode(pInterSectTrafficObjectInSim);
		}
	}
}



void CTrafficObjectmanagerInSim::InitRelateWithOtherRes( LandsideResourceManager* pRes )
{
	for(int i=0;i<getCount();i++)
	{
		if( CCrossWalkInSim* pCross = getItem(i)->toCrossWalk() )
		{
			LandsideStretchInSim* pStretch = pRes->getStretchByID(pCross->getAtStretchID());
			ASSERT(pStretch);
			if(pStretch)
			{
				pStretch->AddCrossWalk(pCross);
			}
		};
	}
}