// ReportItem.cpp: implementation of the CReportItem class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "assert.h"
#include "main\resource.h"
#include "ReportItem.h"
#include "common\mutilreport.h"
#include "inputs\in_term.h"
#include "common\termfile.h"
#include "reports\SimAutoReportPara.h"
#include "common\fileman.h"
#include "reports\ReportParaDB.h"
#include "reports\ReportParameter.h"
#include "inputs\SimParameter.h"

#include "common\exeption.h"
#include "reports\ReportParaOfPaxType.h"
#include "reports\ReportParaOfProcs.h"
#include "reports\ReportParaOfReportType.h"
#include "reports\ReportParaOfThreshold.h"
#include "reports\ReportParaOfTime.h"
#include "reports\ReportParaWithArea.h"
#include "reports\ReportParaWithPortal.h"
#include "reports\ReportParaOfTwoGroupProcs.h"
#include "reports\ReportParaOfRuns.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CString arrayReortName[];
extern LOGTOFILENAME inputfilename[];
#define  REPORTTYPENUM 28

CReportItem::CReportItem()
{
	m_strParaFileName   = "";
	m_strResultFileName = "" ;
	m_strReportDirectoryName = "";
	m_iReportType       = -1;
	m_bRealCreate		= false;
}

CReportItem::~CReportItem()
{

}

CReportItem::CReportItem( const CString& _strDirectory, const CString& _strPara, const CString& _strResult,  int _iType )
{
	m_strParaFileName   = _strPara;
	m_strResultFileName = _strResult ;
	m_strReportDirectoryName = _strDirectory;
	m_iReportType       = _iType;
	m_bRealCreate		= false;
}


void CReportItem::readFromFile( ArctermFile& file )
{
	char szBuf[256];

	file.getLine();
	file.getInteger( m_iReportType );

	file.getLine();
	file.getField( szBuf,256 );
	m_strParaFileName = CString(szBuf);

	file.getLine();
	file.getField( szBuf,256 );
	m_strResultFileName = CString(szBuf);

	file.getLine();
	file.getField( szBuf,256 );
	m_strReportDirectoryName = CString(szBuf);
	
	file.getLine();
	char chCreate;
	file.getChar( chCreate );
	m_bRealCreate = chCreate=='Y' ? true : false;
	
	return;
}

void CReportItem::writeToFile(ArctermFile& file ) const
{	
	file.writeInt( m_iReportType );
	file.writeLine();
	file.writeField( m_strParaFileName );
	file.writeLine();
	file.writeField( m_strResultFileName );
	file.writeLine();
	file.writeField( m_strReportDirectoryName );
	file.writeLine();	
	
	char chCreate = m_bRealCreate ? 'Y' : 'N';
	file.writeChar( chCreate );
	file.writeLine();

	return;
}

void CReportItem::SaveReportParaToFile( const CString& _strProjectPath,CReportParameter* m_pReportPara )
{
	ArctermFile file;
    file.openFile ( _strProjectPath +"\\"+ m_strParaFileName, WRITE);

	file.writeLine();
	
	file.writeField( m_pReportPara->GetName() );
	file.writeLine();
	file.writeInt( m_pReportPara->GetReportCategory() );
	file.writeLine();
	
	m_pReportPara->WriteClassType( file );
	file.writeLine();
	m_pReportPara->WriteClassData( file );
	file.writeLine();
	file.endFile();
}
CReportParameter* CReportItem::GetReoprtParaFromFile(  const CString& _strProjectPath,InputTerminal* _pInTerm )
{
	ArctermFile file;
    file.openFile ( _strProjectPath +"\\"+ m_strParaFileName, READ);

	
	char tempChar[256];
	file.getLine();
	file.getField( tempChar, 256 );
	CString sName( tempChar );


	file.getLine();
	ENUM_REPORT_TYPE enType;
	int iType;
	file.getInteger( iType );
	enType = ( ENUM_REPORT_TYPE )iType;

	file.getLine();		
	int iClassType;
	std::vector<CReportParameter*>vPara;
	CReportParameter* pPara;
	while( file.getInteger( iClassType ) )
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
				pNewInstance = new CReportParaOfRuns( NULL );
				break;
			default:
				{
					throw new FileFormatError(  _strProjectPath +"\\"+ m_strParaFileName );
				}
		}

		vPara.push_back( pNewInstance );
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
	//file.getLine();		
	pPara->ReadClassData( file, _pInTerm );
	file.closeIn();
	pPara->SetName( sName );
	pPara->SetReportCategory( enType );
	return pPara;
}
bool CReportItem::initReportItem(  InputTerminal* _pInTerm , const CString& _strPath )
{	
	// make directory and create file
	// make root
	CString strReportItemPath = _strPath + "\\" + m_strReportDirectoryName;
	FileManager::MakePath( strReportItemPath );
	// write report para file
	
	return true;
}

