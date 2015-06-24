#include "StdAfx.h"
#include ".\landsidesimulation.h"
#include "LandsideVehicleInSim.h"
#include "LandsideResourceManager.h"
#include "LogContainer.h"
#include "ARCportEngine.h"
#include "landside/VehicleDistribtionList.h"
#include "Landside/InputLandside.h"
#include "Landside/CLandSidePublicVehicleType.h"
#include "terminal.h"
#include "Results/OutputLandside.h"
#include "SimEngineConfig.h"
#include "LandsideCurbsideInSim.h"
#include "Landside/IntersectionTrafficCtrlList.h" 
#include "Common\ALTObjectID.h"
#include "LandsideIntersectionInSim.h"
#include "LandsideExternalNodeInSim.h"
#include "CrossWalkInSim.h"
#include "LandsideItinerantVehicleInSim.h"
#include "Common/CPPUtil.h"
#include "ARCportEngine.h"
#include "landside/ResidentVehiclTypeContainer.h"
#include "landside/VehicleGroupProperty.h"
#include "landside/LandsideResidentRelatedContainer.h"
#include "landside/LandsideVehicleAssignment.h"
#include "Landside/VehicleOperationNonPaxRelatedContainer.h"
#include "Landside/NonPaxVehicleAssignment.h"
//#include "landside/LandsideBulkVehicleInSim.h"
//////////////////////////////////////////////////////////////////////////
#include "landside/RensidentVehicleDeployor.h"
#include "landside/LandsideNonPaxVehicleManager.h"
#include "landside/LandsidePaxVehicleManager.h"
#include "landside/FacilityBehavior.h"

#include "WalkwayInSim.h"

#include "LandsideTerminalLinkageInSim.h"
#include "LandsidePaxTypeLinkageInSimManager.h"
#include "LandsideCellPhoneProcess.h"
#include "LandsideVehicleLeadToInSimManager.h"

#include "LandsideBusStationInSim.h"
#include "LandsideResidentPaxDirectionManager.h"
#include "LandsideTaxiQueueInSim.h"
#include "Common/IFloorDataSet.h"
#include "FacilityBehaviorsInSim.h"
#include "Common/RenderFloor.h"
#include "LandsideSimTimeStepManager.h"
#include "LandsideTaxiPoolInSim.h"


typedef std::vector<int> IntList;
LandsideSimulation::LandsideSimulation(InputLandside* pInput, OutputLandside* pOutput)
{
	m_pInput = pInput;
	m_pOutput = pOutput;
	m_resManagerInst = new LandsideResourceManager();
	m_pLandsideWalkwaySystem = new CLandsideTrafficSystem();
	//m_pEngine = NULL;
	pTrafficCtrlList = NULL;
	//mpItinerantVehicles = new LandsideItineratVehicleGenerator();
	m_pResidentVehicles = NULL;	
	m_pPaxVehicles = NULL;
	m_pNonPaxVehicles = NULL;
	m_pNonPaxVehicleAssignmnet = NULL;
	m_pFacilityBehaviorsInSim = NULL;
	m_pLandsideTerminalLinkage = NULL;
	m_pIndividualArrivalPaxVehicleConverger = new LandsideIndividualArrivalPaxVehicleConvergerManager();
	m_pLandsideObjectLeadManager = NULL;
	m_pTimeStepManager = NULL;

}

LandsideSimulation::~LandsideSimulation(void)
{	
	cpputil::deletePtrVector(m_vVehicleList);
	cpputil::autoPtrReset(m_resManagerInst);
	cpputil::autoPtrReset(m_pLandsideWalkwaySystem);
	cpputil::autoPtrReset(pTrafficCtrlList);
	//cpputil::autoPtrReset(mpItinerantVehicles);	
	cpputil::autoPtrReset(m_resManagerInst);
	cpputil::autoPtrReset(m_pResidentVehicles);	
	cpputil::autoPtrReset(m_pPaxVehicles);
	cpputil::autoPtrReset(m_pNonPaxVehicles);
	cpputil::autoPtrReset(m_pInput);
	cpputil::autoPtrReset(m_pFacilityBehaviorsInSim);
	cpputil::autoPtrReset(m_pIndividualArrivalPaxVehicleConverger);
	cpputil::autoPtrReset(m_pLandsideObjectLeadManager);
	cpputil::autoPtrReset(m_pNonPaxVehicleAssignmnet);
	cpputil::autoPtrReset(m_pNonPaxRelatedCon);
	cpputil::autoPtrReset(m_pTimeStepManager);
}




