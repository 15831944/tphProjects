// PaxFlowAvoidDensityAddAreasDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxFlowAvoidDensityAddAreasDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowAvoidDensityAddAreasDlg dialog


CPaxFlowAvoidDensityAddAreasDlg::CPaxFlowAvoidDensityAddAreasDlg(CAreaList* vAreas,CWnd* pParent /*=NULL*/)
	: CDialog(CPaxFlowAvoidDensityAddAreasDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxFlowAvoidDensityAddAreasDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_vAreas=vAreas;
}


void CPaxFlowAvoidDensityAddAreasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxFlowAvoidDensityAddAreasDlg)
	DDX_Control(pDX, IDC_LIST_AREA, m_listBoxArea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxFlowAvoidDensityAddAreasDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxFlowAvoidDensityAddAreasDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowAvoidDensityAddAreasDlg message handlers

BOOL CPaxFlowAvoidDensityAddAreasDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CAreaList::iterator it;
	for(it=m_vAreas->begin();it!=m_vAreas->end();it++)
	{
		m_listBoxArea.AddString((*it)->name);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaxFlowAvoidDensityAddAreasDlg::OnOK() 
{
	int nSelCount=m_listBoxArea.GetSelCount();
	int* pSeled=new int[nSelCount];
	m_listBoxArea.GetSelItems(nSelCount,pSeled);
	for(int i=0;i<nSelCount;i++)
	{
		CString strArea;
		m_listBoxArea.GetText(pSeled[i],strArea);
		m_strArraySeledAreas.Add(strArea);
	}
	delete pSeled;	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
