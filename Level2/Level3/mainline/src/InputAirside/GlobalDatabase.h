#pragma once
#import "..\..\lib\msado26.tlb" rename("EOF","adoEOF")
#include <string>
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CMasterDatabase
{
public:
	CMasterDatabase(const CString& strAppPath);
	~CMasterDatabase(void);


public:
	bool Connect();
	void Close();
	void Execute(const CString& strSQL);
	bool IsDatabaseAttached(const CString& strDatabaseName);
	
	//if exist, return dbname
	//if no
	bool GetProjectDatabaseName(const CString& strProjName,CString& strDBName);
	bool GetProjectDatabaseNameFromDB(const CString& strProjName,CString& strDBName);
	bool CreateNewprojectDatabaseName(const CString& strProjName,CString& strDBName);
	bool IsDatabaseNameExist(const CString& strDBName);
	bool IsProjectDataNameExist(const CString& strDBName);

	void AttachDatabase();
	void DetachDatabase();

	bool OpenOldProject(const CString& strProjName,const CString& strProjPath);
	bool IsOldProject(const CString& strProjName);

	bool InsertProjectDatabaseToDB(const CString& strProjName,const CString& strDBName);
	bool DeleteProjectDatabaseFromDB(const CString& strProjName);

	bool DeleteOldProject(const CString& strProjName,const CString& strProjPath);
	bool IsProjectExists(const CString& strProjName);

	void SetImportedProjectName(const CString& strName,const CString& strDBName);
	bool IsOpenImportedProject();
	void CancelImportedProjectFlag();
	CString GetImportedProjectName();
	CString GetImportedProjectDatabase();
	int GetDatabaseVersion();
	void Log(const CString& strProjName,const CString& strLogMessage);
protected:
	bool ReadConnectInfoFromReg();
	bool ReadDBItem(HKEY hKey, LPCSTR lpEntry, std::string& strResult);


	bool HandleConnectError();

	bool CheckMasterDatabaseAttached(CString strMdfPath,CString strLdfPath);
protected:
	CString m_strAppPath;

	ADODB::_ConnectionPtr m_pConnection;


	std::string m_strServer;
	std::string m_strDataBase;
	std::string m_strUser;
	std::string m_strPassWord;


	bool m_bImportedProject;
	CString m_strImportedProjName;
	CString m_strImportedProjDatabase;

};
