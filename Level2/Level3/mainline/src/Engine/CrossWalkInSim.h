//----------------------------------------------------------------------------------
//						Information:		cross walk use in engine
//						Author:				sky.wen
//						Date:				Jun	29, 2011
//						Purpose:			Provide some engine logic method
//----------------------------------------------------------------------------------
#pragma once
#include "LandsideObjectInSim.h"
#include "LandsideTrafficSystem.h"
#include "core\SAgent.h"
#include "Common\LANDSIDE_BIN.h"
#include "Landside\LandsideCrosswalk.h"
#include "Results\OutputLandside.h"
#include "landside\CrossWalkLogEntry.h"
class PaxLandsideBehavior;
class LandsideVehicleInSim;
class CPath2008;
class LandsideCrosswalk;

class CrossWalkPaxLog
{
public:
	CrossWalkPaxLog()
	{
		m_nPaxID = -1;
	}

	bool Alive(const ElapsedTime& eTime)const
	{
		if (eTime < m_startTime || eTime > m_endTime)
		{
			return false;
		}
		return true;
	}

	int m_nPaxID;
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
};

//class SignalCrossStateChange : public SSignal
//{
//public:
//	virtual CString getDesc(){ return _T("Cross Walk Change"); }
//};
//----------------------------------------------------------------------------------
//Summary:
//		handle conflict passenger with vehicle
//----------------------------------------------------------------------------------

class CCrossWalkInSim : public CLandsideWalkTrafficObjectInSim, public SAgent
{
public:
	CCrossWalkInSim(LandsideCrosswalk* pCrossWalk);
	virtual ~CCrossWalkInSim();

	virtual CCrossWalkInSim* toCrossWalk(){ return this; }

	//------------------------------------------------------------------------
	//Summary:
	//		retrieve traffic type
	//------------------------------------------------------------------------
	virtual CLandsideWalkTrafficObjectInSim::TrafficObjectType GetTrafficType()const;

	//-----------------------------------------------------------------------
	//Summary:
	//		retrieve crosswalk center path
	//-----------------------------------------------------------------------
	virtual CPath2008 GetCenterPath()const;

	//----------------------------------------------------------------------
	//Summary:
	//		retrieve crosswalk right path
	//----------------------------------------------------------------------
	virtual CPath2008 GetRightPath()const;

	//----------------------------------------------------------------------
	//Summary:
	//		retrieve crosswalk left path
	//----------------------------------------------------------------------
	virtual CPath2008 GetLeftPath()const;

	//----------------------------------------------------------------------
	//Summary:
	//		check cross walk whether valid for pax
	//----------------------------------------------------------------------
	bool PaxValid()const;
	//----------------------------------------------------------------------
	//Summary:
	//		check crosswalk whether valid for vehicle
	//----------------------------------------------------------------------
	bool VehicleValid()const;

	//----------------------------------------------------------------------
	//Summary:
	//		step on crosswalk
	//Parameter:
	//		time[out]: passed time
	//		routePath: move on path
	//		pFreeMoveLogic[in]: person free moving logic
	//---------------------------------------------------------------------
	void StepOnCrossWalk(ElapsedTime& time,LandsideTrafficGraphVertexList& routePath,CFreeMovingLogic* pFreeMoveLogic);
	//-------------------------------------------------------------------
	//Summary:
	//		wake up vehicle and need implement
	//-------------------------------------------------------------------
	void NoticefyVehicle(const ElapsedTime& t);
	//-------------------------------------------------------------------
	//Summary:
	//		wake up person and need implement
	//--------------------------------------------------------------------
	void NoticefyPax(const ElapsedTime& t);

	//-------------------------------------------------------------------
	//Summary:
	//		vehicle operation on crosswalk
	//------------------------------------------------------------------
	void AddVehicleOccupied(LandsideVehicleInSim* pVehicleInSim);
	void ReleaseVehilceOccupied(LandsideVehicleInSim* pVehicleInSim);
	bool IsVehicleOccupied(LandsideVehicleInSim* pVehicleInSim)const;

	//void AddVehicleApproach(LandsideVehicleInSim* pVehicleInSim);
	//void ReleaseVehicleApproach(LandsideVehicleInSim* pVehicleInSim);
	
	//--------------------------------------------------------------
	//Summary:
	//		person operation on crosswalk
	//-------------------------------------------------------------
	void AddPaxOccupied(CFreeMovingLogic* pLandsideBehavior);
	void ReleasePaxOccupied(CFreeMovingLogic* pLandsideBehavior);
	void AddPaxApproach(CFreeMovingLogic* pLandsideBehavior);
	void ReleasePaxApproach(CFreeMovingLogic* pLandsideBehavior);

	bool CanVehicleCross(const ElapsedTime& eTime)const; 

	int getAtStretchID()const;
	double getAtStretchPos()const;
	double getHalfWidth()const;
	CrossType getCrossType(){return m_pCrossWalk->getCrossType();}
	Cross_LightState getPaxLightState(){return m_ePaxLightState;}
	void setPaxLightState(Cross_LightState lightState);
	void pressCtrlButton();

	virtual void OnActive(CARCportEngine*pEngine);
	virtual void OnTerminate(CARCportEngine*pEngine);
	void InitLogEntry(OutputLandside* pOutput );
	void WriteLog(const ElapsedTime& t);

	int GetLinkIntersectionID();
	int GetLinkGroup();

	void Kill(OutputLandside* pOutput, const ElapsedTime& t);

	
	//---------------------------------------------------------------------
	//Summary:
	//		check input time that croswalk whether has passenger
	//---------------------------------------------------------------------
	bool CorsswalkIdle(const ElapsedTime& eTime)const;

	//---------------------------------------------------------------------
	//Summary:
	//		write passenger move on crosswalk information
	//--------------------------------------------------------------------
	void WritePassengerOnCrosswalk(int nPaxID,const ElapsedTime& tEnterTime,const ElapsedTime& tExitTime,bool bCreate = true);

	LandsideCrosswalk* GetCrosswalk() {return m_pCrossWalk;}
private:
	LandsideCrosswalk* m_pCrossWalk;		//input crosswalk data

	std::vector<CFreeMovingLogic*> m_PaxOccupiedList;//occupied crosswalk person
	std::vector<CFreeMovingLogic*> m_PaxApproachList;//approach occupied crosswalk person
	std::vector<LandsideVehicleInSim*> m_VehicleOccupiedList;//occupied crosswalk vehicl
	//std::vector<LandsideVehicleInSim*> m_VehicleapproachList;//approach occupied crosswalk vehicle

	Cross_LightState m_ePaxLightState;
	long m_lBufferTime;
	long m_lGreenTime;

	bool m_bKilled;
	
	//pay attention that crosswalk's can have more than on same paxid
	std::vector<CrossWalkPaxLog> m_vCorssWalkPax;

public:
	CCrossWalkLogEntry m_logEntry;	
};