CString CReportItem::getReportString( int _type )
{
	CString strReportType;

	switch( (ENUM_REPORT_TYPE)_type ) 
	{
	case ENUM_PAXLOG_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_LOG );
		break;
		
	case ENUM_DISTANCE_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_DISTANCE );
		break;
		
	case ENUM_QUEUETIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINQ );
		break;
		
	case ENUM_DURATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINT );
		break;
		
	case ENUM_SERVICETIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINS );
		break;
		
	case ENUM_ACTIVEBKDOWN_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_BREAKDOWN );
		break;
		
	case ENUM_UTILIZATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_UTILISATION );
		break;
		
	case ENUM_THROUGHPUT_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_THROUGHPUT );
		break;
		
	case ENUM_QUEUELENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_QLENGTH );
		break;
		
	case ENUM_AVGQUEUELENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_QUEUEPARAM );
		break;
		
	case ENUM_PAXCOUNT_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_OCCUPANCY );
		break;
		
	case ENUM_PAXDENS_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_DENSITY );
		break;
		
	case ENUM_SPACETHROUGHPUT_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_THROUGHPUT );
		break;
		
	case ENUM_COLLISIONS_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_COLLISIONS );
		break;
		
	case ENUM_ACOPERATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_AIRCRAFT );
		break;
		
	case ENUM_BAGCOUNT_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_COUNT );
		break;
		
	case ENUM_BAGWAITTIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_WAITTIME );
		break;
		
	case ENUM_BAGDISTRIBUTION_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_DISTRIBUTION );
		break;
		
	case ENUM_BAGDELIVTIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_DELIVERYTIME );
		break;
		
	case ENUM_ECONOMIC_REP:
		strReportType.LoadString( IDS_TVNN_REP_ECONOMIC );
		break;
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
		break;
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
		break;
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
		strReportType = _T("Exposure Incidence");
		break;
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
		strReportType = _T("Incremental Visit");
		break;

	default:
		strReportType = "ERROR!";
		break;
	}
	
	return strReportType;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CReportSet::CReportSet( const CString& _strDirect )
{
	m_strDirectoryName = _strDirect;
}

void CReportSet::readFromFile( ArctermFile& file )
{
	int iReportNum;
	CReportItem item;	

	char szBuf[256];
	file.getLine();
	file.getField( szBuf, 256 );
	m_strDirectoryName = CString( szBuf );

	file.getLine();
	file.getInteger( iReportNum );
	
	for( int i=0; i<iReportNum; i++ )
	{
		item.readFromFile( file );
		m_mapReporTypeToName[ (ENUM_REPORT_TYPE)item.getReportType() ] = item;
	}

	return;
}

void CReportSet::writeToFile( ArctermFile& file ) const
{
	file.writeField( m_strDirectoryName );
	file.writeLine();

	file.writeInt((int) m_mapReporTypeToName.size() );
	file.writeLine();
	
	ReportTypeToNameMap::const_iterator iter		= m_mapReporTypeToName.begin();
	ReportTypeToNameMap::const_iterator iterend	= m_mapReporTypeToName.end();
	for( ; iter != iterend; ++iter )
	{
		iter->second.writeToFile( file );
	}
	return;
}

