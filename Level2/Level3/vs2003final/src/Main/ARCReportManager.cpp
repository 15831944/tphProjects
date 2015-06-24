#include "StdAfx.h"
#include ".\arcreportmanager.h"
#include "../Reports/MutiRunReportAgent.h"
#include "../AirsideReport/ReportManager.h"
#include "../Common/template.h"
#include "../Reports/ReportParameter.h"
#include "../Common/exeption.h"

#include "../OnboardReport/OnboardReportManager.h"
#include "../LandsideReport/LandsideReportManager.h"



CARCReportManager::CARCReportManager(void)
{
	m_pAirsideReportManager = new CAirsideReportManager(NULL);
	m_pCurrentReportPara		= NULL;

	m_reportType = ENUM_PAXLOG_REP;

	m_multiRunsRportType= ENUM_PAXLOG_REP;

	m_eLoadReportType = load_by_system;

	m_bShowGraph = TRUE;
	m_pOnboardReportManager = new OnboardReportManager();

	m_pLandsideReportManager = new LandsideReportManager();
}

CARCReportManager::~CARCReportManager(void)
{
	delete m_pAirsideReportManager;
}

CString CARCReportManager::GetRepTitle()
{
	switch( m_reportType )
	{
	case ENUM_BAGDELIVTIME_REP:
		return CString("Bag Delivery Time Report");

	case ENUM_BAGDISTRIBUTION_REP:
		return CString("Bag Distribution Report");

	case ENUM_ACOPERATION_REP:
		return CString("Aircraft Operations Report");

	case ENUM_PAXLOG_REP:
		return CString( "Passenger Log Report" );

	case ENUM_UTILIZATION_REP:
		return CString( "Utilization Report" );

	case ENUM_THROUGHPUT_REP:
		return CString( "Throughput Report" );

	case ENUM_QUEUELENGTH_REP:
		return CString( "Queue Length History Report" );

	case ENUM_AVGQUEUELENGTH_REP:
		return CString( "Critical Queue Parameters Report" );

	case ENUM_PAXCOUNT_REP:
		return CString("Passenger Count Report");
	case ENUM_RETAIL_REP:
		return CString("Retail Activity Report");
	case ENUM_DISTANCE_REP:
		return CString("Distance Report");

	case ENUM_QUEUETIME_REP:
		return CString(" Queue Time Report");

	case ENUM_CONVEYOR_WAIT_TIME_REP:
		return CString(" Conveyor Wait Time Report");

	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		return CString(" Conveyor Wait Length Report");

	case ENUM_DURATION_REP:
		return CString(" Duration Report");

	case ENUM_SERVICETIME_REP:
		return CString(" Service Time Report");

	case ENUM_ACTIVEBKDOWN_REP:
		return CString( " Activity Breakdown Report" );

	case ENUM_BAGWAITTIME_REP:
		return CString( " Baggage Wait Time Report" );

	case ENUM_BAGCOUNT_REP:
		return CString( " Bag Count Report" );

	case ENUM_PAXDENS_REP:
		return CString( " Passenger Density Report" );

	case ENUM_SPACETHROUGHPUT_REP:
		return CString( "Throughput Report" );

	case ENUM_COLLISIONS_REP:
		return CString( "Collisions Report");

	case ENUM_MISSFLIGHT_REP:
		return CString( "Miss Flight Report" );

	}
	return CString( "Report" );
}

CString CARCReportManager::GetAirsideReportTitle() const 
{
	if (m_pAirsideReportManager)
		return m_pAirsideReportManager->GetCurrentReportName();

	return "";
}
CString CARCReportManager::GetOnboardReportTitle() const
{
	if (m_pOnboardReportManager)
		return m_pOnboardReportManager->GetCurrentReportName();

	return "";
}
CString CARCReportManager::GetLandsideReportTitle() const
{
	if (m_pLandsideReportManager)
		return m_pLandsideReportManager->GetCurrentReportName();

	return "";
}

