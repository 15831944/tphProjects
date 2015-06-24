// FindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog


CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDlg)
	m_strFind = _T("");
	m_bMatchWholeWord = FALSE;
	//}}AFX_DATA_INIT
	m_bType = FALSE;
}


void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDlg)
	DDX_Control(pDX, IDC_COMBO_FINDWHAT, m_ctrlComboBox);
	DDX_CBString(pDX, IDC_COMBO_FINDWHAT, m_strFind);
	DDX_Check(pDX, IDC_CHECK_MATCHALL, m_bMatchWholeWord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDlg)
	ON_BN_CLICKED(IDC_BUTTON_MARKALL, OnButtonMarkall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDlg message handlers

void CFindDlg::OnButtonMarkall() 
{
	// TODO: Add your control notification handler code here
	m_bType = TRUE ;
	CFindDlg::OnOK();
}

BOOL CFindDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	std::vector<CString>::iterator p;
	if(m_vStrBuff.size()>0)
	{				
		for( p=m_vStrBuff.begin();p<m_vStrBuff.end();p++)
		{
			m_ctrlComboBox.InsertString(0,CString(*p));	
		}
	}
	// TODO: Add extra initialization here	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
