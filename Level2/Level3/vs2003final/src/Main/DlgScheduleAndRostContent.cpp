// DlgScheduleAndRostContent.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgScheduleAndRostContent.h"
#include ".\dlgscheduleandrostcontent.h"
#include "DlgScheduleAndRosterMapFile.h"
#include "DlgImportScheduleAndRostFile.h"
#include "TermPlanDoc.h"
#include "../Inputs/assigndb.h"


// CDlgScheduleAndRostContent dialog

IMPLEMENT_DYNAMIC(CDlgScheduleAndRostContent, CXTResizeDialog)
CDlgScheduleAndRostContent::CDlgScheduleAndRostContent(InputTerminal* pInputTerm,int nProjectID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgScheduleAndRostContent::IDD, pParent)
	,m_pInputTerm(pInputTerm)
	,m_nProjectID(nProjectID)
	,m_wndTabCtral(&m_aodbImprotMgr)
{
	
}

CDlgScheduleAndRostContent::~CDlgScheduleAndRostContent()
{
}

void CDlgScheduleAndRostContent::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgScheduleAndRostContent, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN,OnBtnOpen)
	ON_BN_CLICKED(IDC_BUTTON_MAP,OnBtnMapFile)
END_MESSAGE_MAP()


// CDlgScheduleAndRostContent message handlers
BOOL CDlgScheduleAndRostContent::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CreateTabCtrl();

	SetResize(m_wndTabCtral.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_OPEN,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_MAP,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgScheduleAndRostContent::CreateTabCtrl()
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_TAB);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		m_wndTabCtral.SetTitle(_T("Schedule"),_T("Roster"));
		m_wndTabCtral.Create(this,rect);
		pWnd->ShowWindow(SW_HIDE);
	}
}

void CDlgScheduleAndRostContent::OnBtnOpen()
{
	CDlgImportScheduleAndRostFile dlg;
	if(dlg.DoModal() == IDOK)
	{
		m_aodbImprotMgr.Init(dlg.GetScheduleFilePath(),dlg.GetRosterFilePath());
		m_aodbImprotMgr.Convert(m_pInputTerm,m_nProjectID);

		m_wndTabCtral.SetTitle(dlg.GetScheduleFileName(),dlg.GetRosterFileName());
		m_wndTabCtral.LoadListData(m_aodbImprotMgr);
	}
}

void CDlgScheduleAndRostContent::OnBtnMapFile()
{
	CDlgScheduleAndRosterMapFile dlg(&m_aodbImprotMgr);
	if (dlg.DoModal() == IDOK)
	{
		m_wndTabCtral.LoadListData(m_aodbImprotMgr);
	}
}

void CDlgScheduleAndRostContent::OnOK()
{
	if(!m_aodbImprotMgr.Inport(m_pInputTerm))
		AfxMessageBox(_T(("Import failed for exsit invalid data")));
	else
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
		if (pDoc)
		{
			m_pInputTerm->flightSchedule->saveDataSet(pDoc->m_ProjInfo.path,true);
			m_pInputTerm->procAssignDB->saveDataSet(pDoc->m_ProjInfo.path,true);
		}
		
		CXTResizeDialog::OnOK();
	}
}