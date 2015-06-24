// DlgVehicleLandSideSpecification.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgVehicleLandSideSpecification.h"
#include "..\Engine\terminal.h"
#include "Resource.h"
#include "../Common/WinMsg.h"
#include "Landside\InputLandside.h"
#include "../Landside/VehicleLandSideDefine.h"
#include "../Landside/VehicleDistribtionList.h"
// CDlgVehicleLandSideSpecification dialog

IMPLEMENT_DYNAMIC(CDlgVehicleLandSideSpecification, CXTResizeDialog)
CDlgVehicleLandSideSpecification::CDlgVehicleLandSideSpecification(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgVehicleLandSideSpecification::IDD, pParent)
{
}

CDlgVehicleLandSideSpecification::CDlgVehicleLandSideSpecification(int nProjID,Terminal *VehicleTerminal,CTermPlanDoc *tmpDoc,CWnd *pParent)
:CXTResizeDialog(CDlgVehicleLandSideSpecification::IDD,pParent)
,m_pTerminal(VehicleTerminal)
{
	m_nProjID=nProjID;
	pDoc=tmpDoc;
	m_isDelete=FALSE;
}
CDlgVehicleLandSideSpecification::~CDlgVehicleLandSideSpecification()
{
}

void CDlgVehicleLandSideSpecification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_lstCtrl);
}
int CDlgVehicleLandSideSpecification::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
return 0;
	
}
BOOL CDlgVehicleLandSideSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	if(!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_wndToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME))
	{
		TRACE(_T("Create tool bar error "));
		return FALSE;
	}
	
	InitToolBar();
	InitListCtrl();
	m_pLandSide=pDoc->getARCport()->m_pInputLandside;
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
}
void CDlgVehicleLandSideSpecification::OnEditItem()
{
	int nItem = GetSelectedListItem();
	m_lstCtrl.EditSubLabel(nItem, 0);
	UpdateToolBar();
}
void CDlgVehicleLandSideSpecification::OnNewItem()
{
	CVehicleItemDetail* pNewItem = new CVehicleItemDetail;
	m_pLandSide->getOperationVehicleList().AddNewItem(pNewItem);
	int nIndex = m_lstCtrl.GetItemCount();
	CString strValue;
	strValue.Format("%s%d",pNewItem->getName(),nIndex+1);
	m_lstCtrl.InsertItem(nIndex, strValue);

	enumVehicleLandSideType enumType = pNewItem->getLandSideBaseType();
	m_lstCtrl.SetItemText(nIndex, 1, LandSideVehicleTypeName[enumType - TypeBaseValue]);	

	strValue.Format("%.2f",pNewItem->getLength());
	m_lstCtrl.SetItemText(nIndex, 2, strValue);
	strValue.Format("%.2f",pNewItem->getWidth());
	m_lstCtrl.SetItemText(nIndex, 3, strValue);
	strValue.Format("%.2f",pNewItem->getHeight());
	m_lstCtrl.SetItemText(nIndex, 4, strValue);
	strValue.Format("%.2f",pNewItem->getAcceleration());
	m_lstCtrl.SetItemText(nIndex, 5, strValue);
	strValue.Format("%.2f",pNewItem->getMaxSpeed());
	m_lstCtrl.SetItemText(nIndex, 6, strValue);
	strValue.Format("%.2f",pNewItem->getDecelration());
	m_lstCtrl.SetItemText(nIndex, 7, strValue);
	strValue.Format("%.2f",pNewItem->getCapacity());
	m_lstCtrl.SetItemText(nIndex, 8, strValue);

	enumLandSideVehicleUnit enumUnit = pNewItem->getUnit();
	m_lstCtrl.SetItemText(nIndex, 9, VehicleUnitName[enumUnit]);

	strValue.Format("%.2f",pNewItem->GetFuelCOnsumed());
	m_lstCtrl.SetItemText(nIndex, 10, strValue);

	m_lstCtrl.EditSubLabel(nIndex,0);
	UpdateToolBar();
}
void CDlgVehicleLandSideSpecification::SetListContent()
{
	size_t nVehicleCount = m_pLandSide->getOperationVehicleList().GetElementCount();

	CString strValue;
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleItemDetail* pItem = m_pLandSide->getOperationVehicleList().GetItem(i);

		m_lstCtrl.InsertItem((int)i, pItem->getName());

		enumVehicleLandSideType enumType = pItem->getLandSideBaseType();
		m_lstCtrl.SetItemText((int)i, 1, LandSideVehicleTypeName[enumType - TypeBaseValue]);

		strValue.Format("%.2f",pItem->getLength());
		m_lstCtrl.SetItemText((int)i, 2, strValue);
		strValue.Format("%.2f",pItem->getWidth());
		m_lstCtrl.SetItemText((int)i, 3, strValue);
		strValue.Format("%.2f",pItem->getHeight());
		m_lstCtrl.SetItemText((int)i, 4, strValue);
		strValue.Format("%.2f",pItem->getAcceleration());
		m_lstCtrl.SetItemText((int)i, 5, strValue);
		strValue.Format("%.2f",pItem->getMaxSpeed());
		m_lstCtrl.SetItemText((int)i, 6, strValue);
		strValue.Format("%.2f",pItem->getDecelration());
		m_lstCtrl.SetItemText((int)i, 7, strValue);
		strValue.Format("%.2f",pItem->getCapacity());
		m_lstCtrl.SetItemText((int)i, 8, strValue);

		enumLandSideVehicleUnit enumUnit = pItem->getUnit();
		m_lstCtrl.SetItemText((int)i, 9, VehicleUnitName[enumUnit]);

		strValue.Format("%.2f",pItem->GetFuelCOnsumed());
		m_lstCtrl.SetItemText((int)i, 10, strValue);
	}
}
void CDlgVehicleLandSideSpecification::InitListCtrl()
{
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

	for (int nType = VehiclType_Private; nType < VehicleType_LandSideCount; ++ nType)
	{
		strList.InsertAfter(strList.GetTailPosition(),LandSideVehicleTypeName[nType - TypeBaseValue]);
	}
	lvc.csList = &strList;
	lvc.cx = 110;
	lvc.pszText = _T("Operation Type");
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
void CDlgVehicleLandSideSpecification::InitToolBar()
{
	CRect rcDlg;
	GetWindowRect(&rcDlg);
	ScreenToClient(&rcDlg);

	CRect rcToolbar(12, 20,rcDlg.right-10,45);
	m_wndToolBar.MoveWindow(rcToolbar);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
	m_wndToolBar.ShowWindow(SW_SHOW);
}
void CDlgVehicleLandSideSpecification::UpdateToolBar()
{
	BOOL bEnable = TRUE;
	POSITION pos = m_lstCtrl.GetFirstSelectedItemPosition();
	bEnable = (pos == NULL ? FALSE : TRUE);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT, bEnable);
}
void CDlgVehicleLandSideSpecification::OnDelItem()
{
	int nItem = GetSelectedListItem();
	m_lstCtrl.DeleteItemEx(nItem);
	m_pLandSide->getOperationVehicleList().DeleteItem(nItem);
	
	UpdateToolBar();
	m_isDelete=TRUE;
}
int CDlgVehicleLandSideSpecification::GetSelectedListItem()
{
	POSITION pos = m_lstCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_lstCtrl.GetNextSelectedItem(pos);

	return -1;
}
void CDlgVehicleLandSideSpecification::OnCancel()
{
	m_pLandSide->getOperationVehicleList().ReadDataOperationType(m_nProjID);
	CXTResizeDialog::OnCancel();
}
void CDlgVehicleLandSideSpecification::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_lstCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0|| nItem>=m_lstCtrl.GetItemCount())
		return;

	OnListItemChanged(nItem, nSubItem);
	*pResult = 0;
}
void CDlgVehicleLandSideSpecification::OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBar();
	*pResult = 0;
}
void CDlgVehicleLandSideSpecification::OnBnClickedSave()
{
	
	/*int mTotal=(int)pDoc->getARCport()->m_pInputLandside->getOperationVehicleList().GetElementCount();
	if(mTotal>0 || pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItemCount()>0)
	{
	
		if(m_isDelete&&::AfxMessageBox("The delete datas have been used by others,Do you want to save£¿",MB_YESNO)==IDYES)
		{*/
			
			m_pLandSide->getOperationVehicleList().SaveData(m_nProjID,m_pTerminal->m_pAirportDB);
			if (m_pLandSide->getOperationVehicleList().GetElementCount()<=0)
			{
				m_pLandSide->getOperationVehicleList().ReadDataOperationType(m_nProjID);
			}
			pDoc->getARCport()->m_pInputLandside->getLandSideVehicleProbDisp().setVehicleItemList(&m_pLandSide->getOperationVehicleList());
			pDoc->getARCport()->m_pInputLandside->getLandSideVehicleProbDisp().ReadData();
			pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().readData(m_nProjID);
			
			m_isDelete=FALSE;
		//}
	//}
	
	CXTResizeDialog::OnOK();
}
void CDlgVehicleLandSideSpecification::OnListItemChanged(int nItem, int nSubItem)
{
	CVehicleItemDetail* pItem = m_pLandSide->getOperationVehicleList().GetItem(nItem);
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
void CDlgVehicleLandSideSpecification::OnOK()
{
	
	m_pLandSide->getOperationVehicleList().SaveData(m_nProjID,m_pTerminal->m_pAirportDB);
	if (m_pLandSide->getOperationVehicleList().GetElementCount()<=0)
	{
		m_pLandSide->getOperationVehicleList().ReadDataOperationType(m_nProjID);
	}
	pDoc->getARCport()->m_pInputLandside->getLandSideVehicleProbDisp().setVehicleItemList(&m_pLandSide->getOperationVehicleList());
	pDoc->getARCport()->m_pInputLandside->getLandSideVehicleProbDisp().ReadData();
	pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().readData(m_nProjID);
	
	//pDoc->getARCport()->m_pInputLandside->getOperationVehicleList().ReadDataOperationType(m_nProjID);
	CXTResizeDialog::OnOK();
}
LRESULT CDlgVehicleLandSideSpecification::OnChangeVehicleType(WPARAM wParam, LPARAM lParam)
{
	LRESULT pResult = 0;

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;

	int nItem = plvItem->iSubItem;

	int nCurSel = m_lstCtrl.GetCurSel();
	if (nCurSel == LB_ERR)
		return FALSE;

	CVehicleItemDetail* pItem = m_pLandSide->getOperationVehicleList().GetItem(nCurSel);
	if (pItem == NULL)
		return TRUE;

	if (nItem == 1)
	{
		int nSelType = (int)wParam;
		//enumVehicleLandSideType tempType=pItem->getLandSideBaseType();
		/*if (pItem->getLandSideBaseType()!= (enumVehicleLandSideType)(nSelType+TypeBaseValue))
		{
			pItem->DeleteOperationVehicleTypeDB();
		}*/
		if (nSelType >= 0 && nSelType+TypeBaseValue < VehicleType_LandSideCount)
		{
			pItem->setLandSideBaseType((enumVehicleLandSideType)(nSelType+TypeBaseValue));
		
		}
		else
		{
			pItem->setLandSideBaseType(VehiclType_Private);
		}
		
	}
	else
	{
		int nSelUnit = (int)wParam;
		if (pItem)
		{
			pItem->setUnit((enumLandSideVehicleUnit)nSelUnit);
		}
	}


	return TRUE;
}
BEGIN_MESSAGE_MAP(CDlgVehicleLandSideSpecification, CXTResizeDialog)
	
	
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD, OnNewItem)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL, OnDelItem)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT, OnEditItem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListContents)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnChangeVehicleType)
END_MESSAGE_MAP()


// CDlgVehicleLandSideSpecification message handlers
