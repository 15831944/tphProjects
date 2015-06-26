// ProbDistManager.cpp: implementation of the CProbDistManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProbDistManager.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/ProjectManager.h"
#include "../Common/exeption.h"
#include "../Common/airportdatabase.h"
#include "../inputs/probab.h"
#include "../Database/ARCportADODatabase.h"
#include "assert.h"
#include "..\Inputs\probab.h"

#include "../Common/exeption.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Description: Constructor
// Exception:	FileVersionTooNewError
CProbDistManager::CProbDistManager()
:DataSet( ProbabilityDistributionFile )
{

}

CProbDistManager::~CProbDistManager()
{
	clear();
}



void CProbDistManager::clear()
{
	for(UINT i=0; i<m_vProbDist.size(); i++)
		delete m_vProbDist[i];
	m_vProbDist.clear();
}
void CProbDistManager::ResetAllID()
{
	for(UINT i=0; i<m_vProbDist.size(); i++)
		 m_vProbDist[i]->SetID(-1);
}

bool CProbDistManager::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_PRODISTMANAGER"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	return true;
}

bool CProbDistManager::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	clear();

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_PRODISTMANAGER"));
	CADORecordset adoRecordset ;
	long count = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,count,adoRecordset,pAirportDatabase->GetAirportConnection()) ; 
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return false;
	}
	CString sName(_T(""));
	CString sProbtype(_T(""));
	CString sProbdate(_T(""));
	int nID = -1 ;
	while (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("PRODIST_NAME"),sName) ;
		adoRecordset.GetFieldValue(_T("PROBDISTDATA"),sProbdate) ;
		adoRecordset.GetFieldValue(_T("ID"),nID) ;
		ProbabilityDistribution* pProb = NULL ;
		try
		{
			pProb = ProbabilityDistribution::GetTerminalRelateProbDistributionFromDB (sProbdate);
		}
		catch (TwoStringError& e)
		{
			char string[256];
			e.getMessage (string);
			MessageBox(NULL,"Unknown Probability Distribution ","Error",MB_OK) ;
			adoRecordset.MoveNextData();
			continue ;
		}
		CProbDistEntry* pProbDist = new CProbDistEntry( sName, pProb );
		pProbDist->SetID(nID) ;
		m_vProbDist.push_back(pProbDist);

		adoRecordset.MoveNextData() ;
	}
	return true;
}

bool CProbDistManager::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL ;
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_PRODISTMANAGER")) ;

	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	int nCount = m_vProbDist.size();
	int nID = -1 ;
	for (int i = 0; i < nCount; i++)
	{
		nID = m_vProbDist[i]->m_pProbDist->saveDistribution(m_vProbDist[i]->m_csName,pAirportDatabase);
		m_vProbDist[i]->SetID(nID);
	} 
	return true;
}

///////////////////////////////old code version, no more use///////////////////////////////////////////
void CProbDistManager::ReadDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type )
{
	clear() ;
    CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_PRODISTMANAGER WHERE AIRPORTDB_ID = %d"),_airportDBID) ;
	CADORecordset recordset ;
	long count = 0;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ; 
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString name ;
	CString pProb_type ;
	CString pProb_date ;
	int id = -1 ;
	while (!recordset.IsEOF())
	{
         recordset.GetFieldValue(_T("PRODIST_NAME"),name) ;
		 recordset.GetFieldValue(_T("PROBDISTDATA"),pProb_date) ;
         recordset.GetFieldValue(_T("ID"),id) ;
		 ProbabilityDistribution* pProb = NULL ;
		 try
		 {
			 pProb = ProbabilityDistribution::GetTerminalRelateProbDistributionFromDB (pProb_date);
		 }
		 catch (TwoStringError e)
		 {
		 	MessageBox(NULL,"Unknown Probability Distribution ","Error",MB_OK) ;
			recordset.MoveNextData() ;
			continue ;
		 }
		 CProbDistEntry* pProbDist = new CProbDistEntry( name, pProb );
         pProbDist->SetID(id) ;
		 m_vProbDist.push_back(pProbDist);
         
		 recordset.MoveNextData() ;
	}
}

void CProbDistManager::DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type /* = DATABASESOURCE_TYPE_ACCESS */)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_PRODISTMANAGER WHERE AIRPORTDB_ID = %d") ,_airportDBID) ;
	try
	{
		CADODatabase::BeginTransaction(type) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
		CADODatabase::CommitTransaction(type) ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation(type) ;
		e.ErrorMessage() ;
		return ;
	}
}

