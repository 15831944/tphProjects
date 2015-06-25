// DlgTBTSameRunway.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include ".\dlgtbtsamerunway.h"

// CDlgTBTSameRunway dialog

IMPLEMENT_DYNAMIC(CDlgTBTSameRunway, CPropertyPage)
CDlgTBTSameRunway::CDlgTBTSameRunway( int nProjID, TakeoffClearanceItem* pItem)
	: CPropertyPage(CDlgTBTSameRunway::IDD)
	, m_nProjID(nProjID)
	, m_nTimeAfter(0)
	, m_nFtDownRunway(0)
	, m_nDistApproach(0)
	, m_nTimeApproach(0)
	, m_nTimeSlot(0)
{
	m_pTBTSameRunway = (TakeoffBehindTakeoffSameRunwayOrCloseParalled*)pItem;

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

CDlgTBTSameRunway::~CDlgTBTSameRunway()
{
	delete m_pClsNone; m_pClsNone= NULL;
	delete m_pClsWakeVortexWightBased; m_pClsWakeVortexWightBased= NULL;
	delete m_pClsWingspanBased; m_pClsWingspanBased= NULL;
	delete m_pClsSurfaceBearingWeightBased; m_pClsSurfaceBearingWeightBased= NULL;
	delete m_pClsApproachSpeedBased; m_pClsApproachSpeedBased= NULL;

}

void CDlgTBTSameRunway::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nTimeAfter);
	DDX_Text(pDX, IDC_EDIT2, m_nFtDownRunway);
	DDX_Text(pDX, IDC_EDIT3, m_nDistApproach);
	DDX_Text(pDX, IDC_EDIT4, m_nTimeApproach);
	DDX_Text(pDX, IDC_EDIT5, m_nTimeSlot);
	DDX_Control(pDX, IDC_COMBO_BASIS, m_wndCmbBasis);
	DDX_Control(pDX, IDC_LIST_BASIS, m_wndListBasis);
}

BOOL CDlgTBTSameRunway::OnInitDialog()
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


	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN1))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN2))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN3))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN4))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN5))->SetRange(0,1000);

	ResetListCtrl();

	return TRUE;
}

void CDlgTBTSameRunway::InitListCtrl()
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

void CDlgTBTSameRunway::InitComboBox()
{
	for (int i=0; i<m_strClasifiBasisType.GetCount(); i++)
		m_wndCmbBasis.AddString(m_strClasifiBasisType.GetAt(i));

	FlightClassificationBasisType emType = 
		m_pTBTSameRunway->GetCurrentFltClassificationType();
	m_wndCmbBasis.SetCurSel((int)emType);
}


void CDlgTBTSameRunway::ResetListCtrl()
{
	m_wndListBasis.DeleteAllItems();
	int nSelIndex = m_wndCmbBasis.GetCurSel();
	if(nSelIndex < 0)	return;

	FlightClassificationBasisType fltBasisType = (FlightClassificationBasisType)nSelIndex;

	m_pTBTSameRunway->SetCurrentFlightClassificationBasisType(fltBasisType);

	TakeoffClearanceItem::TakeoffClearanceBasisVector vect;
	m_pTBTSameRunway->GetItemsByType(vect, fltBasisType);

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

void CDlgTBTSameRunway::InitEditBox()
{
	m_nDistApproach = m_pTBTSameRunway->GetDistTakeoffApproach();
	m_nTimeApproach = m_pTBTSameRunway->GetTimeTakeoffAproach();
	m_nTimeSlot = m_pTBTSameRunway->GetTimeTakeoffSlot();

	m_nFtDownRunway = m_pTBTSameRunway->GetFtLanderDownRunway();
	m_nTimeAfter = m_pTBTSameRunway->GetTimeAfter();

	if(m_pTBTSameRunway->IsDistAtLeast())
		CheckDlgButton( IDC_RADIO1, TRUE );
	else
		CheckDlgButton( IDC_RADIO2, TRUE );

	if(m_pTBTSameRunway->IsDistTakeoffApproach())
		CheckDlgButton( IDC_RADIO3, TRUE );
	else
		CheckDlgButton( IDC_RADIO4, TRUE );

	UpdateData(FALSE);

}

void CDlgTBTSameRunway::GetValueBeforSave()
{
	UpdateData(TRUE);

	m_pTBTSameRunway->SetDistTakeoffApproach(m_nDistApproach);
	m_pTBTSameRunway->SetTimeTakeoffAproach(m_nTimeApproach);
	m_pTBTSameRunway->SetTimeTakeoffSlot(m_nTimeSlot);

	m_pTBTSameRunway->SetFtLanderDownRunway(m_nFtDownRunway);
	m_pTBTSameRunway->SetTimeAfter(m_nTimeAfter);


	m_pTBTSameRunway->SetDistAtLeastFlag(IsDlgButtonChecked(IDC_RADIO1) == BST_CHECKED);
	m_pTBTSameRunway->SetDistTakeoffApproachFlag(IsDlgButtonChecked(IDC_RADIO3) == BST_CHECKED);

}

AircraftClassifications* CDlgTBTSameRunway::GetAircraftClsByType(FlightClassificationBasisType fltBasisType)
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

BEGIN_MESSAGE_MAP(CDlgTBTSameRunway, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_BASIS, OnCbnSelchangeComboBasis)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_BASIS, OnLvnEndlabeleditListBasis)
END_MESSAGE_MAP()


// CDlgTBTSameRunway message handlers


void CDlgTBTSameRunway::OnCbnSelchangeComboBasis()
{
	ResetListCtrl();
}

void CDlgTBTSameRunway::OnLvnEndlabeleditListBasis(NMHDR *pNMHDR, LRESULT *pResult)
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


BOOL CDlgTBTSameRunway::OnApply()
{
	GetValueBeforSave();
	return CPropertyPage::OnApply();
}

void CDlgTBTSameRunway::OnOK()
{
	GetValueBeforSave();
	return CPropertyPage::OnOK();
}

void CDlgTBTSameRunway::OnCancel()
{
	return CPropertyPage::OnCancel();
}
