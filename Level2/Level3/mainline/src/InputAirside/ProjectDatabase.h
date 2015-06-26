#pragma once

#include <string>
#include "InputAirsideAPI.h"

//#include "../Common/ProjectManager.h"
class CMasterDatabase;

class INPUTAIRSIDE_API CProjectDatabase
{
public:
	CProjectDatabase(const CString& strAppPath);
	~CProjectDatabase(void);


public:
	void setOpen(BOOL bOpen) { m_bOpen = bOpen;}
	bool OpenProject(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath);

	void CloseProject(CMasterDatabase *pMasterDatabase,const CString& strProjName);
	void ImportProject(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath);
	void ExportProjectBegin(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath);
	void ExportProjectEnd(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath);
protected:
	bool AttachDatabase(CMasterDatabase *pMasterDatabase,const CString& strProjPath,CString& strDatabaseName,const CString& strProjName);
	void DetachDatabase(CMasterDatabase *pMasterDatabase,CString& strDatabaseName);
	
	bool ReadConnectInfoFromReg();
	bool ReadDBItem(HKEY hKey, LPCSTR lpEntry, std::string& strResult);
	void UpdateProject();
	void ShrinkDatabse(const CString& strProjName);

	void CompactRepairMDB( const CString& strFileName, const CString& strPath) const;

	CString LinkDatabaseNameWithARC(const CString& strFormerName);
	
	bool OpenAccessDatabase(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath);
	bool FindSQLServerDataFiles(const CString& strInputPath,  CString& strMdfFile,CString& strLdfFile,CString& strFileName  );

	bool startsqlserver();

protected:
	//please think about it twice before call this function, it got specially used
	void SpecialUpdateForOldPeoject(CMasterDatabase *pMaseterDatabase, const CString& strprojName);
	//check the database have the project name or not
	//if only have one project in the database, it will return TRUE, and assign the database name to project name
	bool IsProjectExistsInDB(CString& strProjName);
	bool IsProjectExistsInDB(const CString& strProjName);
	std::string m_strServer;
	std::string m_strDataBase;
	std::string m_strUser;
	std::string m_strPassWord;
	BOOL m_bOpen;

	CString m_strAppPath;
};
class INPUTAIRSIDE_API CGlobalAirportDataBase
{
public:
	static BOOL OpenGoabalAirportDataBase(const CString& _AppPath) ;
	static BOOL CloseGoabalAirportDataBase() ;
};