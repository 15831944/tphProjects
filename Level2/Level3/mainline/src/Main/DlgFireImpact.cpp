// DlgFireImpact.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "mmsystem.h"
#include "DlgFireImpact.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFireImpact dialog


CDlgFireImpact::CDlgFireImpact(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFireImpact::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFireImpact)
	m_bViewStatus = FALSE;
	//}}AFX_DATA_INIT
	m_nTimerID = 10;
	m_pSimEngineDlg=NULL;
}

CDlgFireImpact::~CDlgFireImpact()
{
	delete m_pSimEngineDlg;
}

void CDlgFireImpact::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFireImpact)
	DDX_Control(pDX, IDC_PROGRESS_FIREIMPACT, m_progressFireImpact);
	DDX_Check(pDX, IDC_CHECK_VIEW_STATUS, m_bViewStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFireImpact, CDialog)
	//{{AFX_MSG_MAP(CDlgFireImpact)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_SimEngineDlg_ProgressBar_SetPos,OnSetPos)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFireImpact message handlers

BOOL CDlgFireImpact::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	m_progressFireImpact.SetRange(0,100);
	m_progressFireImpact.SetPos(0);


	PlaySound("\\arcterm\\blownout.wav",NULL, SND_FILENAME | SND_ASYNC);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFireImpact::OnTimer(UINT nIDEvent) 
{
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgFireImpact::SetSimEngineDlg(CSimEngineDlg *pDlg)
{
	m_pSimEngineDlg=pDlg;
	pDlg->SetUserMsgOwner(this);
	
}

LRESULT CDlgFireImpact::OnSetPos(WPARAM wParam,LPARAM lParam)
{
	m_progressFireImpact.SetPos(wParam);
	int nMin,nMax;
	m_progressFireImpact.GetRange(nMin,nMax);
	if(m_progressFireImpact.GetPos()==nMax)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		UpdateData();
		if(m_bViewStatus)
		{
			ShowWindow(SW_HIDE);
			m_pSimEngineDlg->ShowWindow(SW_SHOW);
		}
		else
			OnOK();
	}
	return TRUE;
}
