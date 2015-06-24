#include "StdAfx.h"
#include ".\paxcirclesensor.h"
#include "PaxOnboardBaseBehavior.h"

#define CIRCLE_RAD 200 
#define NUM_SENSOR 36
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PaxCircleSensor::PaxCircleSensor(void)
{
	m_dRadiusConcern  = CIRCLE_RAD;
	
}



void PaxCircleSensor::Reset()
{
	vSensorList.clear();
	vSensorList.resize(NUM_SENSOR);

	for(int i=0;i<NUM_SENSOR;i++)
	{
		double dR = M_PI*i*2/NUM_SENSOR;	
		vSensorList[i].dir  = ARCVector3(cos(dR),sin(dR),0 );
	}
}

int PaxCircleSensor::GetBest() const
{
	int thebest = -1;
	for(int i=0;i<NUM_SENSOR;i++)
	{
		const Sensor&  isensor = vSensorList[i];
		if(!isensor.bAvaile)
			continue;

		if(thebest<0 || (isensor.dScore > vSensorList[thebest].dScore ) )
		{					
			thebest = i;			
		}
	}

	return thebest;
}

bool distRayToCircle(const ARCVector3&raypos, const ARCVector3& rayDir, const ARCVector3& circlePos, double dcircleRad );

void PaxCircleSensor::SensorOtherPax( PaxOnboardBaseBehavior* pOtherPax, const ARCVector3& vOffset, double dRad )
{
	double r0 = vOffset.Length();

	if(r0 > m_dRadiusConcern )
		return;

	ARCVector3 vNorm = vOffset;
	vNorm.Normalize();	

	//根据圆的极坐标方程 求解
	for(int i=0;i<NUM_SENSOR;i++)
	{
		double b = r0 * vSensorList[i].dir.dot(vNorm);
		double c = r0*r0 - dRad*dRad;
		double dt= b*b-c;

		if(dt>=0 && b >= 0)
		{
			double dDistToPax = b - sqrt(dt);
			Sensor& theSensor = vSensorList[i];				
			theSensor.nBlockCnt++;

			if(theSensor.dDistToBlock >= dDistToPax  )
			{
				theSensor.dDistToBlock = dDistToPax;
				theSensor.pOtherPax = pOtherPax;
			}
		}
	}
}

void PaxCircleSensor::UpdateScore( PaxOnboardBaseBehavior* pthisPax, const ARCVector3& dirToDest ,const ARCVector3& curDir )
{
	for(int i=0;i<NUM_SENSOR;i++)
	{
		Sensor& iSensor = vSensorList[i];
		const ARCVector3& sdir = iSensor.dir;

		double offsetDest = sdir.dot(dirToDest);

		double offsetCur = sdir.dot(curDir);

		double dDist = iSensor.dDistToBlock;
		double dDistRat = 1+sin( dDist/m_dRadiusConcern );

		//double dDistInc = vSensorList[i].dDistToBlock - vLastSecenList[i].dDistToBlock;
		//double dIncRat = 1+sin(dDistInc/GetSensorRadius());

		double density = 0;
		if(iSensor.nBlockCnt)
			density = 1.0/iSensor.nBlockCnt;


		bool bAvable = pthisPax->bCanWaitFor(iSensor.pOtherPax);
		
		double dScore;
		if(dDist>0)
			dScore =  offsetDest*1 + offsetCur*0.1 + dDistRat*1 + density*1;
		else
			bAvable = false;

		iSensor.bAvaile = bAvable;
		iSensor.dScore = dScore;
		//dAvaibleList[i] = bAvable;
	}
}

void PaxCircleSensor::SensorMap()
{

}

PaxCircleSensor::Sensor::Sensor()
{
	dDistToBlock = 0;
	nBlockCnt = 0;
	pOtherPax = NULL;
	bAvaile = true;
	dScore = 1;
}