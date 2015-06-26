// DlgVehiclePoolsAndDeployment.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgVehiclePoolsAndDeployment.h"
#include "..\InputAirside\InputAirside.h"
#include ".\dlgvehiclepoolsanddeployment.h"
#include "DlgStandFamily.h"
#include "DlgTimeRange.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"
#include "..\InputAirside\ALTAirport.h"
//#include "..\common\ProbabilityDistribution.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "../InputAirside/FollowMeCarServiceMeetingPoints.h"
#include "DlgSelectALTObject.h"
#include "Common/ProDistrubutionData.h"

// CDlgVehiclePoolsAndDeployment dialog
static const UINT ID_NEW_POOL = 10;
static const UINT ID_DEL_POOL = 11;

static const UINT ID_NEW_ATTRIBUTE = 20;
static const UINT ID_DEL_ATTRIBUTE = 21;
static const UINT ID_EDIT_ATTRIBUTE = 22;


IMPLEMENT_DYNAMIC(CDlgVehiclePoolsAndDeployment, CXTResizeDialog)
CDlgVehiclePoolsAndDeployment::CDlgVehiclePoolsAndDeployment(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgVehiclePoolsAndDeployment::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
	,m_eSelVehicleType(VehicleType_General)
{
}

CDlgVehiclePoolsAndDeployment::~CDlgVehiclePoolsAndDeployment()
{
}

void CDlgVehiclePoolsAndDeployment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX_VEHICLETYPE, m_wndListBoxVehicleType);
	DDX_Control(pDX, IDC_LISTBOX_POOLS, m_wndListBoxPools);
	DDX_Control(pDX, IDC_TREE_ATTRIBUTES, m_wndTreeAttributes);
}


BEGIN_MESSAGE_MAP(CDlgVehiclePoolsAndDeployment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND(ID_NEW_POOL, OnNewPool)
	ON_COMMAND(ID_DEL_POOL, OnDelPool)
	ON_COMMAND(ID_NEW_ATTRIBUTE, OnNewAttribute)
	ON_COMMAND(ID_DEL_ATTRIBUTE, OnDelAttribute)
	ON_COMMAND(ID_EDIT_ATTRIBUTE, OnEditAttribute)
	ON_COMMAND(ID_NEW_MENU, OnNewAttribute)
	ON_COMMAND(ID_DEL_MENU, OnDelAttribute)
	ON_COMMAND(ID_EDIT_MENU, OnEditAttribute)
	ON_LBN_SELCHANGE(IDC_LISTBOX_VEHICLETYPE, OnLbnSelchangeListboxVehicletype)
	ON_LBN_SELCHANGE(IDC_LISTBOX_POOLS, OnLbnSelchangeListboxPools)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ATTRIBUTES, OnTvnSelchangedTreeAttributes)
	ON_MESSAGE(WM_LBITEMCHANGED,OnListboxPoolsKillFocus)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgVehiclePoolsAndDeployment message handlers

int CDlgVehiclePoolsAndDeployment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndToolbarPools.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbarPools.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	if (!m_wndToolbarAttributes.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),m_wndToolbarPools.GetDlgCtrlID()+1)
		|| !m_wndToolbarAttributes.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& ToolbarCtrlPools = m_wndToolbarPools.GetToolBarCtrl();
	ToolbarCtrlPools.SetCmdID(0, ID_NEW_POOL);
	ToolbarCtrlPools.SetCmdID(1, ID_DEL_POOL);

	CToolBarCtrl& ToolbarCtrlAttributes = m_wndToolbarAttributes.GetToolBarCtrl();
	ToolbarCtrlAttributes.SetCmdID(0, ID_NEW_ATTRIBUTE);
	ToolbarCtrlAttributes.SetCmdID(1, ID_DEL_ATTRIBUTE);
	ToolbarCtrlAttributes.SetCmdID(2, ID_EDIT_ATTRIBUTE);

	return 0;
}

void CDlgVehiclePoolsAndDeployment::InitToolBar()
{
	CRect rcToolbar;

	// set the position of the fltType toolbar	
	m_wndListBoxPools.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 23;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 4;
	m_wndToolbarPools.MoveWindow(rcToolbar);

	m_wndToolbarPools.GetToolBarCtrl().EnableButton(ID_NEW_POOL);
	m_wndToolbarPools.GetToolBarCtrl().EnableButton(ID_DEL_POOL,FALSE);


	// set the position of the Priority toolbar	
	m_wndTreeAttributes.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 23;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 4;
	m_wndToolbarAttributes.MoveWindow(rcToolbar);

	m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_NEW_ATTRIBUTE,FALSE);
	m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_DEL_ATTRIBUTE,FALSE);
	m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_EDIT_ATTRIBUTE,FALSE);
}

BOOL CDlgVehiclePoolsAndDeployment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolBar();
	m_wndListBoxVehicleType.SetHorizontalExtent(120);
	m_vehiclePoolsAndDeployment.SetAirportDB(m_pInputAirside->m_pAirportDB);
	m_vehiclePoolsAndDeployment.ReadData(m_nProjID);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		std::vector<ALTObject> vParkingLot;	
		ALTObject::GetObjectList(ALT_VEHICLEPOOLPARKING,*iterAirportID,vParkingLot);
		std::vector<ALTObject>::iterator iter = vParkingLot.begin();
		for (; iter != vParkingLot.end(); ++iter)
		{
			m_vParkingLot.push_back(*iter);	
		}
	}
	ALTObjectID objName;
	for(int i=0;i<4;i++)
	{
		objName.at(i) = _T("");
	}
	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(ALT_STAND);
	altObjGroup.setName(objName);

	try
	{
		CADODatabase::BeginTransaction();
		m_nDefaultStandFamilyID = altObjGroup.InsertData(m_nProjID);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}
	SetListBoxVehicleType();

	SetResize(m_wndToolbarPools.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(m_wndToolbarAttributes.GetDlgCtrlID(), SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);

	SetResize(IDC_STATIC_LANDINGRUNWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	//SetResize(IDC_STATIC_POOLS, SZ_TOP_CENTER , SZ_TOP_CENTER);
	//SetResize(IDC_STATIC_ATTRIBUTES, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	
	SetResize(IDC_LISTBOX_VEHICLETYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LISTBOX_POOLS, SZ_TOP_CENTER , SZ_MIDDLE_RIGHT);
	SetResize(IDC_GROUPBOX_POOLS, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_ATTRIBUTES, SZ_MIDDLE_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_GROUPBOX_ATTRIBUTES, SZ_MIDDLE_CENTER , SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVehiclePoolsAndDeployment::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	//if(CheckInputData())
	//{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_vehiclePoolsAndDeployment.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation();
	}
		
		CXTResizeDialog::OnOK();
	//}
}

void CDlgVehiclePoolsAndDeployment::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	//if(CheckInputData())
	//{
		m_vehiclePoolsAndDeployment.SaveData(m_nProjID);
		//CXTResizeDialog::OnOK();
	//}
}

bool CDlgVehiclePoolsAndDeployment::CheckInputData()
{
	for(int i=0; i<m_vehiclePoolsAndDeployment.GetVehicleTypePoolsCount(); i++)
	{
		CVehicleTypePools* pItem = m_vehiclePoolsAndDeployment.GetVehicleTypePoolsItem(i);
		int nCount = pItem->GetVehiclePoolCount();
		if(nCount == 0)
		{
			AfxMessageBox("Please define at least one pool for each of these.");
			return 0;
		}
	}
	return 1;
}

