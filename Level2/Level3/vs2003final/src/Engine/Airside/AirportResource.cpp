#include "StdAfx.h"
#include ".\airportresource.h"

#include "../RunwayProc.h"
#include "AirspaceInSim.h"
#include "RunwayInSim.h"
#include "TaxiwayInSim.h"
#include "GateInSim.h"
#include "../StandProc.h"
#include "../../Common/ARCVector.h"

#include "../../Inputs/AirsideInput.h"
#include "../../AirsideInput/AMGraph.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside/Taxiway.h"
#include "InputAirside/Runway.h"
#include "InputAirside/Gate.h"

AirportResource::AirportResource(){
	m_pAirspace = NULL;
	m_pRunway = NULL;
	m_pTaxiway = NULL;
	m_pGate  = NULL;
}
AirportResource::~AirportResource(){
	Clear();
}

void AirportResource::Clear(){
	delete m_pAirspace ; m_pAirspace =  NULL;
	delete m_pRunway ; m_pRunway = NULL;
	delete m_pTaxiway ; m_pTaxiway = NULL;
	delete m_pGate ; m_pGate = NULL; 
}

bool AirportResource::Build(int nProjID )
{

	Clear();
	m_pAirspace = new AirspaceInSim(this);
	m_pRunway = new RunwayInSim(this);
	m_pTaxiway = new TaxiwayInSim(this);
	m_pGate = new GateInSim(this);

	AirportMachine::AirsideGraph amgraph;

	m_pAirspace->Build();


	
	//building Taixway Model

	std::vector<int> vAirport;
	InputAirside::GetAirportList(nProjID,vAirport);
	int nAirportID = vAirport.at(0);

	std::vector<int> vTaxiwayList;
	ALTAirport::GetTaxwayList(nAirportID, vTaxiwayList);
	for(int i =0;i<(int)vTaxiwayList.size();++i)
	{
		Taxiway taxiway;
		taxiway.ReadObject(vTaxiwayList.at(i));
		amgraph.addPath(&taxiway.GetPath());
	}
	
//Runway
		std::vector<int> vRunwayList;
		ALTAirport::GetRunwayList(nAirportID,vRunwayList);
		for(int i =0;i<(int)vRunwayList.size();++i)
		{
			Runway runway;
			runway.ReadObject(vRunwayList.at(i));
			Path pPath = runway.GetPath();
			std::vector<Point> vExitPoints = amgraph.intersectPoints(pPath.getPoint(0),pPath.getPoint(1));
			std::vector<int> exitsIds(vExitPoints.size());
			for(int j=0;j<(int)vExitPoints.size();j++){
				exitsIds[j] = amgraph.addVertex(vExitPoints[j]); 
			}

			RunwaySim * newRw1 = new RunwaySim;
			newRw1->setPhysicalEnd1(pPath.getPoint(0));
			newRw1->setPhysicalEnd2(pPath.getPoint(1));
			newRw1->setStringID(_T(runway.GetMarking1().c_str()));
			newRw1->setThresholdoffset(0);
			for(int j=0;j<(int)exitsIds.size();j++){
				RunwayExitSim *  rwExit = new RunwayExitSim;
				rwExit->m_NodeId = exitsIds[j];
				newRw1->getExitList().push_back(rwExit);
			}			
			m_pRunway->getRunwayList().push_back(newRw1);

			RunwaySim * newRw2 = new RunwaySim;
			newRw2->setPhysicalEnd1(pPath.getPoint(1));
			newRw2->setPhysicalEnd2(pPath.getPoint(0));
			newRw2->setStringID(_T(runway.GetMarking2().c_str()));
			newRw2->setThresholdoffset(0);
			for(int j=(int)exitsIds.size()-1;j>=0;j--){
				RunwayExitSim * rwExit = new  RunwayExitSim;
				rwExit->m_NodeId = exitsIds[j];
				newRw2->getExitList().push_back(rwExit);
			}
			m_pRunway->getRunwayList().push_back(newRw2);

			newRw1->setDependRunway(newRw2);
			newRw2->setDependRunway(newRw1);
		}
//gate
		std::vector<int> vGateList;
		ALTAirport::GetStandList(nAirportID,vGateList);
		for(int i =0;i<(int)vGateList.size();++i)
		{
			Stand stand;
			stand.ReadObject(vGateList.at(i));


				Gate * newGate = new Gate;
				newGate->setStringID(stand.GetObjNameString());
				newGate->m_pos = stand.GetServicePoint();
					
				newGate->m_inNodeId = amgraph.addPath( &stand.GetInContrain());
				if(!stand.IsUsePushBack())
					newGate->m_outNodeId = amgraph.addPath( &stand.GetOutConstrain());

				m_pGate->getGates().push_back(newGate);
			}



	//add gate 
	//for(int i=0;i<(int)pProcList->getProcessorCount();i++)
	//{
	//	Processor * pProc= pProcList->getProcessor(i);
	//	if(StandProcessor == pProc->getProcessorType()){
	//		StandProc * pStand = (StandProc*)pProc;
	//		Gate * newGate = new Gate;
	//		newGate->setStringID(pStand->getIDName());
	//		newGate->m_pos = pStand->getServicePoint(0);
	//		if(pStand->IsUsePushBack()){

	//			Point p1 = pStand->getPushBackWay()->getPoint(0);
	//			Point p2 = pStand->getPushBackWay()->getPoint(1);
	//			Point extPt;
	//			Point cntPt = pStand->getServicePoint(0);			
	//			if(cntPt.distance(p1)>cntPt.distance(p2)){
	//				extPt = p1;
	//			}else
	//				extPt = p2;



	//			std::vector<Point> vExitPoints = amgraph.intersectPoints(p1,p2);
	//			if(vExitPoints.size()>0) 
	//				newGate->m_inNodeId = newGate->m_outNodeId = amgraph.addVertex(vExitPoints[0]);
	//			else newGate->m_inNodeId = newGate->m_outNodeId = amgraph.addVertex(extPt);

	//		}else
	//		{				
	//			Path  * in_Cnst = pStand->inConstraint();
	//			newGate->m_inNodeId = amgraph.addVertex(pStand->inConstraint(0));
	//			newGate->m_outNodeId = amgraph.addVertex( pStand->outConstraint(pStand->outConstraint()->getCount()-1) );

	//		}
	//		m_pGate->getGates().push_back(newGate);
	//	}

	//}

	m_pRunway->Build();
	m_pTaxiway->Build();
	m_pTaxiway->ConvertFromAMGraph(amgraph);
	m_pGate->Build();

	// refresh Runway exit; 
	for(int i=0;i<(int) m_pRunway->getRunwayList().size();i++ ){
		RunwaySim * pRunway = m_pRunway->getRunwayList().at(i).get();
		RwExitList newRwExitList;
		ARCVector2 rwOrien = ARCVector2(pRunway->getPhysicalEnd2() ) - ARCVector2(pRunway->getPhysicalEnd1());
		for(int j=0;j<(int)pRunway->getExitList().size();j++){
			RunwayExitSim * pExit = pRunway->getExitList().at(j).get();
			std::vector<int> addjacentNodes = m_pTaxiway->getAdjacentVertex(pExit->m_NodeId);
			for(int k=0;k<(int)addjacentNodes.size();k++){
				RunwayExitSim * pNewExit = new RunwayExitSim;
				pNewExit->m_NodeId = pExit->m_NodeId;
				pNewExit->m_NodeIdNext = addjacentNodes[k];
				ARCVector2 exitOrien = ARCVector2( m_pTaxiway->getTaixwayNode(pNewExit->m_NodeIdNext)->getPosition() ) - 
					ARCVector2( m_pTaxiway->getTaixwayNode(pNewExit->m_NodeId)->getPosition() );
				pNewExit->setAngle(rwOrien.GetAngleOfTwoVector(exitOrien));
				newRwExitList.push_back(pNewExit);
			}
		}
		pRunway->getExitList() = newRwExitList;
	} 

	return true;
}
