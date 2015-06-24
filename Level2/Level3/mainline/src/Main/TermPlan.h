// TermPlan.h : main header file for the TERMPLAN application
//

#if !defined(AFX_TERMPLAN_H__5D301063_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
#define AFX_TERMPLAN_H__5D301063_30BE_11D4_92F6_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/*
#ifndef IMPORT_ADO_DLL
#define IMPORT_ADO_DLL
#import "c:\Program files\common files\system\ado\msado15.dll" rename("EOF","adoEOF") rename("adError","adoadError")
#include <adoid.h>
#include <adoint.h>
#include "icrsint.h"
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// CTermPlanApp:
// See TermPlan.cpp for the implementation of this class
//

#include "SimualtionControlLicense.h"
class CNewMultiDocTemplate;
class CMasterDatabase;
class CTermPlanApp : public CWinApp
{
public:

    // Constructor...
	CTermPlanApp();
	~CTermPlanApp();

    // Variables...
    CSimualtionControlLicense  SimulationControlLicense;
	CNewMultiDocTemplate*	m_pDetailTemplate;
	CNewMultiDocTemplate*	m_p3DTemplate;
	CNewMultiDocTemplate*	m_pNewRender3DTemplate;   // terminal 3D for new Render Engine
// 	CNewMultiDocTemplate*	m_p3DAirSideTemplate;
// 	CNewMultiDocTemplate*	m_pNewRender3DAirSideTemplate;   // airside 3D for new Render Engine

	CNewMultiDocTemplate*	m_pReportTemplate;
	CNewMultiDocTemplate*	m_pAirsideReportTemplate_New;
	CNewMultiDocTemplate*	m_pOnboardReportTemplate;
	CNewMultiDocTemplate*	m_pMultiRunReportTemplate;
	CNewMultiDocTemplate*	m_pMathReportTemplate;
	CNewMultiDocTemplate*	m_pDensityAnimTemplate;
    CNewMultiDocTemplate*	m_pFIDSAnimTemplate;
	CNewMultiDocTemplate*	m_pInputDataTemplate;
	CNewMultiDocTemplate*	m_pProcessDefineTemplate;
	CNewMultiDocTemplate*   m_pCompareReportTemplate2;
	CNewMultiDocTemplate*	m_pProcessFlowTemplate;
	CNewMultiDocTemplate*	m_pMathematicTemplate;
	CNewMultiDocTemplate*	m_pAirSideReportTemplate;

	CNewMultiDocTemplate*	m_pOnBoardLayoutTemplate;
	CNewMultiDocTemplate*	m_pAircraftModelDatabaseTermplate;
	CNewMultiDocTemplate*	m_pAircraftFurnishingTermplate;
	CNewMultiDocTemplate*   m_pAircraftLayoutTermplate;

	CNewMultiDocTemplate*	m_pLandsideReportTemplate;
	//CNewMultiDocTemplate*   m_pAircraftLayoutMSVTermplate;

//*	./*/CNewMultiDocTemplate*   m_pAircraftFurnishingMSVTermplate;
//	CNewMultiDocTemplate*	m_pAirsideMSVTemplate;
//	CNewMultiDocTemplate*	m_pLandsideMSVTemplate;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTermPlanApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTermPlanApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// check the license, and pop up a dialog when bPopDlg is TRUE;
	static BOOL CheckLicense(BOOL bPopDlg = FALSE);
	BOOL WriteProfileString( LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue );
	BOOL WriteProfileInt( LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue );
	CString GetProfileString( LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL );
	UINT GetProfileInt( LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault );
	//CString GetAppPath() {return m_sAppPath; }
	CString GetProjectsPath() { return m_sProjPath; }
	CString GetShapesDBPath() { return m_sShapesDBPath; }
	CString GetShapeDataPath() { return m_sShapeDataPath; }
	CString GetResourceDataPath() { return m_sResourceDataPath; }
	CString GetTexturePath()  { return m_sTexturePath; }
	CString GetSurfaceTexturePath()  { return m_sSurfaceTexturePath; }
	CString GetAirlineImgPath() { return m_sAirlineImgPath; }
	CString GetLandSideVehicleImgPath(){return m_sLandsideVehicleImgPath;}
	CString GetLandsideVehicleShapePath(){return m_sLandsideVehicleShapePath;}

	UINT GetClipboardFormat();

	void OnCloseProject();

	//While Delete Current Project from main menu,  Update MRU info
	void UpdateMRUList(CString& prjPath);
	CMasterDatabase * GetMasterDatabase();
protected:
	BOOL ConnectToMasterDatabase();
	BOOL ReadRegister();
	BOOL DoPromptForProjectName( CString& fileName );
	UINT m_nShapeFormat;
	UINT m_nACTypeFormat;
	CString m_sAppPath;
	CString m_sProjPath;
	CString m_sShapesDBPath;
	CString m_sShapeDataPath;
	CString m_sResourceDataPath;
	CString m_sTexturePath;
	CString m_sSurfaceTexturePath;
	CString m_sAirlineImgPath;
	CString m_sLandsideVehicleImgPath;
	CString m_sLandsideVehicleShapePath;

	CMasterDatabase* m_pMasterDatabase;
protected:
	BOOL CheckUserAccess() ;
	CString GetSettingINIFilePath() ;
};

/////////////////////////////////////////////////////////////////////////////

extern CTermPlanApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMPLAN_H__5D301063_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)

