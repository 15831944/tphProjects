// FrameCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FrameCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameCtrlDlg dialog


CFrameCtrlDlg::CFrameCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrameCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrameCtrlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFrameCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrameCtrlDlg)
	DDX_Control(pDX, IDC_FRAMECTRL, m_frameCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrameCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CFrameCtrlDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameCtrlDlg message handlers

BOOL CFrameCtrlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_frameCtrl.SetTimeRange(0,60*60*24);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
