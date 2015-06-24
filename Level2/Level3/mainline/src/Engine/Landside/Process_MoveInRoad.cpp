#include "stdafx.h"
#include "Process_MoveInRoad.h"
#include "..\LandsideResourceManager.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "..\CrossWalkInSim.h"
#include "../LandsideIntersectionInSim.h"
#include "../LandsideExternalNodeInSim.h"
#include "../LandsideRoundaboutInSim.h"
//#include "Goal_MoveInRoad2.h"

void Process_MoveToDest::Activate( CARCportEngine* pEngine )
{
	LandsideResourceInSim* pOrign = m_pOwner->getLastState().getLandsideRes();
	DistanceUnit distInRes = m_pOwner->getLastState().distInRes;
	LandsideResourceManager* pRes = pEngine->GetLandsideSimulation()->GetResourceMannager();
	ASSERT(pOrign);
	ASSERT(mpDest);
	if(pOrign==NULL)
	{
		m_iStatus = gs_completed;
		return;
	}
}

void Process_MoveToDest::Process( CARCportEngine* pEngine )
{
	

}