void LandsideSimulation::Start(CARCportEngine *pEngine)
{
	m_pResidentVehicles = new ResidentVehicleManager(m_pInput->getResidentVehiclePlan());
	m_pPaxVehicles = new LandsidePaxVehicleManager();
	m_pNonPaxVehicles = new LandsideNonPaxVehicleManager();


	//init vehicles and update est start time
	ElapsedTime tmpT;
	if( m_pResidentVehicles->GetStartTime(tmpT) ){
		m_estSimulationStartTime = MIN(tmpT,m_estSimulationStartTime);
	}
	if( m_pPaxVehicles->Init(pEngine,tmpT) ){
		m_estSimulationStartTime = MIN(tmpT,m_estSimulationStartTime);
	}
	if( m_pNonPaxVehicles->Init(pEngine,tmpT) ){
		m_estSimulationStartTime = MIN(tmpT,m_estSimulationStartTime);
	}


	m_pResidentVehicles->Start(pEngine);
	m_pPaxVehicles->Start(m_estSimulationStartTime);
	m_pNonPaxVehicles->Start(m_estSimulationStartTime);

	StartTrafficCtrl(m_estSimulationStartTime);

	//update taxi of Pool
	for(size_t i=0;i<GetResourceMannager()->m_vTaxiPools.size();i++)
	{
		LandsideTaxiPoolInSim* pPool = GetResourceMannager()->m_vTaxiPools.at(i);
		pPool->UpdateOrderingVehicle(m_estSimulationStartTime);
	}


	m_pTimeStepManager = new LandsideSimTimeStepManager;
	m_pTimeStepManager->Initialize();

}


void LandsideSimulation::StartTrafficCtrl(const ElapsedTime& t)
{
	if(pTrafficCtrlList)
		return;
	pTrafficCtrlList=new CIntersectionTrafficControlManagement;
	if(!m_pInput || !m_resManagerInst || !pTrafficCtrlList)
		return;

	pTrafficCtrlList->SetInputLandside(m_pInput);
	pTrafficCtrlList->ReadData(-1);
	std::vector<LandsideIntersectionInSim*> &intersections=m_resManagerInst->m_Intersections;
	for (int i=0;i<(int)intersections.size();i++)
	{
		LandsideIntersectionInSim *pIntersection=(LandsideIntersectionInSim *)intersections.at(i);
		const ALTObjectID &IntersectionName=pIntersection->GetIntersectionName();
		CIntersectionTrafficControlIntersection *pTrafficCtrlNode=pTrafficCtrlList->FindMatchCtrlList(IntersectionName);
		if(pTrafficCtrlNode==NULL)
		{
			continue;
		}
		pIntersection->GenerateCtrlEvents(pTrafficCtrlNode,m_resManagerInst,m_pOutput,t,m_estSimulationEndTime);
	}    
	for (int k=0;k<m_resManagerInst->m_vTrafficObjectList.getCount();k++)
	{
		if( CCrossWalkInSim* pCross =m_resManagerInst->m_vTrafficObjectList.getItem(k)->toCrossWalk())
		{
			if (pCross->getCrossType()==Cross_Pelican)
			{
				pCross->setPaxLightState(CROSSLIGHT_RED);
			}
			pCross->InitLogEntry(m_pOutput);
		}
	}
}

void LandsideSimulation::DeactiveTrafficCtrl(const ElapsedTime& t)
{
	std::vector<LandsideIntersectionInSim*> &intersections=m_resManagerInst->m_Intersections;
	for (int i=0;i<(int)intersections.size();i++)
	{
		((LandsideIntersectionInSim *)intersections.at(i))->Deactive();
	}

	for (int k=0;k<m_resManagerInst->m_vTrafficObjectList.getCount();k++)
	{
		if( CCrossWalkInSim* pCross =m_resManagerInst->m_vTrafficObjectList.getItem(k)->toCrossWalk())
		{
			pCross->DeActive();
		}
	}
}

