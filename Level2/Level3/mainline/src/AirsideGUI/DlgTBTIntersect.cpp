// DlgTBTIntersect.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include ".\dlgtbtintersect.h"

// CDlgTBTIntersect dialog

IMPLEMENT_DYNAMIC(CDlgTBTIntersect, CPropertyPage)
CDlgTBTIntersect::CDlgTBTIntersect(int nProjID, TakeoffClearanceItem* pItem)
	: CPropertyPage(CDlgTBTIntersect::IDD)
	, m_nProjID(nProjID)
	, m_nTimeApproach(0)
	, m_nDistApproach(0)
	, m_nTimeSlot(0)
{
	m_pTBTIntersect = (TakeoffBehindTakeoffIntersectingConvergngRunway*)pItem;

	m_strClasifiBasisType.SetSize(5);
	m_strClasifiBasisType[0] = _T("Wingspan Based");
	m_strClasifiBasisType[1] = _T("Surface Bearing Weight Based");
	m_strClasifiBasisType[2] = _T("Wake Vortex Wight Based");
	m_strClasifiBasisType[3] = _T("Approach Speed Based");
	m_strClasifiBasisType[4] = _T("None");

	m_pClsNone = new AircraftClassifications(m_nProjID, NoneBased);
	m_pClsWakeVortexWightBased = new AircraftClassifications(m_nProjID, WakeVortexWightBased);
	m_pClsWingspanBased = new AircraftClassifications(m_nProjID, WingspanBased);
	m_pClsSurfaceBearingWeightBased = new AircraftClassifications(m_nProjID, SurfaceBearingWeightBased);
	m_pClsApproachSpeedBased = new AircraftClassifications(m_nProjID, ApproachSpeedBased);

}

CDlgTBTIntersect::~CDlgTBTIntersect()
{
	delete m_pClsNone; m_pClsNone= NULL;
	delete m_pClsWakeVortexWightBased; m_pClsWakeVortexWightBased= NULL;
	delete m_pClsWingspanBased; m_pClsWingspanBased= NULL;
	delete m_pClsSurfaceBearingWeightBased; m_pClsSurfaceBearingWeightBased= NULL;
	delete m_pClsApproachSpeedBased; m_pClsApproachSpeedBased= NULL;
}

void CDlgTBTIntersect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_nTimeApproach);
	DDX_Text(pDX, IDC_EDIT4, m_nDistApproach);
	DDX_Text(pDX, IDC_EDIT5, m_nTimeSlot);
	DDX_Control(pDX, IDC_COMBO_BASIS, m_wndCmbBasis);
	DDX_Control(pDX, IDC_LIST_BASIS, m_wndListBasis);
}


void CDlgTBTIntersect::InitListCtrl()
{
	DWORD dwStyle = m_wndListBasis.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListBasis.SetExtendedStyle(dwStyle);

	CStringList strList;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Trail type");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_wndListBasis.InsertColumn(0, &lvc);

	lvc.pszText = _T("Lead type");
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_wndListBasis.InsertColumn(1, &lvc);

	lvc.pszText = _T("Min separation(nm)");
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListBasis.InsertColumn(2, &lvc);


}

void CDlgTBTIntersect::InitComboBox()
{
	for (int i=0; i<m_strClasifiBasisType.GetCount(); i++)
		m_wndCmbBasis.AddString(m_strClasifiBasisType.GetAt(i));

	FlightClassificationBasisType emType = 
		m_pTBTIntersect->GetCurrentFltClassificationType();
	m_wndCmbBasis.SetCurSel((int)emType);

}

void CDlgTBTIntersect::InitEditBox()
{

	m_nDistApproach = m_pTBTIntersect->GetDistTakeoffApproach();
	m_nTimeApproach = m_pTBTIntersect->GetTimeTakeoffAproach();
	m_nTimeSlot = m_pTBTIntersect->GetTimeTakeoffSlot();

	if(m_pTBTIntersect->IsDistTakeoffApproach())
		CheckDlgButton( IDC_RADIO5, TRUE );
	else
		CheckDlgButton( IDC_RADIO6, TRUE );

	UpdateData(FALSE);

}


