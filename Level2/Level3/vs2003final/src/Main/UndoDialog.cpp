// UndoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "UndoDialog.h"
#include "common\undoproject.h"
#include ".\undodialog.h"
#include "../InputAirside/ProjectDatabase.h"
#include "../Main/MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUndoDialog dialog


CUndoDialog::CUndoDialog( CTermPlanDoc* _pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CUndoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUndoDialog)
	m_csProjName = _T("");
	//}}AFX_DATA_INIT

	m_pDoc = _pDoc;
}


void CUndoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUndoDialog)
	DDX_Control(pDX, IDC_LIST_UNDOPROJ, m_listboxUndoProj);
	DDX_Control(pDX, IDC_BUTTON_UNDO, m_btnUndo);
	DDX_Text(pDX, IDC_EDIT_PROJNAME, m_csProjName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUndoDialog, CDialog)
	//{{AFX_MSG_MAP(CUndoDialog)
	ON_LBN_SELCHANGE(IDC_LIST_UNDOPROJ, OnSelchangeListUndoproj)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, OnButtonUndo)
	ON_COMMAND(ID_PROJECT_OPTIONS, OnProjectOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndoDialog message handlers

BOOL CUndoDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_pUndoMan =  CUndoManager::GetInStance( m_pDoc->m_ProjInfo.path );

	FillUndoList();


	m_csProjName = m_pDoc->m_ProjInfo.name;
	UpdateData( FALSE );

	m_btnUndo.EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CUndoDialog::OnSelchangeListUndoproj() 
{
	// TODO: Add your control notification handler code here
	if( m_listboxUndoProj.GetCurSel() != LB_ERR )
		m_btnUndo.EnableWindow();
	else
		m_btnUndo.EnableWindow( FALSE );	
}

void CUndoDialog::OnButtonUndo() 
{
	// TODO: Add your control notification handler code here
	int nSelIndex = m_listboxUndoProj.GetCurSel();
	if( nSelIndex == LB_ERR )
		return;
	CString propath = m_pDoc->m_ProjInfo.path ;
	CString undoprojectName ;
	m_listboxUndoProj.GetText(nSelIndex,undoprojectName) ;
	CString msg ;
	msg.Format(_T("The changes to the project since %s will be reverted. Are you sure?"),undoprojectName) ;
	if(MessageBox(msg,NULL,MB_OKCANCEL) == IDOK) 
	{
		const CString strProjName = m_pDoc->m_ProjInfo.name;
		const CString strProjPath = m_pDoc->m_ProjInfo.path;
	
		CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
		CProjectDatabase projDatabase(PROJMANAGER->GetAppPath());
		projDatabase.CloseProject(pMasterDatabase, strProjName);

		// inside this method: make current project into an UNDO
		m_pDoc->OnCloseDocument();

		// restore from that specified UNDO
		int nDBIdx = m_listboxUndoProj.GetItemData( nSelIndex );
		m_pUndoMan->RestoreTheProject( nDBIdx );

		projDatabase.OpenProject(pMasterDatabase, strProjName, strProjPath);

		//m_listboxUndoProj.DeleteString( nSelIndex );
		FillUndoList();
		m_btnUndo.EnableWindow( FALSE );


		//CImportExportManager::ImportProject(projDir,dlg.m_strProjectName);
		((CTermPlanApp*)AfxGetApp())->OpenDocumentFile( propath );
	//	m_pDoc->GetMainFrame()->Flesh3DView() ;

		OnOK();
	}
}

void CUndoDialog::OnProjectOptions() 
{
	// TODO: Add your command handler code here
	
}

int CUndoDialog::FillUndoList(void)
{
	m_listboxUndoProj.ResetContent();
	int nCount = m_pUndoMan->GetProjCount();
	for( int i=0; i<nCount; i++ )
	{
		CUndoProject undoProj = m_pUndoMan->GetProject( i );
		COleDateTime oTime = undoProj.GetTime();

		int nItem = m_listboxUndoProj.AddString(
			CString(_T("Modification of    ")) + oTime.Format( "%x" ) + " " + oTime.Format( "%X" )
			);
		m_listboxUndoProj.SetItemData( nItem, i );
	}

	return nCount;
}
