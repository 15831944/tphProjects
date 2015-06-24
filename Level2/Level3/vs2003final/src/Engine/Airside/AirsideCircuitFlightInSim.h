#pragma once
#include ".\airsideflightinsim.h"


class CTrainingFlightData;
class AirsideSimulation;
class AirsideTrainFlightTypeManagerInSim;
class CTrainingFlightTypeManage;
class CTrainingFlightsManage;
class IFlightProcess;

class ENGINE_TRANSFER AirsideCircuitFlightInSim :
	public AirsideFlightInSim
{
public:
	enum LandingOperation
	{
		LowAndOver_Operation,
		TouchAndGo_Operaiton,
		StopAndGo_Operaiton,
		ForceAndGo_Operation,
		Normal_Operation
	};

	enum CircuitFlightState
	{
		START_PROCESS,
		INTENTION_PROCESS,
		END_PROCESS
	};
	AirsideCircuitFlightInSim(CARCportEngine *pARCPortEngine,Flight *pFlight,int nid);
	~AirsideCircuitFlightInSim(void);


	void GetNextClearance(ClearanceItem& lastItem, Clearance& newClearance);


	void InitCircuitFlight(CTrainingFlightData *pFlightData);
	void getACType (char *p_str) const;
	virtual int fits(const FlightConstraint& _fltcons) const;

	void SetFltConstraint(const FlightConstraint& fltType);

	void SetEntryStart(const ALTObjectID& objID);
	void SetExitSID(const ALTObjectID& objID);

	const ALTObjectID& GetEntryStart()const;
	const ALTObjectID& GetExitSID()const;

	void SetLandingOperation(LandingOperation emType) {m_emType = emType;}
	LandingOperation GetLandingOperation()const {return m_emType;}

	virtual bool IsCircuitFlight(){return true;}
	void SetStopTime(const ElapsedTime& eStopTime) {m_eStopTime = eStopTime;}

	ElapsedTime GetStopTime()const {return m_eStopTime;}
	void SetTouchSpeed(double dSpeed) {m_dTouchSpeed = dSpeed;}
	double GetTouchSpeed()const {return m_dTouchSpeed;}

	void SetLowHigh(double dHigh) {m_dLowHigh = dHigh;}
	double GetLowHigh()const {return m_dLowHigh;}

	void ReleaseIntension(LandingOperation op);
	void ReleaseAllIntension(){ m_nLowOver = m_nStopGo = m_nForceGo = m_nTouchGo = 0; m_intensionState = false;}
	bool LowOverDone() const{return m_nLowOver>0 ? false : true;}
	void ReleaseLowOver() { m_nLowOver--;}

	bool TouchGoDone() const{return m_nTouchGo>0  ? false : true;}
	void ReleaseTouchGo() { m_nTouchGo--;}

	bool StopGoDone() const{return m_nStopGo >0? false : true;}
	void ReleaseStopGo() { m_nStopGo--;}

	bool ForceGoDone() const{return m_nForceGo>0 ? false : true;}
	void ReleaseForceGo() { m_nForceGo--;}

	void SetCircuitState(CircuitFlightState emType) {m_state = emType;}
	CircuitFlightState GetCircuitState()const {return m_state;}

	LandingOperation GetStartLandingOperation()const;

	LandingOperation GetRandomLandingOperation()const;


	void SetTrainFlightManager(CTrainingFlightTypeManage* pTrainFlightTypeManager);
	RunwayExitInSim* GetTakeoffPosition();
	RunwayExitInSim* GetAndAssignTakeoffPosition();

	void SetTakeoffComplete(bool bComplete){m_bCircuitRouteTakeoff = bComplete;}
	void SetLandingComplete(bool bComplete){m_bCircuitRouteLanding = bComplete;}

	bool IsTakeoffComplete()const {return m_bCircuitRouteTakeoff;}
	bool IsLandingComplete()const{return m_bCircuitRouteLanding;}

	bool MoveOnCircuitComplete()const {return m_bCircuitRouteLanding&&m_bCircuitRouteTakeoff ? true : false;}
	void ClearMoveCircuitRouteState(){m_bCircuitRouteLanding = false; m_bCircuitRouteTakeoff = false;}

	void SetStartIntension(bool bStart);
	bool IntensionOn()const;

	bool isAllIntensionDone()const;
private:
	FlightConstraint m_fltCons;
	IFlightProcess* m_process;

	ALTObjectID m_entryStart;
	ALTObjectID m_exitSID;
	LandingOperation m_emType;
	CircuitFlightState m_state;

	double m_dTouchSpeed;
	double m_dLowHigh;
	AirsideTrainFlightTypeManagerInSim* m_pTrainFlightTypeManager;
	ElapsedTime m_eStopTime;

	bool m_bCircuitRouteTakeoff;
	bool m_bCircuitRouteLanding;

	int  m_nLowOver;
	int m_nTouchGo;
	int m_nStopGo;
	int m_nForceGo;

	bool m_intensionState;
};

class AirsideCircuiteFlightInSimGenerator
{
public:
	AirsideCircuiteFlightInSimGenerator(InputTerminal* pInputTerminal,AirsideSimulation *pAirsideSimulation,int nid);
	~AirsideCircuiteFlightInSimGenerator();
	
	void GenerateCircuiteFlight(CARCportEngine *pARCPortEngine,CTrainingFlightsManage* pTrainFlightManager);
	
private:
	void InitializeFlight(CARCportEngine *pARCPortEngine,CTrainingFlightData *pFlightData,CTrainingFlightTypeManage* pTrainFlightTypeManager);

private:
	InputTerminal *m_pInputTerminal;
	AirsideSimulation *m_pAirsideSimulation;
	int m_nFlightID;
};
