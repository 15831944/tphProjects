#include "StdAfx.h"
#include "ReportManager.h"
#include "AirsideDistanceTravelParam.h"
#include "AirsideDistanceTravelReport.h"
#include "AirsideDurationParam.h"
#include "AirsideDurationReport.h"
#include "AirsideFlightDelayParam.h"
#include "AirsideFlightDelayReport.h"
#include "FlightActivityParam.h"
#include "FlightActivityReport.h"
#include "AirsideOperationParam.h"
#include "AirsideOperationReport.h"
#include "AirsideNodeDelayReport.h"
#include "AirsideNodeDelayReportParameter.h"
#include "FlightOperationalParam.h"
#include "FlightOperationalReport.h"
#include "AirsideVehicleOperaterReport.h"
#include "AirsideVehicleOperParameter.h"

#include "AirsideRunwayOperationReportParam.h"
#include "AirsideRunwayOperationsReport.h"

#include "./AirsideRunwayUtilizationReport.h"
#include "./AirsideRunwayUtilizationReportParam.h"
#include "../Engine/terminal.h"


#include "TaxiwayDelayParameters.h"
#include "TaxiwayDelayReport.h"

#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"

#include "RunwayExitParameter.h"
#include "RunwayExitReport.h"
#include "AirsideFlightRunwayDelayReport.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "AirsideFlightFuelBurnReport.h"
#include "AirsideFlightBurnParmater.h"
#include "AirsideRunwayCrossingsReport.h"
#include "AirsideRunwayCrossingsPara.h"
#include "AirsideTaxiwayUtilizationReport.h"
#include "AirsideTaxiwayUtilizationPara.h"
#include "../AirsideReport/AirsideFlightStandOperationReport.h"
#include "../AirsideReport/FlightStandOperationParameter.h"
#include "../InputAirside/ARCUnitManager.h"
#include "AirsideIntersectionOperationReport.h"
#include "AirsideIntersectionReportParam.h"
#include "AirsideReportObjectIntersecNodeIDNameMap.h"
#include "AirsideControllerWorkloadParam.h"
#include "AirsideControllerWorkloadReport.h"
#include "AirsideTakeoffProcessReport.h"
#include "AirsideTakeoffProcessParameter.h"


CAirsideReportManager::CAirsideReportManager()
{
	m_pParamters		= NULL;
	m_pAirsideReport	= NULL;
	m_pTerminal			= NULL;
	m_nProjID			= -1;
	m_pGetLogFilePath	= NULL; 
	m_emReportType = UnknownType;
	m_csProjPath		=_T(""); 
	m_vMultipleRun.push_back(Airside_FlightDelay);
	m_vMultipleRun.push_back(Airside_StandOperations);
	m_vMultipleRun.push_back(Airside_TakeoffProcess);
}

CAirsideReportManager::~CAirsideReportManager()
{
	delete m_pParamters;
	delete m_pAirsideReport;

}

