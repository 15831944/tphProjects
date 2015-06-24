// MutiRunReportAgent.cpp: implementation of the CMutiRunReportAgent class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ReportParameter.h"
#include "MutiRunReportAgent.h"

#include "reports\PrintLog.h"
#include "reports\UtilizationReport.h"
#include "reports\DistanceReport.h"
#include "reports\PaxCountReport.h"
#include "reports\QueueReport.h"
#include "reports\ThroughputReport.h"
#include "reports\QueueLengthReport.h"
#include "reports\AverageQueueLengthReport.h"
#include "reports\DurationReport.h"
#include "reports\ServiceReport.h"
#include "reports\QueueElement.h"
#include "reports\servelem.h"
#include "reports\ActivityBreakdownReport.h"
#include "reports\ActivityElements.h"
#include "reports\BagWaitReport.h"
#include "reports\BagWaitElement.h"
#include "reports\BagCountReport.h"
#include "reports\PassengerDensityReport.h"
#include "reports\SpaceThroughputReport.h"
#include "reports\CollisionReport.h"
#include "reports\AcOperationsReport.h"
#include "reports\BagDistReport.h"
#include "reports\BagDeliveryTimeReport.h"
#include "reports\FireEvacutionReport.h"
#include "reports\MissFlightReport.h"
#include "common\SimAndReportManager.h"
#include "ConveyorWaitTimeReport.h"
#include "ConveyorWaitLenghtReport.h"
#include "engine\terminal.h"
#include "Reports/BillboardExposureIncidenceReport.h"
#include "Reports/BillboardLinkedProcIncrementalVisitReport.h"
#include "../Common/ARCTracker.h"
#include "Reports/RetailReport.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutiRunReportAgent::CMutiRunReportAgent()
{

}