void CProbDistManager::WriteDataToDB(int _airportDBID,DATABASESOURCE_TYPE type)
{

		CString SQL ;
		SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_PRODISTMANAGER WHERE AIRPORTDB_ID = %d") ,_airportDBID) ;

			CADODatabase::ExecuteSQLStatement(SQL,type) ;
		int nCount = m_vProbDist.size();
		int id = -1 ;
		for (int i = 0; i < nCount; i++)
		{
		id = m_vProbDist[i]->m_pProbDist->WriteDistributionToDB(  m_vProbDist[i]->m_csName , _airportDBID,type);
		m_vProbDist[i]->SetID(id) ;
		} 
}

//////////////////////////////////////////////////////////////////////////
void CProbDistManager::readData( ArctermFile& _file )
{
	clear();
	while( _file.getLine() )
	{
		char szName[128];
		_file.getField( szName, 128 );

		ProbabilityDistribution* pProb = GetTerminalRelateProbDistribution (_file,m_pInTerm);
		
		CProbDistEntry* pProbDist = new CProbDistEntry( CString(szName), pProb );
		
		m_vProbDist.push_back(pProbDist);

	}
}

void CProbDistManager::writeData( ArctermFile& _file ) const
{
	int nCount = m_vProbDist.size();
	for (int i = 0; i < nCount; i++)
	{
		_file.writeField( m_vProbDist[i]->m_csName );
		m_vProbDist[i]->m_pProbDist->writeDistribution( _file );
		_file.writeLine();
	}
}

const char* CProbDistManager::getTitle() const
{
	return "Probability Distribution File";
}


const char* CProbDistManager::getHeaders() const
{
	return "Name,ProbDist Type,ProbDist Data";
}


// create a new name 
// "New Probability Distribution 1" ... "New Probability Distribution n"
CString CProbDistManager::GetNewName()
{
	int nIdx = 1;
	int nCount = getCount();
	CString csBase( "NEW PROBABILITY DISTRIBUTION " );
	for( int i=0; i<nCount; i++ )
	{
		CString csName = m_vProbDist[i]->m_csName;
		if( csName.Find( csBase ) == 0 )
		{
			CString csString = csName.Right( csName.GetLength() - csBase.GetLength() );
			int n = atoi( csString );
			if( n >= nIdx )
				nIdx = n + 1;
		}
	}
	CString csRet;
	csRet.Format( "%s%d", csBase, nIdx );
	return csRet;
}

CString CProbDistManager::GetCopyName(CString _strOri)
{
	int nIdx = 1;
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
	{
		CString csName = m_vProbDist[i]->m_csName;
		if(csName.Find(_strOri) == 0)
		{
			CString strTemp = csName.Right(csName.GetLength() - _strOri.GetLength());
			int nLeft = strTemp.Find('(');
			if(nLeft != -1)
			{
				int nRight = strTemp.Find(')', nLeft);
				if(nRight != -1)
				{
					strTemp = strTemp.Mid(nLeft+1, nRight-nLeft-1);
				}
			}
			int n = atoi(strTemp);
			if(n >= nIdx)
				nIdx = n + 1;
		}
	}
	CString csRet;
	csRet.Format("%s(%d)", _strOri, nIdx);
	return csRet;
}

void CProbDistManager::DeleteItem(int _nIdx)
{
	m_vProbDist.erase( m_vProbDist.begin() + _nIdx );
}

CProbDistEntry* CProbDistManager::getItemByName( const CString& s ) const
{
	size_t nCount = getCount();
	for( size_t m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = getItem( m );			
		if(pPBEntry->m_csName.CompareNoCase(s) == 0)
		{
			return pPBEntry;
		}
	}
	return NULL;
}

CPROBDISTLIST CProbDistManager::getItemListByType(ProbTypes probType)
{
    CPROBDISTLIST vResult;
    size_t nCount = m_vProbDist.size();
    for(size_t i=0; i<nCount; i++)
    {
        ProbabilityDistribution* pProb = m_vProbDist[i]->m_pProbDist;
        if(pProb->getProbabilityType() == probType)
            vResult.push_back(m_vProbDist[i]);
    }
    return vResult;
}

CProbDistEntry* CProbDistManager::getItemByValue(const ProbabilityDistribution* pProb) const
{
    size_t nCount = m_vProbDist.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(m_vProbDist[i]->m_pProbDist->isEqual(pProb))
            return m_vProbDist[i];
    }
    return NULL;
}


