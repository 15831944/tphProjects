#pragma once
#include "AirsideResource.h"
#include "AirsideFlightInSim.h"
#include "AirsideObserver.h"
#include "../../InputAirside/AirHold.h"
#include "../../Common/Path2008.h"


class AirWayPointInSim;
class AirRouteNetworkInSim;


class ENGINE_TRANSFER AirspaceHoldInSim :public AirsideResource, public CAirsideSubject
{
	friend class AirRouteNetworkInSim;
public:
	explicit AirspaceHoldInSim( AirHold* pHold,AirRouteNetworkInSim* pNetwork, AirWayPointInSim* pWaypoint, double dAirportVar);
	~AirspaceHoldInSim(void);

	virtual ResourceType GetType()const{ return ResType_AirspaceHold; }

	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode);

	virtual CString GetTypeName()const{ return _T("AirspaceHold"); }
	virtual CString PrintResource()const;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const; 

	virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime);
	virtual bool GetLock(AirsideFlightInSim * pFlight);
	virtual bool TryLock(AirsideFlightInSim * pFlight);

	void RemoveLockFlight(AirsideFlightInSim* pFlight);

	double getAvailableHeight(AirsideFlightInSim* pFlight);

	AirHold* GetHoldInput() const;
	AirWayPointInSim* GetWaypointInSim() const;

	int GetHeadFlightCount(AirsideFlightInSim* pFlight);
	CPath2008 GetPath() const;
	bool IsAtFirstHeightLevel(AirsideFlightInSim* pFlight);

	void AddHoldingFlight(AirsideFlightInSim* pFlight);

	double GetATCManagedDistance(){ return m_dATCManagedDist; }
	AirRouteNetworkInSim* GetAirRouteNetwork();/*{ return m_pAirRouteNetwork; }*/

private:
	AirWayPointInSim* m_pWaypoint;
	std::vector<AirsideFlightInSim*> m_vLockFlights;
	std::vector<AirsideFlightInSim*> m_vHoldingFlights;
	AirHold* m_pAirHoldInput;
	int m_nMaxFlightNum;
	CPath2008 m_path;
	double LevelHeight;
	double m_dATCManagedDist;
	AirRouteNetworkInSim* m_pAirRouteNetwork;

};
