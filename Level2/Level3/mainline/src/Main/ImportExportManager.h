#pragma once

//the class's responsibilities is in the charge of import and export project,
//include terminal, airside and landside
//specially, airside still has a class with responsibility for import/export --CAirsideImportExportManager
class CAirportDatabase;
class CImportExportManager
{
public:
	CImportExportManager(void);
	~CImportExportManager(void);


public:
	static bool ImportProject(const CString& strProjPath,const CString& strProjName,bool bExistProject);
	static void ExportProject(const CString& strProjPath,const CString& strProjName);
	static CAirportDatabase* GetAirportDB(const CString& strprojName);
};