bool LandsideSimulation::Initialize( CARCportEngine *pEngine, const IFloorDataSet& floorlist, CString _sprjPath )
{
	InitLog(_sprjPath);
	LandsideVehicleInSim::nLastVehicleId =0;
	//init all inputs data

	m_pFacilityBehaviorsInSim = new CFacilityBehaviorsInSim;
	m_pFacilityBehaviorsInSim->Initialize(&m_pInput->getObjectList(),m_resManagerInst);
	//generate vehicles
	m_resManagerInst->Init(m_pInput,m_pFacilityBehaviorsInSim->getFacilityBehavior());

	//
	m_pLandsideWalkwaySystem->Init(m_resManagerInst);


	//init resident vehicle lead to information
	//
	LandsideResidentPaxDirectionManager residentPaxManager;
	residentPaxManager.initialize(m_pInput->getResidentVehiclePlan(),m_resManagerInst);

	//initialize the arrival passenger destination ,
	//figure out the passenger type and its destination
	//Curbside, Bus station, parking lot
	m_pLandsideTerminalLinkage = new LandsideTerminalLinkageInSimManager;
	m_pLandsideTerminalLinkage->SetProcessorList(pEngine->getTerminal()->GetProcessorList());

	m_pLandsideTerminalLinkage->Initialize(m_pFacilityBehaviorsInSim->getFacilityBehavior(), pEngine->getProjectCommon(), m_pInput->getObjectList(), m_resManagerInst);

	m_pLandisdePaxTypeLinkage = new LandsidePaxTypeLinkageInSimManager;
	m_pLandisdePaxTypeLinkage->Initialize(m_pInput->getNonResidentVehiclePlan() ,m_pInput->getObjectList());

	m_pLandsideObjectLeadManager = new LandsideVehicleLeadToInSimManager();
	m_pLandsideObjectLeadManager->Inilization(m_pInput->getNonResidentVehiclePlan(),m_pInput->getObjectList());


	m_pNonPaxVehicleAssignmnet = new NonPaxVehicleAssignment();
	m_pNonPaxVehicleAssignmnet->ReadData(-1);

	m_pNonPaxRelatedCon = new VehicleOperationNonPaxRelatedContainer();
	m_pNonPaxRelatedCon->ReadData(m_pInput);
	//init link terminal 
	m_vTerminalFloorLinkage.clear();
	for(int i=0;i<floorlist.GetCount();i++)
	{
		const CRenderFloor* pFloor = floorlist.GetFloor(i);
		int nLink = pFloor->linkToTerminalFloor();
		m_vTerminalFloorLinkage.push_back(nLink);
	}

	return true;
}
void LandsideSimulation::End(CARCportEngine *pEngine,const ElapsedTime& endTime,bool bCancel)
{
	//flush vehicles
	for(size_t i=0;i<m_vVehicleList.size();i++)
	{
		LandsideVehicleInSim* pVehicle = m_vVehicleList[i];
		if(!bCancel && !pVehicle->IsTerminated())
		{
			//ASSERT(FALSE);
		}
		pVehicle->Kill(this,endTime);
	}


	std::vector<LandsideIntersectionInSim *> &intersections=m_resManagerInst->m_Intersections;
	for (size_t j=0;j<intersections.size();j++)
	{
		LandsideIntersectionInSim * pIntersect  = intersections.at(j);
		pIntersect->Kill(m_pOutput,endTime);		
	}	

	for (int k=0;k<m_resManagerInst->m_vTrafficObjectList.getCount();k++)
	{
		if( CCrossWalkInSim* pCross =m_resManagerInst->m_vTrafficObjectList.getItem(k)->toCrossWalk())
		{
			pCross->Kill(m_pOutput,endTime);			
		}
	}

	//flush on pax
	m_resManagerInst->FlushOnResourcePax(pEngine,endTime);
	
}

