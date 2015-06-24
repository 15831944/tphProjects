#pragma once


class StandLeadInLine;
class StandLeadOutLine;
class StandInSim;
class DeiceStandInSim;
class StandLeadOutLine;
class IntersectionNodeInSim;
class IntersectionNodeInSimList;
#include <Common/Path2008.h>
#include ".\RouteResource.h"

class ENGINE_TRANSFER StandLeadOutLineInSim : public RouteDirPath
{
public:
	StandLeadOutLineInSim(StandInSim* pStand, int inLineidx);
	StandLeadOutLineInSim(StandInSim* pStand,StandLeadInLine* pLeadInLine);
	StandLeadOutLineInSim(DeiceStandInSim* pDecie);
	~StandLeadOutLineInSim();
	virtual ResourceType GetType()const { return ResType_StandLeadOutLine; }
	virtual CString GetTypeName()const { return _T("StandLeadOutLine"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const;
	DistanceUnit GetEndDist();

	void InitRelate( IntersectionNodeInSimList& nodes );
	int GetIntersectNodeCount()const{ return (int)m_vOutNodes.size(); }
	IntersectionNodeInSim* GetIntersectNode(int idx){ return m_vOutNodes.at(idx); }

	// set exit time of flight
	virtual void SetExitTime(CAirsideMobileElement * pFlight, const ElapsedTime& exitT);
	virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime);
	virtual bool GetLock(AirsideFlightInSim * pFlight);
	virtual AirsideFlightInSim * GetLockedFlight();
	virtual bool TryLock(AirsideFlightInSim * pFlight);
	//StandLeadOutLine* GetInput(){ return m_pLeadOutLine; }
	const CPath2008& GetPath()const{ return m_linePath; }
	bool IsPushBack()const;
	bool IsReleasePoint()const;
	DistanceUnit GetReleaseDistance()const;
	int GetID()const;

	RouteResource* getRouteResource()const;
	
public:	
	StandInSim* GetStandInSim()const{ return m_pStand; }
	IntersectionNodeInSim* GetEndNode();
protected:
	StandInSim * m_pStand;
	StandLeadOutLine* m_pLeadOutLine;
	StandLeadOutLine* m_pDeiceOutCons;
	CPath2008 m_linePath;

	std::vector<IntersectionNodeInSim*> m_vOutNodes;

};