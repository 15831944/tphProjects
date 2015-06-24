#include "StdAfx.h"
#include ".\laneseginsim.h"

bool LaneSegInSim::FindFreePos(LandsideVehicleInSim* pVehicle,int& iLoop, double& dRangeF, double& dRangeT)const
{
	dRangeF = mDistT;
	for(int i=0;i<(int)mOcyVehicles.size();i++)
	{
		const VehicleOcy& vo = mOcyVehicles[i];			
		dRangeT = vo.dPos + vo.pVeh->GetLength()*0.5;		
		if(i>=iLoop)
		{
			if(dRangeF - dRangeT > pVehicle->GetLength()+500)
			{					
				iLoop=i+1;
				return true;
			}
		}
		dRangeF = vo.dPos - vo.pVeh->GetLength()*0.5;			
	}
	dRangeT = mDistF;
	if(dRangeF - dRangeT > pVehicle->GetLength())
	{			
		iLoop=i+1;
		return true;
	}
	return false;
}
bool LaneSegInSim::FindPathToRange(LandsideVehicleInSim* pVehicle,LandsideLaneInSim* pFromLane, const CPoint2008& dFromPos, double dRangF, double dRangT,LandsideLaneNodeList& followPath)
{
	LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
	pLaneEntry->SetPosition(pFromLane,dFromPos);
	pLaneEntry->SetFromRes(mpLane);

	if(pLaneEntry->m_distInlane > dRangF)
	{
		delete pLaneEntry;
		return false;
	}	


	followPath.push_back(pLaneEntry);

	
	if(mpLane!=pFromLane)
	{
		LandsideLaneEntry* pNextLaneEntry = new LandsideLaneEntry();		
		pNextLaneEntry->SetPosition(mpLane,dRangF-pVehicle->GetLength()-100);
		pNextLaneEntry->SetFromRes(pFromLane);

		LandsideLaneExit* pLaneExit = new LandsideLaneExit();
		pLaneExit->SetPosition(pFromLane,pNextLaneEntry->GetPosition());
		pLaneExit->SetToRes(mpLane);

		followPath.push_back(pLaneExit);		
		followPath.push_back(pNextLaneEntry);
	}

	LandsideLaneExit* pLastLaneExit = new LandsideLaneExit();
	pLastLaneExit->SetPosition(mpLane,dRangF-100);
	pLastLaneExit->SetToRes(mpLane);

	followPath.push_back(pLastLaneExit);		
	return true;
}


void LaneSegInSim::removeVehicleOcy(LandsideVehicleInSim* pVeh)
{
	for(VehicleOcyList::iterator itr=mOcyVehicles.begin();itr!=mOcyVehicles.end();++itr)
	{
		if(itr->pVeh == pVeh)
		{
			mOcyVehicles.erase(itr);
			return;
		}
	}
}
void LaneSegInSim::addVehicleOcy(LandsideVehicleInSim* pVeh, double dPos)
{
	removeVehicleOcy(pVeh);
	VehicleOcy ocy;
	ocy.pVeh=pVeh; ocy.dPos = dPos;
	mOcyVehicles.push_back(ocy);
	std::sort(mOcyVehicles.begin(),mOcyVehicles.end());
}
