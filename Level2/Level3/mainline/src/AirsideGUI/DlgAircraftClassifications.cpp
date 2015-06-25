// DlgAircraftRelatedCategories.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgAircraftClassifications.h"

static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

static const int MAXITEMCOUNT = 6;

// CDlgAircraftClassifications dialog

IMPLEMENT_DYNAMIC(CDlgAircraftClassifications, CXTResizeDialog)

CDlgAircraftClassifications::CDlgAircraftClassifications(int nProjID, FlightClassificationBasisType emCategoryType, CWnd* pParent)
	: CXTResizeDialog(CDlgAircraftClassifications::IDD, pParent)
	, m_emCategoryType(emCategoryType)
	, m_nProjID(nProjID)
{
	m_pAircraftClassifications = new AircraftClassifications(nProjID, emCategoryType);
}


CDlgAircraftClassifications::~CDlgAircraftClassifications()
{
	delete m_pAircraftClassifications;
	m_pAircraftClassifications = NULL;
}

void CDlgAircraftClassifications::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTENTS, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(CDlgAircraftClassifications, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDeleteItem)
	ON_COMMAND(ID_EDIT_ITEM, OnEditItem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTENTS, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_CONTENTS, OnLvnEndlabeleditListContents)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
END_MESSAGE_MAP()

BOOL CDlgAircraftClassifications::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	switch(m_emCategoryType)
	{
	case WakeVortexWightBased:
		SetWindowText(_T("Wake Vortex Weight Categories"));
		CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_19,this);
		CUnitPiece::SetUnitInUse(ARCUnit_Weight_InUse);
		CUnitPiece::SetCurSelWeightUnit(AU_WEIGHT_POUND) ;
		break;
	case ApproachSpeedBased:
		SetWindowText(_T("Approach Speed Categories"));
		CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_20,this);
		CUnitPiece::SetUnitInUse(ARCUnit_Velocity_InUse);
		CUnitPiece::SetCurSelVelocityUnit(AU_VELOCITY_KTS);
		break;
	case SurfaceBearingWeightBased:
		SetWindowText(_T("Surface Bearing Categories"));
		CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_21,this);
		CUnitPiece::SetUnitInUse(ARCUnit_Weight_InUse);
		break;
	case WingspanBased:
		SetWindowText(_T("Wingspan Categories"));
		CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_22,this);
		CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
		break;
	default:
		ASSERT(FALSE);
	}

	m_pAircraftClassifications->ReadData();
	
	InitToolBar();
	InitListCtrl();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_TOOLBARCONTENTER, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_CONTENTS, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	/////	
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndListCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	/////

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

int CDlgAircraftClassifications::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT);

	return 0;
}

void CDlgAircraftClassifications::InitToolBar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->GetWindowRect(&rectToolBar);
	ScreenToClient(&rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(&rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	UpdateToolBar();
}

void CDlgAircraftClassifications::InitListCtrl()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);
    

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;

	lvc.pszText = "Name";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	CString strLabMin = _T("Min   "), strLabMax = _T("Max   ");
	CString strTemp = _T("");

	switch(m_emCategoryType)
	{
	case WingspanBased:
		strTemp.Format(_T("Span (%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
		strLabMin += strTemp;
		strLabMax += strTemp;
		break;
	case SurfaceBearingWeightBased:
		strTemp.Format(_T("MRW (%s)"),CARCWeightUnit::GetWeightUnitString(CUnitPiece::GetCurSelWeightUnit()));
		strLabMin += strTemp;
		strLabMax += strTemp;
		break;
	case WakeVortexWightBased:
		strTemp.Format(_T("MTOW (%s)"),CARCWeightUnit::GetWeightUnitString(CUnitPiece::GetCurSelWeightUnit()));
		strLabMin += strTemp;
		strLabMax += strTemp;
		break;
	case ApproachSpeedBased:
		strTemp.Format(_T("IAS (%s)"),CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()));
		strLabMin += strTemp;
		strLabMax += strTemp;
		break;

	default:
		ASSERT(FALSE);
	}


	lvc.pszText = strLabMin.GetBuffer(0);
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = strLabMax.GetBuffer(0);
	lvc.cx = 100;
	if (m_emCategoryType == WakeVortexWightBased)
		lvc.fmt = LVCFMT_NOEDIT;
	else
		lvc.fmt = LVCFMT_NUMBER;
	
	
	m_wndListCtrl.InsertColumn(2, &lvc);

//load aircraft classification data
	loadData();
}

