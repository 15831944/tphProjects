#pragma once
#include "../../Common/Point2008.h"
#include "../EngineDll.h"
#include "../../InputAirside/MeetingPoint.h"
#include "AirsideResource.h"
#include "TaxiwayResource.h"

class CMeetingPoint;
class AirportResourceManager;
class VehicleLaneEntry;
class ENGINE_TRANSFER AirsideMeetingPointInSim : public AirsideResource
{
public:
	AirsideMeetingPointInSim(CMeetingPoint* pPoint);
	~AirsideMeetingPointInSim(void);

	TaxiwaySegInSim* GetAttachedTaxiwaySegment(){ return m_pTaxiwaySeg; }
	void SetAttachedTaxiwaySegment(TaxiwaySegInSim* pTaxiwaySeg) { m_pTaxiwaySeg = pTaxiwaySeg; }

	CPoint2008 GetPosition();
	CString GetName();
	CMeetingPoint* GetMeetingPointInput();

	virtual ResourceType GetType()const;
	virtual CString GetTypeName()const;
	virtual CString PrintResource()const;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const; 
	CPoint2008 GetWaitingPosition();
	CPoint2008 GetCenterPoint();
	bool IsPathOverlapWaitingArea(const CPath2008& path);

	int GetOutLaneCount()const{ return m_vOutLanes.size(); }
	VehicleLaneEntry* GetOutLane(int idx){ return m_vOutLanes.at(idx); }
	void AddOutLane(VehicleLaneEntry* laneEntry);

private:
	CMeetingPoint::RefPtr m_pMeetingPoint;		//input data
	TaxiwaySegInSim* m_pTaxiwaySeg;				//attached taxiway segment
	CPoint2008 m_Position;		//position
	std::vector<VehicleLaneEntry*> m_vOutLanes;	
};
