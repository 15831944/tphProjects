#pragma once


#include <common/ARCVector.h>
#include <engine/Airside/AirsideResource.h>
#include <vector>

class AirsideFlightInSim;
//airspace hold 
class AirEntrySystemHoldInInSim : public AirsideResource
{
public:
	AirEntrySystemHoldInInSim(const ARCVector3& dir, const CPoint2008& pos);
	~AirEntrySystemHoldInInSim(void);

	//////////////////////////////////////////////////////////////////////////

	virtual ResourceType GetType()const;
	virtual CString GetTypeName()const;
	virtual CString PrintResource()const;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const; 


	//////////////////////////////////////////////////////////////////////////

	CPoint2008 GetLevelPos(int idx)const;
	ARCVector3 GetDir()const;


	void RemoveFlightInQueue(AirsideFlightInSim* pFlight);
	void AddFlightToQueue(AirsideFlightInSim* pFlight);
	AirsideFlightInSim* GetFrontFlight()const;
	int GetInQFlightCount()const{ return (int)m_flightQueue.size(); }
	bool isFlightInQ(AirsideFlightInSim* pFlight)const;

	void writeFlightLog(AirsideFlightInSim* pFlight, const ElapsedTime& exitHoldTime);

	void OnFlightEnter(AirsideFlightInSim* pFlight, const ElapsedTime& enterTime);
	void OnFlightExit(AirsideFlightInSim* pFlight,const ElapsedTime& exitTime);

protected:
	ARCVector3 m_dir;
	CPoint2008 m_level0pos;
	
	typedef std::vector<AirsideFlightInSim*> FlightListType;
	FlightListType m_flightQueue;
	
};
