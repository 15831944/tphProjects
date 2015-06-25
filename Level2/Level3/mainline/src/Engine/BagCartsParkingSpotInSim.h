#pragma once
#include "Airside\airsideresource.h"
#include "../Common/Path2008.h"

class AirsideBagCartParkSpot;
class AirsideVehicleInSim;
class ClearanceItem;

class CBagCartsParkingSpotInSim :
	public AirsideResource, public CAirsideSubject
{
public:

	typedef ref_ptr<CBagCartsParkingSpotInSim> RefPtr;


	CBagCartsParkingSpotInSim(AirsideBagCartParkSpot *pPaxParkingSpot);
	~CBagCartsParkingSpotInSim(void);

public:
	ResourceType GetType()const{ return ResType_BagCartsParkingPosition; }
	virtual CString GetTypeName()const{ return _T("Baggage carts parking position"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	virtual CPoint2008 GetDistancePoint(double dist)const;

	AirsideBagCartParkSpot *GetBagCartsSpotInput();

	CPoint2008 GetServicePoint();

	bool GetExitParkSpotClearance(AirsideVehicleInSim * pVehicleInSim, const ElapsedTime& tTime,const CPoint2008& ptLocation);
	bool GetEnterParkSpotClearance(AirsideVehicleInSim * pVehicleInSim,  ClearanceItem& lastItem);

private:
	void WriteVehiclePathLog(AirsideVehicleInSim * pVehicleInSim,const CPath2008& path,const ElapsedTime& startTime, double startSpeed,
		const ElapsedTime& endTime,double endSpd,BOOL bPushBack);
	void InitParkSpotPath(AirsideBagCartParkSpot * pParkSpot);
protected:
	AirsideBagCartParkSpot *m_pBagCartsSpotInput;

	CPath2008 m_leadinPath;
	CPath2008 m_leadoutPath;
};
