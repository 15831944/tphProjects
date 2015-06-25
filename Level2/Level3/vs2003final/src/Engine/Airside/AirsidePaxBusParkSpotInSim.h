#pragma once
#include "AirsideResource.h"
#include "AirsideObserver.h"
#include "../../Common/Path2008.h"
#include "../../InputAirside/AirsidePaxBusParkSpot.h"

class AirsideVehicleInSim;
class AirsidePaxBusParkSpotInSim;
class ClearanceItem;
class AirsidePaxBusParkSpotInSim : public AirsideResource/*, public CAirsideSubject*/
{
public:
	AirsidePaxBusParkSpotInSim(AirsidePaxBusParkSpot * pParkSpot);
	~AirsidePaxBusParkSpotInSim(void);

	bool GetExitParkSpotClearance(AirsideVehicleInSim * pVehicleInSim, const ElapsedTime& tTime,const CPoint2008& ptLocation);
	bool GetEnterParkSpotClearance(AirsideVehicleInSim * pVehicleInSim,  ClearanceItem& lastItem);
	virtual ResourceType GetType()const {return ResType_PaxBusParking;}
	virtual CString GetTypeName()const {return _T("PaxBusParking");}
	virtual CString PrintResource()const;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const; 

	AirsidePaxBusParkSpot* GetParkSpot();

private:
	void WriteVehiclePathLog(AirsideVehicleInSim * pVehicleInSim,const CPath2008& path,const ElapsedTime& startTime, double startSpeed,
		const ElapsedTime& endTime,double endSpd,BOOL bPushBack);
	void InitParkSpotPath(AirsidePaxBusParkSpot * pParkSpot);
private:
	AirsidePaxBusParkSpot::RefPtr m_pParkSpot;

	CPath2008 m_leadinPath;
	CPath2008 m_leadoutPath;
};
