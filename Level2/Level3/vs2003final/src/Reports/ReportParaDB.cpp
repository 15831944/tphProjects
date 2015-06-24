// ReportParaDB.cpp: implementation of the CReportParaDB class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "main\resource.h"
#include "ReportParaDB.h"
#include "ReportParameter.h"
#include "common\exeption.h"
#include "ReportParaOfPaxType.h"
#include "ReportParaOfProcs.h"
#include "ReportParaOfReportType.h"
#include "ReportParaOfThreshold.h"
#include "ReportParaOfTime.h"
#include "ReportParaWithArea.h"
#include "ReportParaWithPortal.h"
#include "ReportParaOfTwoGroupProcs.h"
#include "ReportParaOfRuns.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaDB::CReportParaDB():DataSet( ReportParameterFile )
{
	m_fVersion = 5.0f;	
}

CReportParaDB::~CReportParaDB()
{
	clear();
}

void CReportParaDB::clear()
{
	int iSize = m_vAllReportPara.size();
	for( int i=0; i<iSize; ++i )
	{
		delete m_vAllReportPara[i];
	}
	m_vAllReportPara.clear();
}
CReportParameter* CReportParaDB:: operator [] ( int _iIdx )
{
	if( _iIdx<0 || (unsigned)_iIdx>m_vAllReportPara.size() )
	{
		throw new OutOfRangeError( "CReportParaDB:: operator []");
	}

	return m_vAllReportPara[ _iIdx ];
}

CReportParameter* CReportParaDB::GetParameterbyName(const CString& _strParaName ) const
{
	int iSize = m_vAllReportPara.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vAllReportPara[i]->GetName() == _strParaName )
		{
			return m_vAllReportPara[i];
		}
	}

	return NULL;
}

CReportParameter* CReportParaDB::GetParameterbyType(ENUM_REPORT_TYPE _reportType ) const
{
	int iSize = m_vAllReportPara.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vAllReportPara[i]->GetReportCategory() == _reportType )
		{
			return m_vAllReportPara[i];
		}
	}

	return NULL;
}

