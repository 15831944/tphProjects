// DlgInTrailSeparation.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgInTrailSeparation.h"
#include ".\dlgintrailseparation.h"
#include "../Database//DBElementCollection_Impl.h"

// CDlgInTrailSeparation dialog

IMPLEMENT_DYNAMIC(CDlgInTrailSeparation, CXTResizeDialog)
CDlgInTrailSeparation::CDlgInTrailSeparation(int nProjID, Terminal* pTerminal, CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgInTrailSeparation::IDD, pParent)
{
	m_nProjID = nProjID;
	m_pTerminal = pTerminal;
	m_pCurInTrailSepData = NULL;
	m_pCurInTrailSep = NULL;

	m_pClsNone = new AircraftClassifications(m_nProjID, NoneBased);
	m_pClsNone->ReadData();

	m_pClsWakeVortexWightBased = new AircraftClassifications(m_nProjID, WakeVortexWightBased);
	m_pClsWakeVortexWightBased->ReadData();

	m_pClsWingspanBased = new AircraftClassifications(m_nProjID, WingspanBased);
	m_pClsWingspanBased->ReadData();

	m_pClsSurfaceBearingWeightBased = new AircraftClassifications(m_nProjID, SurfaceBearingWeightBased);
	m_pClsSurfaceBearingWeightBased->ReadData();

	m_pClsApproachSpeedBased = new AircraftClassifications(m_nProjID, ApproachSpeedBased);
	m_pClsApproachSpeedBased->ReadData();
}

CDlgInTrailSeparation::~CDlgInTrailSeparation()
{
	if ( m_pClsNone )
	{
		delete m_pClsNone;
		m_pClsNone = NULL;
	}
	if ( m_pClsWakeVortexWightBased )
	{
		delete m_pClsWakeVortexWightBased;
		m_pClsWakeVortexWightBased = NULL;
	}
	if ( m_pClsWingspanBased )
	{
		delete m_pClsWingspanBased;
		m_pClsWingspanBased = NULL;
	}
	if ( m_pClsSurfaceBearingWeightBased )
	{
		delete m_pClsSurfaceBearingWeightBased;
		m_pClsSurfaceBearingWeightBased = NULL;
	}
	if ( m_pClsApproachSpeedBased )
	{
		delete m_pClsApproachSpeedBased;
		m_pClsApproachSpeedBased = NULL;
	}
}

void CDlgInTrailSeparation::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_lstData);
	DDX_Control(pDX, IDC_LIST_PHASE_TYPE, m_lstPhaseType);
	DDX_Control(pDX, IDC_COMBO_CATEGORY_TYPE, m_cbCategoryType);
}

BEGIN_MESSAGE_MAP(CDlgInTrailSeparation, CXTResizeDialog)
	ON_LBN_SELCHANGE(IDC_LIST_PHASE_TYPE, OnLbnSelchangeListPhaseType)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY_TYPE, OnCbnSelchangeComboCategoryType)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListData)
END_MESSAGE_MAP()


// CDlgInTrailSeparation message handlers

BOOL CDlgInTrailSeparation::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitPhaseType();
	InitCategoryType();
	InitListCtrl();
	LoadData();
	DelInitData();

	AddAllNewData();
	SaveData();

	DisplayCurData(CruisePhase);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_RIGHTGROUPBOX, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	//SetResize(IDC_STATIC_LEFTGROUPBOX, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	//SetResize(IDC_STATIC_PHASE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_PHASE_TYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC_CLASSIFICATION, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_COMBO_CATEGORY_TYPE, SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_LIST_DATA, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgInTrailSeparation::InitPhaseType()
{
	m_lstPhaseType.AddString( "Cruise" );
	m_lstPhaseType.AddString( "Terminal" );
	m_lstPhaseType.AddString( "Approach to land" );

	m_lstPhaseType.SetCurSel(0);

	//add by adam 2007-04-27
	m_emCurPhaseType = CruisePhase;
	//End add by adam 2007-04-27
}

void CDlgInTrailSeparation::InitCategoryType()
{
	m_cbCategoryType.AddString( "None" );
	m_cbCategoryType.AddString( "Wake Vortex" );
	m_cbCategoryType.AddString( "Wingspans" );
	m_cbCategoryType.AddString( "Surface bearing weight" );
	m_cbCategoryType.AddString( "Approach speed" );

	m_cbCategoryType.SetCurSel( 0 );

	//add by adam 2007-04-27
	m_emCurCategoryType = NoneBased;
	//End add by adam 2007-04-27
}

void CDlgInTrailSeparation::InitListCtrl()
{
	DWORD dwStyle = m_lstData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstData.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Trail Type", "Lead Type", "Min Distance(nm)", "Min Time" };
	int DefaultColumnWidth[] = { 85, 85, 100, 80 };

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT, 
			LVCFMT_NOEDIT,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER
	};

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 4; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_lstData.InsertColumn(i, &lvc);
	}
}

