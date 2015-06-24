#pragma once

class StandLeadInLine;
class StandInSim;
class DeiceStandInSim;
class StandLeadInLine;
class IntersectionNodeInSim;
class IntersectionNodeInSimList;
#include <Common/Path2008.h>
#include ".\RouteResource.h"

class ENGINE_TRANSFER StandLeadInLineInSim : public RouteDirPath
{
public:
	StandLeadInLineInSim(StandInSim* pStand, int inLineidx);
	StandLeadInLineInSim(DeiceStandInSim* pDecie);

	~StandLeadInLineInSim();

	virtual ResourceType GetType()const { return ResType_StandLeadInLine; }
	virtual CString GetTypeName()const { return _T("StandLeadInLine"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const;
	
	DistanceUnit GetEndDist();
	
	void InitRelate(IntersectionNodeInSimList& nodelist);

	int GetIntersectNodeCount()const{ return (int)m_vInNodes.size(); }
	IntersectionNodeInSim* GetIntersectNode(int idx)const{ return m_vInNodes.at(idx); }

	virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime);
	virtual bool GetLock(AirsideFlightInSim * pFlight);
	virtual AirsideFlightInSim * GetLockedFlight();
	virtual bool TryLock(AirsideFlightInSim * pFlight);

	// set enter time of flight
	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode);

	const CPath2008&  GetPath()const{ return m_linePath; }
	int GetID()const;

	RouteResource* getRouteResource()const;
	IntersectionNodeInSim* GetEndNode();
public:
	//void SetStandInSim(StandInSim* pStand){ m_pStand = pStand; }
	StandInSim* GetStandInSim()const{ return m_pStand; }
protected:
	StandInSim * m_pStand;
	StandLeadInLine* m_pLeadInLine;
	StandLeadInLine* m_pDeiceInCons;
	CPath2008 m_linePath;
	std::vector<IntersectionNodeInSim*> m_vInNodes;

};