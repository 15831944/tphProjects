// ComparaReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ComparaReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComparaReportDlg dialog


CComparaReportDlg::CComparaReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComparaReportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComparaReportDlg)
	m_sDiscription = _T("");
	m_sName = _T("");
	m_sReport = _T("");
	m_sStatus = _T("");
	m_sModel = _T("");
	m_nProcessor = 0;
	//}}AFX_DATA_INIT
}


void CComparaReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComparaReportDlg)
	DDX_Control(pDX, IDC_LIST_MODELS, m_ListModels);
	DDX_Control(pDX, IDC_STATIC_TOOLBARREPORTS, m_StaticToolBarReports);
	DDX_Control(pDX, IDC_STATIC_TOOLBARMODELS, m_StaticToolBarModels);
	DDX_Control(pDX, ID_TREECRL_REPORT, m_TreeCtlReport);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDiscription);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDX_Text(pDX, IDC_EDIT_REPORT, m_sReport);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_sStatus);
	DDX_Text(pDX, IDC_EDIT_MODEL, m_sModel);
	DDX_Slider(pDX, IDC_SLIDER1, m_nProcessor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComparaReportDlg, CDialog)
	//{{AFX_MSG_MAP(CComparaReportDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComparaReportDlg message handlers

void CComparaReportDlg::InitToolBar()
{  
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );

}

void CComparaReportDlg::InitListEx()
{
  m_ListModels.DeleteAllItems();
  DWORD dyStyle=m_ListModels.GetExtendedStyle();
  dyStyle|=	LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
  m_ListModels.SetExtendedStyle(dyStyle);
  LV_COLUMNEX lvc;
  lvc.mask=LVCF_WIDTH|LVCF_TEXT;
  char columnLable[][128]=
  {"Name","Location"};
  CRect rclist;
  m_ListModels.GetWindowRect(rclist);
  int defultColumnWidth[]={rclist.Width/4;rclist.Width/4};
  int _iFormat[]={LVCFMT_CENTER|LVCFMT_EDIT,LVCFMT_CENTER|LVCFMT_EDIT
  };
  for(int i=0;i<2;i++)
  {
	  lvc.csList=&s;
	  lvc.pszText=columnLable[i];
	  lvc.cx=defultColumnWidth[i];
	  lvc.fmt=_iFormat[i];
	  m_ListModels.InsertColumn(i,&lvc);
	  m_ListModels.InsertItem(0,1,_T("Model1"));
	  m_ListModels.InsertItem(0,2,_T("Location"));
  }
}

void CComparaReportDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
