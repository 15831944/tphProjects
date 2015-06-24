#include "StdAfx.h"
#include ".\rensidentvehicledeployor.h"
#include "Common\CPPUtil.h"
#include "Engine\LandsideSimulation.h"
#include "landside\ResidentVehiclTypeContainer.h"
#include "../LandsideResidentVehicleInSim.h"
#include "../EVENT.H"
#include "../ARCportEngine.h"


class ResidentVehicleDeployPlanInSim : public Event
{
public:	
	ResidentVehicleDeployPlanInSim(ResidentRelatedVehicleTypePlan * pPlan, RensidentVehicleDeployorInSim* _pDeployer)
	{
		iIndex = 0;
		m_pPlan = pPlan;
		pDeployer = _pDeployer;
		//m_pLandsideSimulation = pSim;
		setTime(pPlan->GetTimeRange().GetStartTime());
	}
	bool DeployVehicle(LandsideSimulation* pSim )
	{
		for(int i=0;i<pDeployer->getVehicleNum();i++)
		{
			LandsideResidentVehicleInSim* pVehicle = pDeployer->getVehicle(i);
			if(pVehicle->IsWaitOnBase())	
			{
				if( pVehicle->BeginServicePlan(m_pPlan,pSim))
				{
					return true;
				}			
			}
		}
		return false;
	}

	
	// Main event processing routine for each Event type.
	virtual int process (CARCportEngine* pEngine )
	{
		DeployVehicle( pEngine->GetLandsideSimulation() );
		iIndex ++;		

		if(iIndex >= m_pPlan->GetNumDeploy())
		{
			pDeployer->UnRegDeployPlan(this);
			return TRUE;
		}
		else
		{
			setTime(getTime()+m_pPlan->GetHeadwayTime());
			addEvent();
		}		
		return FALSE;
	}

	//For shut down event
	virtual int kill (void){ return FALSE; }
	//It returns event's name
	virtual const char *getTypeName (void) const{ return _T("Deploy Resident Vehicle Start Service"); }
	//It returns event type
	virtual int getEventType (void) const{  return NormalEvent; }
protected:
	ResidentRelatedVehicleTypePlan* m_pPlan;
	int iIndex;
	RensidentVehicleDeployorInSim* pDeployer;
	//LandsideSimulation			*m_pLandsideSimulation;
};

RensidentVehicleDeployorInSim::RensidentVehicleDeployorInSim( ResidenRelatedtVehicleTypeData*pData )
:m_pData(pData)
{}

RensidentVehicleDeployorInSim::~RensidentVehicleDeployorInSim(void)
{
	cpputil::deletePtrVector(m_vRegPlans);
}


class DeployGenVehicles : public Event
{
public:
	DeployGenVehicles(RensidentVehicleDeployorInSim* pD){ m_pDeploy = pD; }

	// Main event processing routine for each Event type.
	//Modified by Xie Bo 2002.3.22  if return TRUE will delete this event
	virtual int process (CARCportEngine* pEngine ) { m_pDeploy->GenerateVehicles(pEngine); return TRUE; }

	//For shut down event
	virtual int kill (void) { return TRUE; }

	//It returns event's name
	virtual const char *getTypeName (void) const { return _T("Generate Vehicles"); }

	//It returns event type
	virtual int getEventType (void) const { return NormalEvent; }
protected:
	RensidentVehicleDeployorInSim* m_pDeploy;
};



void RensidentVehicleDeployorInSim::Start(CARCportEngine* pEngine )
{
	ElapsedTime t = pEngine->GetLandsideSimulation()->m_estSimulationStartTime;
	//generate vehicles
	Event* pE = new DeployGenVehicles(this);
	pE->setTime(t);
	pE->addEvent();
	//start deploy plan
	for(int i=0;i<m_pData->GetCount();i++)
	{
		ResidentRelatedVehicleTypePlan* pPlan = m_pData->GetItem(i);
		if(pPlan->GetNumDeploy()>0)
		{
			ResidentVehicleDeployPlanInSim* pDeployEvt = new ResidentVehicleDeployPlanInSim(pPlan, this);
			RegDeployPlan(pDeployEvt);
			pDeployEvt->addEvent();
		}		
	}
}

bool RensidentVehicleDeployorInSim::GetStartTime( ElapsedTime& t ) const
{
	if(m_pData->GetCount()<1)
		return false;

	ElapsedTime earlyT = ElapsedTime::Max();
	for(int i=0;i<m_pData->GetCount();i++)
	{
		ResidentRelatedVehicleTypePlan* pData =m_pData->GetItem(i);
		earlyT = MIN(earlyT,pData->GetTimeRange().GetStartTime());
	}
	t = earlyT;
	return true;
}

ElapsedTime RensidentVehicleDeployorInSim::getParkingTime() const
{
	double tMin = m_pData->GetProDist().GetProbDistribution()->getRandomValue();
	return ElapsedTime(tMin*60);
}

LandsideFacilityObject* RensidentVehicleDeployorInSim::getHomeBase()
{
	return m_pData->GetHomeBase();
}

CString RensidentVehicleDeployorInSim::GetVehicleType() const
{
	return m_pData->GetVehicleType();
}

void RensidentVehicleDeployorInSim::RegDeployPlan( ResidentVehicleDeployPlanInSim* pEvt )
{
	m_vRegPlans.push_back(pEvt);
}

void RensidentVehicleDeployorInSim::UnRegDeployPlan( ResidentVehicleDeployPlanInSim* pEvt )
{
	std::vector<ResidentVehicleDeployPlanInSim*>::iterator itr;
	itr = std::find(m_vRegPlans.begin(),m_vRegPlans.end(),pEvt);
	if(itr!=m_vRegPlans.end())
		m_vRegPlans.erase(itr);
}

void RensidentVehicleDeployorInSim::GenerateVehicles( CARCportEngine* pEngine )
{
	for(int i=0;i<m_pData->GetNum();i++)
	{
		LandsideResidentVehicleInSim* pVehicle = new LandsideResidentVehicleInSim(this);	
		if( pVehicle->InitBirth(pEngine) )
		{
			m_vVehicles.push_back(pVehicle);
			pEngine->GetLandsideSimulation()->AddVehicle(pVehicle);
		}
		else
			delete pVehicle;
	}
}


ResidentVehicleManager::~ResidentVehicleManager()
{
	cpputil::deletePtrVector(m_vList);
}

ResidentVehicleManager::ResidentVehicleManager(  ResidentRelatedVehiclTypeContainer* pVehicles )
{
	for(int i=0;i<pVehicles->GetItemCount();i++){
		RensidentVehicleDeployorInSim* pDeployer = new RensidentVehicleDeployorInSim(pVehicles->GetItem(i));
		m_vList.push_back(pDeployer);
	}
}

void ResidentVehicleManager::Start( CARCportEngine* pEngine )
{
	for(size_t i=0;i<m_vList.size();i++){
		RensidentVehicleDeployorInSim* pDeployer = m_vList[i];
		pDeployer->Start(pEngine);
	}
}

bool ResidentVehicleManager::GetStartTime( ElapsedTime& t ) const
{
	bool bGet = false;

	ElapsedTime earlyT;;
	for(size_t i=0;i<m_vList.size();i++){
		RensidentVehicleDeployorInSim* pDeploy = m_vList[i];
		ElapsedTime tS;
		if(pDeploy->GetStartTime(tS)){
			if(!bGet){
				earlyT = tS;
				bGet = true;
			}
			else{
				earlyT = MIN(earlyT, tS);
			}
			bGet = true;
		}		
	}
	if(bGet){
		t = earlyT;
	}
	return bGet;
}