void CReportSet::addReportItem( int _reportType )
{
	assert( _reportType >= 0 );

	CString strReportName = arrayReortName[ _reportType ];
	CString strDir		= m_strDirectoryName + "\\"+ strReportName;
	CString strPara		= strDir + "\\" + strReportParaFile;
	CString strResult	= strDir + "\\" + strReportName + ".rep";

	CReportItem item( strDir, strPara, strResult, _reportType );
	
	m_mapReporTypeToName[ (ENUM_REPORT_TYPE)_reportType ] = item;
	return;
}

void CReportSet::removeReportItem( int _reportType )
{
	m_mapReporTypeToName.erase(  (ENUM_REPORT_TYPE)_reportType );
	return;
}

bool CReportSet::getReportItem( int _reportType, CReportItem& _item )
{
	if( m_mapReporTypeToName.find( (ENUM_REPORT_TYPE)_reportType ) != m_mapReporTypeToName.end() )
	{
		_item = m_mapReporTypeToName[ (ENUM_REPORT_TYPE)_reportType ];
		return true;
	}
	else
		return false;
}

bool CReportSet::initReportSet( const CSimParameter* _pSimPara, InputTerminal* _pInTerm, const CString& _strPath )
{
	// first create all report
	int i=0;
	for(i=0; i<REPORTTYPENUM; i++ )
	{
		CString strReportName	= arrayReortName[i];
		CString strDir			= m_strDirectoryName + "\\" + strReportName;
		CString strPara			= strDir + "\\" + strReportParaFile;
		CString strResult		= strDir + "\\" + strReportName + ".rep";

		CReportItem item( strDir, strPara, strResult, i );
		item.initReportItem( _pInTerm, _strPath );
		m_mapReporTypeToName[ (ENUM_REPORT_TYPE)i ] = item;
	}
	
	// second write report param and set create flag for that is real create report
	if( (const_cast<CSimParameter*>(_pSimPara))->GetSimPara()->GetClassType() == "AUTOREPORT" )
	{
		CSimAutoReportPara* pPara = (CSimAutoReportPara*)(const_cast<CSimParameter*>(_pSimPara))->GetSimPara();
		for( i= 0; i<pPara->GetReportCount(); i++ )
		{
			int iReportType = pPara->At(i);
			CReportParameter* pReportPara =  _pInTerm->m_pReportParaDB->GetReportPara( pPara->At( i ) );
			if( pReportPara )
			{
				m_mapReporTypeToName[ (ENUM_REPORT_TYPE)iReportType ].SaveReportParaToFile( _strPath, pReportPara );
			}
			
			m_mapReporTypeToName[ (ENUM_REPORT_TYPE)iReportType ].setCreateFlag( true );
		}
	}
	
	return true;
}

void CReportSet::getAllReortName( std::vector<CString>& _reportNames ) const
{
	ReportTypeToNameMap::const_iterator iter;
	for( iter = m_mapReporTypeToName.begin(); iter != m_mapReporTypeToName.end(); ++iter )
	{
		CString strReportName = arrayReortName[iter->second.getReportType()];
		_reportNames.push_back( strReportName ) ;
	}
}

const std::map<ENUM_REPORT_TYPE, CString> CReportSet::getReportMap( void ) const
{
	std::map<ENUM_REPORT_TYPE, CString> tempMap;

	ReportTypeToNameMap::const_iterator iter;
	for( iter = m_mapReporTypeToName.begin(); iter != m_mapReporTypeToName.end(); ++iter )
	{
		CString strReportName = iter->second.getResultFileName(); 
		tempMap.insert( std::map<ENUM_REPORT_TYPE, CString>::value_type( (ENUM_REPORT_TYPE)(iter->second.getReportType()), strReportName) );
	}
	return tempMap;
}

bool CReportSet::isReportRealExist( int _reportType ) const 
{
	assert(_reportType>=0 && _reportType< REPORTTYPENUM );
	ReportTypeToNameMap::const_iterator iter;
	iter = m_mapReporTypeToName.find( (ENUM_REPORT_TYPE) _reportType );
	//assert( iter!= m_mapReporTypeToName.end() );
	if( iter == m_mapReporTypeToName.end() )
		return false;
	return iter->second.getCreateFlag();
}

CReportItem* CReportSet::operator[] ( int _type )
{
	return &m_mapReporTypeToName[(ENUM_REPORT_TYPE) _type ];
}