void CDlgAircraftClassifications::loadData()
{
	m_wndListCtrl.DeleteAllItems();
	ASSERT(m_pAircraftClassifications != NULL);
	int nCount = m_pAircraftClassifications->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CString strTemp;
		AircraftClassificationItem* pItem =  m_pAircraftClassifications->GetItem(i);
		if(!pItem)continue;
		m_wndListCtrl.InsertItem(i,pItem->m_strName);

		int nMinValue = 0 , nMaxValue = 0;
		bool hasMaxVaule = true;
		switch(m_emCategoryType)
		{
		case WingspanBased:
			nMinValue = (int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->m_nMinValue) + 0.5);
			nMaxValue = (int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->m_nMaxValue) + 0.5);
			break;
		case SurfaceBearingWeightBased:
			nMinValue = (int)(CARCWeightUnit::ConvertWeight(DEFAULT_DATABASE_WEIGHT_UNIT,CUnitPiece::GetCurSelWeightUnit(),(double)pItem->m_nMinValue) + 0.5);
			nMaxValue = (int)(CARCWeightUnit::ConvertWeight(DEFAULT_DATABASE_WEIGHT_UNIT,CUnitPiece::GetCurSelWeightUnit(),(double)pItem->m_nMaxValue) + 0.5);
			break;		
		case WakeVortexWightBased:
			{
				nMinValue = (int)(CARCWeightUnit::ConvertWeight(DEFAULT_DATABASE_WEIGHT_UNIT,CUnitPiece::GetCurSelWeightUnit(),(double)pItem->m_nMinValue) + 0.5);
				int nValue = m_pAircraftClassifications->GetMaxValue(i);
				if (nValue > -1)
				{
					nMaxValue = (int)(CARCWeightUnit::ConvertWeight(DEFAULT_DATABASE_WEIGHT_UNIT,CUnitPiece::GetCurSelWeightUnit(),(double)nValue) + 0.5);
				}
				else
				{
					hasMaxVaule = false;
				}
			}
			break;		
		case ApproachSpeedBased:
			nMinValue = (int)(CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),(double)pItem->m_nMinValue) + 0.5);
			nMaxValue = (int)(CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),(double)pItem->m_nMaxValue) + 0.5);
			break;
		default:
			ASSERT(FALSE);
		}

		strTemp.Format("%d", nMinValue);
		m_wndListCtrl.SetItemText(i, 1, strTemp);
		if (hasMaxVaule)
		{
			strTemp.Format("%d", nMaxValue);
		}
		else
		{
			strTemp = CString(_T("unlimited"));
		}
		m_wndListCtrl.SetItemText(i, 2, strTemp);

		m_wndListCtrl.SetItemData(i, (DWORD_PTR)pItem);
	}
}

// Update the state of toolbar
void CDlgAircraftClassifications::UpdateToolBar()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	// if the item count > MAXITEMCOUNT, disable the new item button.
	BOOL bEnable = TRUE;
	int nItemCount = m_wndListCtrl.GetItemCount();
	if (nItemCount >= MAXITEMCOUNT)
		bEnable = FALSE;
	else
		bEnable = TRUE;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM, bEnable);

	// if there's no item selected, all of other button should be disable.
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM);
	}
}

void CDlgAircraftClassifications::OnNewItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	int nItemCount = m_wndListCtrl.GetItemCount();
	//TCHAR lpszIndex[32] = {0};
	//_stprintf(lpszIndex, _T("%d"), nItemCount + 1);

	m_wndListCtrl.InsertItem(nItemCount, _T(""));
	m_wndListCtrl.EditSubLabel(nItemCount, 0);

	
	int nIndex = m_wndListCtrl.GetItemCount()-1;

	AircraftClassificationItem* pNewItem = new AircraftClassificationItem(m_emCategoryType);
	pNewItem->SetProjectID(m_nProjID);

	m_wndListCtrl.SetItemData(nIndex, (DWORD_PTR)pNewItem);
	m_pAircraftClassifications->AddItem(pNewItem);

	GetDlgItem(IDC_SAVE)->EnableWindow();


	UpdateToolBar();

}

