#pragma once
#include "FlightRouteSegInSim.h"




class IntersectionNodeInSim;
class RunwayInSim;
class RunwaySegInSim;
class ENGINE_TRANSFER RunwayDirectSegInSim:public FlightGroundRouteDirectSegInSim
{
public:
	RunwayDirectSegInSim(RunwaySegInSim *pSeg, FlightGroundRouteSegDirection dir );
	~RunwayDirectSegInSim();

	virtual int GetObjectID()const;
public:


	// set enter time of flight
	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode);
	// set exit time of flight
	virtual void SetExitTime(CAirsideMobileElement * pFlight, const ElapsedTime& exitT);

	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode,double dSpd);

	virtual ResourceType GetType()const { return AirsideResource::ResType_RunwayDirSeg; }
	virtual CString GetTypeName()const{ return _T("RunwayDirectSegment"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	
	RUNWAY_MARK GetLogicRunwayType();

	RunwaySegInSim *GetRunwaySeg()const;
protected:
private:


};


class ENGINE_TRANSFER RunwaySegInSim : public FlightGroundRouteSegInSim
{
public:
	RunwaySegInSim(IntersectionNodeInSim * pNodeFrom , IntersectionNodeInSim * pNodeTo, RunwayInSim * pRunwayInSim);
	~RunwaySegInSim(void);


	double GetNode1Pos()const;
	double GetNode2Pos()const;

	
	bool bMobileInResource(ARCMobileElement* pmob)const{ return false; }
	int GetRunwayID()const;
	LogicRunwayInSim *GetLogicRunway(RUNWAY_MARK runwayMark);

	RunwayInSim *getRunway();
public:


protected:
	RunwayInSim *m_pRunwayInSim;

};