void LandsideSimulation::SaveLog( CString _sProjPath )
{	
	m_pOutput->m_SimLogs.SaveLogs(_sProjPath);
}

void LandsideSimulation::InitLog( CString _sProjPath )
{
	m_pOutput->m_SimLogs.CreateLogFiles(_sProjPath);
}

void LandsideSimulation::SetEstSimStartEndTime( const ElapsedTime& estStart, const ElapsedTime& estEnd )
{
	m_estSimulationStartTime = estStart;
	m_estSimulationEndTime = estEnd;
}

LandsideVehicleInSim* LandsideSimulation::FindPaxVehicle( int nPaxId )
{
	for(int i=0;i<(int)m_vVehicleList.size();i++)
	{
		LandsideVehicleInSim*pV = m_vVehicleList[i];
		if(pV->IsPaxsVehicle(nPaxId))
			return pV;
	}
	return NULL;
}

bool LandsideSimulation::IsAllVehicleTerminate() const
{
	for(size_t i=0;i<m_vVehicleList.size();i++)
	{
		LandsideVehicleInSim*pVeh = m_vVehicleList[i];
		if(!pVeh->IsTerminated())
			return false;
	}
	if(!m_pNonPaxVehicles->IsEmpty()){
		return false;
	}
	if(!m_pPaxVehicles->IsEmpty()){
		return false;
	}

	return true;
}



ElapsedTime LandsideSimulation::tAwarenessTime  = ElapsedTime(0.2);;




LandsideVehicleProperty* LandsideSimulation::getVehicleProp( const CHierachyName& sName )
{
	if(LandsideVehicleGroupProperty* pProper =  m_pInput->getVehicleGroupProperty())
	{
		return pProper->findBestMatch(sName);
	}
	return NULL;
}

void LandsideSimulation::AddVehicle( LandsideVehicleInSim* pVehicle )
{
	m_vVehicleList.push_back(pVehicle);
}


LandsideTerminalLinkageInSimManager * LandsideSimulation::GetLandsideTerminalLinkageManager()
{
	return m_pLandsideTerminalLinkage;
}


void LandsideSimulation::GenerateIndividualArrivalPaxConverger(CARCportEngine *pEngine, int nPassengerID,const ElapsedTime& _currentTime )
{
	PaxVehicleEntryInfo vehicleInfo;
	if(m_pPaxVehicles->GetVehicleInfo(nPassengerID,vehicleInfo))
	{
		MobLogEntry* pLogEntry = vehicleInfo.GetLogEntry(nPassengerID);
		CMobileElemConstraint mobconst;
		mobconst = PaxVehicleEntryInfo::getConstrain( *pLogEntry, pEngine);
	
		//first item parking lot
		
		if(vehicleInfo.pFirstRoute)//have start route
		{
			/*const LandsideFacilityObject& pFacilityObject = vehicleInfo.m_pFirstRoute->GetFacilityObject();
			if(pFacilityObject.GetType() == LandsideFacilityObject::Landside_Parklot_Facility)
			{
				LandsideLayoutObjectInSim* pParkingLotObj = GetResourceMannager()->getLayoutObjectInSim(pFacilityObject.GetFacilityID());
				if(pParkingLotObj)
				{
					m_pIndividualArrivalPaxVehicleConverger->InsertConverger(nPassengerID,pLogEntry->getID(),pParkingLotObj->getInput()->getName());
					return;
				}
			}*/
		}

		//non-parking lot, it could be landside, for individual arrival passenger
		//curbside
		//find out the curbside A that the passenger could lead to 
		//find out the curbside B that the vehicle could move to
		//then, figure out the curbside shared both
		CHierachyName vehicleType = vehicleInfo.vehicleType;
		LandsidePaxVehicleLinkageInSim* pBestItem = m_pLandisdePaxTypeLinkage->getBestFitItem(mobconst,vehicleType);
		if (pBestItem)
		{
			std::vector<LandsideObjectLinkageItemInSim> vAllObject;
			pBestItem->GetAllObjectCanLeadTo(_currentTime, vAllObject);

			ALTObjectID altObj;
			LandsideObjectLinkageItemInSim fitObjetct(altObj);
			if(m_pLandsideTerminalLinkage->GetRandomObject(vAllObject,fitObjetct))
			{
				m_pIndividualArrivalPaxVehicleConverger->InsertConverger(nPassengerID,pLogEntry->getID(),fitObjetct.getObject());
			}
		}
		
	}
	else
	{
		LandsideVehicleInSim* pVehicle = FindPaxVehicle(nPassengerID);
		//ASSERT(pVehicle);
		if (pVehicle)
		{
			LandsidePaxVehicleInSim* pPaxVehicle = (LandsidePaxVehicleInSim*)pVehicle;
			GenerateIndividualArrivalVehicleConverger(pEngine,pPaxVehicle,_currentTime);
		}
	}
}