void CDlgVehiclePoolsAndDeployment::UpdateToolBar()
{
	int nSel = m_wndListBoxPools.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolbarPools.GetToolBarCtrl().EnableButton(ID_DEL_POOL,TRUE);
		HTREEITEM hSelTreeItem = m_wndTreeAttributes.GetSelectedItem();
		CString strSel = m_wndTreeAttributes.GetItemText(hSelTreeItem);
		strSel = strSel.Left(2);
		if(hSelTreeItem == m_hTreeItemDeployment || strSel == "St" || strSel == "Fl")
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_NEW_ATTRIBUTE,TRUE);
		else
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_NEW_ATTRIBUTE,FALSE);
		if(strSel == "St" || strSel == "Fl" || strSel == "Ti")
		{
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_DEL_ATTRIBUTE,TRUE);
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_EDIT_ATTRIBUTE,TRUE);
		}
		else
		{
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_DEL_ATTRIBUTE,FALSE);
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_EDIT_ATTRIBUTE,FALSE);
		}
		HTREEITEM hParItem = m_wndTreeAttributes.GetParentItem(hSelTreeItem);
		if (hParItem && hParItem == m_hTreeItemDeployment)
		{
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_DEL_ATTRIBUTE,TRUE);
			m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_EDIT_ATTRIBUTE,FALSE);		
		}
	}
	else
	{
		m_wndToolbarPools.GetToolBarCtrl().EnableButton(ID_DEL_POOL,FALSE);
		m_wndToolbarAttributes.GetToolBarCtrl().EnableButton(ID_NEW_ATTRIBUTE,FALSE);
	}
}

void CDlgVehiclePoolsAndDeployment::SetListBoxVehicleType()
{
	for(int i=0; i<m_vehiclePoolsAndDeployment.GetVehicleTypePoolsCount(); i++)
	{
		CVehicleTypePools* pItem = m_vehiclePoolsAndDeployment.GetVehicleTypePoolsItem(i);
		CString strVehicleTypeName = pItem->GetVehicleTypeName();
		int nIndex = m_wndListBoxVehicleType.AddString(strVehicleTypeName);
		m_wndListBoxVehicleType.SetItemData(nIndex,(DWORD_PTR)pItem);
	}
	m_wndListBoxVehicleType.SetCurSel(0);
	OnLbnSelchangeListboxVehicletype();
}

void CDlgVehiclePoolsAndDeployment::OnNewPool()
{
	int  nvParkingLotSize = (int)m_vParkingLot.size();
	if(nvParkingLotSize == 0)
	{
		AfxMessageBox("There is no PoolParking.");
		return;
	}

	CVehicleTypePools* pItem = GetCurSelVehicleTypePools();
	if(!pItem)
		return;

	bool bFollowMeCar = pItem->IsFollowMeCarPoolsData();

	CVehiclePool* pNewItem = new CVehiclePool(bFollowMeCar);
	pItem->AddVehiclePoolItem(pNewItem);

	int nCount = m_wndListBoxPools.GetCount();
	CString strPoolName;
	strPoolName.Format("New Service area %d", nCount + 1);

	int nItem = m_wndListBoxPools.AddString(strPoolName);
	m_wndListBoxPools.SetItemData(nItem, (DWORD_PTR)pNewItem);
	m_wndListBoxPools.SetCurSel(nItem);	
	m_wndListBoxPools.EditItem(nItem);
	pNewItem->SetPoolName(strPoolName);
	if(nItem >  nvParkingLotSize-1)
		nItem = nvParkingLotSize-1;
	pNewItem->SetParkingLotID(m_vParkingLot[nItem].getID());

	if (bFollowMeCar)
	{
		LoadFollowMeCarTree(pNewItem);
		return;
	}
	else
	{
		if (pItem->IsTowTruckPoolsData())
		{
			LoadTowTruckTree(pNewItem);
			return;
		}
	}

	CVehicleServiceStandFamily* pStandFamilyNewItem = new CVehicleServiceStandFamily;
	pStandFamilyNewItem->SetStandfamilyID(m_nDefaultStandFamilyID);
	pNewItem->AddServiceStandFamilyItem(pStandFamilyNewItem);

	CVehicleServiceFlightType* pFlightTypeNewItem = new CVehicleServiceFlightType;
	pStandFamilyNewItem->AddServiceFlightTypeItem(pFlightTypeNewItem);

	CVehicleServiceTimeRange* pTimeRangeNewItem = new CVehicleServiceTimeRange;
	if (m_eSelVehicleType == VehicleType_DeicingTruck /*|| m_eSelVehicleType == VehicleType_TowTruck*/)
		pTimeRangeNewItem->SetLeaveType(BeforeAircraftDeparts);
	
	pFlightTypeNewItem->AddServiceTimeRangeItem(pTimeRangeNewItem);

	LoadTree();
}

void CDlgVehiclePoolsAndDeployment::OnDelPool()
{
	CVehicleTypePools* pVehicleTypePoolsItem = GetCurSelVehicleTypePools();
	if(!pVehicleTypePoolsItem)
		return;
	CVehiclePool* pVehiclePoolItem = GetCurSelVehiclePool();
	if(!pVehiclePoolItem)
		return;
	pVehicleTypePoolsItem->DelVehiclePoolItem(pVehiclePoolItem);
	int nSel = m_wndListBoxPools.GetCurSel();
	m_wndListBoxPools.DeleteString(nSel);

	int nCount = m_wndListBoxPools.GetCount();
	if(nCount - 1 >= 0)
	{
		if(nSel != 0)
			m_wndListBoxPools.SetCurSel(nSel-1);
		else
			m_wndListBoxPools.SetCurSel(0);
		OnLbnSelchangeListboxPools();
	}
	else
		m_wndTreeAttributes.DeleteAllItems();
}

