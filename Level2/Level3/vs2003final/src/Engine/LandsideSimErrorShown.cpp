#include "StdAfx.h"
#include ".\landsidesimerrorshown.h"
#include "common\EngineDiagnoseEx.h"
#include "LandsideVehicleInSim.h"
#include "SimulationDiagnoseDelivery.h"
#include "PAX.H"

void LandsideSimErrorShown::VehicleSimWarning( LandsideVehicleInSim* pVehicle, const CString& strError, const CString& errorType, const ElapsedTime& tTime )
{
	LandsideDiagnose* pNewDiagnose = new LandsideDiagnose(tTime);
	pNewDiagnose->AddDetailsItem(strError);
	pNewDiagnose->SetErrorType(errorType);
	pNewDiagnose->SetMobElementID(pVehicle->getID());

	DiagnoseDelivery()->DeliveryLandsideDiagnose(pNewDiagnose);

}

void LandsideSimErrorShown::PaxSimWarning( Person* pPax, const CString& strError, const CString& errorType, const ElapsedTime& tTime )
{
	LandsideDiagnose* pNewDiagnose = new LandsideDiagnose(tTime);
	pNewDiagnose->AddDetailsItem(strError);
	pNewDiagnose->SetErrorType(errorType);
	pNewDiagnose->SetMobElementID(pPax->getID());

	DiagnoseDelivery()->DeliveryLandsideDiagnose(pNewDiagnose);
}

void LandsideSimErrorShown::NoSpotInParkingLot( LandsideParkingLotInSim* pLot, LandsideVehicleInSim* pVehicle )
{
	
//	VehicleSimWarning(pVehicle, )
}


