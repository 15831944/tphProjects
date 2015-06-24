// DlgLandsideSpeedConstraint.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideSpeedConstraint.h"
#include "Landside//InputLandside.h"
#include "Landside/StretchSpeedConstraintList.h"
#include "DlgCurbsideSelect.h"
#include "Common/WinMsg.h"
#include "Common/UnitPiece.h"

// CDlgLandsideSpeedConstraint dialog

const UINT ID_NEW_STRETCH = 31;
const UINT ID_EDIT_STRETCH = 32;
const UINT ID_DEL_STRETCH = 33;
const UINT ID_NEW_DATA = 41;
const UINT ID_DEL_DATA = 42;

IMPLEMENT_DYNAMIC(CDlgLandsideSpeedConstraint, CXTResizeDialog)
CDlgLandsideSpeedConstraint::CDlgLandsideSpeedConstraint(InputLandside* pLandsideInput,int nProjectID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLandsideSpeedConstraint::IDD, pParent)
	,m_pLandsideInput(pLandsideInput)
	,m_nProjectID(nProjectID)
	,m_pStretchSpeedConstraintList(NULL)
{
	
}

CDlgLandsideSpeedConstraint::~CDlgLandsideSpeedConstraint()
{
	if (m_pStretchSpeedConstraintList)
	{
		delete m_pStretchSpeedConstraintList;
		m_pStretchSpeedConstraintList = NULL;
	}
}

void CDlgLandsideSpeedConstraint::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STRETCH, m_wndListBox);
	DDX_Control(pDX, IDC_LIST_CONSTRAINT, m_wndListCtrl);
}

// CDlgLandsideSpeedConstraint message handlers
BEGIN_MESSAGE_MAP(CDlgLandsideSpeedConstraint, CXTResizeDialog)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_STRETCH, OnLbnSelchangeListStretch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONSTRAINT, OnLvnItemchangedListConstraint)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_COMMAND(ID_NEW_STRETCH,OnAddStretch)
	ON_COMMAND(ID_DEL_STRETCH,OnDeleteStretch)
	ON_COMMAND(ID_EDIT_STRETCH,OnEditStretch)
	ON_COMMAND(ID_NEW_DATA,OnAddConstraint)
	ON_COMMAND(ID_DEL_DATA,OnDeleteConstraint)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_CONSTRAINT, OnLvnEndlabeleditListData)
END_MESSAGE_MAP()

BOOL CDlgLandsideSpeedConstraint::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();

	SetResize(m_wndStretchToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndConstraintToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_STRETCH, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_STRETCH, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_CONSTRAINT, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_CONSTRAINT, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	/////	
	CUnitPiece::InitializeUnitPiece(m_nProjectID,UP_ID_1000,this);

	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndListBox.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Velocity_InUse);
	/////

	LoadData();
	CreateComboxString();
	InitListBox();
	InitListCtrl();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	UpdateStretchToolbar();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgLandsideSpeedConstraint::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_wndStretchToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndStretchToolBar.LoadToolBar(IDR_ADDDELEDIT_PRODUCT))
	{
		return -1;
	}

	m_wndStretchToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_STRETCH);
	m_wndStretchToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_STRETCH);
	m_wndStretchToolBar.GetToolBarCtrl().SetCmdID(2, ID_EDIT_STRETCH);


	if(m_wndConstraintToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndConstraintToolBar.LoadToolBar(IDR_ADDDEL_FLIGHT))
	{
		return -1;
	}

	m_wndConstraintToolBar.SetDlgCtrlID(m_wndStretchToolBar.GetDlgCtrlID()+1);
	m_wndConstraintToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_DATA);
	m_wndConstraintToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_DATA);

	return 0;
}

void CDlgLandsideSpeedConstraint::OnAddStretch()
{
	if (m_pStretchSpeedConstraintList == NULL)
		return;
	
	if (m_pLandsideInput == NULL)
		return;
	
	LandsideFacilityLayoutObjectList* pLandsideObjectList = &m_pLandsideInput->getObjectList();
	CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LSTRETCH,m_pParentWnd);
	if (dlg.DoModal() == IDOK)
	{
		if (m_pStretchSpeedConstraintList->availableStretch(dlg.getALTObjectID()))
		{
			StretchSpeedConstraint* pStretchSpeedConstraint = new StretchSpeedConstraint();
			pStretchSpeedConstraint->SetStretchName(dlg.getALTObjectID());
			m_pStretchSpeedConstraintList->AddNewItem(pStretchSpeedConstraint);
			int idx = m_wndListBox.AddString(dlg.getALTObjectID().GetIDString());
			m_wndListBox.SetItemData(idx,(DWORD_PTR)pStretchSpeedConstraint);
			m_wndListBox.SetCurSel(idx);
			UpdateStretchToolbar();
			UpdateConstraintToolbar();
			SetListContent();
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		else
		{
			MessageBox(_T("Exist same stretch name, please reinput the stretch name!!!"),_T("Warning"));
		}
	}
}

