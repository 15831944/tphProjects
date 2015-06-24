// DlgGetString.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgGetString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGetString dialog


CDlgGetString::CDlgGetString(UINT nDlgTitle,
							 CString sValue, /*=_T("")*/
							 CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGetString::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGetString)
	m_sValue = sValue;
	//}}AFX_DATA_INIT
	m_sTitle.LoadString(nDlgTitle);
}

CDlgGetString::CDlgGetString( CString strTitle, CString sValue /*= _T("")*/, CWnd* pParent /*= NULL*/ )
	: CDialog(CDlgGetString::IDD, pParent)
	, m_sTitle(strTitle)
	, m_sValue(sValue)
{

}

void CDlgGetString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGetString)
	DDX_Text(pDX, IDC_INPUTSTRING, m_sValue);
	DDV_MaxChars(pDX, m_sValue, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGetString, CDialog)
	//{{AFX_MSG_MAP(CDlgGetString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGetString message handlers

BOOL CDlgGetString::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_sTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
