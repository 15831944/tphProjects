#include "StdAfx.h"
#include ".\flightserviceevent.h"
#include "AirsideFlightInSim.h"
#include "FlightServiceInformation.h"
#include "FlightServiceRoute.h"
#include "AirsideVehicleInSim.h"
#include "Clearance.h"


CFlightServiceEvent::CFlightServiceEvent(AirsideVehicleInSim *pVehicle)
:CAirsideMobileElementEvent(pVehicle),
m_pVehicle(pVehicle)
{
	if(m_pVehicle->GetMode() == OnMoveToService)
	{
		int i =0;
		i += 1;
	}
}

CFlightServiceEvent::~CFlightServiceEvent(void)
{
}
int CFlightServiceEvent::Process()
{
	//find next movement
	//CString strFileName;
	//strFileName.Format("C:\\VehicleDebug\\%d-%d", (int)m_pVehicle->GetVehicleTypeID(), m_pVehicle->m_id );	
	//ofsstream echoFile ( strFileName, stdios::app);
	//echoFile << getTime() << "("<<(long)getTime() <<"):"<<m_pVehicle->GetMode()<< std::endl;

	AirsideFlightInSim *pFlight = m_pVehicle->GetServiceFlight();
	
	CFlightServiceInformation * pInfo =	pFlight->GetFlightServiceInformation();

	CFlightServiceRoute *pRoute = pInfo->GetFlightServiceRoute();
	ClearanceItem item(NULL,OnService,0);
	Clearance newItem;

	//long lFindStartTime = GetTickCount();
	pRoute->FindClearanceInConcern(m_pVehicle,item,100,newItem);


	//long lFindEndTime = GetTickCount();

	//static long totalFindTime = 0L;
	//totalFindTime += lFindEndTime - lFindStartTime;
	//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
	//echoFile<<"CFlightServiceEvent::Process()        " <<lFindEndTime - lFindStartTime<<"          "
	//	<<totalFindTime
	//	<<"\r\n";
	//echoFile.close();

	//echoFile << m_pVehicle->GetTime()<<"("<<(long)m_pVehicle->GetTime() <<")"<< m_pVehicle->GetPosition() <<":"<<m_pVehicle->GetMode()<<std::endl;
	//echoFile << std::endl;
	//echoFile.close();

	return 0;
}