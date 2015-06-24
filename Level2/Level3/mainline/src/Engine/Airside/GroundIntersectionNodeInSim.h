#pragma once
#include "AirsideResource.h"
class GroundIntersectionNodeInSim: public AirsideResource
{
public:
	GroundIntersectionNodeInSim(void);
	virtual ~GroundIntersectionNodeInSim(void);

	virtual ResourceType GetType()const = 0;
	virtual CString GetTypeName()const = 0;
	virtual CString PrintResource()const = 0;

	virtual CPoint2008 GetDistancePoint(double dist)const = 0; 

	virtual void getDesc(ResourceDesc& resDesc)=0;
};
