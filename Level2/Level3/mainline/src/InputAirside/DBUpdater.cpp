#include "StdAfx.h"
#include ".\dbupdater.h"
#include "../Database/ARCportDatabaseConnection.h"
#include <fstream>
#include "../Database/ADODatabase.h"



//////////////////////////////////////////////////////////////////////////
//DBUpdateItem
DBUpdateItem::DBUpdateItem()
:m_nVersion(0)
,m_strFullPath(_T(""))
{

}
DBUpdateItem::~DBUpdateItem()
{

}
void DBUpdateItem::ReadItem(const CString&strARCTermPath, char* szline)
{
	CString strLine(szline);

	int nPos = strLine.Find(_T("="));

	CString strVersion =  strLine.Left(nPos);
	strVersion.Trim();
	m_nVersion = atoi(strVersion);

	CString strName = strLine.Mid(nPos+1);
	strName.Trim();

	m_strFullPath.Append(strARCTermPath);
	m_strFullPath.Append(_T("\\arctermdb\\script\\"));
	m_strFullPath.Append(strName);

}
int DBUpdateItem::Update()
{


	try
	{	
		CString strMsg ;
		strMsg.Format(_T("Update to version: %d"),m_nVersion);


		//arcportDB.Update(m_strFullPath);

		std::vector<std::string> vSQLScripts;
		ParserSQLScriptFile(m_strFullPath,vSQLScripts);


		//DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->BeginTrans();
		CADODatabase::BeginTransaction(DATABASESOURCE_TYPE_SQLSERVER);
		try
		{

			for (int i =0; i < static_cast<int>(vSQLScripts.size());++i)
			{
				_variant_t varCount;
				DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(vSQLScripts[i].c_str()), &varCount, ADODB::adOptionUnspecified);
			}

			CADODatabase::CommitTransaction(DATABASESOURCE_TYPE_SQLSERVER);

			//DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->CommitTrans();
		}
		catch (_com_error&e)
		{
			//DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->RollbackTrans();

			CADODatabase::RollBackTransation(DATABASESOURCE_TYPE_SQLSERVER);
			throw _com_error(e);

		}

	}
	catch (_com_error& e) 
	{	
		CString strMsg ;
		strMsg.Format(_T("Update to version %d failed, error message: %s"),m_nVersion,(char *)e.Description());
		MessageBox(NULL,strMsg,_T("WARNING"),MB_OK);;
		throw _com_error(e);
		//e.Description();
		return UPDATEERROR;		
	}
	


	return UPDATESUCCESS;
}
bool DBUpdateItem::ParserSQLScriptFile(LPCTSTR lpszSQLFile,std::vector<std::string >& vSQLScript)
{
	std::ifstream file;
	file.open(lpszSQLFile);
	if (!file)
	{
		std::string strErrorMsg = _T("cann't open file: ") + CString(lpszSQLFile) +_T("update failed");
		vSQLScript.push_back(strErrorMsg);

		return false;
	}
	std::string strSQL;
	while (!file.eof())
	{
		char szLine[10240];
		file.getline(szLine, 10240);
		if (strlen(szLine) == 0)
		{
			if (!strSQL.empty())
			{
				vSQLScript.push_back(strSQL);
				strSQL.clear();
			}			
		}
		else
		{
			strSQL.append(szLine, strlen(szLine));
			strSQL.append("    ");
		}
	}

	if (!strSQL.empty())
	{
		vSQLScript.push_back(strSQL);
	}

	return true;
}

int DBUpdateItem::getVersion()
{ 
	return m_nVersion;
}
//////////////////////////////////////////////////////////////////////////
DBUpdateConfigure::DBUpdateConfigure()
:m_strFullPath(_T(""))
{

}

