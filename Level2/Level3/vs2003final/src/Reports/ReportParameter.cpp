// ReportParameter.cpp: implementation of the CReportParameter class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ReportParameter.h"
#include "reports\ReportParaDB.h"
#include "reports\ReportParameter.h"
#include "reports\reportParaOfTime.h"
#include "reports\reportParaOfProcs.h"
#include "reports\reportParaOfReportType.h"
#include "reports\reportParaOfThreshold.h"
#include "reports\reportParaOfPaxtype.h"
#include "reports\reportParaWithArea.h"
#include "reports\reportParaWithPortal.h"
#include "reports\ReportParaOfTwoGroupProcs.h"
#include "common\exeption.h"
#include "reports\ReportParaOfRuns.h"

const CString strFormatString = "ARCTERM REPORT PARAMETER";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParameter::CReportParameter(  CReportParameter* _pDecorator  )
:m_pDecorator( _pDecorator )
{

}

CReportParameter::~CReportParameter()
{

}


void CReportParameter::SaveReportParaToFile( const CString& _strPathName )
{
	ArctermFile file;
    file.openFile (_strPathName, WRITE);
	
	file.writeLine();
	file.writeField( strFormatString );
	file.writeLine();
	
	file.writeField( GetName() );
	file.writeLine();
	file.writeInt( GetReportCategory() );
	file.writeLine();
	
	WriteClassType( file );
	file.writeLine();
	WriteClassData( file );
	file.writeLine();
	file.endFile();
}

CReportParameter* CReportParameter::LoadReoprtParaFromFile(  const CString& _strPathName,InputTerminal* _pInTerm )
{
	ArctermFile file;
    file.openFile ( _strPathName, READ);
	
	
	char tempChar[256];
	file.getLine();
	file.getField( tempChar, 256 );
	if( strcmp( tempChar, strFormatString)!= 0 )
	{
		AfxMessageBox( "There is some wrong with the Report Parameter File!", MB_OK|MB_ICONINFORMATION );
		return NULL;
	}
	
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
			pNewInstance = new CReportParaOfRuns(NULL);
			break;
		default:
			{
				//throw new ARCCriticalException(Error_File_FormatError," Occurrence Place : Report Parameter DataBase");
				throw new FileFormatError( _strPathName );
			}
		}
		
		vPara.push_back( pNewInstance );
	}
	//if(_tcscmp(sName,_T("THROUGHPUT")) == 0)
	//	vPara.push_back(new CReportParaOfPaxType(NULL)) ; 
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

	if(_tcscmp(sName,_T("THROUGHPUT")) == 0 && file.getVersion() <= 2.9)
	{
		vPara[vPara.size()-1]->SetDecorator(new CReportParaOfPaxType(NULL)) ;
	}
	file.closeIn();
	pPara->SetName( sName );
	pPara->SetReportCategory( enType );
	return pPara;
}

bool CReportParameter::operator == ( const CReportParameter& _anotherInstance )
{
	if( m_enReportCategory == _anotherInstance.m_enReportCategory )
	{
		return IsEqual( _anotherInstance );
	}
	else
	{
		return false;
	}
}

bool CReportParameter::GetReportRuns( std::vector<int>& vReportRuns ) const
{
	if(m_pDecorator)
	{
		return m_pDecorator->GetReportRuns(vReportRuns);
	}
	else
	{
		return false;
	}
}

bool CReportParameter::SetReportRuns( const std::vector<int>& vReportRuns )
{
	if(m_pDecorator)
	{
		return m_pDecorator->SetReportRuns(vReportRuns);
	}
	else
	{
		return false;
	}

}

bool CReportParameter::IsMultiRunReport()
{
	if(m_pDecorator)
	{
		return m_pDecorator->IsMultiRunReport();
	}
	else
	{
		return false;
	}	
}