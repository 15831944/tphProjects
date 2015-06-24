// DBNamdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DBNamdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBNamdDlg dialog


CDBNamdDlg::CDBNamdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBNamdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBNamdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDBNamdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBNamdDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBNamdDlg, CDialog)
	//{{AFX_MSG_MAP(CDBNamdDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBNamdDlg message handlers