DBUpdateConfigure::~DBUpdateConfigure()
{

}
void DBUpdateConfigure::ReadConfig(const CString& strARCTermPath)
{
	m_strFullPath = strARCTermPath;
	m_strFullPath.Trim();
	m_strFullPath.TrimRight("\\");
	m_strFullPath.Append(_T("\\arctermdb\\script\\DatabaseUpdate.ini"));


	//open config file

	std::ifstream file;
	file.open(m_strFullPath);
	if(!file)
		return;
	char szHeader[256];

	if (!file.eof())
		file.getline(szHeader,1024);

	char szCurVersion[256];
	if (!file.eof())	
		file.getline(szCurVersion,256);


	char szBuffer[1024];
	memset(szBuffer,0,1024);
	if (!file.eof())
		file.getline(szBuffer,1024);
	
	while (strlen(szBuffer) != 0)
	{

		DBUpdateItem updateItem;
		updateItem.ReadItem(strARCTermPath,szBuffer);
		m_vUpdateItem.push_back(updateItem);


		memset(szBuffer,0,1024);
		if (!file.eof())
			file.getline(szBuffer,1024);
	}

	file.close();
	
	
	{
		CString strMsg;
		strMsg.Format(_T("Version Item Count: %d"),static_cast<int>(m_vUpdateItem.size()));

	}
}
bool DBUpdateConfigure::CheckConfig()
{
	std::vector<DBUpdateItem>::iterator iter = m_vUpdateItem.begin();
	std::vector<DBUpdateItem>::iterator iterEnd = m_vUpdateItem.end();
	
	int nVersion = 0;
	int nPreVersion = 0;

	for (;iter != iterEnd; ++iter)
	{
		nVersion = (*iter).getVersion();
		if (nPreVersion !=0)
		{
			if(nVersion != nPreVersion + 1)
			{
				CString strMsg;
				strMsg.Format(_T("Check configure error: version %d "),nVersion);

				return false;
			}
		}
		nPreVersion = nVersion;
	}

	return true;
}
int DBUpdateConfigure::Update(int& nCurVersion)
{

	std::vector<DBUpdateItem>::iterator iter = m_vUpdateItem.begin();
	std::vector<DBUpdateItem>::iterator iterEnd = m_vUpdateItem.end();
	for (;iter != iterEnd; ++iter)
	{
		if((*iter).getVersion() > nCurVersion)
		{
			if ((*iter).Update() != UPDATEERROR)
			{
				nCurVersion = (*iter).getVersion();
			}
			else
			{
				return UPDATEERROR;
			}
		}
	}

	return UPDATESUCCESS;
}

//////////////////////////////////////////////////////////////////////////
DBUpdater::DBUpdater(const CString& strArctermPath)
:m_strARCTermPath(strArctermPath)
{

}

DBUpdater::~DBUpdater(void)
{

}

int DBUpdater::Update()
{

	int nCurVersion = Version();
	if (nCurVersion == ERRORVERSION)
		return -1;


	DBUpdateConfigure dbUpdateConfog;
	dbUpdateConfog.ReadConfig(m_strARCTermPath);
	
	if(!dbUpdateConfog.CheckConfig())
		return UPDATEERROR;
	
   return dbUpdateConfog.Update(nCurVersion);
		
}

int DBUpdater::Version()
{
	CString strSQL(_T("SELECT Version FROM db_version WHERE NAME = 'ARCTerm'"));


	try
	{
		_variant_t varCount;
		ADODB::_RecordsetPtr pRes = DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(strSQL), &varCount, ADODB::adOptionUnspecified);

		if (pRes->GetadoEOF() == VARIANT_FALSE)
		{
			_variant_t var;
			int version = (int)pRes->Fields->GetItem(_bstr_t("Version"))->GetValue();
			
			if (pRes)
				if(pRes->State == ADODB::adStateOpen)
					pRes->Close();

			return version;
		}
		if (pRes)
			if(pRes->State == ADODB::adStateOpen)
				pRes->Close();

	}
	catch (_com_error& e)
	{
		CString strError =  e.Description();
	}


	return ERRORVERSION;
}