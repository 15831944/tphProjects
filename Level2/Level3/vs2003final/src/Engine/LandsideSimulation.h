#pragma once

#include "Common/elaptime.h"
#include "LandsideTrafficSystem.h"

class LandsideVehicleInSim;

class InputLandside;
class OutputLandside;
class LandsideResourceManager;
class ElapsedTime;
class CARCportEngine;

class LandsideParkingLotInSim;
class LandsideExternalNodeInSim;
class LandsideCurbSideInSim;
class MobLogEntry;
class CIntersectionTrafficControlManagement;
class LandsideItineratVehicleGenerator;
class CVehicleItemDetail;
class LandsideVehicleProperty;
class CHierachyName;
class ResidentVehicleManager;
class LandsidePaxVehicleManager;
class LandsideNonPaxVehicleManager;
class CFacilityBehaviorList;
class LandsideVehicleLeadToInSimManager;
class LandsidePaxVehicleInSim;

class ALTObjectID;
class CWalkwayInSim;

class LandsideTerminalLinkageInSimManager;
class LandsidePaxTypeLinkageInSimManager;
class LandsideIndividualArrivalPaxVehicleConvergerManager;
class LandsideVehicleLeadToInSimManager; 
class IFloorDataSet;
class NonPaxVehicleAssignment;
class VehicleOperationNonPaxRelatedContainer;

class CFacilityBehaviorsInSim;
class LandsideSimTimeStepManager;
class LandsidePaxVehicleInSim1;

class LandsideSimulation
{
public:
	LandsideSimulation(InputLandside* pInput, OutputLandside* pOutput);
	~LandsideSimulation(void);

	//create log files
	virtual bool Initialize(CARCportEngine *pEngine,const IFloorDataSet& floorlist,CString _sprjPath); // do some initialization before engine start
	virtual void Start(CARCportEngine *pEngine);
	virtual void End(const ElapsedTime& t, bool bCancel);

	void SaveLog(CString _sProjPath);

public:
	void StartTrafficCtrl(const ElapsedTime& t);
	void DeactiveTrafficCtrl(const ElapsedTime& t);
	

	bool IsLeftDrive()const;

	LandsideVehicleInSim* FindPaxVehicle(int nPaxId);

	int GetLinkTerminalFloor(int landsidelevel)const;
	int GetLinkLandsideFloor(int terminalFloor)const;
	////all resource
	LandsideResourceManager* GetResourceMannager(){ return m_resManagerInst; }
	CLandsideTrafficSystem* GetLandsideTrafficeManager()const {return m_pLandsideWalkwaySystem;}

	//LandsideCurbSideInSim* AssignCurbside( const MobLogEntry& mob );

	LandsideTerminalLinkageInSimManager *GetLandsideTerminalLinkageManager();
	LandsidePaxTypeLinkageInSimManager* GetLandsidePaxtTypeLinkageManager() {return m_pLandisdePaxTypeLinkage;}
	void SetEstSimStartEndTime(const ElapsedTime& estStart, const ElapsedTime& estEnd);
	ElapsedTime m_estSimulationStartTime;
	ElapsedTime m_estSimulationEndTime;

	bool IsAllVehicleTerminate()const;

	OutputLandside* getOutput()const{ return m_pOutput; }
	InputLandside* getInput()const{ return m_pInput; }

	LandsideVehicleProperty* getVehicleProp(const CHierachyName& sName);

	void AddVehicle(LandsideVehicleInSim* pVehicle);

	void GenerateIndividualArrivalPaxConverger(CARCportEngine *pEngine,int nPassenger,const ElapsedTime& _currentTime);
	void GenerateIndividualArrivalVehicleConverger(CARCportEngine *pEngine,LandsidePaxVehicleInSim* pVehicle,const ElapsedTime& _currentTime);

	LandsideVehicleLeadToInSimManager* GetLandsideLeadToManager() {return m_pLandsideObjectLeadManager;}
	LandsideIndividualArrivalPaxVehicleConvergerManager* GetLandsideConvergerManager(){return m_pIndividualArrivalPaxVehicleConverger;}

	NonPaxVehicleAssignment* GetLandsideNonPaxVehicleAssignment() {return m_pNonPaxVehicleAssignmnet;}
	VehicleOperationNonPaxRelatedContainer* GetLandsideNonPaxRelatedCon() {return m_pNonPaxRelatedCon;}

	CFacilityBehaviorsInSim *getFacilityBehaviors(){return m_pFacilityBehaviorsInSim; }

protected:
	void InitLog(CString _sProjPath);

	std::vector<int> m_vTerminalFloorLinkage;   //the landside level linkage to terminal Floors;

	InputLandside* m_pInput;
	OutputLandside* m_pOutput;	

	LandsideResourceManager* m_resManagerInst;		
	CLandsideTrafficSystem*	m_pLandsideWalkwaySystem;
	CIntersectionTrafficControlManagement *pTrafficCtrlList;//traffic ctrl started flag


	LandsideTerminalLinkageInSimManager *m_pLandsideTerminalLinkage;
	LandsidePaxTypeLinkageInSimManager *m_pLandisdePaxTypeLinkage;

	LandsideVehicleLeadToInSimManager*	m_pLandsideObjectLeadManager; 
	LandsideIndividualArrivalPaxVehicleConvergerManager* m_pIndividualArrivalPaxVehicleConverger;

protected:
	ResidentVehicleManager* m_pResidentVehicles;
	LandsidePaxVehicleManager* m_pPaxVehicles;
	LandsideNonPaxVehicleManager* m_pNonPaxVehicles;
	NonPaxVehicleAssignment* m_pNonPaxVehicleAssignmnet;
	VehicleOperationNonPaxRelatedContainer* m_pNonPaxRelatedCon;
	//vehicle created
	std::vector<LandsideVehicleInSim*> m_vVehicleList;
	CFacilityBehaviorsInSim *m_pFacilityBehaviorsInSim;

	LandsideSimTimeStepManager* m_pTimeStepManager;

public:
	static ElapsedTime tAwarenessTime;
};

