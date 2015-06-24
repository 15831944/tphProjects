// UnitBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h" 
#include "afxwin.h"
#include "comdef.h"
#include "AirsideReportView.h"
#include "MainFrm.h"
#include "TermPlanDoc.h"
#include "UnitBar.h"
#include "../common/CARCUnit.h"
#include ".\unitbar.h"
#include "../InputAirside/ARCUnitManager.h"
#include "ViewMsg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitBar
CUnitBar::CUnitBar()
{
}

CUnitBar::~CUnitBar()
{
	for (int i = (int)m_arUnitComboBox.GetSize() - 1;i >= 0; --i) {
		if(m_arUnitComboBox[i])
			delete m_arUnitComboBox[i];
	}
	m_arUnitComboBox.RemoveAll();
}


BEGIN_MESSAGE_MAP(CUnitBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CUnitBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_CBN_SELENDOK (IDC_COMBO_WUNITS, OnSelectWUnits)
	ON_CBN_SELENDOK (IDC_COMBO_LUNITS, OnSelectLUnits)
	ON_CBN_SELENDOK (IDC_COMBO_TUNITS, OnSelectTUnits)
	ON_CBN_SELENDOK (IDC_COMBO_VUNITS, OnSelectVUnits)
	ON_CBN_SELENDOK (IDC_COMBO_AUNITS, OnSelectAUnits)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnTbnDropDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitBar message handlers

int CUnitBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetSizes(CSize(80,32), CSize(24,24));
		
	TBBUTTONINFO tbbi;
	memset(&tbbi,0,sizeof(TBBUTTONINFO));
	tbbi.cbSize = sizeof(TBBUTTONINFO);
	tbbi.dwMask = TBIF_STATE ;
	tbbi.fsState = TBSTATE_INDETERMINATE; 

	m_arUnitComboBox.Add(new CComboBoxXP());
	AddTextComboBox(" L: ",m_arUnitComboBox[0],CSize(80,0),IDC_COMBO_LUNITS,CBS_DROPDOWNLIST);
	GetToolBarCtrl().SetButtonInfo(GetItemID(0),&tbbi) ;
	m_arUnitComboBox.Add(new CComboBoxXP());
	AddTextComboBox(" W: ",m_arUnitComboBox[1],CSize(80,0),IDC_COMBO_WUNITS,CBS_DROPDOWNLIST); 	
	GetToolBarCtrl().SetButtonInfo(GetItemID(1),&tbbi) ;
	m_arUnitComboBox.Add(new CComboBoxXP());
	AddTextComboBox(" T: ",m_arUnitComboBox[2],CSize(80,0),IDC_COMBO_TUNITS,CBS_DROPDOWNLIST);
	GetToolBarCtrl().SetButtonInfo(GetItemID(2),&tbbi) ;
	m_arUnitComboBox.Add(new CComboBoxXP());
	AddTextComboBox(" V: ",m_arUnitComboBox[3],CSize(80,0),IDC_COMBO_VUNITS,CBS_DROPDOWNLIST);
	GetToolBarCtrl().SetButtonInfo(GetItemID(3),&tbbi) ;
	m_arUnitComboBox.Add(new CComboBoxXP());
	AddTextComboBox(" A: ",m_arUnitComboBox[4],CSize(80,0),IDC_COMBO_AUNITS,CBS_DROPDOWNLIST);
	GetToolBarCtrl().SetButtonInfo(GetItemID(4),&tbbi) ;	

	int i = -1;
	CStringArray ayUnitStr;
	int nArraySize = -1;
	CARCLengthUnit::EnumLengthUnitString(ayUnitStr);
	nArraySize = ayUnitStr.GetCount();
	for(i = 0; i < nArraySize; i++)
		m_arUnitComboBox[0]->AddString(ayUnitStr[i]);
	
	ayUnitStr.RemoveAll();
	nArraySize = -1;
	CARCWeightUnit::EnumWeightUnitString(ayUnitStr);
	nArraySize = ayUnitStr.GetCount();
	for(i = 0; i < nArraySize; i++)
		m_arUnitComboBox[1]->AddString(ayUnitStr[i]);

	ayUnitStr.RemoveAll();
	nArraySize = -1;
	CARCTimeUnit::EnumTimeUnitString(ayUnitStr);
	nArraySize = ayUnitStr.GetCount();
	for(i = 0; i < nArraySize; i++)
		m_arUnitComboBox[2]->AddString(ayUnitStr[i]);

	ayUnitStr.RemoveAll();
	nArraySize = -1;
	CARCVelocityUnit::EnumVelocityUnitString(ayUnitStr);
	nArraySize = ayUnitStr.GetCount();
	for(i = 0; i < nArraySize; i++)
		m_arUnitComboBox[3]->AddString(ayUnitStr[i]);

	ayUnitStr.RemoveAll();
	nArraySize = -1;
	CARCAccelerationUnit::EnumAccelerationUnitString(ayUnitStr);
	nArraySize = ayUnitStr.GetCount();
	for(i = 0; i < nArraySize; i++)
		m_arUnitComboBox[4]->AddString(ayUnitStr[i]);
	
	for (int i = (int)m_arUnitComboBox.GetSize() - 1;i >= 0;--i)
		m_arUnitComboBox[i]->SetCurSel(-1); 

	SetWindowText("Units"); 
	AddArrowButtonAtTail();
	return 0;
}

