#pragma once
#include "AirsideVehicleInSim.h"
#include "../../InputAirside/VehicleDefine.h"
#include "VehicleRouteInSim.h"

class CAbandonmentPoint;
class ClearanceItem;
class AirsideMeetingPointInSim;
class ENGINE_TRANSFER AirsideFollowMeCarInSim: public AirsideVehicleInSim
{
public:
	AirsideFollowMeCarInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem);
	~AirsideFollowMeCarInSim(void);

	bool IsReadyToService(AirsideFlightInSim* pFlight);

	void GetNextCommand();

	void ServicingFlight();

	void SetReturnRoute(const VehicleRouteInSim& pReturnRoute);

	void SetAbandonData(CAbandonmentPoint* pAbandonData);
	CAbandonmentPoint* GetAbandonData();

	void SetMeetingPoint(AirsideMeetingPointInSim* pPoint);
	AirsideMeetingPointInSim* GetMeetingPoint();

	void SetDestNode(AirsideResource* pDest);

	void UpdateState(const ClearanceItem& item);

	virtual void WakeUp(const ElapsedTime& tTime);
	virtual void ReturnVehiclePool(ProbabilityDistribution* pTurnAroundDistribute);
	virtual void SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent);

	void GetCompleteReturnRouteToPool();

protected:
	virtual void _ChangeToService();
private:
	VehicleRouteInSim m_ReturnRoute;
	CAbandonmentPoint* m_pAbandonData;
	AirsideMeetingPointInSim* m_pMeetingPoint;
	AirsideResource* m_pDestRes;
};
