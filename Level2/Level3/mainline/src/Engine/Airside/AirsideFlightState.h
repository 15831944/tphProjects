#pragma once
#include "AirsideResource.h"
#include "../../Common/Point2008.h"
#include "common\elaptime.h"

class AirsideFlightState
{
public:
	AirsideFlightState();
	~AirsideFlightState(void);

	ElapsedTime m_tTime; 
	CPoint2008 m_pPosition;   //cm
	DistanceUnit m_dAlt; //cm
	double m_vSpeed;    //cm/s
	CPoint2008 m_vDirection;  //
	AirsideMobileElementMode m_fltMode;
	AirsideResource* m_pResource;
	DistanceUnit m_dist;      //cm        
	DistanceUnit m_offsetAngle;
	int m_nDelayId;
	double m_dAcc; //acc

	bool m_bPushBack;


};
