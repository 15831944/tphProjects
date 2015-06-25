// DlgVehicleSpecification.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "resource.h"
#include "DlgVehicleSpecification.h"
#include ".\dlgvehiclespecification.h"
#include "../Common/WinMsg.h"
#include "../Database/DBElementCollection_Impl.h"
// CDlgVehicleSpecification dialog

IMPLEMENT_DYNAMIC(CDlgVehicleSpecification, CXTResizeDialog)

CDlgVehicleSpecification::CDlgVehicleSpecification(int nProjID,CWnd* pParent /*=NULL*/)
	//: CXTResizeDialog(CDlgVehicleSpecification::IDD, pParent)
	: CXTResizeDialog(IDD_DIALOG_VEHICLESPECIFICATION, pParent),
	m_nProjID(nProjID)
{
}

CDlgVehicleSpecification::~CDlgVehicleSpecification()
{
}

void CDlgVehicleSpecification::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_lstCtrl);
}


BEGIN_MESSAGE_MAP(CDlgVehicleSpecification, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_VECHILE_ADD, OnNewItem)
	ON_COMMAND(ID_TOOLBAR_VECHILE_DEL, OnDelItem)
	ON_COMMAND(ID_TOOLBAR_VERCHILE_EDIT, OnEditItem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListContents)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnChangeVehicleType)
	ON_MESSAGE(WM_COLLUM_INDEX,OnPreEditListCtrlItem)
END_MESSAGE_MAP()


// CDlgVehicleSpecification message handlers
int CDlgVehicleSpecification::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_VEHICLESPECIFICATION))
	{
		TRACE(_T("Create tool bar error "));
		return -1;
	}
	return 0;
}
BOOL CDlgVehicleSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	InitToolBar();
	InitListCtrl();
	m_vehicleSpecifications.ReadData(m_nProjID);
	SetListContent();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_FRAME, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);	
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgVehicleSpecification::InitToolBar()
{
	CRect rcDlg;
	GetWindowRect(&rcDlg);
	ScreenToClient(&rcDlg);

	CRect rcToolbar(12, 23,rcDlg.right-10,40);
	m_wndToolBar.MoveWindow(rcToolbar);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_VECHILE_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_VECHILE_DEL,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_VERCHILE_EDIT,FALSE);

}
void CDlgVehicleSpecification::UpdateToolBar()
{
	BOOL bEnable = TRUE;
	POSITION pos = m_lstCtrl.GetFirstSelectedItemPosition();
	bEnable = (pos == NULL ? FALSE : TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_VECHILE_DEL, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_VERCHILE_EDIT, bEnable);

}
void CDlgVehicleSpecification::InitListCtrl()
{	
	// set list control window style
	DWORD dwStyle = m_lstCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstCtrl.SetExtendedStyle(dwStyle);


	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCFMT_CENTER;
	lvc.pszText = _T("Name");
	lvc.cx = 108;
	lvc.fmt = LVCFMT_EDIT;
	lvc.csList = &strList;
	m_lstCtrl.InsertColumn(0, &lvc);

	for (int nType = VehicleType_General; nType < VehicleType_Count; ++ nType)
	{
		strList.InsertAfter(strList.GetTailPosition(),VehicleTypeName[nType - TypeBaseValue]);
	}
	lvc.csList = &strList;
	lvc.cx = 110;
	lvc.pszText = _T("Logic");
	lvc.fmt = LVCFMT_DROPDOWN;
	m_lstCtrl.InsertColumn(1, &lvc);


	lvc.cx = 62;
	lvc.pszText = _T("Length(m)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("Width(m)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(3, &lvc);

	lvc.pszText = _T("Height(m)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(4, &lvc);

	lvc.cx = 103;

	lvc.pszText = _T("Acceleration(m/s2)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(5, &lvc);

	lvc.pszText = _T("Max Speed(km/hr)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(6, &lvc);

	lvc.pszText = _T("Deceleration(m/s2)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(7, &lvc);

	lvc.pszText = _T("Capacity");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(8, &lvc);

	strList.RemoveAll();
	for (int nUnit = 0; nUnit < VehicleUnit_Count; ++ nUnit)
	{
		strList.InsertAfter(strList.GetTailPosition(),VehicleUnitName[nUnit]);
	}
	lvc.csList = &strList;
	lvc.cx = 100;
	lvc.pszText = _T("Unit");
	lvc.fmt = LVCFMT_DROPDOWN;
	m_lstCtrl.InsertColumn(9, &lvc);

	lvc.pszText = _T("Fuel Consumed(liter/km)");
	lvc.fmt = LVCFMT_NUMBER;
	m_lstCtrl.InsertColumn(10, &lvc);
}

void CDlgVehicleSpecification::SetListContent()
{
	size_t nVehicleCount = m_vehicleSpecifications.GetElementCount();

	
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetItem(i);
		m_lstCtrl.InsertItem((int)i, pItem->getName());
		EditItemText(i);
	}
}
void CDlgVehicleSpecification::OnNewItem()
{
	CVehicleSpecificationItem* pNewItem = new CVehicleSpecificationItem;
	m_vehicleSpecifications.AddNewItem(pNewItem);
	int nIndex = m_lstCtrl.GetItemCount();
	CString strValue;
	strValue.Format("%s%d",pNewItem->getName(),nIndex+1);
	m_lstCtrl.InsertItem(nIndex, strValue);
	
	EditItemText(nIndex);

	m_lstCtrl.EditSubLabel(nIndex,0);
	UpdateToolBar();
}
void CDlgVehicleSpecification::OnDelItem()
{
	int nItem = GetSelectedListItem();
	m_lstCtrl.DeleteItemEx(nItem);
	m_vehicleSpecifications.DeleteItem(nItem);
	UpdateToolBar();
}
void CDlgVehicleSpecification::OnEditItem()
{
	int nItem = GetSelectedListItem();
	m_lstCtrl.EditSubLabel(nItem, 0);
	UpdateToolBar();
}
int CDlgVehicleSpecification::GetSelectedListItem()
{
	POSITION pos = m_lstCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_lstCtrl.GetNextSelectedItem(pos);

	return -1;
}
void CDlgVehicleSpecification::OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBar();
	*pResult = 0;
}
void CDlgVehicleSpecification::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_lstCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0 || nItem>=m_lstCtrl.GetItemCount())
		return;

	OnListItemChanged(nItem, nSubItem);
	*pResult = 0;
}
void CDlgVehicleSpecification::OnListItemChanged(int nItem, int nSubItem)
{
	CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetItem(nItem);
	ASSERT(pItem);
	CString strValue = m_lstCtrl.GetItemText(nItem,nSubItem);
	double dValue = atof(strValue);
	if(nSubItem == 0)
		pItem->setName(strValue);
	
	if (nSubItem == 2)
		pItem->setLength(dValue);
	else if(nSubItem == 3)
		pItem->setWidth(dValue);
	else if(nSubItem == 4)
		pItem->setHeight(dValue);
	else if(nSubItem == 5)
		pItem->setAcceleration(dValue);
	else if(nSubItem == 6)
		pItem->setMaxSpeed(dValue);
	else if(nSubItem == 7)
		pItem->setDeceleration(dValue);
	else if(nSubItem == 8)
		pItem->setCapacity(dValue);
	else if(nSubItem == 10)
		pItem->SetFuelConsumed(dValue);
}
void CDlgVehicleSpecification::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_vehicleSpecifications.SaveData(m_nProjID);
	CXTResizeDialog::OnOK();
}
void CDlgVehicleSpecification::OnBnClickedSave()
{
	m_vehicleSpecifications.SaveData(m_nProjID);
	CXTResizeDialog::OnOK();
}
LRESULT CDlgVehicleSpecification::OnChangeVehicleType( WPARAM wParam, LPARAM lParam)
{
	LRESULT pResult = 0;

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
	
	int nItem = plvItem->iSubItem;

	int nCurSel = m_lstCtrl.GetCurSel();
	if (nCurSel == LB_ERR)
		return FALSE;

	CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetItem(nCurSel);
	if (pItem == NULL)
		return TRUE;

	if (nItem == 1)
	{
		int nSelType = (int)wParam;
		if (nSelType >= 0 && nSelType+TypeBaseValue < VehicleType_Count)
		{
			pItem->setBaseType((enumVehicleBaseType)(nSelType+TypeBaseValue));
			EditItemText(nCurSel);
		}
		else
		{
			pItem->setBaseType(VehicleType_General);
		}
	}
	else
	{
		int nSelUnit = (int)wParam;
		if (pItem)
		{
			pItem->setUnit((enumVehicleUnit)nSelUnit);
		}
	}



	return TRUE;

}

