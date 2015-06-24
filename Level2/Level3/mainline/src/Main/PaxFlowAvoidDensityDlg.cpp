// PaxFlowAvoidDensityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxFlowAvoidDensityDlg.h"
#include "MainFrm.h"
#include "3DView.h"
#include "DlgAreaPortal.h"
#include ".\paxflowavoiddensitydlg.h"
#include "TVN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowAvoidDensityDlg dialog


CPaxFlowAvoidDensityDlg::CPaxFlowAvoidDensityDlg(CTermPlanDoc* pDoc,CAreaList* vAreas,CFlowDestination* arcInfo,CWnd* pParent /*=NULL*/)
	: CDialog(CPaxFlowAvoidDensityDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxFlowAvoidDensityDlg)
	m_fLimit = 0.0f;
	//}}AFX_DATA_INIT
	m_arcInfo=arcInfo;
	m_vAreas=vAreas;
	m_pDoc=pDoc;
}


void CPaxFlowAvoidDensityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxFlowAvoidDensityDlg)
	DDX_Control(pDX, IDC_LIST_AREA, m_listBoxArea);
	DDX_Control(pDX, IDC_EDIT_LIMIT, m_editLimit);
	DDX_Text(pDX, IDC_EDIT_LIMIT, m_fLimit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxFlowAvoidDensityDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxFlowAvoidDensityDlg)
	ON_BN_CLICKED(IDC_BUTTON_NEW_AREA, OnButtonNewArea)
	ON_BN_CLICKED(IDC_BUTTON_ADD_AREA, OnButtonAddArea)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_AREA, OnButtonDeleteArea)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_DLGAREAPORTAL_SHOW_OR_HIDE,OnDlgAreaPortalShowOrHide)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowAvoidDensityDlg message handlers

BOOL CPaxFlowAvoidDensityDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_fLimit=static_cast<float>(m_arcInfo->GetDensityOfArea());

	std :: vector<CString>& vString=m_arcInfo->GetDensityArea();
	std::vector<CString>::iterator it;
	for(it=	vString.begin();it!=vString.end();it++)
	{
		m_listBoxArea.AddString((*it));
		
	}
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaxFlowAvoidDensityDlg::OnButtonNewArea() 
{
	CArea* pNewArea = new CArea();
	pNewArea->color = m_pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::AreasColor);
	//bring up 3d window
	C3DView* p3DView = NULL;
	if((p3DView = m_pDoc->Get3DView()) == NULL) 
	{
		m_pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = m_pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	CDlgAreaPortal dlg(m_pDoc,CDlgAreaPortal::area, pNewArea, p3DView,this);
	if(dlg.DoFakeModal() == IDOK) {
		pNewArea->floor = m_pDoc->m_nActiveFloor;
		m_pDoc->GetTerminal().m_pAreas->m_vAreas.push_back(pNewArea);
		CTVNode* pAllAreasNode = m_pDoc->m_msManager.FindNodeByName(IDS_TVNN_AREAS);
		ASSERT(pAllAreasNode != NULL);
		CAreaNode* pNode = new CAreaNode(m_pDoc->GetTerminal().m_pAreas->m_vAreas.size()-1);
		pAllAreasNode->m_eState = CTVNode::expanded;
		pNode->Name(pNewArea->name);
		pAllAreasNode->AddChild(pNode);
		m_pDoc->UpdateRender3DObject(pNewArea);
		m_pDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pAllAreasNode);
		m_pDoc->GetTerminal().m_pAreas->saveDataSet(m_pDoc->m_ProjInfo.path, true);
		if(!CheckIsExist(pNewArea->name))
			m_listBoxArea.AddString(pNewArea->name);
	}
	else {
		delete pNewArea;
	}
	
}

void CPaxFlowAvoidDensityDlg::OnButtonAddArea() 
{
	CPaxFlowAvoidDensityAddAreasDlg dlg(m_vAreas);
	if(dlg.DoModal()==IDOK)
	{
		int nSelCount=dlg.m_strArraySeledAreas.GetSize();
		for(int i=0;i<nSelCount;i++)
		{
			if(!CheckIsExist(dlg.m_strArraySeledAreas[i]))
				m_listBoxArea.AddString(dlg.m_strArraySeledAreas[i]);
		}
	}
	
}

void CPaxFlowAvoidDensityDlg::OnButtonDeleteArea() 
{
	int nSelCount=m_listBoxArea.GetSelCount();
	int* pSeled=new int[nSelCount];
	m_listBoxArea.GetSelItems(nSelCount,pSeled);
	for(int i=nSelCount;i>=0;i--)
	{
		m_listBoxArea.DeleteString(pSeled[i]);
	}
	delete pSeled;	// TODO: Add extra validation here
	
}

void CPaxFlowAvoidDensityDlg::OnOK() 
{
	UpdateData();
	if(m_fLimit<=0)
	{
		AfxMessageBox("Minumn value must be larger than 0.");
		return;
	}
	m_arcInfo->GetDensityArea().clear();

	for(int i=0;i<m_listBoxArea.GetCount();i++)
	{
		CString strArea;
		m_listBoxArea.GetText(i,strArea);
		m_arcInfo->GetDensityArea().push_back(strArea);
	}
	m_arcInfo->SetDensityOfArea(m_fLimit);
	CDialog::OnOK();
}

LRESULT CPaxFlowAvoidDensityDlg::OnDlgAreaPortalShowOrHide(WPARAM wParam,LPARAM lParam)
{
	if(wParam==0)
	{
		m_pParentWnd->ShowWindow(SW_HIDE);
		ShowWindow(SW_HIDE);
	}
	else if(wParam==1)
	{
		ShowWindow(SW_SHOW);
		m_pParentWnd->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

BOOL CPaxFlowAvoidDensityDlg::CheckIsExist(const CString &name)
{
	CString strTemp1;
	CString strTemp2;
	for(int i=0;i<m_listBoxArea.GetCount();i++)
	{
		
		m_listBoxArea.GetText(i,strTemp1);
		strTemp2=name;
		strTemp1.MakeLower();
		strTemp2.MakeLower();
		if(strTemp1==strTemp2)
		{
			strTemp1.Format("%s has existed.",name);
			AfxMessageBox(strTemp1);
			return TRUE;
		}
	}
	return FALSE;
}

void CPaxFlowAvoidDensityDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