int CARCReportManager::GetSpecType()
{
	switch( m_reportType )
	{
	case ENUM_BAGDELIVTIME_REP:
		return BagDeliveryTimeSpecFile;

	case ENUM_BAGDISTRIBUTION_REP:
		return BagDistSpecFile;

	case ENUM_ACOPERATION_REP:
		return AcOperationsSpecFile;
		
	case ENUM_PAXLOG_REP:
		return PaxLogSpecFile;

	case ENUM_UTILIZATION_REP:
		return UtilizationSpecFile;
		
	case ENUM_THROUGHPUT_REP:
		return ThroughputSpecFile;

	case ENUM_QUEUELENGTH_REP:
		return QueueLengthSpecFile;

	case ENUM_AVGQUEUELENGTH_REP:
		return AverageQueueLengthSpecFile;

	case ENUM_PAXCOUNT_REP:
		return PassengerCountSpecFile;

	case ENUM_DISTANCE_REP:
		return DistanceSpecFile;

	case ENUM_RETAIL_REP:
		return RetailSpecFile;

	case ENUM_QUEUETIME_REP:
		return QueueTimeSpecFile;
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		return ConveyorWaitTimeSpecFile;

	case ENUM_DURATION_REP:
		return DurationSpecFile;

	case ENUM_SERVICETIME_REP:
		return ServiceTimeSpecFile;

	case ENUM_ACTIVEBKDOWN_REP:
		return ActivityBreakdownSpecFile;

	case ENUM_BAGWAITTIME_REP:
		return BagWaitTimeSpecFile;

	case ENUM_BAGCOUNT_REP:
		return BagCountSpecFile;

	case ENUM_PAXDENS_REP:
		return PassengerDensitySpecFile;

	case ENUM_SPACETHROUGHPUT_REP:
		return SpaceThroughputSpecFile;

	case ENUM_COLLISIONS_REP:
		return CollisionSpecFile;

	case ConveyorWaitLengthReport:
		return ConveyorWaitLengthSpecFile;
	case ConveyorWaitTimeReport:
		return ConveyorWaitTimeSpecFile;
		break;
	}
	return 0;//?
}

void CARCReportManager::GenerateReport(const PROJECTINFO& ProjInfo, Terminal* pTerminal, int nFloorCount)
{
	ASSERT(pTerminal != NULL);
	if(pTerminal == NULL)
	{
		AfxMessageBox(_T("Cannot generate report, parameter error,NULL termial poniter"));
		return;
	}
	try
	{
		if(m_pCurrentReportPara)
		{
			if(m_pCurrentReportPara->IsMultiRunReport())
			{
				CCompareModel model;
				model.SetModelName(ProjInfo.name);
				model.SetModelLocation(ProjInfo.path);
				getMultiReport().GetCompareModelList().Clear();
				getMultiReport().GetCompareModelList().AddModel(model);

				m_multiReport.GenerateReport(m_pCurrentReportPara,ProjInfo.path,pTerminal,nFloorCount);
			}
			else
			{

				CMutiRunReportAgent temp;
				//temp.Init( m_ProjInfo.path, &m_terminal );
				temp.Init(ProjInfo.path,pTerminal);
				temp.AddReportWhatToGen( m_pCurrentReportPara->GetReportCategory(), m_pCurrentReportPara ,nFloorCount);								
				temp.GenerateAll();
			}
		}


	}
	catch( FileOpenError* pError )
	{
		MessageBox( NULL, "No Data Available", "Error", MB_OK | MB_ICONWARNING );
		delete pError;
		return;
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;			
		return;
	}
	catch(StringError* pError)
	{
		char szBuf[128];
		pError->getMessage(szBuf);
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete pError;			
		return;
	}
}

void CARCReportManager::SetLoadReportType( LoadReportType _type )
{
	m_eLoadReportType = _type;
}

void CARCReportManager::SetUserLoadReportFile( const CString& _strFile )
{
	m_strUserLoadReportFile = _strFile;
}

LoadReportType CARCReportManager::GetLoadReportType(void) const
{
	return m_eLoadReportType;
}

const CString& CARCReportManager::GetUserLoadReportFile( void ) const
{
	return m_strUserLoadReportFile;
}

void CARCReportManager::SetReportPara( CReportParameter* _pPara )
{
	m_pCurrentReportPara = _pPara;
}
































