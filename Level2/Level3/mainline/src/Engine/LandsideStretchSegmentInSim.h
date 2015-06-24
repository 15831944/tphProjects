#pragma once
#include "landside/LandsideStretchSegment.h"
#include "LandsideLayoutObjectInSim.h"

class LandsideStretchSegmentInSim : public LandsideLayoutObjectInSim
{
public:
	LandsideStretchSegmentInSim(LandsideStretchSegment*pSeg);
	~LandsideStretchSegmentInSim(void);
	
	LandsideStretchSegment* GetLandsideSeg(){ return (LandsideStretchSegment*)getInput(); }

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{}
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const{}

};
