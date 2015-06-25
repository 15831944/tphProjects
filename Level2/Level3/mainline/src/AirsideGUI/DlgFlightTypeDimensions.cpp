#include "stdafx.h"
#include "Resource.h"
#include "DlgFlightTypeDimensions.h"
#include "../InputAirside/FlightTypeDetailsManager.h"
#include "../InputAirside/FlightTypeDetailsItem.h"
#include "../Common/AirportDatabase.h"
#include ".\dlgflighttypedimensions.h"

#define ListRowCount 11

namespace
{
	const UINT ID_NEW_FLTTYPE_DIM = 11;
	const UINT ID_DEL_FLTTYPE_DIM = 12;

}

IMPLEMENT_DYNAMIC(CDlgFlightTypeDimensions, CDialog)
CDlgFlightTypeDimensions::CDlgFlightTypeDimensions(int nProjID,PSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFlightTypeDimensions::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pAirportDB(pAirportDB)
	,m_pSelectFlightType(pSelectFlightType)
	,m_nExtent(50)
	,m_nMinorInterval(2)
	,m_bInit(FALSE)
{
	m_pFlightTypeDetailsMgr = new FlightTypeDetailsManager(m_pAirportDB, m_nProjID);
}

CDlgFlightTypeDimensions::~CDlgFlightTypeDimensions()
{
	delete m_pFlightTypeDetailsMgr;
}

void CDlgFlightTypeDimensions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FLTTYPE, m_wndListBox);
	DDX_Control(pDX, IDC_LIST_DIMENSION, m_wndListCtrl);
	DDX_Control(pDX, IDC_SLIDER_EXTX, m_wndExtXSlider);
	DDX_Control(pDX, IDC_SLIDER_EXTY, m_wndExtYSlider);
	DDX_Text(pDX, IDC_EDIT1, m_nExtent);
	DDX_Text(pDX, IDC_EDIT2, m_nMinorInterval);
}


BEGIN_MESSAGE_MAP(CDlgFlightTypeDimensions, CDialog)
	ON_WM_CREATE()

	ON_COMMAND(ID_NEW_FLTTYPE_DIM, OnNewFltType)
	ON_COMMAND(ID_DEL_FLTTYPE_DIM, OnDelFltType)

	ON_LBN_SELCHANGE(IDC_LIST_FLTTYPE, OnSelchangeListFltType)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DIMENSION, OnLvnEndlabelEditListCtrl)

	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEditExtent)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEditMinorInterval)
	ON_WM_HSCROLL()
	
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCSAVE, OnBnClickedCsave)
END_MESSAGE_MAP()


// CDlgFlightTypeDimensions message handlers
int CDlgFlightTypeDimensions::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& fltToolbar = m_wndFltToolbar.GetToolBarCtrl();
	fltToolbar.SetCmdID(0, ID_NEW_FLTTYPE_DIM);
	fltToolbar.SetCmdID(1, ID_DEL_FLTTYPE_DIM);

	return 0;
}

void CDlgFlightTypeDimensions::UpdateUIState()
{
	int nSel = m_wndListBox.GetCurSel();
	BOOL bEnable = (nSel != LB_ERR);

	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE_DIM, bEnable);
	m_wndListCtrl.EnableWindow(bEnable);

}

BOOL CDlgFlightTypeDimensions::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	m_wndFltToolbar.MoveWindow(&rectFltToolbar, true);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLTTYPE_DIM, TRUE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE_DIM, FALSE);

	//InitPropertyGrid();
	InitListCtrl();

	
	m_wndExtXSlider.SetRange(0, 10);
	m_wndExtYSlider.SetRange(0, 10);
	m_wndExtXSlider.SetPos(5);
	m_wndExtYSlider.SetPos(5);

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN1))->SetRange(30,500);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN2))->SetRange(1,50);

	m_spFlight2D = GetDlgItem(IDC_FLIGHT2D1)->GetControlUnknown();
	m_spGridPtr = m_spFlight2D;
	m_spFltDimensions = m_spFlight2D;
	m_spServiceLocations = m_spFlight2D;


	m_pFlightTypeDetailsMgr->ReadData();
	InitListBox();
	UpdateUIState();

	UpdateData(FALSE);
	m_bInit = TRUE;

	return TRUE;
}