CReportParameter* CReportParaDB::GetReportPara( ENUM_REPORT_TYPE _enReportCategory )
{
	int iSize = m_vAllReportPara.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vAllReportPara[ i ]->GetReportCategory() == _enReportCategory )
		{
			return m_vAllReportPara[ i ];
		}
	}

	return NULL;
}
void CReportParaDB::readData( ArctermFile& p_file )
{
	int iSize ;
	p_file.getLine();
	p_file.getInteger( iSize );
	char strName[256];
	int iReportCateGory;
//	if (iSize == 26)
//	{
		for( int i=0; i<iSize; ++i )
		{
			p_file.getLine();
			p_file.getField( strName, 256 );
			int iClassType;
			
			p_file.getInteger( iReportCateGory );
			std::vector<CReportParameter*>vPara;
			std::vector<int> vType;
			CReportParameter* pPara;
			while( p_file.getInteger( iClassType ) )
			{
				CReportParameter* pNewInstance = NULL;
				switch ( iClassType ) 
				{
					case PaxType_Para:
						pNewInstance = new CReportParaOfPaxType(NULL);
						break;
					case ProcGroup_Para :
						pNewInstance = new CReportParaOfProcs(NULL);
						break;
					case ReportType_Para:
						pNewInstance = new CReportParaOfReportType(NULL);
						break;
					case Threshold_Para:
						pNewInstance = new CReportParaOfThreshold(NULL);
						break;
					case Time_Para:
						pNewInstance = new CReportParaOfTime(NULL);
						break;
					case Areas_Para:
						pNewInstance = new CReportParaWithArea(NULL);
						break;
					case Portals_Para:
						pNewInstance = new CReportParaWithPortal(NULL);
						break;
					case FromTo_Para:
						pNewInstance = new CReportParaOfTwoGroupProcs( NULL );
						break;
					case Runs_Para:
						pNewInstance = new CReportParaOfRuns(NULL);
						break;
					default:
						{
							//throw new ARCCriticalException(Error_File_FormatError," Occurrence Place : Report Parameter DataBase");
							throw new FileFormatError( "Occurrence Place : Report Parameter DataBase" );
						}
				}

				vPara.push_back( pNewInstance );
				vType.push_back(iClassType);
			}
		
			int iParaCount = vPara.size();
			if( iParaCount > 0 )
			{
				pPara = vPara[ 0 ];
				for( int i=0,j=1;j<iParaCount; ++j,++i )
				{
					vPara[i]->SetDecorator( vPara[ j ] );
				}
			}

			pPara->SetReportCategory( (ENUM_REPORT_TYPE)iReportCateGory );
			pPara->ReadClassData( p_file, m_pInTerm );

			if(_tcscmp(strName,_T("THROUGHPUT")) == 0)
			{
				if (std::find(vType.begin(),vType.end(),PaxType_Para) == vType.end())
				{
					vPara[vPara.size()-1]->SetDecorator(new CReportParaOfPaxType(NULL)) ;
				}
				
			}
			p_file.getLine();
			pPara->SetName( CString( strName ) );
			m_vAllReportPara.push_back( pPara );
		}
//	}
//	else
//		initDefaultValues();
}
void CReportParaDB::writeData( ArctermFile& p_file ) const
{
	int iSize = m_vAllReportPara.size();
	p_file.writeInt( iSize );
	p_file.writeLine();
	
	for( int i=0; i<iSize; ++i )
	{
		p_file.writeField( m_vAllReportPara[i]->GetName() );
		p_file.writeInt( m_vAllReportPara[i]->GetReportCategory() );
		m_vAllReportPara[i]->WriteClassType( p_file );
		p_file.writeLine();

		m_vAllReportPara[i]->WriteClassData( p_file );		
		p_file.writeLine();
	}
}
void CReportParaDB::initDefaultValues (void)
{
	InitDefaultReportData();
}
CReportParameter* CReportParaDB::At( int _iIdx )
{
	if( _iIdx<0 || (unsigned)_iIdx>m_vAllReportPara.size() )
	{
		throw new OutOfRangeError("CReportParaDB::At" );
	}
	
	return m_vAllReportPara[ _iIdx ];
}
CReportParameter* CReportParaDB::BuildReportParam( ENUM_REPORT_TYPE _enumReportType )
{
	CReportParameter* pReportParam= NULL;
	switch( _enumReportType )
		{
		case ENUM_PAXLOG_REP:	//1
			pReportParam = new CReportParaOfTime( new CReportParaOfPaxType( new CReportParaOfReportType( new CReportParaOfRuns(NULL))) );
			pReportParam->SetReportType( 1 );	// summary
			break;
			
		case ENUM_DISTANCE_REP: //2
		case ENUM_DURATION_REP:	// 4
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfPaxType( new CReportParaOfTwoGroupProcs(  new CReportParaOfRuns(NULL) ) ) ) ) );
			break;
		case ENUM_PAXCOUNT_REP://11
		case ENUM_FIREEVACUTION_REP:
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfPaxType(  new CReportParaOfRuns(NULL)))));
			break;
			
		case ENUM_QUEUETIME_REP: //3
		case ENUM_SERVICETIME_REP:	//5
		case ENUM_BAGWAITTIME_REP://17
		case ENUM_CONVEYOR_WAIT_TIME_REP:
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( 
				new CReportParaOfPaxType( new CReportParaOfProcs(  new CReportParaOfRuns(NULL)) ))));
			break;
			
		case ENUM_ACTIVEBKDOWN_REP:	//6
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfPaxType( new CReportParaOfRuns(NULL)) ));
			break;
			
		case ENUM_UTILIZATION_REP: //7
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfProcs( new CReportParaOfRuns(NULL)) ));
			break;
			
		case ENUM_THROUGHPUT_REP: //8
		case ENUM_RETAIL_REP:
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfProcs(new CReportParaOfPaxType( new CReportParaOfRuns(NULL))) )));
			break;
		case ENUM_QUEUELENGTH_REP://9
		case ENUM_AVGQUEUELENGTH_REP://10
		case ENUM_CONVEYOR_WAIT_LENGTH_REP:
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfProcs( new CReportParaOfRuns(NULL)) )));
			break;
			
		case ENUM_PAXDENS_REP://12
			pReportParam = new CReportParaOfTime( new CReportParaOfPaxType( new CReportParaWithArea( new CReportParaOfReportType(  new CReportParaOfRuns(NULL))) ) );
			pReportParam->SetReportType( 0 ); // detail
			break;
			
		case ENUM_SPACETHROUGHPUT_REP://13
			pReportParam = new CReportParaOfTime( new CReportParaOfPaxType( new CReportParaWithPortal( new CReportParaOfReportType(  new CReportParaOfRuns(NULL)) )) );
			pReportParam->SetReportType( 0 ); // detail
			break;
			
		case ENUM_COLLISIONS_REP://14
			pReportParam = new CReportParaOfTime( new CReportParaOfThreshold( new CReportParaOfPaxType( new CReportParaWithArea( new CReportParaOfReportType(  new CReportParaOfRuns(NULL)) ) ) ) );
			pReportParam->SetReportType( 0 ); // detail
			break;
			
		case ENUM_ACOPERATION_REP://15
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfPaxType(  new CReportParaOfRuns(NULL)) ) );
			pReportParam->SetReportType( 1 ); // summary
			break;

		case ENUM_BAGDISTRIBUTION_REP://18
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType(  new CReportParaOfRuns(NULL)) );
			pReportParam->SetReportType( 0 ); // detail
			break;
			
		case ENUM_BAGCOUNT_REP://16
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfProcs(  new CReportParaOfRuns(NULL) )) ) );
			break;
			
		case ENUM_BAGDELIVTIME_REP://19
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfPaxType(  new CReportParaOfRuns(NULL)) ));
			break;

		case ENUM_PASSENGERNO_REP:
		case ENUM_MISSFLIGHT_REP:
			pReportParam = new CReportParaOfTime( new CReportParaOfPaxType( new CReportParaOfReportType(  new CReportParaOfRuns(NULL))) );
			pReportParam->SetReportType( 0 ); // detail
			break;

		case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
		case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
			pReportParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfProcs( new CReportParaOfRuns(NULL)) )));
			break;

			
		default:
			pReportParam = NULL;
		}

	if( pReportParam )
	{
		std::vector<CMobileElemConstraint>vPaxType;
		if( pReportParam->GetPaxType(vPaxType))
		{
			CMobileElemConstraint temp(m_pInTerm);
			//temp.SetInputTerminal( m_pInTerm );
			temp.initDefault();
			vPaxType.push_back( temp );
			pReportParam->SetPaxType( vPaxType );
		}
	}
	return pReportParam;
}
void CReportParaDB::InitDefaultReportData()
{	
	CReportParameter* pReportParam= NULL;
	CString strReportType;
	
	strReportType.LoadString( IDS_TVNN_REP_PAX_LOG );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_PAXLOG_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_PAXLOG_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

		
	strReportType.LoadString( IDS_TVNN_REP_PAX_DISTANCE );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_DISTANCE_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_DISTANCE_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
		

	strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINQ );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_QUEUETIME_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_QUEUETIME_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
		
	
	strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINT );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_DURATION_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_DURATION_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINS );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_SERVICETIME_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_SERVICETIME_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	strReportType.LoadString( IDS_TVNN_REP_PAX_BREAKDOWN );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_ACTIVEBKDOWN_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_ACTIVEBKDOWN_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	strReportType.LoadString( IDS_TVNN_REP_PROCESSOR ); // by aivin
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_PASSENGERNO_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_PASSENGERNO_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

	strReportType.LoadString( IDS_TVNN_REP_PROC_UTILISATION );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_UTILIZATION_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_UTILIZATION_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
		
	
	strReportType.LoadString( IDS_TVNN_REP_PROC_THROUGHPUT );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_THROUGHPUT_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_THROUGHPUT_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	
	strReportType.LoadString( IDS_TVNN_REP_PROC_QLENGTH );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_QUEUELENGTH_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_QUEUELENGTH_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	
	strReportType.LoadString( IDS_TVNN_REP_PROC_QUEUEPARAM );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_AVGQUEUELENGTH_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_AVGQUEUELENGTH_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
		
	strReportType.LoadString( IDS_TVNN_REP_SPACE_OCCUPANCY );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_PAXCOUNT_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_PAXCOUNT_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

		
	
	strReportType.LoadString( IDS_TVNN_REP_SPACE_DENSITY );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_PAXDENS_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_PAXDENS_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	
	strReportType.LoadString( IDS_TVNN_REP_SPACE_THROUGHPUT );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_SPACETHROUGHPUT_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_SPACETHROUGHPUT_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
	
	strReportType.LoadString( IDS_TVNN_REP_SPACE_COLLISIONS );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_COLLISIONS_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_COLLISIONS_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
		
	
	strReportType.LoadString( IDS_TVNN_REP_AIRCRAFT );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_ACOPERATION_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_ACOPERATION_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
		
	
	strReportType.LoadString( IDS_TVNN_REP_BAG_COUNT );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_BAGCOUNT_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_BAGCOUNT_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

		
	
	strReportType.LoadString( IDS_TVNN_REP_BAG_WAITTIME );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_BAGWAITTIME_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_BAGWAITTIME_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
		
	strReportType.LoadString( IDS_TVNN_REP_BAG_DISTRIBUTION );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_BAGDISTRIBUTION_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_BAGDISTRIBUTION_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
			
	
	strReportType.LoadString( IDS_TVNN_REP_BAG_DELIVERYTIME );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_BAGDELIVTIME_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_BAGDELIVTIME_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
		
	strReportType.LoadString( IDS_TVNN_REP_ECONOMIC );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_ECONOMIC_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_ECONOMIC_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

	strReportType.LoadString( IDS_TVNN_REP_FIREEVACUTION );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_FIREEVACUTION_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_FIREEVACUTION_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

	strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_CONVEYOR_WAIT_LENGTH_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_CONVEYOR_WAIT_LENGTH_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

	strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_CONVEYOR_WAIT_TIME_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_CONVEYOR_WAIT_TIME_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
	strReportType.LoadString( ID_REPORTS_PASSENGER_MISSFLIGHT );
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_MISSFLIGHT_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_MISSFLIGHT_REP );
		m_vAllReportPara.push_back( pReportParam );
	}
	
	//need to add billboard default

	//strReportType.LoadString( ID_REPORTS_PASSENGER_MISSFLIGHT );
	strReportType = _T("Exposure Incidence");
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

	strReportType = _T("Incremental visit");
	strReportType.MakeUpper();
	pReportParam = BuildReportParam( ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP );
	if( pReportParam )
	{
		pReportParam->SetName( strReportType );
		pReportParam->SetReportCategory( ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP );
		m_vAllReportPara.push_back( pReportParam );
	}

	strReportType = _T("Retail Report");
	strReportType.MakeUpper();
	pReportParam = BuildReportParam(ENUM_RETAIL_REP);
	if (pReportParam)
	{
		pReportParam->SetName(strReportType);
		pReportParam->SetReportCategory(ENUM_RETAIL_REP);
		m_vAllReportPara.push_back(pReportParam);
	}

}