CMutiRunReportAgent::~CMutiRunReportAgent()
{
	Clear();
}
void CMutiRunReportAgent::Init( const CString& _csParentDirectory, Terminal* _pTerm )
{
	m_pTerm =  _pTerm ;
	m_csParentDirectory = _csParentDirectory;
	
}
void CMutiRunReportAgent::Clear()
{ 
	std::map<ENUM_REPORT_TYPE,CBaseReport*>::iterator iter = m_mapMutiRunPerformers.begin();
	std::map<ENUM_REPORT_TYPE,CBaseReport*>::iterator iterEnd = m_mapMutiRunPerformers.end();
	for( ; iter!=iterEnd; ++iter )
	{
		delete iter->second;
	}
	m_mapMutiRunPerformers.clear();	

	m_vReportParameters.clear();
}
//add new report which you want to generate
void CMutiRunReportAgent::AddReportWhatToGen(ENUM_REPORT_TYPE _reportType, CReportParameter* _pReportPara, int _iFloorCount )
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT( m_pTerm );
	ASSERT( _pReportPara );
	
	CBaseReport* pPara = m_mapMutiRunPerformers[ _reportType ];
	if( !pPara )//not exist
	{
		m_pTerm->GetSimReportManager()->SetCurrentReportType( _reportType );
		switch( _reportType )
		{
		case ENUM_PAXLOG_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CPrintLog(m_pTerm, m_csParentDirectory );						
			break;
	    case ENUM_UTILIZATION_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CUtilizationReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_THROUGHPUT_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CThroughputReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_QUEUELENGTH_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CQueueLengthReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_AVGQUEUELENGTH_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CAverageQueueLengthReport(m_pTerm, m_csParentDirectory );			
			break;
		case ENUM_DISTANCE_REP:
			m_mapMutiRunPerformers[ _reportType ]= new CDistanceReport(m_pTerm, m_csParentDirectory );			
			break;
		case ENUM_PAXCOUNT_REP:
			m_mapMutiRunPerformers[ _reportType ]=new CPaxCountReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_QUEUETIME_REP:
			m_mapMutiRunPerformers[ _reportType ]=new CQueueReport(m_pTerm, m_csParentDirectory );					    
			break;
		case ENUM_DURATION_REP:
			m_mapMutiRunPerformers[ _reportType ]=new CDurationReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_RETAIL_REP:
			m_mapMutiRunPerformers[ _reportType ]=new CRetailReport(m_pTerm, m_csParentDirectory );	
			break;
		case ENUM_SERVICETIME_REP:
			m_mapMutiRunPerformers[ _reportType ]=new CServiceReport(m_pTerm, m_csParentDirectory );					    
			break;
		case ENUM_ACTIVEBKDOWN_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CActivityBreakdownReport(m_pTerm, m_csParentDirectory );						
			break;

		case ENUM_BAGWAITTIME_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CBagWaitReport(m_pTerm, m_csParentDirectory );						
			break;

		case ENUM_BAGCOUNT_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CBagCountReport(m_pTerm, m_csParentDirectory );						
			break;

		case ENUM_PAXDENS_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CPassengerDensityReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_SPACETHROUGHPUT_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CSpaceThroughputReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_COLLISIONS_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CCollisionReport(m_pTerm, m_csParentDirectory, _iFloorCount);			
			break;
		case  ENUM_ACOPERATION_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CAcOperationsReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_BAGDISTRIBUTION_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CBagDistReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_BAGDELIVTIME_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CBagDeliveryTimeReport(m_pTerm, m_csParentDirectory );						
			break;
		case ENUM_FIREEVACUTION_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CFireEvacutionReport( m_pTerm, m_csParentDirectory );
			break;
		case ENUM_CONVEYOR_WAIT_LENGTH_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CConveyorWaitLenghtReport( m_pTerm, m_csParentDirectory );
			break;
		case ENUM_CONVEYOR_WAIT_TIME_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CConveyorWaitTimeReport( m_pTerm, m_csParentDirectory );
			break;

		case ENUM_MISSFLIGHT_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CMissFlightReport( m_pTerm, m_csParentDirectory );
			break;

		case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CBillboardExposureIncidenceReport( m_pTerm, m_csParentDirectory );
			break;
		case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
			m_mapMutiRunPerformers[ _reportType ] = new CBillboardLinkedProcIncrementalVisitReport( m_pTerm, m_csParentDirectory );
			break;
			
			
		default:
			return;
		}		
	}


	Pair_Report_Para temp;
	temp.first = _reportType;
	temp.second = _pReportPara;
	m_vReportParameters.push_back( temp );
}

void CMutiRunReportAgent::GenerateAll()
{
	PLACE_METHOD_TRACK_STRING();
	int iDetailed = 0;
	int iSize = m_vReportParameters.size();
	for( int i=0; i<iSize; ++i )
	{
		ENUM_REPORT_TYPE reportType = m_vReportParameters[i].first;
		CReportParameter* pReportPara = m_vReportParameters[i].second ;		
		pReportPara->GetReportType( iDetailed );
		m_mapMutiRunPerformers[ reportType ]->InitParameter( pReportPara );
		m_pTerm->GetSimReportManager()->SetCurrentReportType( reportType );
		m_mapMutiRunPerformers[ reportType ]->GenerateReport(m_csParentDirectory,DefaultIsSummary( reportType,pReportPara) );
	}	
}

bool CMutiRunReportAgent::DefaultIsSummary( ENUM_REPORT_TYPE _reportType,CReportParameter* _pReportPara )
{
	switch( _reportType )
	{
		case ENUM_PAXLOG_REP:
		case ENUM_ACOPERATION_REP:
			return true;
		case ENUM_PAXDENS_REP:// detailed
		case ENUM_SPACETHROUGHPUT_REP:
		case ENUM_COLLISIONS_REP:
		case ENUM_BAGDISTRIBUTION_REP:
			return false;
		default:
			{
				int iType = -1;
				_pReportPara->GetReportType( iType  );
				return iType != 0;
			}

	}

	
}