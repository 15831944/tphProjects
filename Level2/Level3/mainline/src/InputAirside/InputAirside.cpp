// InputAirside.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "InputAirside.h"
#include "ALTAirport.h"
#include "../Database/ADODatabase.h"
#include "../common/termfile.h"
#include "Taxiway.h"
#include "Runway.h"
#include "ALTAirportLayout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE InputAirsideDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("InputAirside.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(InputAirsideDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(InputAirsideDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("InputAirside.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(InputAirsideDLL);
	}
	return 1;   // ok
}
InputAirside::InputAirside()
:m_nProjectID(-1)
,m_bTopograhyLoaded(false)
,m_bLoaded(false)
{

}

InputAirside::~InputAirside()
{
	ClearData();
}
void InputAirside::GetAirportList(int nProjID,std::vector<int>& vAirport)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID	FROM ALTAIRPORT	WHERE PROJID = %d"),nProjID);
	
	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vAirport.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

int InputAirside::GetProjectID(const CString& strName)
{
	int nProjID = -1;
	//strName.Trim();
	if(strName.GetLength() ==0)
	{
		return nProjID;
	}

	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID FROM PROJECT WHERE NAME = '%s'"),strName);

	try
	{
	
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);
		if (!adoRs.IsEOF())
		{
			adoRs.GetFieldValue(_T("ID"),nProjID);
		}
		else
		{
			//check if have project exists
			CString strCheckSQL = _T("SELECT ID FROM PROJECT");
			long nRecordCount;
			CADORecordset adoCheckRs;
			CADODatabase::ExecuteSQLStatement(strCheckSQL,nRecordCount,adoCheckRs);
			if (!adoCheckRs.IsEOF())//the project is not valid
			{
				int nTempProjID = -1;
				adoCheckRs.GetFieldValue(_T("ID"),nTempProjID);

				adoCheckRs.MoveNextData();
				if(!adoCheckRs.IsEOF())//have more project return false
					return nProjID;
				else//the database has only one project,
					return nTempProjID;
			}
			else
			{
				strSQL.Empty();

				strSQL.Format(_T("INSERT INTO PROJECT(NAME) VALUES('%s')"),strName);

				long nRecordCount = -1;
				//CADORecordset adoRecordset;
				CADODatabase::ExecuteSQLStatement(strSQL);

				CADORecordset idRecordset;	
				CString strSQLID = _T("SELECT @@Identity AS id");
				CADODatabase::ExecuteSQLStatement(strSQLID,nRecordCount,idRecordset);

				//CADORecordset idRecordset;
				//adoRecordset.NextRecordset(idRecordset);
				idRecordset.GetFieldValue(_T("ID"),nProjID);
			}


		
		}
	}
	catch (CADOException &e) 
	{
		e.ErrorMessage();
	}

	return nProjID;

}

int InputAirside::GetAirspaceID( int nProjID )
{
	
	ArctermFile arcFile;
	arcFile.openFile("C:\\a.txt",0);
	arcFile.appendValue("sssss");
	char ch[512];
	arcFile.getField(ch,512);




	return 0;
}


int InputAirside::GetAirportID( int nProjID )
{
	std::vector<int> vAirports;
	GetAirportList(nProjID,vAirports);
	return vAirports.at(0);
}

bool InputAirside::IsProjectNameExist(const CString& strprojName)
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID FROM PROJECT WHERE NAME = '%s'"),strprojName);	

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);
		if (!adoRs.IsEOF())
		{
			int nProjID = -1;
			adoRs.GetFieldValue(_T("ID"),nProjID);
			if (nProjID >=0)
				return true;
		}
	}
	catch (CADOException &e) 
	{
		e.ErrorMessage();
	}

	return false;
}
bool InputAirside::DeleteProject(const CString& strProjectName)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM PROJECT WHERE NAME = '%s'"),strProjectName);
	try
	{
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		return false;
	}

	return true;

}

int InputAirside::GetAirportCount()const
{
	return static_cast<int>(m_vAirport.size());
}

void InputAirside::InitData( int nProjID )
{
	ClearData();
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *	FROM ALTAIRPORT	WHERE PROJID = %d"),nProjID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			ALTAirportLayout *pAirport = new ALTAirportLayout;
		
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			pAirport->setID(nID);
			pAirport->ReadRecord(adoRs);
			adoRs.MoveNextData();

			m_vAirport.push_back(pAirport);
		}
	}
	catch (CADOException& e)
	{
		throw e;
	}

	std::vector<ALTAirportLayout *>::iterator iter = m_vAirport.begin();
	for (;iter != m_vAirport.end(); ++iter)
	{
		(*iter)->LoadData();
	}

	m_taxiInterruptLines.ReadData(nProjID);
}

ALTAirportLayout * InputAirside::GetAirportbyIndex( int nIndex )const
{
	ALTAirportLayout *pAirport = NULL;

	int nAirportCount = GetAirportCount();
	if(nIndex >= 0 && nIndex < nAirportCount)
	{
		pAirport = m_vAirport.at(nIndex);
	}

	return pAirport;
}