CComboBox* CUnitBar::GetComboBoxLU()
{
	return m_arUnitComboBox[0];
}

CComboBox* CUnitBar::GetComboBoxWU()
{
	return m_arUnitComboBox[1];
}

CComboBox* CUnitBar::GetComboBoxTU()
{
	return m_arUnitComboBox[2];
}

CComboBox* CUnitBar::GetComboBoxVU()
{
	return m_arUnitComboBox[3];
}

CComboBox* CUnitBar::GetComboBoxAU()
{
	return m_arUnitComboBox[4];
}

void CUnitBar::OnSelectLUnits()
{
	int nIndex = m_arUnitComboBox[0]->GetCurSel(); 
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	if(!pMDIActive)return;
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if(!pDoc)return;
	pDoc->m_umGlobalUnitManager.SetCurSelLengthUnit((ARCUnit_Length)nIndex);
	pDoc->m_umGlobalUnitManager.SaveData();

	//pDoc is active document
	//now we will go through each view of the doc and place it properly	
	/*CAirsideReportView* pAirsideReportView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);

		if (pView->IsKindOf(RUNTIME_CLASS(CAirsideReportView)))
		{
			pAirsideReportView = (CAirsideReportView*)pView;
			if (pAirsideReportView)
				if(pAirsideReportView->GetReportWnd())
					pAirsideReportView->GetReportWnd()->SetLengthUnit(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
		}
	}	*/
	pDoc->UpdateAllViews(NULL,VM_GLOBALUNIT_CHANGER,(CObject*)&(pDoc->m_umGlobalUnitManager)) ;
}

void CUnitBar::UpdateUnitBar(CTermPlanDoc* pDoc)
{ 
	if(!pDoc)return;
	m_arUnitComboBox[0]->SetCurSel((int)pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
	m_arUnitComboBox[1]->SetCurSel((int)pDoc->m_umGlobalUnitManager.GetCurSelWeightUnit());
	m_arUnitComboBox[2]->SetCurSel((int)pDoc->m_umGlobalUnitManager.GetCurSelTimeUnit());
	m_arUnitComboBox[3]->SetCurSel((int)pDoc->m_umGlobalUnitManager.GetCurSelVelocityUnit());
	m_arUnitComboBox[4]->SetCurSel((int)pDoc->m_umGlobalUnitManager.GetCurSelAccelerationUnit());
	
}

void CUnitBar::OnSelectWUnits()
{
	int nIndex = m_arUnitComboBox[1]->GetCurSel(); 
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	if(!pMDIActive)return;
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if(!pDoc)return;
	pDoc->m_umGlobalUnitManager.SetCurSelWeightUnit((ARCUnit_Weight)nIndex);
	pDoc->m_umGlobalUnitManager.SaveData();
	pDoc->UpdateAllViews(NULL,VM_GLOBALUNIT_CHANGER,(CObject*)&(pDoc->m_umGlobalUnitManager)) ;
}

void CUnitBar::OnSelectTUnits()
{
	int nIndex = m_arUnitComboBox[2]->GetCurSel(); 
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	if(!pMDIActive)return;
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if(!pDoc)return;
	pDoc->m_umGlobalUnitManager.SetCurSelTimeUnit((ARCUnit_Time)nIndex);
	pDoc->m_umGlobalUnitManager.SaveData();
	pDoc->UpdateAllViews(NULL,VM_GLOBALUNIT_CHANGER,NULL) ;
}

void CUnitBar::OnSelectVUnits()
{
	int nIndex = m_arUnitComboBox[3]->GetCurSel(); 
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	if(!pMDIActive)return;
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if(!pDoc)return;
	pDoc->m_umGlobalUnitManager.SetCurSelVelocityUnit((ARCUnit_Velocity)nIndex);
	pDoc->m_umGlobalUnitManager.SaveData();
	pDoc->UpdateAllViews(NULL,VM_GLOBALUNIT_CHANGER,NULL) ;
}


void CUnitBar::OnSelectAUnits()
{
	int nIndex = m_arUnitComboBox[4]->GetCurSel(); 
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	if(!pMDIActive)return;
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if(!pDoc)return;
	pDoc->m_umGlobalUnitManager.SetCurSelAccelerationUnit((ARCUnit_Acceleration)nIndex);
	pDoc->m_umGlobalUnitManager.SaveData();
	pDoc->UpdateAllViews(NULL,VM_GLOBALUNIT_CHANGER,NULL) ;
}

void CUnitBar::OnTbnDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR pNMTB = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(pNMTB->iItem == ID_UNIT_DROPDOWN_ARROW)
	{
	}
	*pResult = 0;
}

void CUnitBar::AddArrowButtonAtTail(void)
{
}