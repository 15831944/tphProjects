#pragma once
#include "ALTObject.h"
#include "ALTObjectGroup.h"
#include "Runway.h"
#include "InputAirsideAPI.h"


struct INPUTAIRSIDE_API TaxiwayRunwayIntersection 
{

	TaxiwayRunwayIntersection()
	{
		m_runwayMark = RUNWAYMARK_FIRST;
		m_nInterIndex = 0;
	}

	Runway m_Runway;
	RUNWAY_MARK m_runwayMark;   // first end or last end 0,1
	ALTObject m_Taxiway;

	int m_nInterIndex;
};

class INPUTAIRSIDE_API CBaseBoundRoute
{
public:
	CBaseBoundRoute(void);
	virtual ~CBaseBoundRoute(void);
};