void CAirsideReportManager::InitUpdate(int nProjID)
{
	if (m_pAirsideReport)
	{
		m_pAirsideReport = NULL;
	}


	if (m_pParamters)
	{
		delete m_pParamters;
		m_pParamters = NULL;
	}

	switch(GetReportType())
	{
	case Airside_Legacy:
		{
		}
		break;

	case Airside_DistanceTravel:
		{

			m_pParamters		= new CAirsideDistanceTravelParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new CAirsideDistanceTravelReport(m_pGetLogFilePath);
		}
		break;

	case Airside_Duration :
		{
		}
		break;

	case Airside_FlightDelay:
		{
			m_pParamters		= new CAirsideFlightDelayParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new CAirsideFlightDelayReport(m_pGetLogFilePath);
		}
		break;
	case Airside_FlightActivity:
		{
			m_pParamters		= new CFlightActivityParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new CFlightActivityReport(m_pGetLogFilePath);
		}
		break;

	case Airside_AircraftOperation:
		m_pParamters		= new CAirsideOperationParam();
		m_pParamters->SetProjID(nProjID);
		m_pAirsideReport	= new CAirsideOperationReport(m_pGetLogFilePath);
		break;

	case Airside_NodeDelay:
		m_pParamters		= new CAirsideNodeDelayReportParameter();
		m_pParamters->SetProjID(nProjID);
		m_pAirsideReport	= new CAirsideNodeDelayReport(m_pGetLogFilePath);
		break;

	case Airside_AircraftOperational:
		{
			m_pParamters		= new CFlightOperationalParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new CFlightOperationalReport(m_pGetLogFilePath);
		}
		break;
	case Airside_VehicleOperation:
		{
			m_pParamters = new CAirsideVehicleOperParameter() ;
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideVehicleOperaterReport(m_pGetLogFilePath,(CAirsideVehicleOperParameter*)m_pParamters);
		}
		break ;
	case Airside_RunwayOperaitons:
		{
			m_pParamters		= new AirsideRunwayOperationReportParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new AirsideRunwayOperationsReport(m_pGetLogFilePath);
		}
		break;

	case Airside_RunwayUtilization:
		{
			m_pParamters		= new CAirsideRunwayUtilizationReportParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new CAirsideRunwayUtilizationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_TaxiwayDelay:
		{
			m_pParamters = new CTaxiwayDelayParameters() ;
			m_pParamters->SetProjID(nProjID) ;
			m_pAirsideReport = new CTaxiwayDelayReport(m_pGetLogFilePath,(CTaxiwayDelayParameters*)m_pParamters) ;
		}
		break;
	case Airside_FlightConflict:
		{
			m_pParamters		= new AirsideFlightConflictPara();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new AirsideFlightConflictReport(m_pGetLogFilePath,(AirsideFlightConflictPara*)m_pParamters);
		}
		break;
	case Airside_RunwayExit:
		{
			m_pParamters = new CRunwayExitParameter ;
			m_pParamters->SetProjID(nProjID) ;
			m_pAirsideReport = new CRunwayExitReport(m_pGetLogFilePath,(CRunwayExitParameter*)m_pParamters);
		}
		break;
	case Airside_RunwayDelay:
		{
			m_pParamters		= new AirsideFlightRunwayDelayReportPara();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport	= new AirsideFlightRunwayDelayReport(m_pGetLogFilePath,(AirsideFlightRunwayDelayReportPara*)m_pParamters);
		}
		break;
	case Airside_FlightFuelBurning:
		{
			m_pParamters = new CAirsideFlightFuelBurnParmater ;
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideFlightFuelBurnReport(m_pGetLogFilePath,(CAirsideFlightFuelBurnParmater*)m_pParamters,m_airportDB,nProjID) ;
		}
		break;
	case Airside_RunwayCrossings:
		{
			m_pParamters = new AirsideRunwayCrossingsParameter;
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideRunwayCrossingsReport(m_pGetLogFilePath);
		}
		break;
	case Airside_IntersectionOperation:
		{
			m_pParamters = new CAirsideIntersectionReportParam;
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideIntersectionOperationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_TaxiwayUtilization:
		{
			m_pParamters = new CAirsideTaxiwayUtilizationPara;
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideTaxiwayUtilizationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_StandOperations:
		{
			m_pParamters = new CFlightStandOperationParameter;
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideFlightStandOperationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_ControllerWorkload:
		{
			m_pParamters = new CAirsideControllerWorkloadParam();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideControllerWorkloadReport(m_pGetLogFilePath);
		}
		break;
	case Airside_TakeoffProcess:
		{
			m_pParamters = new CAirsideTakeoffProcessParameter();
			m_pParamters->SetProjID(nProjID);
			m_pAirsideReport = new CAirsideTakeoffProcessReport(m_pGetLogFilePath);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}

	if (m_pParamters != NULL)
		m_pParamters->LoadParameter();

	m_pParamters->SetAirportDB(m_airportDB) ;
	m_pParamters->SetReportFileDir(m_strReportFilePath);	

	if (GetReportType() != Airside_FlightConflict && GetReportType() != Airside_RunwayDelay)
	{

		m_pParamters->LoadParameterFile();
	}

	m_pAirsideReport->SetAirportDB(m_airportDB) ;
	m_pAirsideReport->SetReportFileDir(m_strReportFilePath);


}

void CAirsideReportManager::GenerateResult()
{
	if (m_pAirsideReport == NULL || m_pParamters == NULL)
		return;

	if (AirsideMultipleRunReport())
	{
		if (m_pParamters->GetEnableMultiRun())
		{
			std::vector<int> vReportRun;
			if (m_pParamters->GetReportRuns(vReportRun))
			{
				if (vReportRun.size() > 1)
				{
					m_multiRunReport.GenerateReport(m_pTerminal,m_pGetLogFilePath,m_pGetSetSimResult,m_csProjPath,m_emReportType,m_pParamters);
					return;
				}
			}	
		}
	}

//	else
	{
		m_pParamters->SaveParameterFile();
		m_pAirsideReport->GenerateReport(m_pParamters);
		m_pAirsideReport->IsReportGenerate(TRUE) ;
	}

}

void CAirsideReportManager::RefreshReport(int nUnit)
{
	//ATLASSERT(m_pReportCtrl != NULL);

	//if (m_pDisplay == NULL)
	//	return;
	if (m_pAirsideReport)
	{
		m_pAirsideReport->RefreshReport(m_pParamters);
	}

	//m_pDisplay->SetReportFrameWnd(m_pReportCtrl->GetReportFrameWnd());
	//m_pDisplay->UpdateAllWnds(nUnit);
}

CParameters* CAirsideReportManager::GetParameters()
{
	if (m_pParamters == NULL)
		InitUpdate(m_nProjID);
	return m_pParamters;
}

CAirsideBaseReport * CAirsideReportManager::GetReport()
{
	if (m_pAirsideReport == NULL || m_pAirsideReport->GetReportType() !=GetReportType() )
		InitUpdate(m_nProjID);
	return m_pAirsideReport;
}
void CAirsideReportManager::SetReportType(reportType rpType)
{ 
	m_emReportType = rpType;
}
reportType CAirsideReportManager::GetReportType()
{ 
	return m_emReportType;
}

void CAirsideReportManager::InitReportList(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	if (NULL == m_pAirsideReport)
	{
		return;
	}
	m_pAirsideReport->InitListHead(cxListCtrl, m_pParamters->getReportType(), piSHC);
}

void CAirsideReportManager::SetReportListContent(CXListCtrl& cxListCtrl)
{
	if (NULL == m_pAirsideReport
		&& NULL == m_pParamters)
	{
		return;
	}
	m_pAirsideReport->FillListContent(cxListCtrl, m_pParamters);
}

CString CAirsideReportManager::GetCurrentReportName()
{
	reportType rpType = GetReportType();
	if (rpType < Airside_ReportCount)
		return AirsideReportName[rpType];

	return "";
}
BOOL CAirsideReportManager::GenerateCvsFileFormList(ArctermFile& _file,CXListCtrl& cxListCtrl,CString& ErrorMsg)
{
	if(m_pAirsideReport == NULL)
	{
		ErrorMsg.Format(_T("No data to export")) ;
		return FALSE ;
	}
	BOOL res =  m_pAirsideReport->ExportListCtrlToCvsFile(_file,cxListCtrl,m_pParamters) ;
	_file.endFile() ;
	return res ;
}

BOOL CAirsideReportManager::ExportParaToFile(ArctermFile& _file)
{
	if (m_pParamters == NULL)
	{
		return FALSE;
	}

	_file.writeField(_T("Airside Report Parameter Data")) ;
	_file.writeLine() ;
	//write the report type 
	_file.writeInt(m_pAirsideReport->GetReportType()) ;
	_file.writeLine() ;

	m_pParamters->SaveParameterToFile(_file) ;
	return TRUE;
}

BOOL CAirsideReportManager::ImportParaFromFile(ArctermFile& _file)
{
	//slip the head 
	//_file.getLine() ;
	//read report type
	int type = 0 ;
	if(!_file.getInteger(type))
	{
		return FALSE ;
	}
	CParameters* _paramter = NULL ;
	switch(type)
	{
	case Airside_DistanceTravel:
		{
			_paramter		= new CAirsideDistanceTravelParam();
		}
		break;
	case Airside_FlightDelay:
		{
			_paramter		= new CAirsideFlightDelayParam();
		}
		break;
	case Airside_FlightActivity:
		{
			_paramter		= new CFlightActivityParam();
		}
		break;

	case Airside_AircraftOperation:
		_paramter		= new CAirsideOperationParam();
		break;

	case Airside_NodeDelay:
		_paramter		= new CAirsideNodeDelayReportParameter();
		break;

	case Airside_AircraftOperational:
		{
			_paramter		= new CFlightOperationalParam();
		}
		break;

	case Airside_RunwayOperaitons:
		{
			_paramter		= new AirsideRunwayOperationReportParam();
		}
		break;
	case Airside_RunwayUtilization:
		{
			_paramter		= new CAirsideRunwayUtilizationReportParam();
		}
		break;
	case Airside_VehicleOperation:
		{
			_paramter		= new CAirsideVehicleOperParameter();
		}
		break ;
	case Airside_FlightConflict:
		{
			_paramter		= new AirsideFlightConflictPara();
		}
		break;
	case Airside_TaxiwayDelay:
		{
			_paramter = new CTaxiwayDelayParameters ;
		}
		break ;
	case Airside_RunwayExit:
		{
			_paramter = new CRunwayExitParameter ;
		}
		break ;
	case Airside_RunwayDelay:
		{
			_paramter		= new AirsideFlightRunwayDelayReportPara();
		}
		break;
	case Airside_StandOperations:
		{
			_paramter = new CFlightStandOperationParameter();
		}
		break;
	case Airside_RunwayCrossings:
		{
			_paramter = new AirsideRunwayCrossingsParameter();
		}
		break;
	case Airside_IntersectionOperation:
		{
			_paramter = new CAirsideIntersectionReportParam();
		}
		break;
	case Airside_TaxiwayUtilization:
		{
			_paramter = new CAirsideTaxiwayUtilizationPara();
		}
		break;
	case Airside_ControllerWorkload:
		{
			_paramter = new CAirsideControllerWorkloadParam;
		}
		break;
	case Airside_TakeoffProcess:
		{
			_paramter = new CAirsideTakeoffProcessParameter();
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;

	}
	if(_paramter == NULL)
	{
		return FALSE ;
	}
	_paramter->SetAirportDB(m_airportDB) ;
	_paramter->SetReportFileDir(m_strReportFilePath);
	//read data 
	_file.getLine() ;
	_paramter->LoadParameterFromFile(_file);
	
	
	_file.getLine() ;
	if(m_pParamters != NULL)
		delete m_pParamters ;

	m_pParamters = _paramter ;
	m_emReportType = (reportType)type ;
	return TRUE ;
}

BOOL CAirsideReportManager::ExportReportToFile(ArctermFile& _file,CString& ErrorMsg)
{
	if(m_pParamters == NULL || m_pAirsideReport == NULL)
	{
		ErrorMsg.Format(_T("There is no data to export! Please generate the report firstly!")) ;
		return FALSE ;
	}
	//Export file head 
	_file.writeField(_T("Airside Report Data")) ;
	_file.writeLine() ;
    //write the report type 
	_file.writeInt(m_pAirsideReport->GetReportType()) ;
	_file.writeLine() ;

	BOOL res = m_pParamters->ExportFile(_file) ;
	res = m_pAirsideReport->ExportReportData(_file,ErrorMsg,m_pParamters->getReportType()) ;
	return res ;
}
BOOL CAirsideReportManager::ImportReportFromFile(ArctermFile& _file,CString& ErrorMsg,int nProID)
{
	//slip the head 
	//_file.getLine() ;
	//read report type
	int type = 0 ;
	if(!_file.getInteger(type))
	{
		ErrorMsg.Format(_T("File Format Error")) ;
		return FALSE ;
	}
	CParameters* _paramter = NULL ;
	CAirsideBaseReport* _Preport = NULL ;

	switch(type)
	{
	case Airside_DistanceTravel:
		{

			_paramter		= new CAirsideDistanceTravelParam();
			_Preport	= new CAirsideDistanceTravelReport(m_pGetLogFilePath);
		}
		break;
	case Airside_FlightDelay:
		{
			_paramter		= new CAirsideFlightDelayParam();
			_Preport	= new CAirsideFlightDelayReport(m_pGetLogFilePath);
		}
		break;
	case Airside_FlightActivity:
		{
			_paramter		= new CFlightActivityParam();
			_Preport	= new CFlightActivityReport(m_pGetLogFilePath);
		}
		break;

	case Airside_AircraftOperation:
		_paramter		= new CAirsideOperationParam();
		_Preport	= new CAirsideOperationReport(m_pGetLogFilePath);
		break;

	case Airside_NodeDelay:
		_paramter		= new CAirsideNodeDelayReportParameter();
		_Preport	= new CAirsideNodeDelayReport(m_pGetLogFilePath);
		break;

	case Airside_AircraftOperational:
		{
			_paramter		= new CFlightOperationalParam();
			_Preport	= new CFlightOperationalReport(m_pGetLogFilePath);
		}
		break;

	case Airside_RunwayOperaitons:
		{
			_paramter		= new AirsideRunwayOperationReportParam();
			_Preport	= new AirsideRunwayOperationsReport(m_pGetLogFilePath);
		}
		break;
	case Airside_RunwayUtilization:
		{
			_paramter		= new CAirsideRunwayUtilizationReportParam();
			_Preport	= new CAirsideRunwayUtilizationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_VehicleOperation:
		{
			_paramter		= new CAirsideVehicleOperParameter();
			_Preport	= new CAirsideVehicleOperaterReport(m_pGetLogFilePath,(CAirsideVehicleOperParameter*)_paramter);
		}
		break ;
	case Airside_FlightConflict:
		{
			_paramter		= new AirsideFlightConflictPara();
			_Preport	= new AirsideFlightConflictReport(m_pGetLogFilePath, (AirsideFlightConflictPara*)_paramter);
		}
		break;
	case Airside_TaxiwayDelay:
		{
			_paramter = new CTaxiwayDelayParameters ;
			_Preport = new CTaxiwayDelayReport(m_pGetLogFilePath,(CTaxiwayDelayParameters*)_paramter) ;
		}
		break ;
	case Airside_RunwayExit:
		{
			_paramter = new CRunwayExitParameter ;
			_Preport = new CRunwayExitReport(m_pGetLogFilePath,(CRunwayExitParameter*)_paramter) ;
		}
		break ;
	case Airside_RunwayDelay:
		{
			_paramter		= new AirsideFlightRunwayDelayReportPara();
			_Preport	= new AirsideFlightRunwayDelayReport(m_pGetLogFilePath, (AirsideFlightRunwayDelayReportPara*)_paramter);
		}
		break;
	case Airside_FlightFuelBurning:
		{
			_paramter = new CAirsideFlightFuelBurnParmater ;
			_Preport = new CAirsideFlightFuelBurnReport(m_pGetLogFilePath,(CAirsideFlightFuelBurnParmater*)_paramter,m_airportDB,nProID) ;
		}
		break;
	case Airside_RunwayCrossings:
		{
			_paramter = new AirsideRunwayCrossingsParameter();
			_Preport = new CAirsideRunwayCrossingsReport(m_pGetLogFilePath);
		}
		break;
	case Airside_IntersectionOperation:
		{
			_paramter = new CAirsideIntersectionReportParam();
			_Preport = new CAirsideIntersectionOperationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_TaxiwayUtilization:
		{
			_paramter = new CAirsideTaxiwayUtilizationPara();
			_Preport = new CAirsideTaxiwayUtilizationReport(m_pGetLogFilePath);
		}
		break;
	case Airside_StandOperations:
		{
			_paramter = new CFlightStandOperationParameter();
			_Preport = new CAirsideFlightStandOperationReport(m_pGetLogFilePath);
		}
		break;

	case Airside_ControllerWorkload:
		{
			_paramter = new CAirsideControllerWorkloadParam();
			_Preport = new CAirsideControllerWorkloadReport(m_pGetLogFilePath);
		}
		break;
	case  Airside_TakeoffProcess:
		{
			_paramter = new CAirsideTakeoffProcessParameter();
			_Preport = new CAirsideTakeoffProcessReport(m_pGetLogFilePath);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;



	}
	if(_paramter == NULL || _Preport == NULL)
	{
		ErrorMsg.Format(_T("Read Report File Error : Error Report Type")) ;
		return FALSE ;
	}
	_paramter->SetAirportDB(m_airportDB) ;
	_paramter->SetReportFileDir(m_strReportFilePath);
	_Preport->SetAirportDB(m_airportDB) ;
	_Preport->SetReportFileDir(m_strReportFilePath);
	_paramter->SetProjID(nProID) ;
	//read data 
	_file.getLine() ;
	if(!_paramter->ImportFile(_file) )
	{
		ErrorMsg.Format(_T("Read Report File Error : Read Paramter Error")) ;
		return FALSE ;
	}
	_file.getLine() ;
	if(!_Preport->ImportReportData(_file,ErrorMsg,_paramter->getReportType()) )
	{
		ErrorMsg.Format(_T("Read Report File Error ")) ;
		return FALSE ;
	}
	if(m_pParamters != NULL)
		delete m_pParamters ;
	if(m_pAirsideReport != NULL)
		delete m_pAirsideReport ;

	m_pParamters = _paramter ;
	m_pAirsideReport = _Preport ;
	m_emReportType = (reportType)type ;
	return TRUE ;
}

void CAirsideReportManager::SetReportPath( const CString& strReportPath )
{
	m_strReportFilePath = strReportPath;
}

void CAirsideReportManager::SetTerminalAndProjectPath(Terminal* pTerminal,const CString& _csProjPath)
{
	m_pTerminal = pTerminal;
	m_csProjPath = _csProjPath;
}

void CAirsideReportManager::InitMultiReportList( CXListCtrl& cxListCtrl, int iType /*=0*/,CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	m_multiRunReport.InitListHead(m_emReportType,cxListCtrl,m_pParamters,iType,piSHC);
}

void CAirsideReportManager::SetMultiReportListContent( CXListCtrl& cxListCtr,int iType /*=0*/ )
{
	m_multiRunReport.FillListContent(m_emReportType,cxListCtr,m_pParamters,iType);
}

void CAirsideReportManager::updateMultiRun3Dchart( CARC3DChart& chartWnd,int iType /*=0*/ )
{
	m_multiRunReport.Draw3DChart(m_emReportType,chartWnd,m_pParamters,iType);
}

bool CAirsideReportManager::AirsideMultipleRunReport( ) const
{
	if (std::find(m_vMultipleRun.begin(),m_vMultipleRun.end(),m_emReportType) != m_vMultipleRun.end())
		return true;
	
	return false;
}

