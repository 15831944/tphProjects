#pragma once
#include "../Common/ARCVector.h"
class PaxOnboardBaseBehavior;

class PaxCircleSensor
{
public:
	struct Sensor
	{
		Sensor();
		double dDistToBlock;		
		int nBlockCnt;
		PaxOnboardBaseBehavior* pOtherPax;
		ARCVector3 dir;

		bool bAvaile;
		double dScore;
	};

	void Reset();
	PaxCircleSensor();
	int GetBest() const;
	const Sensor& GetSensor(int i)const{ return vSensorList[i]; }

	void SensorOtherPax( PaxOnboardBaseBehavior* pOtherPax,const ARCVector3& vOffset, double dRad );
	void SensorMap();
	void UpdateScore( PaxOnboardBaseBehavior* pthisPax, const ARCVector3& dirToDest ,const ARCVector3& curDir);

protected:
	double m_dRadiusConcern;
	std::vector<Sensor> vSensorList;
};
