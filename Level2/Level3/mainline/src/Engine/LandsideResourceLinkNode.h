#pragma once
#include <vector>
#include<common/ARCMathCommon.h>
#include <common/ARCVector.h>

class LandsideResourceInSim;
///landside linkage info
class LandsideResourceLinkNode
{
public:
	LandsideResourceLinkNode()
	{
		m_pFrom = m_pTo = NULL;
		m_distF = m_distT = 0;
	}
	LandsideResourceLinkNode(LandsideResourceInSim* pFrom, DistanceUnit distF, LandsideResourceInSim* pTo, DistanceUnit distT)
		:m_pFrom(pFrom)
		,m_pTo(pTo)
		,m_distF(distF)
		,m_distT(distT)
	{

	}

	const ARCVector3& getPos()const{ return mPos; }
	LandsideResourceInSim* getToRes()const{ return m_pTo; }
	LandsideResourceInSim* getFromRes()const{ return m_pFrom; }
public:
	LandsideResourceInSim* m_pFrom;
	LandsideResourceInSim* m_pTo;
	DistanceUnit m_distF; //dist in resource from
	DistanceUnit m_distT; //dist in resource to 
	ARCVector3 mPos;
};

class LandsideResourceLinkNodeList : public std::vector<LandsideResourceLinkNode>
{
public:
};


