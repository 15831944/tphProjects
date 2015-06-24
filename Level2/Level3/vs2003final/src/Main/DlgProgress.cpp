// DlgProgress.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog


CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProgress)
	m_sPercent = _T("");
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_nID = CDlgProgress::IDD;
	m_bCancel = FALSE;
}


void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgress)
	DDX_Control(pDX, IDC_PROGRESS1, m_pbProgress);
	DDX_Text(pDX, IDC_PERCENT, m_sPercent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress message handlers

void CDlgProgress::OnCancel() 
{
	m_bCancel = TRUE;
	//// TRACE("CANCEL pressed\n");
}

void CDlgProgress::PostNcDestroy() 
{
	delete this;
}

BOOL CDlgProgress::Create()
{
	BOOL ret = CDialog::Create(m_nID, m_pParent);
	if(ret)
		m_pbProgress.SetRange(0,100);
	return ret;
}

void CDlgProgress::SetPercentage(short percent)
{
	m_sPercent.Format("%d%%",percent);
	m_pbProgress.SetPos(percent);
	UpdateData(FALSE);
	PumpMessages();
}

void CDlgProgress::SetTitle(CString sTitle)
{
	this->SetWindowText(sTitle);
}

void CDlgProgress::SetTitle(UINT nID)
{
	CString s;
	s.LoadString(nID);
	this->SetWindowText(s);
}

void CDlgProgress::PumpMessages()
{
	ASSERT(m_hWnd!=NULL);
	MSG msg;
	// Handle dialog messages
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
	  if(!IsDialogMessage(&msg))
	  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);  
	  }
	}
}

BOOL CDlgProgress::CheckCancelButton() 
{
	PumpMessages();
	BOOL bResult = m_bCancel;
    m_bCancel = FALSE;
	//// TRACE("CANCEL checked\n");
    return bResult;
}

BOOL CDlgProgress::DestroyWindow() 
{
	return CDialog::DestroyWindow();
}
