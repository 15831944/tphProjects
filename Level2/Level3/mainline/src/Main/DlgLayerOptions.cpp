// DlgLayerOptions.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgLayerOptions.h"
#include "Common/BaseFloor.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLayerOptions dialog


CDlgLayerOptions::CDlgLayerOptions(CFloorLayerList* pLayerList, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayerOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLayerOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pFloorLayerList = pLayerList;
	m_bLayersChanged = FALSE;
}


void CDlgLayerOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLayerOptions)
	DDX_Control(pDX, IDC_LIST_LAYERS, m_lstLayers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLayerOptions, CDialog)
	//{{AFX_MSG_MAP(CDlgLayerOptions)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLayerOptions message handlers

BOOL CDlgLayerOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect listrect;
	m_lstLayers.GetClientRect(&listrect);
	m_lstLayers.InsertColumn(0, CString(_T("Layer name")), LVCFMT_LEFT, listrect.Width()/2);
	m_lstLayers.InsertColumn(1, CString(_T("Color")), LVCFMT_LEFT, listrect.Width()/4);
	m_lstLayers.InsertColumn(2, CString(_T("Visible")), LVCFMT_LEFT, listrect.Width()/4);

	ASSERT(m_pFloorLayerList != NULL);
	m_lstLayers.PopulateLayerList(*m_pFloorLayerList);
	
	Invalidate();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgLayerOptions::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CDlgLayerOptions::OnOK() 
{
	
	
	if(m_lstLayers.UpdateFloorData())
		m_bLayersChanged = TRUE;
	CDialog::OnOK();
}

