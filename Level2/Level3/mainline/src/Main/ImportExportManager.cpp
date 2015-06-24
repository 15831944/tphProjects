#include "StdAfx.h"
#include "ImportExportManager.h"
#include "TermPlan.h"
#include "InputAirside\AirsideImportExportManager.h"
#include "Common\ProjectManager.h"
#include "Common\exeption.h"
#include "InputAirside\ProjectDatabase.h"
#include "AirsideGround.h"
#include "MainFrm.h"


CImportExportManager::CImportExportManager(void)
{
}

CImportExportManager::~CImportExportManager(void)
{
}

bool CImportExportManager::ImportProject(const CString& strProjPath,const CString& strProjName,bool bExistProject)
{

	CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
	//pMasterDatabase->Connect();
	CString strAirsidePath = _T("");
	strAirsidePath.Format(_T("%s\\INPUT\\Airside\\"),strProjPath);
	CAirsideImportExportManager airsideImportManager(strAirsidePath,strProjName);
	try
	{	
		
		CString strDatabaseName;// = _T("");
		CString strOldProjName;// = _T("");
		CString appPath = PROJMANAGER->GetAppPath();
	
		airsideImportManager.ImportAirside(pMasterDatabase,appPath,strOldProjName,strDatabaseName,bExistProject);	
	}
	catch (FileOpenError *exception)
	{
		delete exception;
		//return;
	}

	CProjectDatabase projDatabase(PROJMANAGER->GetAppPath());
	if(projDatabase.OpenProject(pMasterDatabase,strProjName,strProjPath) == false)
	{	
		projDatabase.CloseProject(pMasterDatabase,strProjName);
		return false;
	}
	try
	{
		
		CAirsideImportFile importFile(&airsideImportManager,airsideImportManager.GetFilePath(AS_GROUND),"",airsideImportManager.getVersion());
		if(importFile.getVersion() < 1042)
		{
			CAirsideGround::ImportAirsideGrounds(importFile);	
		}
	

	}
	catch (FileOpenError *exception)
	{
		//projDatabase.CloseProject(pMasterDatabase,strProjName);
		delete exception;
		//return;
	}
	projDatabase.CloseProject(pMasterDatabase,strProjName);
	return true;
}

void CImportExportManager::ExportProject(const CString& strProjPath,const CString& strProjName)
{
	CString strDirPath = _T("");
	strDirPath.Format(_T("%s\\INPUT\\Airside"),strProjPath);

	CAirsideImportExportManager airsideExportManager(strDirPath+_T("\\"),strProjName);
	//airsideExportManager.SetAirportDB( GetAirportDB(CString()));

	CString strDBName;
	
	//CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();

	//pMasterDatabase->GetProjectDatabaseNameFromDB(strProjName,strDBName);
	//int nVersion =pMasterDatabase->GetDatabaseVersion();
	int nVersion = 1074;//must larger than 1073
	airsideExportManager.ExportAirside(strDBName,nVersion);


	//export airside level data
	//CAirsideExportFile exportFile(&airsideExportManager,airsideExportManager.GetFilePath(AS_GROUND),"",airsideExportManager.getVersion());
	//CAirsideGround::ExportAirsideGrounds(exportFile);
	//exportFile.getFile().endFile();
}
CAirportDatabase* CImportExportManager::GetAirportDB(const CString& strProjName)
{
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	ASSERT(pMDIActive != NULL);
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	return pDoc->GetTerminal().m_pAirportDB ;
	/*PROJECTINFO projInfo;
	PROJMANAGER->GetProjectInfo(strProjName,&projInfo);
	CAirportDatabase* pAirportDB = AIRPORTDBLIST->getAirportDBByID(projInfo.lUniqueIndex);

	if (pAirportDB == NULL)
	{

		CString _strErr;
		_strErr.Format("%s project's global DB index: %d does not exist in the DB list.", projInfo.name, projInfo.lUniqueIndex );
		AfxMessageBox( _strErr, MB_OK|MB_ICONINFORMATION );
	}
	return pAirportDB;*/
}