void CDlgAircraftClassifications::OnDeleteItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			AircraftClassificationItem* pNewItem = (AircraftClassificationItem*)(m_wndListCtrl.GetItemData(nItem));
			m_pAircraftClassifications->DeleteItem(pNewItem);

			m_wndListCtrl.DeleteItemEx(nItem);
			m_pAircraftClassifications->sort();
			loadData();
		}
	}

	GetDlgItem(IDC_SAVE)->EnableWindow();
	UpdateToolBar();
}

void CDlgAircraftClassifications::OnEditItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			m_wndListCtrl.EditSubLabel(nItem, 0);
		}
	}
}



void CDlgAircraftClassifications::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	UpdateToolBar();

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			// TRACE("Unselected Item %d...\n", pnmv->iItem);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
	}
	else
		ASSERT(0);
}

void CDlgAircraftClassifications::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);

	//Update
	int nItem = pnmv->iSubItem;
	AircraftClassificationItem* pNewItem = (AircraftClassificationItem*)m_wndListCtrl.GetItemData(nItem);
	CString strName = m_wndListCtrl.GetItemText(nItem, 0);
	pNewItem->setName(strName);

	double dMinValue = _tstof(m_wndListCtrl.GetItemText(nItem, 1));
	double dMaxValue = _tstof(m_wndListCtrl.GetItemText(nItem, 2));
	int nMinValue = 0 , nMaxValue = 0;
	switch(m_emCategoryType)
	{
	case WingspanBased:
		nMinValue = (int)(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dMinValue) + 0.5);
		nMaxValue = (int)(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dMaxValue) + 0.5);
		break;
	case SurfaceBearingWeightBased:
		nMinValue = (int)(CARCWeightUnit::ConvertWeight(CUnitPiece::GetCurSelWeightUnit(),DEFAULT_DATABASE_WEIGHT_UNIT,dMinValue) + 0.5);
		nMaxValue = (int)(CARCWeightUnit::ConvertWeight(CUnitPiece::GetCurSelWeightUnit(),DEFAULT_DATABASE_WEIGHT_UNIT,dMaxValue) + 0.5);
		break;		
	case WakeVortexWightBased:
		nMinValue = (int)(CARCWeightUnit::ConvertWeight(CUnitPiece::GetCurSelWeightUnit(),DEFAULT_DATABASE_WEIGHT_UNIT,dMinValue) + 0.5);
		nMaxValue = (int)(CARCWeightUnit::ConvertWeight(CUnitPiece::GetCurSelWeightUnit(),DEFAULT_DATABASE_WEIGHT_UNIT,dMaxValue) + 0.5);
		break;		
	case ApproachSpeedBased:
		nMinValue = (int)(CARCVelocityUnit::ConvertVelocity(CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,dMinValue) + 0.5);
		nMaxValue = (int)(CARCVelocityUnit::ConvertVelocity(CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,dMaxValue) + 0.5);
		break;
	default:
		ASSERT(FALSE);
	}	

	if (pNewItem->m_nMinValue != nMinValue)
	{	
		pNewItem->setMinValue(nMinValue);
		if (m_emCategoryType == WakeVortexWightBased)
		{
			m_pAircraftClassifications->sort();
			loadData();
		}
	}

	if (m_emCategoryType != WakeVortexWightBased)
	{
		pNewItem->setMaxValue(nMaxValue);
	}
	
    GetDlgItem(IDC_SAVE)->EnableWindow();

	*pResult = 0;
}

void CDlgAircraftClassifications::OnBnClickedOk()
{
	m_pAircraftClassifications->SaveData();
	OnOK();
}

void CDlgAircraftClassifications::OnBnClickedCancel()
{
	m_pAircraftClassifications->ReadData();
	OnCancel();
}

void CDlgAircraftClassifications::OnBnClickedSave()
{
	m_pAircraftClassifications->SaveData();
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

bool CDlgAircraftClassifications::ConvertUnitFromDBtoGUI(void)
{
	return (false);
}

bool CDlgAircraftClassifications::RefreshGUI(void)
{
	InitListCtrl();
	return (true);
}

bool CDlgAircraftClassifications::ConvertUnitFromGUItoDB(void)
{
	return (false);
}