void CDlgInTrailSeparation::LoadData()
{
	m_inTrailSepList.ReadData( m_nProjID );
}

void CDlgInTrailSeparation::SaveData()
{	
	try
	{
		CADODatabase::BeginTransaction() ;
		m_inTrailSepList.SaveData( m_nProjID );
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}

void CDlgInTrailSeparation::DelInitData()
{
	FlightClassificationBasisType emCategoryType;
	for (int i = 0; i < m_inTrailSepList.GetItemCount(); i++)
	{
		m_pCurInTrailSep = m_inTrailSepList.GetItem(i);
		emCategoryType = m_pCurInTrailSep->getCategoryType();
		if (emCategoryType == NoneBased)
		{
			continue;
		}

		switch ( emCategoryType )
		{
		case WakeVortexWightBased:
			m_pCurClassifications = m_pClsWakeVortexWightBased;
			break;
		case WingspanBased:
			m_pCurClassifications = m_pClsWingspanBased;
			break;
		case SurfaceBearingWeightBased:
			m_pCurClassifications = m_pClsSurfaceBearingWeightBased;
			break;
		case ApproachSpeedBased:
			m_pCurClassifications = m_pClsApproachSpeedBased;
			break;
		default:
			break;

		}

		for (int j = 0; j < m_pCurInTrailSep->GetItemCount(); j++)
		{
			BOOL IsTrailExist = FALSE;
			BOOL IsLeadExist = FALSE;
			m_pCurInTrailSepData = m_pCurInTrailSep->GetItem(j);
			m_pCurClsTrailItem = m_pCurInTrailSepData->getClsTrailItem();
			m_pCurClsLeadItem = m_pCurInTrailSepData->getClsLeadItem();

			for (int k = 0; k < m_pCurClassifications->GetCount(); k++)
			{
				if (m_pCurClassifications->GetItem(k)->m_nID == m_pCurClsTrailItem->m_nID)					
				{
					IsTrailExist = TRUE;
				}
				if (m_pCurClassifications->GetItem(k)->m_nID == m_pCurClsLeadItem->m_nID)
				{
					IsLeadExist = TRUE;
				}
				if (IsTrailExist && IsLeadExist)
				{
					break;
				}
			} // end for k

			if ( !(IsTrailExist && IsLeadExist) )
			{
				m_pCurInTrailSep->Deltem(j);
				j--;
			}

		} // end for j

	} // end for i

}

void CDlgInTrailSeparation::AddAllNewData()
{
	AddDataByType(CruisePhase, NoneBased);
	AddDataByType(CruisePhase, WingspanBased);
	AddDataByType(CruisePhase, SurfaceBearingWeightBased);
	AddDataByType(CruisePhase, WakeVortexWightBased);
	AddDataByType(CruisePhase, ApproachSpeedBased);

	CInTrailSeparation *pInTrailSeparation = NULL;
	pInTrailSeparation = m_inTrailSepList.GetItemByType(CruisePhase);
	if (NULL == pInTrailSeparation)
	{
		m_inTrailSepList.SetItemActive(CruisePhase, NoneBased, TRUE);
	}

	AddDataByType(TerminalPhase, NoneBased);
	AddDataByType(TerminalPhase, WingspanBased);
	AddDataByType(TerminalPhase, SurfaceBearingWeightBased);
	AddDataByType(TerminalPhase, WakeVortexWightBased);
	AddDataByType(TerminalPhase, ApproachSpeedBased);

	pInTrailSeparation = m_inTrailSepList.GetItemByType(TerminalPhase);
	if (NULL == pInTrailSeparation)
	{
		m_inTrailSepList.SetItemActive(TerminalPhase, WakeVortexWightBased, TRUE);
	}

	AddDataByType(ApproachPhase, NoneBased);
	AddDataByType(ApproachPhase, WingspanBased);
	AddDataByType(ApproachPhase, SurfaceBearingWeightBased);
	AddDataByType(ApproachPhase, WakeVortexWightBased);
	AddDataByType(ApproachPhase, ApproachSpeedBased);

	pInTrailSeparation = m_inTrailSepList.GetItemByType(ApproachPhase);
	if (NULL == pInTrailSeparation)
	{
		m_inTrailSepList.SetItemActive(ApproachPhase, WakeVortexWightBased, TRUE);
	}
}

void CDlgInTrailSeparation::AddDataByType(PhaseType emPhaseType, FlightClassificationBasisType emCategoryType)
{
	BOOL IsExistType = TRUE;
	m_pCurInTrailSep = m_inTrailSepList.GetItemByType(emPhaseType, emCategoryType);
	if (m_pCurInTrailSep == NULL)
	{
		IsExistType = FALSE;
		m_pCurInTrailSep = new CInTrailSeparation(emPhaseType, emCategoryType);
		m_pCurInTrailSep->setProjID( m_nProjID );
		//m_emCurCategoryType = emCategoryType;
	}
	//else
	//{
	//	m_emCurCategoryType = m_pCurInTrailSep->getCategoryType();
	//}

	switch ( emCategoryType )
	{
	case NoneBased:
		{
			if (m_pCurInTrailSep->GetItemCount() <= 0)
			{
				AircraftClassificationItem classificationItem(emCategoryType);
				classificationItem.setName( "Default" );
				m_pCurInTrailSepData = new CInTrailSeparationData();
				m_pCurInTrailSepData->setInTrailSepID( m_pCurInTrailSep->getID() );
				m_pCurInTrailSepData->setClsLeadItem( classificationItem );
				m_pCurInTrailSepData->setClsTrailItem( classificationItem );
				m_pCurInTrailSepData->setMinDistance(5);
				m_pCurInTrailSepData->setMinTime(2);
				m_pCurInTrailSep->AddItem( m_pCurInTrailSepData );
				m_inTrailSepList.AddItem( m_pCurInTrailSep );
			}
		}
		return;
	case WakeVortexWightBased:
		m_pCurClassifications = m_pClsWakeVortexWightBased;
		break;
	case WingspanBased:
		m_pCurClassifications = m_pClsWingspanBased;
		break;
	case SurfaceBearingWeightBased:
		m_pCurClassifications = m_pClsSurfaceBearingWeightBased;
		break;
	case ApproachSpeedBased:
		m_pCurClassifications = m_pClsApproachSpeedBased;
		break;
	default:
		break;

	}

	int nCount = m_pCurClassifications->GetCount();

	for (int i = 0; i < nCount; i++)
	{
		for (int j = 0; j < nCount; j++)
		{
			BOOL IsItemExist = false;
			if (m_pCurInTrailSep != NULL)
			{
				for (int k = 0; k < m_pCurInTrailSep->GetItemCount(); k++)
				{
					m_pCurInTrailSepData = m_pCurInTrailSep->GetItem(k);
					int nClsTrailID = m_pCurInTrailSepData->getClsTrailItem()->m_nID;
					int nClsLeadID = m_pCurInTrailSepData->getClsLeadItem()->m_nID;

					if ( nClsTrailID == m_pCurClassifications->GetItem(i)->m_nID &&
						nClsLeadID == m_pCurClassifications->GetItem(j)->m_nID )
					{
						IsItemExist = TRUE;
						break;
					}
				}
			}
			if ( !IsItemExist )
			{
				AircraftClassificationItem *pClsTrailItem = m_pCurClassifications->GetItem(i);
				AircraftClassificationItem *pClsLeadItem = m_pCurClassifications->GetItem(j);
				m_pCurInTrailSepData = new CInTrailSeparationData();
				m_pCurInTrailSepData->setInTrailSepID( m_pCurInTrailSep->getID() );
				m_pCurInTrailSepData->setClsTrailItem( *pClsTrailItem );
				m_pCurInTrailSepData->setClsLeadItem( *pClsLeadItem );

				CString strName;
				strName.Format("%s", pClsLeadItem->m_strName);
				strName.MakeLower();
				if (!strName.CompareNoCase("light"))
				{
					m_pCurInTrailSepData->setMinDistance(3);
				}
				else if (!strName.CompareNoCase("medium"))
				{
					m_pCurInTrailSepData->setMinDistance(4);
				}
				else if (!strName.CompareNoCase("heavy"))
				{
					m_pCurInTrailSepData->setMinDistance(5);
				}
				else
				{
					m_pCurInTrailSepData->setMinDistance(5);
				}
				
				m_pCurInTrailSepData->setMinTime(2);

				m_pCurInTrailSep->AddItem( m_pCurInTrailSepData );
			}
		}
	}
	if ( !IsExistType )
	{
		m_inTrailSepList.AddItem( m_pCurInTrailSep );
	}
}

void CDlgInTrailSeparation::DisplayCurData(PhaseType emPhaseType)
{
	m_lstData.DeleteAllItems();
	m_pCurInTrailSep = m_inTrailSepList.GetItemByType(emPhaseType);
	FlightClassificationBasisType emCurCategoryType = m_pCurInTrailSep->getCategoryType();

	int nSelComBox = GetComboxIndexByCategoryType(emCurCategoryType);
	m_cbCategoryType.SetCurSel(nSelComBox);

	for (int i = 0; i < m_pCurInTrailSep->GetItemCount(); i++)
	{
		CString strTmp;
		int nClsItem = 0;
		m_pCurInTrailSepData = m_pCurInTrailSep->GetItem(i);

		nClsItem = m_pCurInTrailSepData->getClsTrailItem()->getID();
		strTmp = GetClassificationsName(nClsItem, emCurCategoryType);
		m_lstData.InsertItem(i, strTmp);

		nClsItem = m_pCurInTrailSepData->getClsLeadItem()->getID();
		strTmp = GetClassificationsName(nClsItem, emCurCategoryType);
		m_lstData.SetItemText(i, 1, strTmp);

		strTmp.Format("%ld", m_pCurInTrailSepData->getMinDistance());
		m_lstData.SetItemText(i, 2, strTmp);

		strTmp.Format("%ld", m_pCurInTrailSepData->getMinTime());
		m_lstData.SetItemText(i, 3, strTmp);

		m_lstData.SetItemData(i, (DWORD_PTR)m_pCurInTrailSepData);
	}
}

int CDlgInTrailSeparation::GetComboxIndexByCategoryType(FlightClassificationBasisType emCategoryType)
{
	int nSelComBox = 0;
	switch ( emCategoryType )
	{
	case NoneBased:
		nSelComBox = 0;
		break;
	case WakeVortexWightBased:
		nSelComBox = 1;
		break;
	case WingspanBased:
		nSelComBox = 2;
		break;
	case SurfaceBearingWeightBased:
		nSelComBox = 3;
		break;
	case ApproachSpeedBased:
		nSelComBox = 4;
		break;
	default:
		break;
	}
	return nSelComBox;
}

CString CDlgInTrailSeparation::GetClassificationsName(int nClsItemID, FlightClassificationBasisType emCategoryType)
{
	CString strClsName;
	switch ( emCategoryType )
	{
	case NoneBased:
		strClsName = "Default";
		return strClsName;
	case WakeVortexWightBased:
		m_pCurClassifications = m_pClsWakeVortexWightBased;
		break;
	case WingspanBased:
		m_pCurClassifications = m_pClsWingspanBased;
		break;
	case SurfaceBearingWeightBased:
		m_pCurClassifications = m_pClsSurfaceBearingWeightBased;
		break;
	case ApproachSpeedBased:
		m_pCurClassifications = m_pClsApproachSpeedBased;
		break;
	default:
		break;

	}

	for (int i = 0; i < m_pCurClassifications->GetCount(); i++)
	{
		AircraftClassificationItem* m_pCurClsItem;
		m_pCurClsItem = m_pCurClassifications->GetItem(i);
		if (m_pCurClsItem->getID() == nClsItemID)
		{
			strClsName = m_pCurClsItem->m_strName;
		}
	}

	return strClsName;
}

void CDlgInTrailSeparation::OnLbnSelchangeListPhaseType()
{
	// TODO: Add your control notification handler code here
	int nSel = m_lstPhaseType.GetCurSel();
	switch ( nSel )
	{
	case 0:
		m_emCurPhaseType = CruisePhase;
		break;
	case 1:
		m_emCurPhaseType = TerminalPhase;
		break;
	case 2:
		m_emCurPhaseType = ApproachPhase;
		break;
	default:
		break;
	}

	DisplayCurData(m_emCurPhaseType);
}

void CDlgInTrailSeparation::OnCbnSelchangeComboCategoryType()
{
	// TODO: Add your control notification handler code here
	int nSel = m_cbCategoryType.GetCurSel();

	switch ( nSel )
	{
	case 0:
		m_emCurCategoryType = NoneBased;
		break;
	case 1:
		m_emCurCategoryType = WakeVortexWightBased;
		break;
	case 2:
		m_emCurCategoryType = WingspanBased;
		break;
	case 3:
		m_emCurCategoryType = SurfaceBearingWeightBased;
		break;
	case 4:
		m_emCurCategoryType = ApproachSpeedBased;
		break;
	default:
		break;
	}

	m_inTrailSepList.SetItemActive(m_emCurPhaseType, m_emCurCategoryType, TRUE);

	//m_inTrailSepList.DelItemByType( m_emCurPhaseType );
	//AddDataByType(m_emCurPhaseType, m_emCurCategoryType);
	DisplayCurData(m_emCurPhaseType);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgInTrailSeparation::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();
	OnOK();
}

void CDlgInTrailSeparation::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgInTrailSeparation::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	SaveData();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgInTrailSeparation::OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_lstData.OnEndlabeledit(pNMHDR, pResult);

	int nItem = pDispInfo->item.iItem;
	m_pCurInTrailSepData = (CInTrailSeparationData*)m_lstData.GetItemData(nItem);

	int nMinDistance = atoi(m_lstData.GetItemText(nItem, 2));
	int nMinTime = atoi(m_lstData.GetItemText(nItem, 3));

	m_pCurInTrailSepData->setMinDistance(nMinDistance);
	m_pCurInTrailSepData->setMinTime(nMinTime);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	*pResult = 0;
}