ALTAirport * InputAirside::GetAirportbyID( int nAirportID )
{
	ALTAirport *pAirport = NULL;
	int nAirportCount = GetAirportCount();
	if(nAirportID >= 0)
	{
		for(int nIndex = 0; nIndex < nAirportCount; ++ nIndex)
		{
			ALTAirport *pTempAirport = m_vAirport.at(nIndex);

			if (pTempAirport->getID() == nAirportID)
			{
				pAirport = pTempAirport;
				break;
			}
		}
	}

	return pAirport;
}

ALTAirport * InputAirside::GetAirport( const CGuid& guid )
{
	ALTAirport *pAirport = NULL;
	int nAirportCount = GetAirportCount();
	{
		for(int nIndex = 0; nIndex < nAirportCount; ++ nIndex)
		{
			ALTAirport *pTempAirport = m_vAirport.at(nIndex);

			if (pTempAirport->getGuid() == guid)
			{
				pAirport = pTempAirport;
				break;
			}
		}
	}

	return pAirport;
}
void InputAirside::ClearData()
{
	std::vector<ALTAirportLayout *>::iterator iter = m_vAirport.begin();
	for (;iter != m_vAirport.end(); ++iter)
	{
		delete *iter;
	}
	m_vAirport.clear();
}

Runway * InputAirside::GetRunwayByID( int nID )
{
	Runway *pRetRunway = NULL;
	std::vector<ALTAirportLayout *>::iterator iter = m_vAirport.begin();
	for (;iter != m_vAirport.end(); ++iter)
	{
		pRetRunway  = (*iter)->GetRunwayByID(nID);

		if(pRetRunway != NULL)
			break;
	}	

	return pRetRunway;
}

Taxiway* InputAirside::GetTaxiwayByID( int nID )
{
	Taxiway *pRetTaxiway = NULL;
	std::vector<ALTAirportLayout *>::iterator iter = m_vAirport.begin();
	for (;iter != m_vAirport.end(); ++iter)
	{
		pRetTaxiway  = (*iter)->GetTaxiwayByID(nID);

		if(pRetTaxiway != NULL)
			break;
	}	

	return pRetTaxiway;

}

ALTObject * InputAirside::GetObjectByID( int nID )
{
	ALTObject *pRetObject = NULL;
	std::vector<ALTAirportLayout *>::iterator iter = m_vAirport.begin();
	for (;iter != m_vAirport.end(); ++iter)
	{
		pRetObject  = (*iter)->GetObjectByID(nID);

		if(pRetObject != NULL)
			break;
	}	

	return pRetObject;
}

void InputAirside::LoadAirspace()
{
	ASSERT(m_nProjectID >= 0);
	
	if(m_nProjectID >= 0)//project id is airspace id, there are same value
		m_airspace.LoadAirspace(m_nProjectID);

	m_bLoaded = true;
}

void InputAirside::LoadTopography()
{
	ASSERT(m_nProjectID >= 0);

	if(m_nProjectID >= 0)//project id is airspace id, there are same value
		m_topography.LoadTopography(m_nProjectID);

	m_bTopograhyLoaded = true;
}

ALTAirspace* InputAirside::GetAirspace()
{
	if(!m_bLoaded)
		LoadAirspace();


	return &m_airspace;
}

Topography* InputAirside::GetTopography()
{
	if (!m_bTopograhyLoaded)
		LoadTopography();
	
	return &m_topography;
}

void InputAirside::SetProjID( int nProjID )
{
	m_nProjectID = nProjID;
}

void InputAirside::LoadInputAirside()
{
	ASSERT(m_nProjectID >=0);
	if(m_nProjectID < 0)
		return;

	LoadAirspace();
	LoadTopography();

}

ALTAirportLayout * InputAirside::GetActiveAirport()
{
	if(m_vAirport.size() == 0)
		InitData(m_nProjectID);
	
	if(m_vAirport.size())
		return m_vAirport.at(0);

	return NULL;
}

TaxiInterruptLineList& InputAirside::GetTaxiInterruptLines()
{
	return m_taxiInterruptLines;
}

const TaxiInterruptLineList& InputAirside::GetTaxiInterruptLines() const
{
	return m_taxiInterruptLines;
}

ALTAirportLayout * InputAirside::GetAirportLayoutByID( int nAirportID )const
{
	int nAirportCount = GetAirportCount();

	for(int nIndex = 0; nIndex < nAirportCount; ++ nIndex)
	{
		ALTAirportLayout *pTempAirport = m_vAirport.at(nIndex);

		if (pTempAirport->getID() == nAirportID)
		{
			return pTempAirport;
		}
	}
	

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//CAirsideDatabase
int CAirsideDatabase::GetDatabaseVersion()
{
	int nVersion = ERRORDBVERSION;
	
	CString strSQL(_T("SELECT Version FROM db_version WHERE NAME = 'ARCTerm'"));
	try
	{		
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);
		if (!adoRs.IsEOF())
		{
			adoRs.GetFieldValue(_T("VERSION"),nVersion);
		}
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
	}

	return nVersion;
}
