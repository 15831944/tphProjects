#include "StdAfx.h"
#include ".\flightgroundrouteresourcemanager.h"
//#include "TempParkingSegmentsInSim.h"
#include "TemporaryParkingInSim.h"
#include "TaxiwayResourceManager.h"
#include "RunwayResourceManager.h"
#include "AirsideFlightInSim.h"
#include "../SimulationDiagnoseDelivery.h"

FlightGroundRouteResourceManager::FlightGroundRouteResourceManager(void)
{
	m_pRunwayResourceManager = NULL;
	m_pTaxiwayResourceManager = NULL;
}

FlightGroundRouteResourceManager::~FlightGroundRouteResourceManager(void)
{
	for(SegPathMap::iterator itr=m_segPathFinders.begin();itr!=m_segPathFinders.end();itr++)
	{
		delete itr->second;
	}
	m_segPathFinders.clear();
}


void FlightGroundRouteResourceManager::Init( TemporaryParkingInSim* tempParkingsegs )
{
	SetParkingSegments(tempParkingsegs);
	m_pathFinder.Init(m_pTaxiwayResourceManager->getTaxiwayResource(),m_pRunwayResourceManager->getRunwayResource(),m_vParkingSegs);	
	//m_taxiwayFinder.Init(m_pTaxiwayResourceManager->getTaxiwayResource(),m_vParkingSegs);
	m_routeFinder.Init(m_pTaxiwayResourceManager->getTaxiwayResource(), m_vParkingSegs);
}
//need to include all restrictions here 
bool FlightGroundRouteResourceManager::GetRoute( IntersectionNodeInSim * pSrc, IntersectionNodeInSim * pDest, AirsideFlightInSim *pFlight,int nPRI,
												FlightGroundRouteDirectSegList& path, double& dPathWeight, FlightGroundRouteFinderStrategy* pStrategy /*= NULL*/ )
{
	//
	//DistanceUnit dDistance = m_taxiwayFinder.GetRoute(pFlight,pSrc,pDest,path);
	//if(path.size() == 0)//can not find a path in taxiroute network
	//{	
		/*dDistance = m_pathFinder.GetRoute(pFlight,pSrc,pDest,path);
		if(dDistance == -1)
		{
			CString strWarn;
			strWarn.Format("There is no path between %s and %s", pSrc->GetNodeInput().GetName(), pDest->GetNodeInput().GetName());
			CString strTime = pFlight->GetTime().printTime();
			CString strID ;
			pFlight->getFullID(strID.GetBuffer(1024));
			AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);
			pNewDiagnose->AddDetailsItem(strWarn);
			CString strError = _T("FLIGHT ERROR");
			pNewDiagnose->SetErrorType(strError);
			DiagnoseDelivery()->DeliveryAirsideDiagnose(pNewDiagnose);
		}*/
	//}
	
	if( !m_routeFinder.GetRoute(pFlight,pSrc,pDest,path,dPathWeight,pStrategy) )
	{
		CString strWarn;
		strWarn.Format("There is no path between %s and %s", pSrc->GetNodeInput().GetName(), pDest->GetNodeInput().GetName());
		CString strTime = pFlight->GetTime().printTime();
		CString strID  =pFlight->GetCurrentFlightID();
		AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);
		pNewDiagnose->AddDetailsItem(strWarn);
		CString strError = _T("FLIGHT ERROR");
		pNewDiagnose->SetErrorType(strError);
		DiagnoseDelivery()->DeliveryAirsideDiagnose(pNewDiagnose);
		return false;
	}
	else 
	{
		return true;//m_routeFinder.GetRouteDist(pSrc, pDest);
	}
	return false;
}

void FlightGroundRouteResourceManager::SetParkingSegments( TemporaryParkingInSim* tempParkingsegs )
{
	m_vParkingSegs.clear();
	for(int i=0;i<tempParkingsegs->GetParkingCount();i++)
	{
		TempParkingSegmentsInSim* pSegs = tempParkingsegs->GetParkingSeg(i);
		for(int j=0;j<(int)pSegs->m_vUseSegs.size();j++)
		{
			FlightGroundRouteSegInSim* pSeg = pSegs->m_vUseSegs.at(j);
			if( m_vParkingSegs.end() == std::find(m_vParkingSegs.begin(),m_vParkingSegs.end(),pSeg) )
			{
				m_vParkingSegs.push_back(pSeg);
			}
		}

	}	
}

void FlightGroundRouteResourceManager::SetResourceManager( TaxiwayResourceManager *pTaxiwayResourceManager, RunwayResourceManager * pRunwayResourceManager )
{
	m_pRunwayResourceManager = pRunwayResourceManager;
	m_pTaxiwayResourceManager = pTaxiwayResourceManager;
}