LRESULT CDlgVehicleSpecification::OnPreEditListCtrlItem( WPARAM wParam, LPARAM lParam )
{
	int index = (int)wParam;
	int column = int(lParam);
	CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetItem(index);
	LVCOLDROPLIST* pToChange =  m_lstCtrl.GetColumnStyle(column);
	if(pItem->getBaseType() == VehicleType_JointBagTug && (column==5 || column==6 || column==7 || column==10))
	{
		pToChange->Style = NO_EDIT;
	}
	else
	{
		if(column == 1 || column == 9)
		{
			pToChange->Style = DROP_DOWN;
		}
		else
		{
			pToChange->Style = NUMBER_ONLY;
		}
	}
	return TRUE;
}

void CDlgVehicleSpecification::EditItemText(size_t index )
{
	CString strValue;
	CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetItem(index);
	enumVehicleBaseType enumType = pItem->getBaseType();
	m_lstCtrl.SetItemText((int)index, 1, VehicleTypeName[enumType - TypeBaseValue]);

	strValue.Format("%.2f",pItem->getLength());
	m_lstCtrl.SetItemText((int)index, 2, strValue);
	strValue.Format("%.2f",pItem->getWidth());
	m_lstCtrl.SetItemText((int)index, 3, strValue);
	strValue.Format("%.2f",pItem->getHeight());
	m_lstCtrl.SetItemText((int)index, 4, strValue);
	if(pItem->getBaseType() == VehicleType_JointBagTug)
	{
		m_lstCtrl.SetItemText((int)index, 5,"N/A");
		m_lstCtrl.SetItemText((int)index, 6,"N/A");
		m_lstCtrl.SetItemText((int)index, 7,"N/A");
		m_lstCtrl.SetItemText((int)index, 10,"N/A");
	}
	else
	{
		strValue.Format("%.2f",pItem->getAcceleration());
		m_lstCtrl.SetItemText((int)index, 5, strValue);
		strValue.Format("%.2f",pItem->getMaxSpeed());
		m_lstCtrl.SetItemText((int)index, 6, strValue);
		strValue.Format("%.2f",pItem->getDecelration());
		m_lstCtrl.SetItemText((int)index, 7, strValue);

		strValue.Format("%.2f",pItem->GetFuelCOnsumed());
		m_lstCtrl.SetItemText((int)index, 10, strValue);
	}

	strValue.Format("%.2f",pItem->getCapacity());
	m_lstCtrl.SetItemText((int)index, 8, strValue);

	enumVehicleUnit enumUnit = pItem->getUnit();
	m_lstCtrl.SetItemText((int)index, 9, VehicleUnitName[enumUnit]);
}
