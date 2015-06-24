//-----------------------------------------------------------------------------
//				class:		CTrafficObjectmanagerInSim
//				Author:		sky.wen
//				Date:		July 7, 2011
//				Purpose:	traffic object manage class
//------------------------------------------------------------------------------
#pragma once
#include "..\Common\containr.h"
#include "LandsideObjectInSim.h"
class LandsideResourceManager;

class CTrafficObjectmanagerInSim : public UnsortedContainer<CLandsideWalkTrafficObjectInSim>
{
public:
	CTrafficObjectmanagerInSim(void);
	virtual ~CTrafficObjectmanagerInSim(void);

	//----------------------------------------------------------------------------
	//Summary:
	//		calculate every single object intersection node with same type object
	//----------------------------------------------------------------------------
	void CalculateIntersectionNode();

	//----------------------------------------------------------------------------
	//Summary:
	//		calculate every single object intersection node with different object
	//----------------------------------------------------------------------------
	void CalculateIntersectionNode(CTrafficObjectmanagerInSim* pTrafficManagerInSim);


	void InitRelateWithOtherRes(LandsideResourceManager* pRes);
};
