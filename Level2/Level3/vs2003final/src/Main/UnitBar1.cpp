// UnitBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "UnitBar1.h"
#include "../common/UnitsManager.h"
#include "afxwin.h"
#include "comdef.h"
#include "AirsideReportView.h"
#include "MainFrm.h"
#include "TermPlanDoc.h"
#include "Onboard/MeshEditTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitBar1

CUnitBar1::CUnitBar1()
{
}

CUnitBar1::~CUnitBar1()
{
}


BEGIN_MESSAGE_MAP(CUnitBar1, CToolBar24X24)
	//{{AFX_MSG_MAP(CUnitBar1)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_CBN_SELENDOK (IDC_COMBO_WUNITS_TEMP, OnSelectWUnits)
	ON_CBN_SELENDOK (IDC_COMBO_LUNITS_TEMP, OnSelectLUnits)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitBar1 message handlers

int CUnitBar1::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetSizes(CSize(55,32), CSize(24,24));
	//Insert a temp button(size 2)
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	tbb.iBitmap = -1;
	tbb.idCommand =TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;
	tbb.fsState &=~ TBSTATE_ENABLED;
	AddButton(&tbb,2,TRUE);
	
	AddTextComboBox(" Units:  ",&m_cbUint1,CSize(90,160),IDC_COMBO_LUNITS_TEMP,CBS_DROPDOWNLIST);
	AddTextComboBox("  ",&m_cbUint2,CSize(60,160),IDC_COMBO_WUNITS_TEMP,CBS_DROPDOWNLIST);
	
	for(int i=UM_WEIGHT_FIRST; i<UM_WEIGHT_LAST+1; i++)
	{
		m_cbUint2.AddString(UNITSMANAGER->GetWeightUnitString(i));
	}	
	
	for( i=UM_LEN_FIRST; i<UM_LEN_LAST+1; i++)
	{
		m_cbUint1.AddString(UNITSMANAGER->GetLengthUnitString(i));
	}
	
	m_cbUint1.SelectString(0, UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()));
	m_cbUint2.SelectString(0, UNITSMANAGER->GetWeightUnitString(UNITSMANAGER->GetWeightUnits()));
	SetWindowText("Unit");
	return 0;
}

CComboBox* CUnitBar1::GetComboBoxLU()
{
	return &m_cbUint1;

}

CComboBox* CUnitBar1::GetComboBoxWU()
{
	return &m_cbUint2;

}

void CUnitBar1::OnSelectLUnits()
{
	int nIndex = m_cbUint1.GetCurSel();
	CUnitsManager::GetInstance()->SetLengthUnits(nIndex + UM_LEN_OFFSET);
	CMDIChildWnd* pMDIActive =((CMainFrame *)AfxGetApp()->m_pMainWnd)->MDIGetActive();
	ASSERT(pMDIActive != NULL);
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	//pDoc is active document
	//now we will go through each view of the doc and place it properly
	
	CAirsideReportView* pAirsideReportView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);

		if (pView->IsKindOf(RUNTIME_CLASS(CAirsideReportView)))
		{
			pAirsideReportView = (CAirsideReportView*)pView;
			break;
		}

		if(pView && pView->IsKindOf(RUNTIME_CLASS(CMeshEditTreeView)))
		{
			((CMeshEditTreeView*)pView)->UpdateUnit() ;
		}
	}

	if (pAirsideReportView == NULL)
	{
		return;
	}
	ASSERT(pAirsideReportView != NULL);

//	pAirsideReportView->GetReportWnd()->SetLengthUnit((UM_LEN)UNITSMANAGER->GetLengthUnits());
}

void CUnitBar1::OnSelectWUnits()
{
	int nIndex = m_cbUint2.GetCurSel();
	CUnitsManager::GetInstance()->SetWeightUnits(nIndex + UM_WEIGHT_OFFSET);
}