FlightGroundRouteDirectSegList FlightGroundRouteResourceManager::GetAllLinkedDirectSegments( IntersectionNodeInSim * pNode )
{
	FlightGroundRouteDirectSegList returnSegList;

	FlightGroundRouteDirectSegList runwaySegList =  m_pRunwayResourceManager->GetLinkedDirectSegments(pNode);

	returnSegList.insert(returnSegList.begin(), runwaySegList.begin(),runwaySegList.end());

	FlightGroundRouteDirectSegList taxiwaySegList = m_pTaxiwayResourceManager->GetLinkedDirectSegmentsTaxiway(pNode);

	returnSegList.insert(returnSegList.begin(), taxiwaySegList.begin(),taxiwaySegList.end());

	return returnSegList;
}

FlightGroundRouteDirectSegList FlightGroundRouteResourceManager::GetLinkedDirectSegments( IntersectionNodeInSim * pNode )
{
	FlightGroundRouteDirectSegList returnSegList;

	FlightGroundRouteDirectSegList runwaySegList =  m_pRunwayResourceManager->GetLinkedDirectSegments(pNode);

	returnSegList.insert(returnSegList.begin(), runwaySegList.begin(),runwaySegList.end());

	FlightGroundRouteDirectSegList taxiwaySegList = m_pTaxiwayResourceManager->GetLinkedDirectSegmentsTaxiway(pNode);

	returnSegList.insert(returnSegList.begin(), taxiwaySegList.begin(),taxiwaySegList.end());

	return returnSegList;
}

CFlightGroundRouteFinder* FlightGroundRouteResourceManager::_getSegPath( FlightGroundRouteDirectSegInSim*pSeg ) 
{
	SegPathMap::const_iterator itrfind = m_segPathFinders.find(pSeg);
	if( m_segPathFinders.end()!= itrfind )
		return itrfind->second;
	
	CFlightGroundRouteFinder* pPathFinder = new CFlightGroundRouteFinder();
	std::vector<FlightGroundRouteSegInSim*> vexcludeSegs = m_vParkingSegs;
	vexcludeSegs.push_back(pSeg->GetRouteSegInSim());
	pPathFinder->Init(m_pTaxiwayResourceManager->getTaxiwayResource(), vexcludeSegs);
	_addSegPath(pSeg, pPathFinder);	
	return pPathFinder;
}

void FlightGroundRouteResourceManager::_addSegPath( FlightGroundRouteDirectSegInSim* pSeg, CFlightGroundRouteFinder* pPath )
{
	m_segPathFinders[pSeg] = pPath;
}

bool FlightGroundRouteResourceManager::GetRoute( FlightGroundRouteDirectSegInSim* pSeg, IntersectionNodeInSim* pDest, AirsideFlightInSim* pFlight,
												FlightGroundRouteDirectSegList& path, double& dPathWeight, FlightGroundRouteFinderStrategy* pStrategy /*= NULL*/ )
{
	FlightGroundRouteDirectSegList nomalSeg;
	FlightGroundRouteDirectSegInSim* pOpSeg =  pSeg->GetOppositeSegment();	
	IntersectionNodeInSim* pSrc = pSeg->GetExitNode();

	bool bRet = GetRoute(pSrc, pDest,pFlight,0, nomalSeg,dPathWeight, pStrategy);
	if(!bRet)
		return false;

	if( std::find(nomalSeg.begin(),nomalSeg.end(),pOpSeg)!=nomalSeg.end() ) //route is invalid
	{
		CFlightGroundRouteFinder* pPathFinder = _getSegPath(pSeg);
		assert(pPathFinder);
		if(!pPathFinder || !pPathFinder->GetRoute(pFlight,pSrc,pDest,path,dPathWeight,pStrategy) )
		{
			//CString strWarn;
			//strWarn.Format("There is no path between %s and %s", pSrc->GetNodeInput().GetName(), pDest->GetNodeInput().GetName());
			//CString strTime = pFlight->GetTime().printTime();
			//CString strID  =pFlight->GetCurrentFlightID();
			//AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);
			//pNewDiagnose->AddDetailsItem(strWarn);
			//CString strError = _T("FLIGHT ERROR");
			//pNewDiagnose->SetErrorType(strError);
			//DiagnoseDelivery()->DeliveryAirsideDiagnose(pNewDiagnose);
			path.clear();
			return false;
		}
		else 
		{
			return true;// pPathFinder->GetRouteDist(pSrc, pDest);
		}
	}
	path = nomalSeg;	
	return true;//dist+pSeg->GetLength();
	

}