void CDlgFlightTypeDimensions::InitListBox()
{
	for (int i=0; i<m_pFlightTypeDetailsMgr->GetCount(); i++)
	{
		FlightTypeDetailsItem* pItem = m_pFlightTypeDetailsMgr->GetItem(i);

		FlightConstraint fltType = pItem->GetFltType();
		CString strFltType;
		fltType.screenPrint(strFltType);
		int nSel = m_wndListBox.AddString(strFltType);
		m_wndListBox.SetItemData(nSel, (DWORD_PTR)pItem);			
	}

	if(m_pFlightTypeDetailsMgr->GetCount()>0)
	{
		m_wndListBox.SetCurSel(0);
		OnSelchangeListFltType();
	}

}


void CDlgFlightTypeDimensions::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	CStringList strList;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Item");
	lvc.cx = 160;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Details");
	lvc.cx = 55;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(1, &lvc);

	char *szLabels[] = 
	{
		//"Grid Extent (m)",
		//"Grid Spacing (m)",
		"Max Wing Span (m)",
		"Max Wing Thickness (m)",
		"Min Wing Thickness (m)",
		"Wing sweep (degree)",
		"Max fuselage length (m)",
		"Max fuselage width (m)",
		"Wing root position (%fuselage)",
		"Tail plane span (m)",
		"Tail plane thickness (m)",
		"Tail plane sweep (degree)",
		"Tail plane position (%fuselage)",
	};

	for (int iCntr = 0; iCntr < ListRowCount; iCntr++)
		m_wndListCtrl.InsertItem(iCntr, szLabels[iCntr]);
}

void CDlgFlightTypeDimensions::OnNewFltType()
{
	if (m_pSelectFlightType == NULL)
		return;
	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	if(m_pFlightTypeDetailsMgr->FlightConstraintHasDefine(fltType))
	{
		AfxMessageBox(_T("current Flight type has been defined!"));
		return;
	}


	FlightTypeDetailsItem* pNewItem = new FlightTypeDetailsItem(m_pAirportDB, m_nProjID);
	pNewItem->SetFltType(fltType);

	m_pFlightTypeDetailsMgr->AddItem(pNewItem);

	CString strFltType;
	fltType.screenPrint(strFltType);

	int nSel = m_wndListBox.AddString(strFltType);
	m_wndListBox.SetCurSel(nSel);
	m_wndListBox.SetItemData(nSel, (DWORD_PTR)pNewItem);

	OnSelchangeListFltType();
}

void CDlgFlightTypeDimensions::OnDelFltType()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	m_pFlightTypeDetailsMgr->DeleteItem(pItem);

	m_wndListBox.DeleteString(nSel);

	if(m_wndListBox.GetSelCount())
	{
		m_wndListBox.SetCurSel(0);
	}
	OnSelchangeListFltType();
}

void CDlgFlightTypeDimensions::OnSelchangeListFltType()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		UpdateUIState();
		return;
	}

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);

	m_nExtent = (int)pItem->GetDimensions().GetValue(0);
	m_nMinorInterval = (int)pItem->GetDimensions().GetValue(1);

	ResetListCtrl(pItem);
	ResetARCFlightControl(pItem);

	m_wndExtXSlider.SetPos(5);
	m_wndExtYSlider.SetPos(5);

	UpdateUIState();
}