void LandsideSimulation::GenerateIndividualArrivalVehicleConverger( CARCportEngine *pEngine,LandsidePaxVehicleInSim* pVehicle,const ElapsedTime& _currentTime )
{
	size_t nSize = pVehicle->getEntryInfo().vPaxEntryList.size();
	for (size_t i = 0; i < nSize; i++)
	{
		MobLogEntry logEntry = pVehicle->getEntryInfo().vPaxEntryList.at(i);
		CMobileElemConstraint mobconst;
		mobconst = PaxVehicleEntryInfo::getConstrain( logEntry, pEngine);


		//first item parking lot

		//if(pVehicle->getEntryInfo().m_pFirstRoute)//have start route
		//{
		//	const LandsideFacilityObject& pFacilityObject = pVehicle->getEntryInfo().m_pFirstRoute->GetFacilityObject();
		//	if(pFacilityObject.GetType() == LandsideFacilityObject::Landside_Parklot_Facility)
		//	{
		//		LandsideLayoutObjectInSim* pParkingLotObj = GetResourceMannager()->getLayoutObjectInSim(pFacilityObject.GetFacilityID());
		//		if(pParkingLotObj)
		//		{
		//			m_pIndividualArrivalPaxVehicleConverger->InsertConverger(logEntry.getID(),pVehicle->getID(),pParkingLotObj->getInput()->getName());
		//			return;
		//		}
		//	}
		//}

		//non-parking lot, it could be landside, for individual arrival passenger
		//curbside
		//find out the curbside A that the passenger could lead to 
		//find out the curbside B that the vehicle could move to
		//then, figure out the curbside shared both

		LandsidePaxVehicleLinkageInSim* pBestItem = m_pLandisdePaxTypeLinkage->getBestFitItem(mobconst,pVehicle->getName());
		if (pBestItem == NULL)
			continue;
		
		std::vector<LandsideObjectLinkageItemInSim> vAllObject;
		pBestItem->GetAllObjectCanLeadTo(_currentTime, vAllObject);

		ALTObjectID altObj;
		LandsideObjectLinkageItemInSim fitObjetct(altObj);
		if(m_pLandsideTerminalLinkage->GetRandomObject(vAllObject,fitObjetct))
		{
			m_pIndividualArrivalPaxVehicleConverger->InsertConverger(logEntry.getID(),pVehicle->getID(),fitObjetct.getObject());
		}
	}
}

int LandsideSimulation::GetLinkTerminalFloor( int landsidelevel ) const
{
	if(landsidelevel>=0 && landsidelevel<(int)m_vTerminalFloorLinkage.size())
	{
		int nTernFloor =  m_vTerminalFloorLinkage[landsidelevel];
		ASSERT(nTernFloor>=0);
		if(nTernFloor<0)
			return 0;
		return nTernFloor;
	}
	return 0;
}

int LandsideSimulation::GetLinkLandsideFloor( int terminalFloor ) const
{
	for (unsigned i = 0; i < m_vTerminalFloorLinkage.size(); i++)
	{
		int nTermFloor =  m_vTerminalFloorLinkage[i];
		if (nTermFloor == terminalFloor)
		{
			return i;
		}
	}
	return 0;
}

bool LandsideSimulation::IsLeftDrive() const
{
	return getInput()->IsLeftDrive();
}





