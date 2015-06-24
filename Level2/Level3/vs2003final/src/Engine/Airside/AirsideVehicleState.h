#pragma once

#include "../../Common/point2008.h"
#include "../../Common/elaptime.h"
//#include "AirsideResource.h"
//#include "../../Common/ProbabilityDistribution.h"
#include "CommonInSim.h"

class AirsideFlightInSim;
class AirsideResource;
class AirsideFlightInSim;
class ProbabilityDistribution;


class CAirsideVehicleState
{
public:
	CAirsideVehicleState(void);
	~CAirsideVehicleState(void);


public:
	ElapsedTime m_tCurrentTime;
	CPoint2008 m_pPosition;
	double m_vSpeed;    //cm/s
	CPoint2008 m_vDirection;  //
	AirsideMobileElementMode m_vehicleMode;
	AirsideResource* m_pResource;
	DistanceUnit m_dist;  
	ElapsedTime tMinTurnAroundTime;

	double  m_vStuffPercent;
	AirsideFlightInSim* m_pServiceFlight;
	ElapsedTime m_tLeavePoolTime;
	bool m_bAvailable;
	int m_nServicePointCount;
	ProbabilityDistribution* m_pServiceTimeDistribution;
	int m_pCurrentPointIndex;

protected:
private:

};