void CDlgLandsideSpeedConstraint::OnDeleteStretch()
{
	m_wndListBox.SetFocus();

	if (m_pStretchSpeedConstraintList == NULL)
		return;

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	StretchSpeedConstraint* pStretchSpeedConstraint = (StretchSpeedConstraint*)m_wndListBox.GetItemData(nSel);
	if (pStretchSpeedConstraint == NULL)
		return;
	
	m_pStretchSpeedConstraintList->DeleteItem(pStretchSpeedConstraint);
	m_wndListBox.DeleteString(nSel);
	
	if (m_wndListBox.GetCount())
	{
		int nNewSel = (nSel == 0 ? 0 : nSel - 1);
		m_wndListBox.SetCurSel(nNewSel);
	}
	else
	{
		m_wndListCtrl.EnableWindow(FALSE);
		m_wndConstraintToolBar.EnableWindow(FALSE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_NEW_STRETCH,TRUE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_STRETCH,FALSE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_DEL_STRETCH,FALSE);
	}

	SetListContent();
	UpdateConstraintToolbar();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideSpeedConstraint::OnEditStretch()
{
	m_wndListBox.SetFocus();
	if (m_pStretchSpeedConstraintList == NULL)
		return;

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	StretchSpeedConstraint* pStretchSpeedConstraint = (StretchSpeedConstraint*)m_wndListBox.GetItemData(nSel);
	if (pStretchSpeedConstraint == NULL)
		return;

	LandsideFacilityLayoutObjectList* pLandsideObjectList = &m_pLandsideInput->getObjectList();
	CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LSTRETCH,m_pParentWnd);
	if (dlg.DoModal() == IDOK)
	{
		if (!(dlg.getALTObjectID() == pStretchSpeedConstraint->GetStretchName()))
		{
			pStretchSpeedConstraint->SetStretchName(dlg.getALTObjectID());
			m_wndListBox.DeleteString(nSel);
			m_wndListBox.InsertString(nSel,dlg.getALTObjectID().GetIDString());
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgLandsideSpeedConstraint::OnAddConstraint()
{
	m_wndListCtrl.SetFocus();
	if (m_pLandsideInput == NULL)
		return;
	
	int nBoxSel = m_wndListBox.GetCurSel();
	if (nBoxSel == LB_ERR)
		return;

	StretchSpeedConstraint* pStretchSpeedConstraint = (StretchSpeedConstraint*)m_wndListBox.GetItemData(nBoxSel);
	if (pStretchSpeedConstraint == NULL)
		return;

	int nCount = m_wndListCtrl.GetItemCount();
	/*const CVehicleItemDetailList& vehicleTypeList = m_pLandsideInput->getOperationVehicleList();
	unsigned nVehicleSpeCount = vehicleTypeList.GetElementCount();
	for(unsigned i = 0; i < nVehicleSpeCount; i++) 
	{
		CVehicleItemDetail* pVehicleItem = vehicleTypeList.GetItem(i);
		if(!pStretchSpeedConstraint->FindItem(pVehicleItem->GetID()))
		{
			StretchSpeedConstraintData* pConstraintData = new StretchSpeedConstraintData();
			pConstraintData->SetVechileType(pVehicleItem->GetID());

			m_wndListCtrl.InsertItem(nCount,pVehicleItem->getName());
			CString strSpeed;
			double dSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),pConstraintData->GetMaxSpeed());
			strSpeed.Format(_T("%.2f"),dSpeed);
			m_wndListCtrl.SetItemText(i,1,strSpeed);
			m_wndListCtrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_wndListCtrl.SetCurSel(i);
			m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pConstraintData);
			pStretchSpeedConstraint->AddItem(pConstraintData);
			break;
		}
	}*/
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideSpeedConstraint::OnDeleteConstraint()
{
	m_wndListCtrl.SetFocus();
	//check list box sel
	int nBoxSel = m_wndListBox.GetCurSel();
	if (nBoxSel == LB_ERR)
		return;

	//check list ctrl sel
	int nListSel = m_wndListCtrl.GetCurSel();
	if (nListSel == LB_ERR)
		return;

	StretchSpeedConstraint* pStretchSpeedConstraint = (StretchSpeedConstraint*)m_wndListBox.GetItemData(nBoxSel);
	if (pStretchSpeedConstraint == NULL)
		return;

	StretchSpeedConstraintData* pConstraintData = (StretchSpeedConstraintData*)m_wndListCtrl.GetItemData(nListSel);
	if (pConstraintData == NULL)
		return;

	pStretchSpeedConstraint->DeleteItem(pConstraintData);
	m_wndListCtrl.DeleteItem(nListSel);
	int nCount = m_wndListCtrl.GetItemCount();
	if(nCount > 0)
	{
		int nNewSel = (nListSel == 0 ? 0 : nListSel - 1);
		m_wndListCtrl.SetItemState(nNewSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_wndListCtrl.SetCurSel(nNewSel);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideSpeedConstraint::LoadData()
{
	if (m_pStretchSpeedConstraintList)
	{
		delete m_pStretchSpeedConstraintList;
		m_pStretchSpeedConstraintList = NULL;
	}
	
	if (m_pLandsideInput)
	{
		m_pStretchSpeedConstraintList = new StretchSpeedConstraintList(m_pLandsideInput);
		m_pStretchSpeedConstraintList->ReadData();
	}
}

void CDlgLandsideSpeedConstraint::InitListBox()
{
	if (m_pStretchSpeedConstraintList == NULL)
		return;
	
	for (unsigned i = 0; i < m_pStretchSpeedConstraintList->GetElementCount(); i++)
	{
		StretchSpeedConstraint* pStretchConstraint = m_pStretchSpeedConstraintList->GetItem(i);
		m_wndListBox.InsertString(i,pStretchConstraint->GetStretchName().GetIDString());
		m_wndListBox.SetItemData(i,(DWORD_PTR)pStretchConstraint);
	}

	if (m_wndListBox.GetCount() > 0)
	{
		m_wndListBox.SetCurSel(0);
	}
}

void CDlgLandsideSpeedConstraint::InitListCtrl()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	//vehicle Type
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.csList = &m_strListNode;	
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Vehicle Type");
	lvc.cx = 140;
	m_wndListCtrl.InsertColumn(0,&lvc);

	//max v
	CString strTitle;
	strTitle.Format(_T("Max V(%s)"),CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()));
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strTitle.GetBuffer();
	strTitle.ReleaseBuffer();
	lvc.cx = 140;
	m_wndListCtrl.InsertColumn(1,&lvc);

	SetListContent();
}

void CDlgLandsideSpeedConstraint::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	StretchSpeedConstraint* pStretchSpeedConstraint = (StretchSpeedConstraint*)m_wndListBox.GetItemData(nSel);
	if (pStretchSpeedConstraint == NULL)
		return;
	
	/*const CVehicleItemDetailList& vehicleTypeList = m_pLandsideInput->getOperationVehicleList();
	int nCount = pStretchSpeedConstraint->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		StretchSpeedConstraintData* pConstraintData = pStretchSpeedConstraint->GetItem(i);
		CVehicleItemDetail* pVehicleItem = vehicleTypeList.GetVehicleItemByID(pConstraintData->GetVehicleType());
		if (pVehicleItem  == NULL)
			continue;
		
		m_wndListCtrl.InsertItem(i,pVehicleItem->getName());
		CString strSpeed;
		double dSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),pConstraintData->GetMaxSpeed());
		strSpeed.Format(_T("%.2f"),dSpeed);
		m_wndListCtrl.SetItemText(i,1,strSpeed);
		m_wndListCtrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pConstraintData);
	}*/
}

void CDlgLandsideSpeedConstraint::InitToolBar()
{
	CRect rcToolbar;
	m_wndListBox.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 2;
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_wndStretchToolBar.MoveWindow(rcToolbar);

	m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_NEW_STRETCH);
	m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_DEL_STRETCH, FALSE);

	m_wndListCtrl.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 4;
	m_wndConstraintToolBar.MoveWindow(&rcToolbar);

	m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA, FALSE);
	m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA, FALSE);
}

