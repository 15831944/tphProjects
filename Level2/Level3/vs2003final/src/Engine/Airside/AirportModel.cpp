#include "stdafx.h"
#include "AirportModel.h"


#include "../RunwayProc.h"
#include "AirspaceModel.h"
#include "RunwayModel.h"
#include "TaxiwayModel.h"
#include "GateModel.h"
#include "../StandProc.h"
#include "../../Common/ARCVector.h"

#include "../../Inputs/AirsideInput.h"
#include "../../AirsideInput/AMGraph.h"


NAMESPACE_AIRSIDEENGINE_BEGINE

AirportModel::AirportModel(){
	m_pAirspace = NULL;
	m_pRunwayModel = NULL;
	m_pTaxiwayModel = NULL;
	m_pGateModel  = NULL;
}
AirportModel::~AirportModel(){
	Clear();
}

void AirportModel::Clear(){
	delete m_pAirspace ; m_pAirspace =  NULL;
	delete m_pRunwayModel ; m_pRunwayModel = NULL;
	delete m_pTaxiwayModel ; m_pTaxiwayModel = NULL;
	delete m_pGateModel ; m_pGateModel = NULL; 
}

bool AirportModel::Build( CAirsideInput* _inputC, AirsideInput* _inputD){
	
	Clear();
	m_pAirspace = new AirspaceModel(this);
	m_pRunwayModel = new RunwayModel(this);
	m_pTaxiwayModel = new TaxiwayModel(this);
	m_pGateModel = new GateModel(this);

	AirportMachine::AirsideGraph amgraph;

	m_pAirspace->Build(_inputC,_inputD);

	// build the model in the ground;
	ProcessorList * pProcList = _inputC->GetProcessorList();
	
	//building Taixway Model
	for(int i=0;i<pProcList->getProcessorCount();i++){
		Processor * pProc = pProcList->getProcessor(i);
		if(TaxiwayProcessor == pProc->getProcessorType()){
			//amgraph.addPath(pProc->serviceLocationPath());
			TaxiwayProc* pTaxiway = (TaxiwayProc*)pProc;
			CString strName = pTaxiway->getIDName();
			TaxiwayProc::SegmentList& segmentList = pTaxiway->GetSegmentList();
			for (TaxiwayProc::SegmentItr iter = segmentList.begin(); iter != segmentList.end(); ++iter)
			{
				if (iter->emType == TaxiwayProc::Bidirection)
				{
					amgraph.addEdge(iter->StartPoint, iter->EndPoint);
					amgraph.addEdge(iter->EndPoint, iter->StartPoint);
				}
				else if (iter->emType == TaxiwayProc::PositiveDirection)
					amgraph.addEdge(iter->StartPoint, iter->EndPoint);
				else
					amgraph.addEdge(iter->EndPoint, iter->StartPoint);
			}
		}
	}

	//add runway
	for(int i=0;i<(int)pProcList->getProcessorCount();i++){
		Processor * pProc = pProcList->getProcessor(i);
		
		if(RunwayProcessor == pProc->getProcessorType()){
			RunwayProc * pRunway = (RunwayProc* )pProc;
			Path * pPath = pRunway->serviceLocationPath();
			std::vector<Point> vExitPoints = amgraph.intersectPoints(pPath->getPoint(0),pPath->getPoint(1));
			std::vector<int> exitsIds(vExitPoints.size());
			for(int j=0;j<(int)vExitPoints.size();j++){
				exitsIds[j] = amgraph.addVertex(vExitPoints[j]); 
			}
			
			Runway * newRw1 = new Runway;
			newRw1->setPhysicalEnd1(pRunway->getServicePoint(0));
			newRw1->setPhysicalEnd2(pRunway->getServicePoint(1));
			newRw1->setStringID(_T(pRunway->GetMarking1().c_str()));
			newRw1->setThresholdoffset(pRunway->GetThreshold1());
			for(int j=0;j<(int)exitsIds.size();j++){
				RunwayExit *  rwExit = new RunwayExit;
				rwExit->m_NodeId = exitsIds[j];
				newRw1->getExitList().push_back(rwExit);
			}			
			m_pRunwayModel->getRunwayList().push_back(newRw1);

			Runway * newRw2 = new Runway;
			newRw2->setPhysicalEnd1(pRunway->getServicePoint(1));
			newRw2->setPhysicalEnd2(pRunway->getServicePoint(0));
			newRw2->setStringID(_T(pRunway->GetMarking2().c_str()));
			newRw2->setThresholdoffset(pRunway->GetThreshold2());
			for(int j=(int)exitsIds.size()-1;j>=0;j--){
				RunwayExit * rwExit = new  RunwayExit;
				rwExit->m_NodeId = exitsIds[j];
				newRw2->getExitList().push_back(rwExit);
			}
			m_pRunwayModel->getRunwayList().push_back(newRw2);

			newRw1->setDependRunway(newRw2);
			newRw2->setDependRunway(newRw1);
		}
	}
	m_pRunwayModel->Build(_inputC,_inputD);
	//add gate 
	for(int i=0;i<(int)pProcList->getProcessorCount();i++)
	{
		Processor * pProc= pProcList->getProcessor(i);
		if(StandProcessor == pProc->getProcessorType()){
			StandProc * pStand = (StandProc*)pProc;
			Gate * newGate = new Gate;
			newGate->setStringID(pStand->getIDName());
			newGate->m_pos = pStand->getServicePoint(0);
			if(pStand->IsUsePushBack()){
				
			Point p1 = pStand->getPushBackWay()->getPoint(0);
			Point p2 = pStand->getPushBackWay()->getPoint(1);
			Point extPt;
			Point cntPt = pStand->getServicePoint(0);			
			if(cntPt.distance(p1)>cntPt.distance(p2)){
				extPt = p1;
			}else
				extPt = p2;

				
			
			std::vector<Point> vExitPoints = amgraph.intersectPoints(p1,p2);
			if(vExitPoints.size()>0) 
				newGate->m_inNodeId = newGate->m_outNodeId = amgraph.addVertex(vExitPoints[0]);
			else newGate->m_inNodeId = newGate->m_outNodeId = amgraph.addVertex(extPt);
				
			}else
			{				
				Path  * in_Cnst = pStand->inConstraint();
				newGate->m_inNodeId = amgraph.addVertex(pStand->inConstraint(0));
				newGate->m_outNodeId = amgraph.addVertex( pStand->outConstraint(pStand->outConstraint()->getCount()-1) );
				
			}
			m_pGateModel->getGates().push_back(newGate);
		}

	}
	
	m_pTaxiwayModel->Build(_inputC,_inputD);
	m_pTaxiwayModel->ConvertFromAMGraph(amgraph);
	m_pGateModel->Build(_inputC,_inputD);
// refresh Runway exit; 
	for(int i=0;i<(int) m_pRunwayModel->getRunwayList().size();i++ ){
		Runway * pRunway = m_pRunwayModel->getRunwayList().at(i).get();
		RwExitList newRwExitList;
		ARCVector2 rwOrien = ARCVector2(pRunway->getPhysicalEnd2() ) - ARCVector2(pRunway->getPhysicalEnd1());
		for(int j=0;j<(int)pRunway->getExitList().size();j++){
			RunwayExit * pExit = pRunway->getExitList().at(j).get();
			std::vector<int> addjacentNodes = m_pTaxiwayModel->getAdjacentVertex(pExit->m_NodeId);
			for(int k=0;k<(int)addjacentNodes.size();k++){
				RunwayExit * pNewExit = new RunwayExit;
				pNewExit->m_NodeId = pExit->m_NodeId;
				pNewExit->m_NodeIdNext = addjacentNodes[k];
				ARCVector2 exitOrien = ARCVector2( m_pTaxiwayModel->getTaixwayNode(pNewExit->m_NodeIdNext)->getPosition() ) - 
									   ARCVector2( m_pTaxiwayModel->getTaixwayNode(pNewExit->m_NodeId)->getPosition() );
				pNewExit->setAngle(rwOrien.GetAngleOfTwoVector(exitOrien));
				newRwExitList.push_back(pNewExit);
			}
		}
		pRunway->getExitList() = newRwExitList;
	} 
	
	return true;
}











NAMESPACE_AIRSIDEENGINE_END