void CDlgVehiclePoolsAndDeployment::OnNewAttribute()
{
	HTREEITEM hSelTreeItem = m_wndTreeAttributes.GetSelectedItem();
	CString strSel = m_wndTreeAttributes.GetItemText(hSelTreeItem);

	CVehicleTypePools* pItem = GetCurSelVehicleTypePools();
	if(!pItem)
		return;

	bool bFollowMeCar = pItem->IsFollowMeCarPoolsData();
	if (bFollowMeCar)
	{
		CDlgSelectALTObject dlg(m_nProjID,ALT_MEETINGPOINT,this);
		if (IDOK == dlg.DoModal())
		{
			ALTObjectID objName = dlg.GetALTObject();
			FollowMeCarServiceMeetingPoints* pMeetingPoints = GetCurSelVehiclePool()->GetFollowMeCarServiceMeetingPoints();
			if (pMeetingPoints)
			{
				 bool bNew = pMeetingPoints->AddMeetingPoint(objName);
				 if (!bNew)
				 {
					 MessageBox("The Meeting point has already defined!");
					 return;
				 }

				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this,cni);
				cni.nt=NT_NORMAL;
				cni.net=NET_COMBOBOX;
				cni.bAutoSetItemText = false;
				m_wndTreeAttributes.InsertItem(objName.GetIDString(), cni, FALSE, FALSE, hSelTreeItem);	
				m_wndTreeAttributes.Expand(hSelTreeItem,TVE_EXPAND);

			}
		}
		return;
	}

	if(pItem->IsTowTruckPoolsData())
	{
		if(strSel == "Service stand family:")
		{
			CDlgStandFamily  dlg(m_nProjID);
			if(IDOK == dlg.DoModal())
			{	
				CVehicleServiceStandFamily* pStandFamilyNewItem = new CVehicleServiceStandFamily;
				int nStandFamilyID = dlg.GetSelStandFamilyID();
				if(nStandFamilyID == -1)
					pStandFamilyNewItem->SetStandfamilyID(m_nDefaultStandFamilyID);
				else
					pStandFamilyNewItem->SetStandfamilyID(nStandFamilyID);
				CVehiclePool* pSelItem = GetCurSelVehiclePool();
                pSelItem->AddServiceStandFamilyItem(pStandFamilyNewItem);
                LoadTree();
			}
		}
	}

	strSel = strSel.Left(2);
	if(strSel == "De")
	{
		CDlgStandFamily  dlg(m_nProjID);
		if(IDOK == dlg.DoModal())
		{	
			CVehicleServiceStandFamily* pStandFamilyNewItem = new CVehicleServiceStandFamily;
			int nStandFamilyID = dlg.GetSelStandFamilyID();
			if(nStandFamilyID == -1)
				pStandFamilyNewItem->SetStandfamilyID(m_nDefaultStandFamilyID);
			else
				pStandFamilyNewItem->SetStandfamilyID(nStandFamilyID);
			CVehiclePool* pSelItem = GetCurSelVehiclePool();
			pSelItem->AddServiceStandFamilyItem(pStandFamilyNewItem);

			CVehicleServiceFlightType* pFlightTypeNewItem = new CVehicleServiceFlightType;
			pStandFamilyNewItem->AddServiceFlightTypeItem(pFlightTypeNewItem);

			CVehicleServiceTimeRange* pTimeRangeNewItem = new CVehicleServiceTimeRange;
			if (m_eSelVehicleType == VehicleType_DeicingTruck || m_eSelVehicleType == VehicleType_TowTruck)
				pTimeRangeNewItem->SetLeaveType(BeforeAircraftDeparts);
            pFlightTypeNewItem->AddServiceTimeRangeItem(pTimeRangeNewItem);
            HTREEITEM hStandItem = InsertStandItem(pStandFamilyNewItem);
            m_wndTreeAttributes.SelectItem(hStandItem);
		}
	}
	else if(strSel == "St")
	{
		if (m_pSelectFlightType == NULL)
			return;
		FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
		CVehicleServiceFlightType* pFlightTypeNewItem = new CVehicleServiceFlightType;
		pFlightTypeNewItem->SetFltType(fltType);
		CVehicleServiceStandFamily* pSelItem = (CVehicleServiceStandFamily*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
		pSelItem->AddServiceFlightTypeItem(pFlightTypeNewItem);

		CVehicleServiceTimeRange* pTimeRangeNewItem = new CVehicleServiceTimeRange;
		if (m_eSelVehicleType == VehicleType_DeicingTruck || m_eSelVehicleType == VehicleType_TowTruck)
			pTimeRangeNewItem->SetLeaveType(BeforeAircraftDeparts);
        pFlightTypeNewItem->AddServiceTimeRangeItem(pTimeRangeNewItem);
        HTREEITEM hFlightTypeItem = InsertFlightTypeItem(pFlightTypeNewItem, hSelTreeItem);
        m_wndTreeAttributes.SelectItem(hFlightTypeItem);
	}
	else if(strSel == "Fl")
	{
		CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
		if(IDOK != dlg.DoModal())
			return;
		CVehicleServiceTimeRange* pTimeRangeNewItem = new CVehicleServiceTimeRange;
		if (m_eSelVehicleType == VehicleType_DeicingTruck || m_eSelVehicleType == VehicleType_TowTruck)
			pTimeRangeNewItem->SetLeaveType(BeforeAircraftDeparts);

		pTimeRangeNewItem->SetStartTime(dlg.GetStartTime());
		pTimeRangeNewItem->SetEndTime(dlg.GetEndTime());
		CVehicleServiceFlightType* pSelItem = (CVehicleServiceFlightType*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
        pSelItem->AddServiceTimeRangeItem(pTimeRangeNewItem);
        HTREEITEM hTimeRangeItem = InsertTimeRangeItem(pTimeRangeNewItem, hSelTreeItem);
        m_wndTreeAttributes.SelectItem(hTimeRangeItem);
	}
}

void CDlgVehiclePoolsAndDeployment::OnDelAttribute()
{
	HTREEITEM hSelTreeItem = m_wndTreeAttributes.GetSelectedItem();
	CString strSel = m_wndTreeAttributes.GetItemText(hSelTreeItem);

	CVehicleTypePools* pItem = GetCurSelVehicleTypePools();
	if(!pItem)
		return;

	bool bFollowMeCar = pItem->IsFollowMeCarPoolsData();
	if (bFollowMeCar)
	{
		ALTObjectID objName(strSel);
		FollowMeCarServiceMeetingPoints* pMeetingPoints = GetCurSelVehiclePool()->GetFollowMeCarServiceMeetingPoints();
		if (pMeetingPoints)
			pMeetingPoints->DelMeetingPoint(objName);

		m_wndTreeAttributes.DeleteItem(hSelTreeItem);				
		return;
	}

	HTREEITEM hParentTreeItem = m_wndTreeAttributes.GetParentItem(hSelTreeItem);

	if(pItem->IsTowTruckPoolsData())
	{
		strSel = m_wndTreeAttributes.GetItemText(hParentTreeItem);
		if(strSel == "Service stand family:")
		{
			CVehiclePool* pParentItem = GetCurSelVehiclePool();
			CVehicleServiceStandFamily* pSelItem = (CVehicleServiceStandFamily*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
			pParentItem->DelServiceStandFamilyItem(pSelItem);
		}
		m_wndTreeAttributes.DeleteItem(hSelTreeItem);
		return;
	}

	strSel = strSel.Left(2);
	if(strSel == "St")
	{	
		CVehiclePool* pParentItem = GetCurSelVehiclePool();
		CVehicleServiceStandFamily* pSelItem = (CVehicleServiceStandFamily*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
		pParentItem->DelServiceStandFamilyItem(pSelItem);
	}
	else if(strSel == "Fl")
	{
		CVehicleServiceStandFamily* pParentItem = (CVehicleServiceStandFamily*)m_wndTreeAttributes.GetItemData(hParentTreeItem);
		CVehicleServiceFlightType* pSelItem = (CVehicleServiceFlightType*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
		pParentItem->DelServiceFlightTypeItem(pSelItem);
	}
	else if(strSel == "Ti")
	{
		CVehicleServiceFlightType* pParentItem = (CVehicleServiceFlightType*)m_wndTreeAttributes.GetItemData(hParentTreeItem);
		CVehicleServiceTimeRange* pSelItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
		pParentItem->DelServiceTimeRangeItem(pSelItem);
	}
	m_wndTreeAttributes.DeleteItem(hSelTreeItem);
}

void CDlgVehiclePoolsAndDeployment::OnEditAttribute()
{
	HTREEITEM hSelTreeItem = m_wndTreeAttributes.GetSelectedItem();
	CString strSel = m_wndTreeAttributes.GetItemText(hSelTreeItem);
	strSel = strSel.Left(2);
	if(strSel == "St")
	{
		CDlgStandFamily  dlg(m_nProjID);
		if(IDOK == dlg.DoModal())
		{	
			CVehicleServiceStandFamily* pSelItem = (CVehicleServiceStandFamily*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
			int nStandFamilyID = dlg.GetSelStandFamilyID();
			if(nStandFamilyID == -1)
				pSelItem->SetStandfamilyID(m_nDefaultStandFamilyID);
			else
				pSelItem->SetStandfamilyID(nStandFamilyID);

            CString strValue;
            if(nStandFamilyID == m_nDefaultStandFamilyID)
            {
                strValue = _T("All stand");
            }
            else
            {
                ALTObjectGroup altObjGroup;
                altObjGroup.ReadData(nStandFamilyID);
                strValue = altObjGroup.getName().GetIDString();
            }
            CString strStand = _T("Stand : ") + strValue;
            m_wndTreeAttributes.SetItemText(hSelTreeItem, strStand);
		}
	}
	else if(strSel == "Fl")
	{	
		if (m_pSelectFlightType == NULL)
			return;
		FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
		CVehicleServiceFlightType* pSelItem = (CVehicleServiceFlightType*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
        pSelItem->SetFltType(fltType);
        CString strFltType = _T("Flight type : ");
        CString strFltType2;
        fltType.screenPrint(strFltType2);
        strFltType = strFltType + strFltType2;
        m_wndTreeAttributes.SetItemText(hSelTreeItem, strFltType);
	}
	else if(strSel == "Ti")
	{
		CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
		if(IDOK != dlg.DoModal())
			return;
		CVehicleServiceTimeRange* pSelItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hSelTreeItem);
		pSelItem->SetStartTime(dlg.GetStartTime());
        pSelItem->SetEndTime(dlg.GetEndTime());
        
        CString strTimeRange;
        strTimeRange.Format(_T("Time window : %s - %s"), dlg.GetStartTime().PrintDateTime(), dlg.GetEndTime().PrintDateTime());
        m_wndTreeAttributes.SetItemText(hSelTreeItem, strTimeRange);
	}
}

void CDlgVehiclePoolsAndDeployment::LoadFollowMeCarTree( CVehiclePool* pItem )
{
	m_wndTreeAttributes.DeleteAllItems();
	if(!pItem)
		return;
	m_wndTreeAttributes.SetRedraw(FALSE);

	//Location
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CString strLocation = _T("Location : ");
	int nParkingLotID = pItem->GetParkingLotID();

	for (int i = 0; i < static_cast<int>(m_vParkingLot.size()); ++i)
	{
		if(m_vParkingLot[i].getID() == nParkingLotID)
		{
			CString strParkingLot = m_vParkingLot[i].GetObjNameString();
			strLocation = strLocation + strParkingLot;
			break;
		}
	}

	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText = false;
	m_hTreeItemLocation = m_wndTreeAttributes.InsertItem(strLocation,cni,FALSE);

	//Quantity
	CString strQuantityTemp = _T("Quantity : ");
	int nQuantity = pItem->GetVehicleQuantity();
	CString strQuantity = _T("");
	strQuantity.Format("%s%d",strQuantityTemp,nQuantity);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	m_hTreeItemQuantity = m_wndTreeAttributes.InsertItem(strQuantity,cni,FALSE);
	m_wndTreeAttributes.SetItemData(m_hTreeItemQuantity,nQuantity);

	//Min turnaround time
	CString strMinturnaround = _T("Min turnaround time in pool (");
	CString strDistScreenPrint = pItem->GetDistScreenPrint();
	strMinturnaround = strMinturnaround + strDistScreenPrint + _T(") mins");
	cni.net=NET_COMBOBOX;
	m_hTreeItemMintrunaroundtime = m_wndTreeAttributes.InsertItem(strMinturnaround,cni,FALSE);

	//Deployment
	cni.net=NET_NORMAL;
	CString strDeployment = _T("Service Meeting Points:");
	CCoolTree::InitNodeInfo(this,cni);
	m_hTreeItemDeployment = m_wndTreeAttributes.InsertItem(strDeployment,cni,FALSE);

	FollowMeCarServiceMeetingPoints* pMeetingPoints = pItem->GetFollowMeCarServiceMeetingPoints();
	if (pMeetingPoints != NULL)
	{
		int nCount = pMeetingPoints->GetServiceMeetingPointCount();
		CString strName;
		for(int i =0; i < nCount; i++)
		{
			strName = pMeetingPoints->GetServiceMeetingPoint(i).GetIDString();
			m_wndTreeAttributes.InsertItem(strName, cni, FALSE, FALSE, m_hTreeItemDeployment);
		}
	}

	m_wndTreeAttributes.Expand(m_hTreeItemDeployment,TVE_EXPAND);
	m_wndTreeAttributes.SetRedraw(TRUE);
	UpdateToolBar();
}

void CDlgVehiclePoolsAndDeployment::LoadTree()
{	
	CVehiclePool* pVehiclePoolItem = GetCurSelVehiclePool();
	if(!pVehiclePoolItem)
	return;
	m_wndTreeAttributes.SetRedraw(FALSE);

	CVehicleTypePools* pItem = GetCurSelVehicleTypePools();
	if(!pItem)
		return;

	if (pItem->IsFollowMeCarPoolsData())
	{
		LoadFollowMeCarTree(pVehiclePoolItem);
		return;
	}

	if (pItem->IsTowTruckPoolsData())
	{
		LoadTowTruckTree(pVehiclePoolItem);
		return;
	}

	m_wndTreeAttributes.DeleteAllItems();
	//Location
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CString strLocation = _T("Location : ");
	int nParkingLotID = pVehiclePoolItem->GetParkingLotID();

	for (int i = 0; i < static_cast<int>(m_vParkingLot.size()); ++i)
	{
		if(m_vParkingLot[i].getID() == nParkingLotID)
		{
			CString strParkingLot = m_vParkingLot[i].GetObjNameString();
			strLocation = strLocation + strParkingLot;
			break;
		}
	}

	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText = false;
	m_hTreeItemLocation = m_wndTreeAttributes.InsertItem(strLocation,cni,FALSE);

	//Quantity
	CString strQuantityTemp = _T("Quantity : ");
	int nQuantity = pVehiclePoolItem->GetVehicleQuantity();
	CString strQuantity = _T("");
	strQuantity.Format("%s%d",strQuantityTemp,nQuantity);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	m_hTreeItemQuantity = m_wndTreeAttributes.InsertItem(strQuantity,cni,FALSE);
	m_wndTreeAttributes.SetItemData(m_hTreeItemQuantity,nQuantity);

	//Min turnaround time
	CString strMinturnaround = _T("Min turnaround time in pool (");
	CString strDistScreenPrint = pVehiclePoolItem->GetDistScreenPrint();
	strMinturnaround = strMinturnaround + strDistScreenPrint + _T(") mins");
	cni.net=NET_COMBOBOX;
	m_hTreeItemMintrunaroundtime = m_wndTreeAttributes.InsertItem(strMinturnaround,cni,FALSE);

	//Deployment
	CString strDeployment;
	strDeployment = _T("Deployment");
	CCoolTree::InitNodeInfo(this,cni);
	m_hTreeItemDeployment = m_wndTreeAttributes.InsertItem(strDeployment,cni,FALSE);

	//Standfamily
	int nStandFamilyCount = pVehiclePoolItem->GetServiceStandFamilyCount();
	for(int i=0; i<nStandFamilyCount; i++)
	{
		CVehicleServiceStandFamily* pStandFamilyItem = pVehiclePoolItem->GetServiceStandFamilyItem(i);
        InsertStandItem(pStandFamilyItem);
	}
	m_wndTreeAttributes.Expand(m_hTreeItemDeployment,TVE_EXPAND);
	m_wndTreeAttributes.SetRedraw(TRUE);
	UpdateToolBar();
}

HTREEITEM CDlgVehiclePoolsAndDeployment::InsertStandItem(CVehicleServiceStandFamily* pStandFamilyItem, HTREEITEM hParent, HTREEITEM hAfter)
{
    COOLTREE_NODE_INFO cni;
    CCoolTree::InitNodeInfo(this, cni);
    CString strStand = _T("Stand : ");
    int nStandfamilyID = pStandFamilyItem->GetStandfamilyID();
    CString strValue;
    if(nStandfamilyID == m_nDefaultStandFamilyID)
        strValue = _T("All stand");
    else
    {
        ALTObjectGroup altObjGroup;
        altObjGroup.ReadData(nStandfamilyID);	
        strValue = altObjGroup.getName().GetIDString();
    }
    strStand = strStand + strValue;
    cni.net=NET_SHOW_DIALOGBOX;
    HTREEITEM hTreeItemStand = m_wndTreeAttributes.InsertItem(strStand,cni,FALSE,FALSE,m_hTreeItemDeployment);
    m_wndTreeAttributes.SetItemData(hTreeItemStand,(DWORD_PTR)pStandFamilyItem);

    //FlightType
    int nFlightTypeCount = pStandFamilyItem ->GetServiceFlightTypeCount();
    for(int j=0; j<nFlightTypeCount; j++)
    {
        InsertFlightTypeItem(pStandFamilyItem->GetServiceFlightTypeItem(j), hTreeItemStand);
    }
    m_wndTreeAttributes.Expand(hTreeItemStand,TVE_EXPAND);
    return hTreeItemStand;
}

HTREEITEM CDlgVehiclePoolsAndDeployment::InsertFlightTypeItem(CVehicleServiceFlightType* pFlightTypeItem, HTREEITEM hParent, HTREEITEM hAfter)
{
    COOLTREE_NODE_INFO cni;
    CCoolTree::InitNodeInfo(this, cni);
    CString strFltType = _T("Flight type : ");
    FlightConstraint fltType = pFlightTypeItem->GetFltType();
    CString strFltType2;
    fltType.screenPrint(strFltType2);
    strFltType = strFltType + strFltType2;
    cni.net=NET_SHOW_DIALOGBOX;
    HTREEITEM hTreeItemFltType = m_wndTreeAttributes.InsertItem(strFltType,cni,FALSE,FALSE,hParent, hAfter);
    m_wndTreeAttributes.SetItemData(hTreeItemFltType,(DWORD_PTR)pFlightTypeItem);

    //TimeRange
    int nTimeRangeCount = pFlightTypeItem->GetServiceTimeRangeCount();
    for(int k=0; k<nTimeRangeCount; k++)
    {
        InsertTimeRangeItem(pFlightTypeItem->GetServiceTimeRangeItem(k), hTreeItemFltType);
    }
    m_wndTreeAttributes.Expand(hTreeItemFltType,TVE_EXPAND);
    return hTreeItemFltType;
}

HTREEITEM CDlgVehiclePoolsAndDeployment::InsertTimeRangeItem(CVehicleServiceTimeRange* pTimeRangeItem, HTREEITEM hParent, HTREEITEM hAfter)
{
    COOLTREE_NODE_INFO cni;
    CCoolTree::InitNodeInfo(this, cni);
    CString strTimeRange = _T("Time window : "),strDay = _T("");

    ElapsedTime etInsert = pTimeRangeItem->GetStartTime();
    //strDay.Format(_T("day%d %02d:%02d:%02d"), etInsert.GetDay(),etInsert.GetHour(),etInsert.GetMinute(),etInsert.GetSecond());
    strDay = etInsert.PrintDateTime();

    strTimeRange = strTimeRange + strDay + _T(" - ");

    etInsert = pTimeRangeItem->GetEndTime();
    //strDay.Format(_T("day%d %02d:%02d:%02d"), etInsert.GetDay(),etInsert.GetHour(),etInsert.GetMinute(),etInsert.GetSecond());
    strDay = etInsert.PrintDateTime();
    strTimeRange += strDay;

    cni.net=NET_SHOW_DIALOGBOX;
    HTREEITEM hTreeItemTimeRange = m_wndTreeAttributes.InsertItem(strTimeRange,cni,FALSE,FALSE,hParent, hAfter);
    m_wndTreeAttributes.SetItemData(hTreeItemTimeRange, (DWORD_PTR)pTimeRangeItem);

    //Service Regimen
    CString strRegimenType = _T("Service regimen : ");
    CString strRegimenType2;
    VehicleRegimenType enumRegimenType = pTimeRangeItem->GetRegimenType();
    if(enumRegimenType == ServiceCloset)
        strRegimenType2 = _T("closest");
    else if(enumRegimenType == ServiceNextDep)
        strRegimenType2 = _T("next departure");
    else if(enumRegimenType == ServiceRandom)
        strRegimenType2 = _T("random");
    else if(enumRegimenType == ServiceFIFO)
        strRegimenType2 = _T("FIFO");
    else if(enumRegimenType == NextArrival)
        strRegimenType2 = _T("Next arrival");
    else if(enumRegimenType == NextOperation)
        strRegimenType2 = _T("Next operation");
    else if(enumRegimenType == FillToCapacity)
        strRegimenType2 = _T("For arr ac fill to capacity");

    strRegimenType = strRegimenType + strRegimenType2;
    cni.nt=NT_NORMAL;
    cni.net=NET_COMBOBOX;
    cni.bAutoSetItemText = false;
    m_wndTreeAttributes.InsertItem(strRegimenType,cni,FALSE,FALSE,hTreeItemTimeRange);

    //LeavePool
    CString strLeavePool = _T("Leave pool");
    CCoolTree::InitNodeInfo(this,cni);
    HTREEITEM hTreeItemLeavePool = m_wndTreeAttributes.InsertItem(strLeavePool,cni,FALSE,FALSE,hTreeItemTimeRange);

    VehicleLeaveType enumLeaveType = pTimeRangeItem->GetLeaveType();
    ElapsedTime tLeaveTime = pTimeRangeItem->GetLeaveTime();
    if ((m_eSelVehicleType == VehicleType_DeicingTruck || m_eSelVehicleType == VehicleType_TowTruck) 
        && enumLeaveType != BeforeAircraftDeparts )
    {
        pTimeRangeItem->SetLeaveType(BeforeAircraftDeparts);
        enumLeaveType = BeforeAircraftDeparts;
        if (tLeaveTime > 600L)
        {
            pTimeRangeItem->SetLeaveTime(ElapsedTime(600L));
            tLeaveTime = 600L;
        }
    }

    CString strLeaveTime,strLeaveType;
    int lMinutes = tLeaveTime.asMinutes();
    strLeaveTime.Format("%d",lMinutes);
    strLeaveTime = _T("[") + strLeaveTime +_T("] mins");
    cni.nt=NT_NORMAL;
    cni.net=NET_EDITSPIN_WITH_VALUE;
    cni.bAutoSetItemText = false;
    HTREEITEM hTreeItemLeaveTime = m_wndTreeAttributes.InsertItem(strLeaveTime,cni,FALSE,FALSE,hTreeItemLeavePool);
    m_wndTreeAttributes.SetItemData(hTreeItemLeaveTime,lMinutes);

    if(enumLeaveType == BeforeAircraftArrival)
        strLeaveType = _T("before aircraft arrive");
    else if(enumLeaveType == AfterAircraftArrival)
        strLeaveType = _T("after aircraft arrive");
    else if(enumLeaveType == BeforeAircraftDeparts)
        strLeaveType = _T("before aircraft departs");
    strLeaveType = _T("(") + strLeaveType +_T(")");

    cni.net=NET_COMBOBOX;
    if (m_eSelVehicleType == VehicleType_DeicingTruck || m_eSelVehicleType == VehicleType_TowTruck)
        cni.net = NET_NORMAL;

    m_wndTreeAttributes.InsertItem(strLeaveType,cni,FALSE,FALSE,hTreeItemLeavePool);
    m_wndTreeAttributes.Expand(hTreeItemLeavePool,TVE_EXPAND);

    //Return after
    CString strAircraftService =_T("Return after [");
    int nAircraftServiceNum = pTimeRangeItem->GetAircraftServiceNum();
    CString strAircraftServiceNum;
    strAircraftServiceNum.Format("%s%d%s",strAircraftService,nAircraftServiceNum,_T("] arircraft services"));
    cni.net=NET_EDITSPIN_WITH_VALUE;
    HTREEITEM hTreeItemNum = m_wndTreeAttributes.InsertItem(strAircraftServiceNum,cni,FALSE,FALSE,hTreeItemTimeRange);
    m_wndTreeAttributes.SetItemData(hTreeItemNum,nAircraftServiceNum);

    //Time between vehicles
    CProDistrubution* pProDestri =  pTimeRangeItem->GetProDistrubution();
    CString strTimeBetweenVeh;
    strTimeBetweenVeh.Format("Time between vehicles ( %s ) secs", pProDestri->getPrintDist());
    cni.net=NET_COMBOBOX;
    m_wndTreeAttributes.InsertItem(strTimeBetweenVeh,cni,FALSE, FALSE,hTreeItemTimeRange);
    m_wndTreeAttributes.Expand(hTreeItemTimeRange,TVE_EXPAND);
    return hTreeItemTimeRange;
}

void CDlgVehiclePoolsAndDeployment::OnLbnSelchangeListboxVehicletype()
{
	// TODO: Add your control notification handler code here
	m_wndListBoxPools.ResetContent();
	CVehicleTypePools* pVehicleTypePoolsItem = GetCurSelVehicleTypePools();
	if(!pVehicleTypePoolsItem)
		return;

	m_eSelVehicleType = m_vehiclePoolsAndDeployment.GetVehicleTypeByID(pVehicleTypePoolsItem->GetVehicleTypeID());

	int nCount = pVehicleTypePoolsItem->GetVehiclePoolCount();
	for(int i=0; i<nCount; i++)
	{
		CVehiclePool* pVehiclePoolItem = pVehicleTypePoolsItem->GetVehiclePoolItem(i);
		CString strPoolName = pVehiclePoolItem->GetPoolName();
		int nIndex = m_wndListBoxPools.AddString(strPoolName);
		m_wndListBoxPools.SetItemData(nIndex,(DWORD_PTR)pVehiclePoolItem);
	}
	if(nCount>0)
	{
		m_wndListBoxPools.SetCurSel(0);
		OnLbnSelchangeListboxPools();
	}
	else
	{
		UpdateToolBar();
		m_wndTreeAttributes.DeleteAllItems();
	}
}

void CDlgVehiclePoolsAndDeployment::OnLbnSelchangeListboxPools()
{
	// TODO: Add your control notification handler code here
	LoadTree();
}

void CDlgVehiclePoolsAndDeployment::OnTvnSelchangedTreeAttributes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

LRESULT CDlgVehiclePoolsAndDeployment::OnListboxPoolsKillFocus( WPARAM wParam, LPARAM lParam )
{
	CVehiclePool* pItem = GetCurSelVehiclePool();
	if(!pItem)
		return 0;
	pItem->SetPoolName((LPCSTR)lParam);

	return 1;
}

CVehicleTypePools* CDlgVehiclePoolsAndDeployment::GetCurSelVehicleTypePools()
{
	int nSel = m_wndListBoxVehicleType.GetCurSel();
	if (nSel == LB_ERR)
		return NULL;
	CVehicleTypePools* pVehicleTypePoolsItem = (CVehicleTypePools*)m_wndListBoxVehicleType.GetItemData(nSel);
	return pVehicleTypePoolsItem;
}

CVehiclePool* CDlgVehiclePoolsAndDeployment::GetCurSelVehiclePool()
{
	int nSel = m_wndListBoxPools.GetCurSel();
	if (nSel == LB_ERR)
		return NULL;
	CVehiclePool* pVehiclePoolItem = (CVehiclePool*)m_wndListBoxPools.GetItemData(nSel);
	return pVehiclePoolItem;
}

LRESULT CDlgVehiclePoolsAndDeployment::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	switch(message)
	{	
		case UM_CEW_EDITSPIN_END:
			{
				//-----------------
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				int ntemp = (int)atoi( strValue.GetBuffer() );
				if(hItem == m_hTreeItemQuantity)
				{
					CVehiclePool* pVehiclePoolItem = GetCurSelVehiclePool();
					if(pVehiclePoolItem)
						pVehiclePoolItem->SetVehicleQuantity(ntemp);
                    int nQuantity = pVehiclePoolItem->GetVehicleQuantity();
                    CString strQuantity;
                    strQuantity.Format(_T("Quantity : %d"), nQuantity);
                    m_wndTreeAttributes.SetItemText(m_hTreeItemQuantity, strQuantity);
                    m_wndTreeAttributes.SetItemData(m_hTreeItemQuantity, ntemp);
                    break;
				}
				CString strSel = m_wndTreeAttributes.GetItemText(hItem);
				strSel = strSel.Left(1);
				if(strSel == "[")
				{
					HTREEITEM hParentItem = m_wndTreeAttributes.GetParentItem(hItem);
					HTREEITEM hParentParentItem = m_wndTreeAttributes.GetParentItem(hParentItem);
					CVehicleServiceTimeRange* pTimeRangeItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hParentParentItem);
					ElapsedTime tLeaveTime;
					tLeaveTime.set(ntemp*60);
					pTimeRangeItem->SetLeaveTime(tLeaveTime);
                    CString strItem;
                    strItem.Format("[%d] mins",ntemp);
                    m_wndTreeAttributes.SetItemText(hItem, strItem);
                    m_wndTreeAttributes.SetItemData(hItem, ntemp);
				}
				else if(strSel == "R")
				{
					HTREEITEM hParentItem = m_wndTreeAttributes.GetParentItem(hItem);
					CVehicleServiceTimeRange* pTimeRangeItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hParentItem);
                    pTimeRangeItem->SetAircraftServiceNum(ntemp);
                    CString strItem;
                    strItem.Format("%s%d%s", _T("Return after ["), ntemp, _T("] arircraft services"));
                    m_wndTreeAttributes.SetItemText(hItem, strItem);
                    m_wndTreeAttributes.SetItemData(hItem, ntemp);
				}
				else if (strSel == "L")
				{
					HTREEITEM hParentItem = m_wndTreeAttributes.GetParentItem(hItem);
					CVehicleServiceStandFamily* pStandItem = (CVehicleServiceStandFamily*)m_wndTreeAttributes.GetItemData(hParentItem);
                    pStandItem->SetLeaveTime(ntemp*60L);
                    LoadTree();
				}
			}
			break;
		case UM_CEW_COMBOBOX_BEGIN:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CWnd* pWnd=m_wndTreeAttributes.GetEditWnd(hItem);
				CRect rectWnd;
				m_wndTreeAttributes.GetItemRect(hItem,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;
				pCB->ResetContent();
				pCB->SetDroppedWidth(250);
				if(hItem == m_hTreeItemLocation)
				{
					std::vector<ALTObject>::iterator iter = m_vParkingLot.begin();
					for (; iter != m_vParkingLot.end(); ++iter)
					{
						CString strObjName = (*iter).GetObjNameString();
						int nIndex = pCB->AddString(strObjName);
						pCB->SetItemData(nIndex,(*iter).getID());
					}
				}	
				else if(hItem == m_hTreeItemMintrunaroundtime)
				{
					CVehiclePool* pVehiclePoolItem = GetCurSelVehiclePool();
					pCB->AddString( "NEW PROBABILITY DISTRIBUTION" );
					CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
					int nCount = static_cast<int>(pProbDistMan ->getCount());
					for( int m=0; m<nCount; m++ )
					{
						CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
						pCB->AddString( pPBEntry->m_csName );
					}
					int nIndex = 0;
					if ((nIndex=pCB->FindString(nIndex, pVehiclePoolItem->GetDistName())) != CB_ERR)
					{
						pCB->SetCurSel( nIndex );
					}
				}
				else
				{
					CString strSel = m_wndTreeAttributes.GetItemText(hItem);
					strSel = strSel.Left(1);
					HTREEITEM hParentItem = m_wndTreeAttributes.GetParentItem(hItem);
					CVehicleServiceTimeRange* pTimeRangeItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hParentItem);
					if(strSel == "S")
					{
						pCB->AddString("closest");
						pCB->AddString("next departure");
						pCB->AddString("random");
						pCB->AddString("FIFO");
						pCB->AddString("Next arrival");
						pCB->AddString("Next operation");
						if (m_eSelVehicleType == VehicleType_BaggageTug || m_eSelVehicleType == VehicleType_PaxTruck)
							pCB->AddString("For arr ac fill to capacity");
					}
					else if(strSel == "(")
					{
						pCB->AddString("before aircraft departs");
						pCB->AddString("before aircraft arrive");
						pCB->AddString("after aircraft arrive");
					}
					else if(strSel == "T")
					{
						pCB->AddString( "NEW PROBABILITY DISTRIBUTION" );
						CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
						int nCount = static_cast<int>(pProbDistMan ->getCount());
						for( int m=0; m<nCount; m++ )
						{
							CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
							pCB->AddString( pPBEntry->m_csName );
						}
					}
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				CWnd* pWnd = m_wndTreeAttributes.GetEditWnd((HTREEITEM)wParam);
				CComboBox* pCB = (CComboBox*)pWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strText = *(CString*)lParam;
				if(hItem == m_hTreeItemLocation)
				{
					int nSel = pCB->GetCurSel();
					int nParkingLotID = pCB->GetItemData(nSel);
					CVehiclePool* pVehiclePoolItem = GetCurSelVehiclePool();
                    pVehiclePoolItem->SetParkingLotID(nParkingLotID);
                    CString strLocation;
                    for (int i = 0; i < static_cast<int>(m_vParkingLot.size()); ++i)
                    {
                        if(m_vParkingLot[i].getID() == nParkingLotID)
                        {
                            strLocation.Format(_T("Location : %s"), m_vParkingLot[i].GetObjNameString());
                            break;
                        }
                    }
                    m_wndTreeAttributes.SetItemText(hItem, strLocation);
				}
				else if(hItem == m_hTreeItemMintrunaroundtime)
				{
					CVehiclePool* pVehiclePoolItem = GetCurSelVehiclePool();
					int nIndex = pCB->GetCurSel();
					CString strSel;

					int n = pCB->GetLBTextLen( nIndex );
					pCB->GetLBText( nIndex, strSel.GetBuffer(n) );
					strSel.ReleaseBuffer();
					ProbabilityDistribution* pProbDist = NULL;
					CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
					if( strcmp( strSel, "NEW PROBABILITY DISTRIBUTION" ) == 0 )
					{
						CProbDistEntry* pPDEntry = NULL;
						pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
						if(pPDEntry == NULL)
							return 0;
						pProbDist = pPDEntry->m_pProbDist;
						assert( pProbDist );
						CString strDistName = pPDEntry->m_csName;
						//if(strDistName == pTimeRangeItem->GetDistName())
						//	return 0;
						pVehiclePoolItem->SetDistName(strDistName);
						char szBuffer[1024] = {0};
						pProbDist->screenPrint(szBuffer);
						pVehiclePoolItem->SetDistScreenPrint(szBuffer);
						pVehiclePoolItem->SetProbTypes((ProbTypes)pProbDist->getProbabilityType());
						pProbDist->printDistribution(szBuffer);
						pVehiclePoolItem->SetPrintDist(szBuffer);
						int nIndex = 0;
						if ((nIndex=pCB->FindString(nIndex,strDistName)) != CB_ERR)
							pCB->SetCurSel(nIndex);	
						else
						{
							nIndex = pCB->AddString(strDistName);
							pCB->SetCurSel(nIndex);
						}
					}
					else
					{
						//if(pVehiclePoolItem->GetDistName() == strSel)
						//	return 0;
						CProbDistEntry* pPDEntry = NULL;
						int nCount = pProbDistMan->getCount();
						for( int i=0; i<nCount; i++ )
						{
							pPDEntry = pProbDistMan->getItem( i );
							if( strcmp( pPDEntry->m_csName, strSel ) == 0 )
								break;
						}
						//assert( i < nCount );
						pProbDist = pPDEntry->m_pProbDist;
						assert( pProbDist );
						pVehiclePoolItem->SetDistName(strSel);
						char szBuffer[1024] = {0};
						pProbDist->screenPrint(szBuffer);
						pVehiclePoolItem->SetDistScreenPrint(szBuffer);
						pVehiclePoolItem->SetProbTypes((ProbTypes)pProbDist->getProbabilityType());
						pProbDist->printDistribution(szBuffer);
						pVehiclePoolItem->SetPrintDist(szBuffer);
					}

                    CString strDistScreenPrint = pVehiclePoolItem->GetDistScreenPrint();
                    CString strMinturnaround;
                    strMinturnaround.Format(_T("Min turnaround time in pool (%s) mins"), strDistScreenPrint);
                    m_wndTreeAttributes.SetItemText(hItem, strMinturnaround);
				}
				else
				{
					CString strSel = m_wndTreeAttributes.GetItemText(hItem);
					strSel = strSel.Left(1);
					HTREEITEM hParentItem = m_wndTreeAttributes.GetParentItem(hItem);
					CVehicleServiceTimeRange* pTimeRangeItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hParentItem);
					if(strSel == "S")
					{
						int nSel = pCB->GetCurSel();
						VehicleRegimenType enumRegimenType = ServiceFIFO;
						if(nSel == 0)
							enumRegimenType = ServiceCloset;
						else if(nSel == 1)
							enumRegimenType = ServiceNextDep;
						else if(nSel == 2)
							enumRegimenType = ServiceRandom;
						else if(nSel == 3)
							enumRegimenType = ServiceFIFO;
						else if(nSel == 4)
							enumRegimenType = NextArrival;
						else if(nSel == 5)
							enumRegimenType = NextOperation;
						else if(nSel == 6)
							enumRegimenType = FillToCapacity;
						pTimeRangeItem->SetRegimenType(enumRegimenType);

                        CString strRegimenType = _T("Service regimen : ");
                        CString strRegimenType2;
                        if(enumRegimenType == ServiceCloset)
                            strRegimenType2 = _T("closest");
                        else if(enumRegimenType == ServiceNextDep)
                            strRegimenType2 = _T("next departure");
                        else if(enumRegimenType == ServiceRandom)
                            strRegimenType2 = _T("random");
                        else if(enumRegimenType == ServiceFIFO)
                            strRegimenType2 = _T("FIFO");
                        else if(enumRegimenType == NextArrival)
                            strRegimenType2 = _T("Next arrival");
                        else if(enumRegimenType == NextOperation)
                            strRegimenType2 = _T("Next operation");
                        else if(enumRegimenType == FillToCapacity)
                            strRegimenType2 = _T("For arr ac fill to capacity");
                        strRegimenType = strRegimenType + strRegimenType2;
                        m_wndTreeAttributes.SetItemText(hItem, strRegimenType);
					}
					else if(strSel == "(")
					{

						HTREEITEM hParentParentItem = m_wndTreeAttributes.GetParentItem(hParentItem);
						CVehicleServiceTimeRange* pTimeRangeItem = (CVehicleServiceTimeRange*)m_wndTreeAttributes.GetItemData(hParentParentItem);
						int nSel = pCB->GetCurSel();
						if(nSel == 0)
							pTimeRangeItem->SetLeaveType(BeforeAircraftDeparts);
						else if(nSel == 1)
							pTimeRangeItem->SetLeaveType(BeforeAircraftArrival);
						else
							pTimeRangeItem->SetLeaveType(AfterAircraftArrival);

                        VehicleLeaveType enumLeaveType = pTimeRangeItem->GetLeaveType();
                        CString strLeaveType;
                        if(enumLeaveType == BeforeAircraftArrival)
                            strLeaveType = _T("before aircraft arrive");
                        else if(enumLeaveType == AfterAircraftArrival)
                            strLeaveType = _T("after aircraft arrive");
                        else if(enumLeaveType == BeforeAircraftDeparts)
                            strLeaveType = _T("before aircraft departs");
                        strLeaveType = _T("(") + strLeaveType +_T(")");
                        m_wndTreeAttributes.SetItemText(hItem, strLeaveType);
					}
					else if(strSel == "T")
					{
						int nIndex = pCB->GetCurSel();
						CString strSel;

						int n = pCB->GetLBTextLen( nIndex );
						pCB->GetLBText( nIndex, strSel.GetBuffer(n) );
						strSel.ReleaseBuffer();
						ProbabilityDistribution* pProbDist = NULL;
						CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
						
						CProDistrubution* pProDestri = pTimeRangeItem->GetProDistrubution();
						if( strcmp( strSel, "NEW PROBABILITY DISTRIBUTION" ) == 0 )
						{
							CProbDistEntry* pPDEntry = NULL;
							pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
							if(pPDEntry == NULL)
								return 0;
							pProbDist = pPDEntry->m_pProbDist;
							assert( pProbDist );
							
							CString strDistName = pPDEntry->m_csName;
							pProDestri->setDistName(strDistName);

 							char szBuffer[1024] = {0};
							pProbDist->printDistribution(szBuffer);
							pProDestri->setPrintDist(szBuffer);

							pProDestri->setProType((ProbTypes)pProbDist->getProbabilityType());
							int nIndex = 0;
							if ((nIndex=pCB->FindString(nIndex,strDistName)) != CB_ERR)
								pCB->SetCurSel(nIndex);	
							else
							{
								nIndex = pCB->AddString(strDistName);
								pCB->SetCurSel(nIndex);
							}
						}
						else
						{
							CProbDistEntry* pPDEntry = NULL;
							int nCount = pProbDistMan->getCount();
							int i;
							for( i=0; i<nCount; i++ )
							{
								pPDEntry = pProbDistMan->getItem( i );
								if( strcmp( pPDEntry->m_csName, strSel ) == 0 )
									break;
							}
//							assert( i < nCount );
							pProbDist = pPDEntry->m_pProbDist;
							assert( pProbDist );

 							pProDestri->setDistName(strSel);

							char szBuffer[1024] = {0};
							pProbDist->printDistribution(szBuffer);
							pProDestri->setPrintDist(szBuffer);
 							pProDestri->setProType((ProbTypes)pProbDist->getProbabilityType());
						}
                        pProDestri->initProbilityDistribution();
                        CString strTimeBetweenVeh;
                        strTimeBetweenVeh.Format("Time between vehicles ( %s ) secs", pProDestri->getPrintDist());
                        m_wndTreeAttributes.SetItemText(hItem, strTimeBetweenVeh);
                    }
				}
			}
			break;
		case UM_CEW_SHOW_DIALOGBOX_BEGIN:
			{
				OnEditAttribute();
			}
			break;
		default:
			break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgVehiclePoolsAndDeployment::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeAttributes.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeAttributes.SetFocus();
	CPoint pt = point;
	m_wndTreeAttributes.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeAttributes.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeAttributes.SelectItem(hRClickItem);

	CMenu menuPopup;
	menuPopup.LoadMenu(IDR_MENU_ADD_DEL_EDIT);
	CMenu* pSubMenu = NULL;

	CString strSel = m_wndTreeAttributes.GetItemText(hRClickItem);
	strSel = strSel.Left(2);

	if(hRClickItem == m_hTreeItemDeployment)
	{
		pSubMenu = menuPopup.GetSubMenu(0);
		pSubMenu->DeleteMenu(ID_DEL_MENU,MF_BYCOMMAND);	
		pSubMenu->DeleteMenu(ID_EDIT_MENU,MF_BYCOMMAND);	
	}	
	else if(strSel == "St" || strSel == "Fl")
	{
		pSubMenu = menuPopup.GetSubMenu(0);
	}
	else if(strSel == "Ti")
	{
		pSubMenu = menuPopup.GetSubMenu(0);
		pSubMenu->DeleteMenu(ID_NEW_MENU,MF_BYCOMMAND);				
	}
	if (pSubMenu != NULL)
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CDlgVehiclePoolsAndDeployment::LoadTowTruckTree( CVehiclePool* pVehiclePoolItem )
{
	m_wndTreeAttributes.DeleteAllItems();
	//Location
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CString strLocation = _T("Location : ");
	int nParkingLotID = pVehiclePoolItem->GetParkingLotID();

	for (int i = 0; i < static_cast<int>(m_vParkingLot.size()); ++i)
	{
		if(m_vParkingLot[i].getID() == nParkingLotID)
		{
			CString strParkingLot = m_vParkingLot[i].GetObjNameString();
			strLocation = strLocation + strParkingLot;
			break;
		}
	}

	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText = false;
	m_hTreeItemLocation = m_wndTreeAttributes.InsertItem(strLocation,cni,FALSE);

	//Quantity
	CString strQuantityTemp = _T("Quantity : ");
	int nQuantity = pVehiclePoolItem->GetVehicleQuantity();
	CString strQuantity = _T("");
	strQuantity.Format("%s%d",strQuantityTemp,nQuantity);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	m_hTreeItemQuantity = m_wndTreeAttributes.InsertItem(strQuantity,cni,FALSE);
	m_wndTreeAttributes.SetItemData(m_hTreeItemQuantity,nQuantity);

	//Min turnaround time
	CString strMinturnaround = _T("Min turnaround time in pool (");
	CString strDistScreenPrint = pVehiclePoolItem->GetDistScreenPrint();
	strMinturnaround = strMinturnaround + strDistScreenPrint + _T(") mins");
	cni.net=NET_COMBOBOX;
	m_hTreeItemMintrunaroundtime = m_wndTreeAttributes.InsertItem(strMinturnaround,cni,FALSE);

	//Deployment
	CString strDeployment;
	strDeployment = _T("Service stand family:");
	CCoolTree::InitNodeInfo(this,cni);
	m_hTreeItemDeployment = m_wndTreeAttributes.InsertItem(strDeployment,cni,FALSE);

	//Standfamily
	int nStandFamilyCount = pVehiclePoolItem->GetServiceStandFamilyCount();
	for(int i=0; i<nStandFamilyCount; i++)
	{
		CVehicleServiceStandFamily* pStandFamilyItem = pVehiclePoolItem->GetServiceStandFamilyItem(i);
		int nStandfamilyID = pStandFamilyItem->GetStandfamilyID();
		CString strValue;
		if(nStandfamilyID == m_nDefaultStandFamilyID)
			strValue = _T("All stand");
		else
		{
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(nStandfamilyID);	
			strValue = altObjGroup.getName().GetIDString();
		}
		cni.net=NET_SHOW_DIALOGBOX;
		HTREEITEM hTreeItemStand = m_wndTreeAttributes.InsertItem(strValue,cni,FALSE,FALSE,m_hTreeItemDeployment);
		m_wndTreeAttributes.SetItemData(hTreeItemStand,(DWORD_PTR)pStandFamilyItem);

		cni.net = NET_EDITSPIN_WITH_VALUE;
		cni.bAutoSetItemText = false;
		strValue.Format("Leave Pool: %d mins before flight departs", pStandFamilyItem->GetLeaveTime().asMinutes());
		m_wndTreeAttributes.InsertItem(strValue, cni, FALSE, FALSE, hTreeItemStand);

		m_wndTreeAttributes.Expand(hTreeItemStand,TVE_EXPAND);
	}
	m_wndTreeAttributes.Expand(m_hTreeItemDeployment,TVE_EXPAND);
	m_wndTreeAttributes.SetRedraw(TRUE);
	UpdateToolBar();
}