void CDlgLandsideSpeedConstraint::CreateComboxString()
{
	if (m_pLandsideInput == NULL)
		return;
	
	m_strListNode.RemoveAll();

	//const CVehicleItemDetailList& vehicleTypeList = m_pLandsideInput->getOperationVehicleList();
	//unsigned nVehicleSpeCount = vehicleTypeList.GetElementCount();
	//for(unsigned i = 0; i < nVehicleSpeCount; i++) 
	//{
	//	CVehicleItemDetail* pVehicleItem = vehicleTypeList.GetItem(i);
	//	if (pVehicleItem == NULL)
	//		continue;
	//	
	//	CString strVehicleType = pVehicleItem->getName();
	//	m_strListNode.AddTail(strVehicleType);
	//}
}

void CDlgLandsideSpeedConstraint::OnOK()
{
	if (m_pStretchSpeedConstraintList == NULL)
		return;
	
	try
	{
		CADODatabase::BeginTransaction();
		m_pStretchSpeedConstraintList->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CXTResizeDialog::OnOK();
}

void CDlgLandsideSpeedConstraint::OnLvnEndlabeleditListData( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int nCurSel = pDispInfo->item.iItem;
	if (nCurSel == LB_ERR)
		return;
	
	StretchSpeedConstraintData* pCurData = (StretchSpeedConstraintData*)m_wndListCtrl.GetItemData(nCurSel);
	if (pDispInfo->item.iSubItem == 1)
	{
		double dMaxSpeed = atof(pDispInfo->item.pszText);
		dMaxSpeed = CARCVelocityUnit::ConvertVelocity(CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,dMaxSpeed);
		pCurData->SetMaxSpeed(dMaxSpeed);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

LRESULT CDlgLandsideSpeedConstraint::OnMsgComboChange( WPARAM wParam, LPARAM lParam )
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	if (nCurSel == LB_ERR)
		return 0;
	
	int nCurSubSel = dispinfo->item.iSubItem;	
	if (nCurSubSel != 0)
		return 0;
	
	/*const CVehicleItemDetailList& vehicleTypeList = m_pLandsideInput->getOperationVehicleList();
	CVehicleItemDetail* pVehicleItem = vehicleTypeList.GetItem(nComboxSel);
	if (pVehicleItem == NULL)
		return 0;*/
	
	/*StretchSpeedConstraintData* pCurData = (StretchSpeedConstraintData*)m_wndListCtrl.GetItemData(nCurSel);
	pCurData->SetVechileType(pVehicleItem->GetID());*/
	
	return 0;
}

bool CDlgLandsideSpeedConstraint::ConvertUnitFromDBtoGUI( void )
{
	return (false);
}

bool CDlgLandsideSpeedConstraint::RefreshGUI( void )
{
	InitListCtrl();
	return (true);
}

bool CDlgLandsideSpeedConstraint::ConvertUnitFromGUItoDB( void )
{
	return (false);
}

void CDlgLandsideSpeedConstraint::OnLbnSelchangeListStretch()
{
	SetListContent();
	UpdateStretchToolbar();
	UpdateConstraintToolbar();
}

void CDlgLandsideSpeedConstraint::UpdateStretchToolbar()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_NEW_STRETCH,TRUE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_STRETCH,TRUE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_DEL_STRETCH,TRUE);
	
		m_wndConstraintToolBar.EnableWindow(TRUE);
		m_wndListCtrl.EnableWindow(TRUE);
	}
	else
	{
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_NEW_STRETCH,TRUE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_STRETCH,FALSE);
		m_wndStretchToolBar.GetToolBarCtrl().EnableButton(ID_DEL_STRETCH,FALSE);

		m_wndConstraintToolBar.EnableWindow(FALSE);
		m_wndListCtrl.EnableWindow(FALSE);
	}

}

void CDlgLandsideSpeedConstraint::UpdateConstraintToolbar()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA,FALSE);
		m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA,FALSE);
	}
	else
	{
		int nDataSel = m_wndListCtrl.GetCurSel();
		if (nDataSel == LB_ERR)
		{
			m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA,TRUE);
			m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA,FALSE);
		}
		else
		{
			m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA,TRUE);
			m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA,TRUE);
		}
	}
}

void CDlgLandsideSpeedConstraint::OnLvnItemchangedListConstraint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA,TRUE);
		m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA,FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA,TRUE);
			m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA,TRUE);
		m_wndConstraintToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA,TRUE);
	}
	else
		ASSERT(0);

}

void CDlgLandsideSpeedConstraint::OnBnClickedSave()
{
	if (m_pStretchSpeedConstraintList == NULL)
		return;

	try
	{
		CADODatabase::BeginTransaction();
		m_pStretchSpeedConstraintList->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}