void CDlgFlightTypeDimensions::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(IDC_SLIDER_EXTX == pScrollBar->GetDlgCtrlID())
	{
		int nPos = m_wndExtXSlider.GetPos();
		if(nPos <=5)
		{
			int n = -(5-nPos);
			m_spFlight2D->DoOffset(VARIANT_TRUE, -(5-nPos));

		}
		else
			m_spFlight2D->DoOffset(VARIANT_TRUE, nPos - 5);

	}
	else if(IDC_SLIDER_EXTY == pScrollBar->GetDlgCtrlID())
	{
		int nPos = m_wndExtYSlider.GetPos();
		if(nPos <=5)
			m_spFlight2D->DoOffset(VARIANT_FALSE, -(5-nPos));
		else
			m_spFlight2D->DoOffset(VARIANT_FALSE, nPos - 5);
	}
}

void CDlgFlightTypeDimensions::OnEnChangeEditExtent()
{
	if(!m_bInit)
		return;

	UpdateData();

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		UpdateUIState();
		return;
	}

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);

	FlightTypeDimensions& dim = pItem->GetDimensions();

	dim.SetValue(0, m_nExtent);
	dim.SetValue(1, m_nMinorInterval);

	ResetARCFlightControl(pItem);

}

void CDlgFlightTypeDimensions::OnEnChangeEditMinorInterval()
{
	OnEnChangeEditExtent();
}

void CDlgFlightTypeDimensions::ResetListCtrl(FlightTypeDetailsItem* pItem)
{
	CString strValue;
	for(int i=0; i<ListRowCount; i++)
	{
		strValue.Format("%.2f", pItem->GetDimensions().GetValue(i+2));
		m_wndListCtrl.SetItemText(i, 1, strValue);
	}

	UpdateData(FALSE);
}

void CDlgFlightTypeDimensions::ResetSliderCtrl()
{

}

void CDlgFlightTypeDimensions::ResetARCFlightControl(FlightTypeDetailsItem* pItem)
{
	FlightTypeDimensions& dim = pItem->GetDimensions();	

	long lVal = (long)dim.GetGridExtent();
	m_spGridPtr->SetExtent(lVal);
	lVal = (long)dim.GetGridSpacing();
	m_spGridPtr->SetMinorInterval(lVal);

	m_spFltDimensions->MaxWingSpan			=	dim.GetMaxWingSpan();
	m_spFltDimensions->MaxWingThickness		=	dim.GetMaxWingThickness();
	m_spFltDimensions->MinWingThickness		=	dim.GetMinWingThickness();
	m_spFltDimensions->WingSweep			=	dim.GetWingSweep();
	m_spFltDimensions->MaxFuselageLength	=	dim.GetMaxFuselageLength();
	m_spFltDimensions->MaxFuselageWidth		=	dim.GetMaxFuselageWidth();
	m_spFltDimensions->WingRootPosition		=	dim.GetWingRootPosition();
	m_spFltDimensions->TailPlaneSpan		=	dim.GetTailPlaneSpan();
	m_spFltDimensions->TailPlaneThickness	=	dim.GetTailPlaneThickness();
	m_spFltDimensions->TailPlaneSweep		=	dim.GetTailPlaneSweep();
	m_spFltDimensions->TailPlanePosition	=	dim.GetTailPlanePosition();

	m_spFlight2D->Refresh();

}

void CDlgFlightTypeDimensions::OnLvnEndlabelEditListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	int nItem = pnmv->iSubItem;
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);

	double dValue = atof(m_wndListCtrl.GetItemText(nItem, 1));

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	FlightTypeDimensions& dim = pItem->GetDimensions();

	if(dValue == dim.GetValue(nItem+2))
		return;

	dim.SetValue(nItem+2, dValue);

	ResetARCFlightControl(pItem);

	GetDlgItem(IDCSAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

void CDlgFlightTypeDimensions::OnBnClickedOk()
{
	m_pFlightTypeDetailsMgr->SaveData();
	OnOK();
}

void CDlgFlightTypeDimensions::OnBnClickedCsave()
{
	m_pFlightTypeDetailsMgr->SaveData();
	GetDlgItem(IDCSAVE)->EnableWindow(FALSE);
}
