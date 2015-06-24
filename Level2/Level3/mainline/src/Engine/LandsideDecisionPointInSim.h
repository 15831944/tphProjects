#pragma once
#include "LandsideResourceInSim.h"
#include "LandsideLayoutObjectInSim.h"

class LandsideDecisionPoint;

class LandsideDecisionPointInSim : public LandsideResourceInSim, public LandsideLayoutObjectInSim
{
public:
	//
	LandsideDecisionPointInSim(LandsideDecisionPoint*pInput);
	//
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual CString print()const;

	virtual LandsideResourceInSim* IsResource(){ return this; }

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const;
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const;

};