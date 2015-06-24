// FacilityDataSheet.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "FacilityDataSheet.h"
#include "..\Common\ALT_TYPE.h"
#include "TermPlanDoc.h"

#include "FacilityDataPage.h"
#include "TaxiPoolFacilityDataPage.h"
#include "TaxiQFacilityDataPage.h"
#include "ParkingLotFacilityDataPage.h"
#include ".\facilitydatasheet.h"
#include "..\Landside\FacilityBehavior.h"
#include "..\Landside\FacilityTaxiPoolBehavior.h"
#include "..\Landside\FacilityTaxiQBehavior.h"
#include "CurbsideFacilityDataPage.h"
// CFacilityDataSheet

IMPLEMENT_DYNAMIC(CFacilityDataSheet, CResizableSheet)
CFacilityDataSheet::CFacilityDataSheet(InputLandside *pInptLandside, InputTerminal* _pInTerm, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pParent = pParentWnd;
	m_pInputLandside = pInptLandside;
	m_pInputTerm = _pInTerm;
	m_nProjectID = -1;
	InitSheet();
}

CFacilityDataSheet::CFacilityDataSheet(InputLandside *pInptLandside,InputTerminal* _pInTerm, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(pszCaption, pParentWnd, iSelectPage)
{

	m_pParent = pParentWnd;
	m_pInputLandside = pInptLandside;
	m_pInputTerm = _pInTerm;
	m_nProjectID =  -1;
	InitSheet();

}
void CFacilityDataSheet::InitSheet()
{

	if (m_pParent == NULL)
		return;
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParent)->GetDocument();	
	if (pDoc == NULL)
		return;
	m_nProjectID = pDoc->GetProjectID();
	

	m_pBehavior = new CFacilityBehaviorList;
	m_pBehavior->ReadData(m_nProjectID);


	std::vector<std::pair<ALTOBJECT_TYPE, UINT> > vFacilityTypes;
	vFacilityTypes.push_back(std::make_pair(ALT_LCURBSIDE,   IDS_LANDSIDE_CURBSIDE));
	vFacilityTypes.push_back(std::make_pair(ALT_LBUSSTATION, IDS_LANDSIDE_BUSSTATION));
	//vFacilityTypes.push_back(std::make_pair(ALT_LWALKWAY,    IDS_LANDSIDE_WALKWAY));
	vFacilityTypes.push_back(std::make_pair(ALT_LTAXIPOOL,    IDS_LANDSIDE_TAXIPOOL));
	vFacilityTypes.push_back(std::make_pair(ALT_LTAXIQUEUE,   IDS_LANDSIDE_TAXIQ));
	vFacilityTypes.push_back(std::make_pair(ALT_LPARKINGLOT,   IDS_LANDSIDE_PARKINGLOT));
	int nBehaviorTypeCount = static_cast<int>(vFacilityTypes.size());
	for (int nBehaviorType = 0; nBehaviorType < nBehaviorTypeCount; ++ nBehaviorType)
	{
		if ( nBehaviorType == BehaviorType_BUSSTATION /*|| nBehaviorType == BehaviorType_WALKWAY*/)
		{
			CFacilityDataPage *pPage = new CFacilityDataPage(m_pInputLandside,m_pInputTerm, m_pBehavior ,vFacilityTypes[nBehaviorType].first, vFacilityTypes[nBehaviorType].second, m_nProjectID);
			AddPage(pPage);
			
			m_vPages.push_back(pPage);
		}
		else if(nBehaviorType == BehaviorType_CURBSIDE)
		{
			CCurbsideFacilityDataPage *pPage = new CCurbsideFacilityDataPage(m_pInputLandside,m_pInputTerm, m_pBehavior ,vFacilityTypes[nBehaviorType].first, vFacilityTypes[nBehaviorType].second, m_nProjectID);
			AddPage(pPage);

		}
		else if (nBehaviorType == BehaviorType_TAXIPOOL)
		{
			CTaxiPoolFacilityDataPage* pTaxiPoolPage = new CTaxiPoolFacilityDataPage(m_pInputLandside,m_pInputTerm, m_pBehavior ,vFacilityTypes[nBehaviorType].first, vFacilityTypes[nBehaviorType].second, m_nProjectID);
			AddPage(pTaxiPoolPage);
		}

		else if (nBehaviorType == BehaviorType_TAXIQUEUE)
		{
			CTaxiQFacilityDataPage* pTaxiQPage = new CTaxiQFacilityDataPage(m_pInputLandside,m_pInputTerm, m_pBehavior ,vFacilityTypes[nBehaviorType].first, vFacilityTypes[nBehaviorType].second, m_nProjectID);
			AddPage(pTaxiQPage);
		}
		else if (nBehaviorType == BehaviorType_PARKINGLOT)
		{
			CParkingLotFacilityDataPage* pParkingLotPage = new CParkingLotFacilityDataPage(m_pInputLandside,m_pInputTerm, m_pBehavior ,vFacilityTypes[nBehaviorType].first, vFacilityTypes[nBehaviorType].second, m_nProjectID);
			AddPage(pParkingLotPage);
		}
	}

}
CFacilityDataSheet::~CFacilityDataSheet()
{
	if (m_pBehavior)
	{
		delete m_pBehavior;
		m_pBehavior = NULL;
	}	
}


BEGIN_MESSAGE_MAP(CFacilityDataSheet, CResizableSheet)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CFacilityDataSheet message handlers


BOOL CFacilityDataSheet::OnInitDialog()
{
	m_bModeless = FALSE;
	m_nFlags |= WF_CONTINUEMODAL;
	BOOL bResult = CResizableSheet::OnInitDialog();
	m_bModeless = TRUE;
	m_nFlags &= ~WF_CONTINUEMODAL;

	// TODO:  Add your specialized code here
	EnableSaveRestore(_T("FacilityProperties"), TRUE, TRUE);

	return bResult;
}

void CFacilityDataSheet::OnSize(UINT nType, int cx, int cy)
{
	CResizableSheet::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

BOOL  CFacilityDataSheet::OnCommand(WPARAM  wParam,  LPARAM  lParam)   
{  
	if  (HIWORD(wParam) == BN_CLICKED)  
	{  
		switch(LOWORD(wParam))  
		{  
		case  IDOK:  
			PressButton(PSBTN_OK);  
			DestroyWindow();  
			return TRUE;  
		case  ID_APPLY_NOW:    
			PressButton(PSBTN_APPLYNOW);  
			return TRUE;  
		case  IDCANCEL:  
			PressButton(PSBTN_CANCEL);  
			DestroyWindow();  
			return TRUE;  
		case  IDHELP:  
			PressButton(PSBTN_HELP);  
			return TRUE;  
		}  
	}  
	return  CPropertySheet::OnCommand(wParam,  lParam);  
}  