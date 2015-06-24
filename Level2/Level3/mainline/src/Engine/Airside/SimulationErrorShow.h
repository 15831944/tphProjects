#pragma once
#include "../../Common/EngineDiagnoseEx.h"
#include "../SimulationDiagnoseDelivery.h"
#include "AirsideFlightInSim.h"
#include "AirsideVehicleInSim.h"
#include "../../Common\ARCException.h"


class FlightPlanConflictFlightPerformanceException : public ARCException
{
public:
	FlightPlanConflictFlightPerformanceException( CString strException, CString strType)
	{
		m_strErrorMsg = strException;
		m_strErrorType = strType;
	}
	~FlightPlanConflictFlightPerformanceException(){}

	CString GetExcepDesc()const{return m_strErrorMsg;}
	CString GetExcepType()const{return m_strErrorType;}
protected:

private:
};
class AirsideSimErrorShown
{
public:
	static void SimpleSimWarning(const CString& strID, const CString& strError, const CString& strErrorType, const ElapsedTime tTime = -1L)
	{
		AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(tTime>=0L ? tTime.printTime() : _T("Initialization"), strID);
		pNewDiagnose->AddDetailsItem(strError);
		pNewDiagnose->SetErrorType(strErrorType);
		DiagnoseDelivery()->DeliveryAirsideDiagnose(pNewDiagnose);	
	}

	static void SimWarning(const AirsideFlightInSim* pFlight, const CString& strError, const CString& strErrorType, const ElapsedTime tTime = -1L)
	{
		ElapsedTime t = tTime;
		CString strID;
		if (pFlight)
		{
			if (t<0L)
				t = pFlight->GetTime();
			strID = pFlight->GetCurrentFlightID();
		}
		SimpleSimWarning(strID, strError, strErrorType, t);
	}

	static void VehicleSimWarning(const AirsideVehicleInSim* pVehicle, const CString& strError, const CString& strErrorType, const ElapsedTime tTime = -1L)
	{
		ElapsedTime t = tTime;
		CString strID;
		if (pVehicle)
		{
			if (t < 0L)
				t = pVehicle->GetTime();

			strID.Format("V %d", pVehicle->GetID() );
		}
		SimpleSimWarning(strID, strError, strErrorType, t);
	}

};