void CDlgTBTIntersect::ResetListCtrl()
{
	m_wndListBasis.DeleteAllItems();
	int nSelIndex = m_wndCmbBasis.GetCurSel();
	if(nSelIndex < 0)	return;

	FlightClassificationBasisType fltBasisType = (FlightClassificationBasisType)nSelIndex;

	m_pTBTIntersect->SetCurrentFlightClassificationBasisType(fltBasisType);

	TakeoffClearanceItem::TakeoffClearanceBasisVector vect;
	m_pTBTIntersect->GetItemsByType(vect, fltBasisType);

	AircraftClassifications* pACInfo = GetAircraftClsByType(fltBasisType);

	for (int i=0; i<(int)vect.size(); i++)
	{
		TakeoffClearanceClassfiBasisItem* pBasisItem = vect[i];


		if (NoneBased == (FlightClassificationBasisType)nSelIndex)
		{
			m_wndListBasis.InsertItem(i, _T("Default"));
			m_wndListBasis.SetItemText(i, 1, _T("Default"));
		}
		else
		{
			AircraftClassificationItem* pLeader = pACInfo->GetItemByID(pBasisItem->m_nLeadID);
			AircraftClassificationItem* pTrail =  pACInfo->GetItemByID(pBasisItem->m_nTrailID);
			ASSERT(pLeader != NULL && pTrail != NULL);

			m_wndListBasis.InsertItem(i, pLeader->getName());
			m_wndListBasis.SetItemText(i, 1, pTrail->getName());
		}

		CString strTemp;
		strTemp.Format("%d", pBasisItem->m_nMinSeparation);
		m_wndListBasis.SetItemText(i, 2, strTemp);
		m_wndListBasis.SetItemData(i, (DWORD_PTR)pBasisItem);
	}
}



void CDlgTBTIntersect::GetValueBeforSave()
{
	UpdateData(TRUE);

	m_pTBTIntersect->SetDistTakeoffApproach(m_nDistApproach);
	m_pTBTIntersect->SetTimeTakeoffAproach(m_nTimeApproach);
	m_pTBTIntersect->SetTimeTakeoffSlot(m_nTimeSlot);

	m_pTBTIntersect->SetDistTakeoffApproachFlag( IsDlgButtonChecked(IDC_RADIO5) == BST_CHECKED);

}


BEGIN_MESSAGE_MAP(CDlgTBTIntersect, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_BASIS, OnCbnSelchangeComboBasis)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_BASIS, OnLvnEndlabeleditListBasis)
END_MESSAGE_MAP()


// CDlgTBTIntersect message handlers

BOOL CDlgTBTIntersect::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pClsNone->ReadData();
	m_pClsWakeVortexWightBased->ReadData();
	m_pClsWingspanBased->ReadData();
	m_pClsSurfaceBearingWeightBased->ReadData();
	m_pClsApproachSpeedBased->ReadData();

	InitListCtrl();
	InitComboBox();

	InitEditBox();


	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN3))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN4))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN5))->SetRange(0,1000);

	ResetListCtrl();

	return TRUE;
}




void CDlgTBTIntersect::OnCbnSelchangeComboBasis()
{
	ResetListCtrl();
}

void CDlgTBTIntersect::OnLvnEndlabeleditListBasis(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	m_wndListBasis.OnEndlabeledit(pNMHDR, pResult);


	int nItem = pnmv->iSubItem;
	TakeoffClearanceClassfiBasisItem* pItem = 
		(TakeoffClearanceClassfiBasisItem*)m_wndListBasis.GetItemData(nItem);
	int nMinSeparation = atoi(m_wndListBasis.GetItemText(nItem, 2));
	pItem->m_nMinSeparation = nMinSeparation; 

	*pResult = 0;
}

AircraftClassifications* CDlgTBTIntersect::GetAircraftClsByType(FlightClassificationBasisType fltBasisType)
{
	switch(fltBasisType) {
	case WingspanBased:
		return m_pClsWingspanBased;
	case SurfaceBearingWeightBased:
		return m_pClsSurfaceBearingWeightBased;
	case WakeVortexWightBased:
		return m_pClsWakeVortexWightBased;
	case ApproachSpeedBased:
		return m_pClsApproachSpeedBased;
	case NoneBased:
		return m_pClsNone;
	default:
		return NULL;
	}
}


BOOL CDlgTBTIntersect::OnApply()
{
	GetValueBeforSave();
	return CPropertyPage::OnApply();
}
void CDlgTBTIntersect::OnCancel()
{
	return CPropertyPage::OnCancel();
}
void CDlgTBTIntersect::OnOK()
{
	GetValueBeforSave();
	return CPropertyPage::OnOK();
}
