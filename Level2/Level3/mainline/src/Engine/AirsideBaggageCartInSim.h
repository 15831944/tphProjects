#pragma once
#include "airside\airsidevehicleinsim.h"

class AirsideBaggageBehavior;
class CLoader;
class CBagCartsParkingSpotInSim;
class AirsideBaggageCartInSim :
	public AirsideVehicleInSim
{
public:
	AirsideBaggageCartInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem);
	~AirsideBaggageCartInSim(void);

	void WirteLog(const CPoint2008& p, const double speed, const ElapsedTime& t, bool bPushBack /* = false */);

public:
	int getCapacity()const;

	
	void AddBaggage(AirsideBaggageBehavior *pBag);


	int GetBagCount() const;

	void ReleaseBaggage(Processor *pProc, CBagCartsParkingSpotInSim* pBagCartsSpotInSim, ElapsedTime &eTime);
	void ReleaseBaggageToFlight(const CPoint2008& ptCargoDoor, ElapsedTime &eTime);



	bool IsFull() const;
protected:
	std::vector<AirsideBaggageBehavior *> m_vBaggage;
	CPoint2008 prePos;
	ElapsedTime m_preTime;
	double m_preSpeed;
	double m_dir; //angle from x
};
