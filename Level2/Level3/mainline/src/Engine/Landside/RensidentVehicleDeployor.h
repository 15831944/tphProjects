#pragma once
class LandsideResidentVehicleInSim;
class ElapsedTime;
class ResidenRelatedtVehicleTypeData;
class CARCportEngine;
class ResidentRelatedVehicleTypePlan;


class ResidentVehicleDeployPlanInSim;
class LandsideFacilityObject;

class RensidentVehicleDeployorInSim
{
public:
	RensidentVehicleDeployorInSim(ResidenRelatedtVehicleTypeData*pData);
	~RensidentVehicleDeployorInSim(void);		
	void Start(CARCportEngine* pEngine);
	bool GetStartTime( ElapsedTime& t ) const;

	void GenerateVehicles(CARCportEngine* pEngine);


	void RegDeployPlan(ResidentVehicleDeployPlanInSim* pEvt);
	void UnRegDeployPlan(ResidentVehicleDeployPlanInSim* pEvt);
	bool AllDone()const{ return m_vRegPlans.empty(); }

	ElapsedTime getParkingTime()const;
	LandsideFacilityObject* getHomeBase();
	CString GetVehicleType()const;
	ResidenRelatedtVehicleTypeData* m_pData;	

	int getVehicleNum()const{ return m_vVehicles.size(); }
	LandsideResidentVehicleInSim* getVehicle(int idx){ return m_vVehicles[idx]; }
protected:	
	//the plan that deploy vehicles
	std::vector<ResidentVehicleDeployPlanInSim*> m_vRegPlans;
	//vehicles managed by this deployer
	std::vector<LandsideResidentVehicleInSim*> m_vVehicles; //
};

class ResidentRelatedVehiclTypeContainer;
class ResidentVehicleManager
{
public:
	ResidentVehicleManager(ResidentRelatedVehiclTypeContainer* pVehicles);
	~ResidentVehicleManager();
	void Start(CARCportEngine* pEngine);

	bool GetStartTime( ElapsedTime& t ) const;
protected:
	typedef std::vector<RensidentVehicleDeployorInSim*> ResidentVehicleDeployList;
	ResidentVehicleDeployList m_vList;	
};