void CReportParaDB::readObsoleteData ( ArctermFile& p_file )
{
	if( p_file.getVersion() >=2.4 && p_file.getVersion() <2.5 ) 
	{
		// read old data
		readData( p_file );

		// add new (fire rep)
		CString strReportType;
		CReportParameter* pReportParam= NULL;
		
		strReportType.LoadString( IDS_TVNN_REP_FIREEVACUTION );
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_FIREEVACUTION_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_FIREEVACUTION_REP );
			m_vAllReportPara.push_back( pReportParam );
		}

		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_CONVEYOR_WAIT_LENGTH_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_CONVEYOR_WAIT_LENGTH_REP );
			m_vAllReportPara.push_back( pReportParam );
		}
		
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_CONVEYOR_WAIT_TIME_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_CONVEYOR_WAIT_TIME_REP );
			m_vAllReportPara.push_back( pReportParam );
		}

		int iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_ACOPERATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfPaxType( NULL) );
				break;
			}
		}
		
		iSize = m_vAllReportPara.size();
		for( i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_DISTANCE_REP
				|| m_vAllReportPara[i]->GetReportCategory() == ENUM_DURATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfTwoGroupProcs( NULL) );				
			}			
		}
		iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{	
			CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
			assert( pPara );				
			while( CReportParameter* pParaNext = pPara->GetDecorator() )
			{
				pPara = pParaNext;
			}
			pPara->SetDecorator( new CReportParaOfRuns( NULL) );
			
		}


	}
	else if ( p_file.getVersion() >=2.5 && p_file.getVersion() <2.59 ) 
	{
		// read old data
		readData( p_file );
		CString strReportType;
		CReportParameter* pReportParam= NULL;
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_CONVEYOR_WAIT_LENGTH_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_CONVEYOR_WAIT_LENGTH_REP );
			m_vAllReportPara.push_back( pReportParam );
		}
		
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_CONVEYOR_WAIT_TIME_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_CONVEYOR_WAIT_TIME_REP );
			m_vAllReportPara.push_back( pReportParam );
		}

		int iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_ACOPERATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfPaxType( NULL) );
				break;
			}
		}

		iSize = m_vAllReportPara.size();
		for( i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_DISTANCE_REP
				|| m_vAllReportPara[i]->GetReportCategory() == ENUM_DURATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfTwoGroupProcs( NULL) );				
			}			
		}

		iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{	
			CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
			assert( pPara );				
			while( CReportParameter* pParaNext = pPara->GetDecorator() )
			{
				pPara = pParaNext;
			}
			pPara->SetDecorator( new CReportParaOfRuns( NULL) );
			
		}

	}
	else if ( p_file.getVersion() >=2.59 && p_file.getVersion() <2.7 ) 
	{
		// read old data
		readData( p_file );
		int iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_ACOPERATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfPaxType( NULL) );
				break;
			}
		}

		iSize = m_vAllReportPara.size();
		for( i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_DISTANCE_REP
				|| m_vAllReportPara[i]->GetReportCategory() == ENUM_DURATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfTwoGroupProcs( NULL) );				
			}			
		}

		iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{	
			CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
			assert( pPara );				
			while( CReportParameter* pParaNext = pPara->GetDecorator() )
			{
				pPara = pParaNext;
			}
			pPara->SetDecorator( new CReportParaOfRuns( NULL) );
			
		}

	}
	else if(p_file.getVersion() >=2.69 && p_file.getVersion() <2.79 ) 
	{															
		readData( p_file );
		int iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{
			if( m_vAllReportPara[i]->GetReportCategory() == ENUM_DISTANCE_REP
				|| m_vAllReportPara[i]->GetReportCategory() == ENUM_DURATION_REP )
			{
				CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
				assert( pPara );				
				while( CReportParameter* pParaNext = pPara->GetDecorator() )
				{
					pPara = pParaNext;
				}
				pPara->SetDecorator( new CReportParaOfTwoGroupProcs( NULL) );				
			}			
		}

		iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{	
			CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
			assert( pPara );				
			while( CReportParameter* pParaNext = pPara->GetDecorator() )
			{
				pPara = pParaNext;
			}
			pPara->SetDecorator( new CReportParaOfRuns( NULL) );
			
		}


	}
	else if (p_file.getVersion() >= 2.79 && p_file.getVersion() < 2.9)
	{
		//read old data
		readData(p_file);
		//add new type
		CString strReportType;
		CReportParameter* pReportParam= NULL;
		strReportType = _T("Exposure Incidence");
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP );
			m_vAllReportPara.push_back( pReportParam );
		}

		strReportType = _T("Incremental visit");
		strReportType.MakeUpper();
		pReportParam = BuildReportParam( ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP );
		if( pReportParam )
		{
			pReportParam->SetName( strReportType );
			pReportParam->SetReportCategory( ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP );
			m_vAllReportPara.push_back( pReportParam );
		}

		int iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{	
			CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
			assert( pPara );				
			while( CReportParameter* pParaNext = pPara->GetDecorator() )
			{
				pPara = pParaNext;
			}
			pPara->SetDecorator( new CReportParaOfRuns( NULL) );
		}

	}
	else if(p_file.getVersion()>=2.9 &&p_file.getVersion()<3.9)
	{
		readData(p_file);
		int iSize = m_vAllReportPara.size();
		for( int i=0; i<iSize; ++i )
		{	
			CReportParameter* pPara = m_vAllReportPara[i]->GetDecorator();
			assert( pPara );				
			while( CReportParameter* pParaNext = pPara->GetDecorator() )
			{
				pPara = pParaNext;
			}
			pPara->SetDecorator( new CReportParaOfRuns( NULL) );
		}
	}
	else if (p_file.getVersion() >= 3.9 && p_file.getVersion() < 4.9)
	{
		readData(p_file);

		CString strReportType;
		CReportParameter* pReportParam= NULL;
		strReportType = _T("Retail Report");
		strReportType.MakeUpper();
		pReportParam = BuildReportParam(ENUM_RETAIL_REP);
		if (pReportParam)
		{
			pReportParam->SetName(strReportType);
			pReportParam->SetReportCategory(ENUM_RETAIL_REP);
			m_vAllReportPara.push_back(pReportParam);
		}
	}
	else
	{
		InitDefaultReportData();
	}
}