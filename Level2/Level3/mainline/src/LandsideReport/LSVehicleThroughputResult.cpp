#include "StdAfx.h"
#include ".\lsvehiclethroughputresult.h"
#include "LandsideBaseParam.h"
#include "..\Results\LandsideVehicleDelayLog.h"
#include "..\Common\ProgressBar.h"
#include "..\Landside\InputLandside.h"
#include "..\Common\Point2008.h"
#include "..\Common\Line2008.h"
#include "LSVehicleThroughputParam.h"

LSVehicleThroughputResult::LSVehicleThroughputResult(void)
{
}

LSVehicleThroughputResult::~LSVehicleThroughputResult(void)
{
}

void LSVehicleThroughputResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSVehicleThroughputResult::CaculateVehicleCrossPortal( LandsideVehicleLogEntry& element,LandsideBaseParam* pParameter )
{
	if (pParameter == NULL)
		return;

	LSVehicleThroughputParam* pThroughputPara = (LSVehicleThroughputParam*)pParameter;

	int iEventCount  = element.getCount();

	double portalZ = pThroughputPara->GetStartPoint().getZ();
	std::vector<LandsideVehicleEventStruct> vPointEvent;
	bool bChangeHeight = false;
	for (int i = 0; i < iEventCount; i++)
	{
		LandsideVehicleEventStruct nextEvent = element.getEvent(i);
		if(nextEvent.z == portalZ)
		{
			vPointEvent.push_back(nextEvent);
			if( !bChangeHeight )
			{
				bChangeHeight = true;
			}
		}
		else
		{
			if( bChangeHeight )
			{
				bChangeHeight = false;
				CaculateVehicleCorssResult(vPointEvent,element.GetVehicleDesc().vehicleID,pThroughputPara);
				vPointEvent.clear();
			}
		}
	}

	if( bChangeHeight )
	{
		CaculateVehicleCorssResult(vPointEvent,element.GetVehicleDesc().vehicleID,pThroughputPara);
	}
}
