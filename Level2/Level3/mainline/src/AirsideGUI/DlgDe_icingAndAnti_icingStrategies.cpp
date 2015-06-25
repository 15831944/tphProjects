// DlgDe_icingAndAnti_icingStrategies.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgDe_icingAndAnti_icingStrategies.h"
#include "DlgStandFamily.h"
#include "DlgDeicePadFamily.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/ALT_BIN.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/ALTObjectGroup.h"
#include <algorithm>


// CDlgDe_icingAndAnti_icingStrategies dialog
const char* strPreType[] = {"Non-freezing","Freezing drizzle","Light Snow","Medium Snow","Heavy snow"};
const char* strSurfaceType[] = {"Clear","Light Frost","Ice layer","Dry/powder snow","Wet Snow "};
const char* strPosType[] = {"power in","tow to nose in","tow to tail"};
const char* strStateType[] = {"Running","Off"};
IMPLEMENT_DYNAMIC(CDlgDe_icingAndAnti_icingStrategies, CXTResizeDialog)
CDlgDe_icingAndAnti_icingStrategies::CDlgDe_icingAndAnti_icingStrategies(int nProjID,CAirportDatabase* pAirportDB,PSelectFlightType pSelectFlightType,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgDe_icingAndAnti_icingStrategies::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_deiceAnticeManager(pAirportDB)
{
}

CDlgDe_icingAndAnti_icingStrategies::~CDlgDe_icingAndAnti_icingStrategies()
{
}

void CDlgDe_icingAndAnti_icingStrategies::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgDe_icingAndAnti_icingStrategies, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_RUNWAY_ADD,OnAddFlightType)
	ON_COMMAND(ID_TOOLBAR_RUNWAY_DEL,OnRemoveFlightType)
	ON_COMMAND(ID_TOOLBAR_RUNWAY_EDIT,OnEditFlightType)
	ON_COMMAND(ID_ADD_STAND,OnAddDepStand)
	ON_COMMAND(ID_DEL_STAND,OnRemoveDepStand)
	ON_COMMAND(ID_EDIT_STAND,OnEditDepStand)
	ON_COMMAND(ID_DEICE_PAD,ChangeDeicePad)
	ON_COMMAND(ID_DEP_STAND,ChangeDepStand)
	ON_COMMAND(ID_LEAD_OUTLINE,ChangeLeadOutLine)
	ON_COMMAND(ID_REMOTE_STAND,ChangeRemote)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA, OnSelChanged)
END_MESSAGE_MAP()


// CDlgDe_icingAndAnti_icingStrategies message handlers

BOOL CDlgDe_icingAndAnti_icingStrategies::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_LOAD,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_SAVEAS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	OnInitToolbar();
	OnUpdateToolbar();
	m_deiceAnticeManager.ReadData(m_nProjID);
	OnInitTreeCtrl();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgDe_icingAndAnti_icingStrategies::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,TRUE);
}

int CDlgDe_icingAndAnti_icingStrategies::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgDe_icingAndAnti_icingStrategies::OnInitTreeCtrl()
{
	int nCount = m_deiceAnticeManager.getCount();
	for (int i = 0; i < nCount; i++)
	{
		CDeiceAndAnticeFlightType* pFlightType = m_deiceAnticeManager.getItem(i);
		FlightConstraint& fltType = pFlightType->GetFltType();
		char szBuffer[1024] = {0};
		fltType.screenPrint(szBuffer);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		cni.unMenuID = IDR_MENU_DEPSTAND;
		CString strValue = _T("");
		strValue.Format(_T("Flight Type: %s"),szBuffer);
		HTREEITEM hFltTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE);

		CTreeNodeData* pNodeData = new CTreeNodeData();
		pNodeData->dwData = (DWORD_PTR)pFlightType;
		m_wndTreeCtrl.SetItemData(hFltTypeItem,(DWORD_PTR)pNodeData);

		for (int j = 0; j < pFlightType->getCount(); j++)
		{
			CDeiceAndAnticeStand* pStand = pFlightType->getItem(j);
			CString strNodeTitle = _T("");
			if(pStand->getStand() == -1)
				strNodeTitle.Format(_T("Departure Stand(s): All"));
			else
			{
				ALTObjectGroup altGroup;
				altGroup.ReadData(pStand->getStand());
				strNodeTitle.Format(_T("Departure Stand(s): %s"),altGroup.getName().GetIDString());
			}
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.nt = NT_NORMAL;
			cni.net = NET_SHOW_DIALOGBOX;
			cni.unMenuID = IDR_MENU_DEICESTAND;

			CTreeNodeData* pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pStand;
			pNodeData->emType = NOTYPE;
			HTREEITEM hStandItem = m_wndTreeCtrl.InsertItem(strNodeTitle,cni,FALSE,FALSE,hFltTypeItem);
			m_wndTreeCtrl.SetItemData(hStandItem,(DWORD_PTR)pNodeData);
			m_vDepartureStand.push_back(hStandItem);

			InitPrecipitonTypeItem(hStandItem,pStand->getNonFreezing());
			InitPrecipitonTypeItem(hStandItem,pStand->getFreezingDrizzle());
			InitPrecipitonTypeItem(hStandItem,pStand->getLightSnow());
			InitPrecipitonTypeItem(hStandItem,pStand->getMediumSnow());
			InitPrecipitonTypeItem(hStandItem,pStand->getHeavySnow());
			m_wndTreeCtrl.Expand(hStandItem,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hFltTypeItem,TVE_EXPAND);
	}
}
void CDlgDe_icingAndAnti_icingStrategies::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_deiceAnticeManager.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CXTResizeDialog::OnOK();
}

void CDlgDe_icingAndAnti_icingStrategies::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
        m_deiceAnticeManager.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}
void CDlgDe_icingAndAnti_icingStrategies::OnAddFlightType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	
	int nCount = m_deiceAnticeManager.getCount();
	for (int i=0; i<nCount; i++)
	{
		CDeiceAndAnticeFlightType* pData = m_deiceAnticeManager.getItem(i);
		char szFltType[1024] = {0};
		pData->GetFltType().screenPrint(szFltType);
		if (!strcmp(szBuffer,szFltType))
		{
			MessageBox(_T("The flight Type already exists!"));

			return;
		}
	}

	CDeiceAndAnticeFlightType* pItem = new CDeiceAndAnticeFlightType();
	pItem->SetFltType(fltType);
	m_deiceAnticeManager.addItem(pItem);
	
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	cni.unMenuID = IDR_MENU_DEPSTAND;
	CString strValue = _T("");
	strValue.Format(_T("Flight Type: %s"),szBuffer);
	HTREEITEM hFltTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE);

	CTreeNodeData* pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pItem;
	m_wndTreeCtrl.SetItemData(hFltTypeItem,(DWORD_PTR)pNodeData);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::OnRemoveFlightType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CDeiceAndAnticeFlightType* pFlightType = (CDeiceAndAnticeFlightType*)pNodeData->dwData;
	m_deiceAnticeManager.removeItem(pFlightType);
	m_wndTreeCtrl.DeleteItem(hItem);
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::OnEditFlightType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	int nCount = m_deiceAnticeManager.getCount();
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CDeiceAndAnticeFlightType* pFlightType = (CDeiceAndAnticeFlightType*)pNodeData->dwData;
	for (int i=0; i<nCount; i++)
	{
		CDeiceAndAnticeFlightType* pData = m_deiceAnticeManager.getItem(i);
		char szFltType[1024] = {0};
		pData->GetFltType().screenPrint(szFltType);
		if (!strcmp(szBuffer,szFltType) && pFlightType != pData)
		{
			MessageBox(_T("The flight Type already exists!"));

			return;
		}
	}

	pFlightType->SetFltType(fltType);
	CString strValue = _T("");
	strValue.Format(_T("Flight Type: %s"),szBuffer);
	m_wndTreeCtrl.SetItemText(hItem,strValue);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::OnAddDepStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CDeiceAndAnticeFlightType* pFlightType = (CDeiceAndAnticeFlightType*)pNodeData->dwData;

	CDlgStandFamily dlgSelect(m_nProjID);
	
	if(dlgSelect.DoModal()==IDOK)
	{
		
		CString strNodeTitle = _T("");
		if(dlgSelect.GetSelStandFamilyID() == -1)
			strNodeTitle.Format(_T("Departure Stand(s): All"));
		else
			strNodeTitle.Format(_T("Departure Stand(s): %s"),dlgSelect.GetSelStandFamilyName());
		int nCount = pFlightType->getCount();
		for (int i = 0; i < nCount; i++)
		{
			CDeiceAndAnticeStand* pItem = pFlightType->getItem(i);
			if (dlgSelect.GetSelStandFamilyID() == pItem->getStand())
			{
				::AfxMessageBox(_T("The stand is exist"));
				return;
			}
		}
		CDeiceAndAnticeStand* pStand = new CDeiceAndAnticeStand();
		pStand->setStand( dlgSelect.GetSelStandFamilyID() );

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		cni.unMenuID = IDR_MENU_DEICESTAND;

		CTreeNodeData* pNodeData = new CTreeNodeData();
		pNodeData->dwData = (DWORD_PTR)pStand;
		pNodeData->emType = DEPARTURE_STAND;
		HTREEITEM hStandItem = m_wndTreeCtrl.InsertItem(strNodeTitle,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hStandItem,(DWORD_PTR)pNodeData);
		m_vDepartureStand.push_back(hStandItem);
		pFlightType->addItem(pStand);

		InitPrecipitonTypeItem(hStandItem,pStand->getNonFreezing());
		InitPrecipitonTypeItem(hStandItem,pStand->getFreezingDrizzle());
		InitPrecipitonTypeItem(hStandItem,pStand->getLightSnow());
		InitPrecipitonTypeItem(hStandItem,pStand->getMediumSnow());
		InitPrecipitonTypeItem(hStandItem,pStand->getHeavySnow());
		m_wndTreeCtrl.Expand(hStandItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgDe_icingAndAnti_icingStrategies::OnRemoveDepStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	CDeiceAndAnticeFlightType* pFlightType = (CDeiceAndAnticeFlightType*)pParentData->dwData;
	CDeiceAndAnticeStand* pStand = (CDeiceAndAnticeStand*)pNodeData->dwData;
	pFlightType->removeItem(pStand);
	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::OnEditDepStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CDlgStandFamily dlgSelect(m_nProjID);
	if(dlgSelect.DoModal()==IDOK)
	{
		CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
		CDeiceAndAnticeFlightType* pFlightType = (CDeiceAndAnticeFlightType*)pParentData->dwData;
		CDeiceAndAnticeStand* pStand = (CDeiceAndAnticeStand*)pNodeData->dwData;
		CString strNodeTitle = _T("");
		if(dlgSelect.GetSelStandFamilyID() == -1)
			strNodeTitle.Format(_T("Departure Stand(s): All"));
		else
			strNodeTitle.Format(_T("Departure Stand(s): %s"),dlgSelect.GetSelStandFamilyName());
		int nCount = pFlightType->getCount();
		for (int i = 0; i < nCount; i++)
		{
			CDeiceAndAnticeStand* pItem = pFlightType->getItem(i);
			if (dlgSelect.GetSelStandFamilyID() == pItem->getStand() && pItem != pStand)
			{
				::AfxMessageBox(_T("The stand is exist"));
				return;
			}
		}
		pStand->setStand( dlgSelect.GetSelStandFamilyID() );
		m_wndTreeCtrl.SetItemText(hItem,strNodeTitle);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pStand);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgDe_icingAndAnti_icingStrategies::InitPrecipitonTypeItem(HTREEITEM hItem,CDeiceAndAnticePrecipitionType* pPrecipition)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CString strValue = _T("");
	strValue.Format(_T("Precipition Type: %s"),strPreType[pPrecipition->getType()]);
	HTREEITEM hPreItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
	CTreeNodeData* pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pPrecipition;
	m_wndTreeCtrl.SetItemData(hPreItem,(DWORD_PTR)pNodeData);

	InitSurfaceConditionItem(hPreItem,pPrecipition->getClear());
	InitSurfaceConditionItem(hPreItem,pPrecipition->getLightFrost());
	InitSurfaceConditionItem(hPreItem,pPrecipition->getIceLayer());
	InitSurfaceConditionItem(hPreItem,pPrecipition->getDryPowerSnow());
	InitSurfaceConditionItem(hPreItem,pPrecipition->getWetSnow());
}

void CDlgDe_icingAndAnti_icingStrategies::InitSurfaceConditionItem(HTREEITEM hItem,CDeiceAndAnticeInitionalSurfaceCondition* pSurface)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CString strValue = _T("");
	strValue.Format(_T("Initial surface condition: %s"),strSurfaceType[pSurface->getType()]);
	HTREEITEM hSurItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
	CTreeNodeData* pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pSurface;
	m_wndTreeCtrl.SetItemData(hSurItem,(DWORD_PTR)pNodeData);
	InitVehiclesItem(hSurItem,pSurface->getDeiceVehicle(),pSurface->getAntiVehicle());
	InitPriorityItem(hSurItem,pSurface);
}

void CDlgDe_icingAndAnti_icingStrategies::InitVehiclesItem(HTREEITEM hItem,IceVehicles* pDeiceVehicles,IceVehicles* pAntiVehicles)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CTreeNodeData* pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pDeiceVehicles;
	pNodeData->emType = NOTYPE;
	HTREEITEM hDeVehiclesItem = m_wndTreeCtrl.InsertItem(_T("De-ice Vehicles"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hDeVehiclesItem,(DWORD_PTR)pNodeData);
	CString strValue = _T("");
	strValue.Format(_T("Number: %d"),pDeiceVehicles->m_nNumber);
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pDeiceVehicles->m_nNumber);
	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pDeiceVehicles;
	pNodeData->emType = DE_NUMBER;
	HTREEITEM hDeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hDeVehiclesItem);
	m_wndTreeCtrl.SetItemData(hDeItem,(DWORD_PTR)pNodeData);
	strValue.Format(_T("Flow Rate litres (USG/min): %d"),pDeiceVehicles->m_nFlowRate);
	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pDeiceVehicles;
	pNodeData->emType = DE_FLOWRATE;
	cni.fMinValue = static_cast<float>(pDeiceVehicles->m_nFlowRate);
	HTREEITEM hFlowRateITem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hDeVehiclesItem);
	m_wndTreeCtrl.SetItemData(hFlowRateITem,(DWORD_PTR)pNodeData);

	cni.net = NET_NORMAL;
	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pAntiVehicles;
	pNodeData->emType = NOTYPE;
	HTREEITEM hAnVehiclesItem = m_wndTreeCtrl.InsertItem(_T("Anti-ice Vehicles"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hAnVehiclesItem,(DWORD_PTR)pNodeData);
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pAntiVehicles->m_nNumber);
	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pAntiVehicles;
	pNodeData->emType = ANTI_NUMBER;
	strValue.Format(_T("Number: %d"),pAntiVehicles->m_nNumber);
	HTREEITEM hAnItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hAnVehiclesItem);
	m_wndTreeCtrl.SetItemData(hAnItem,(DWORD_PTR)pNodeData);
	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pAntiVehicles;
	pNodeData->emType = ANTI_FLOWRATE;
	cni.fMinValue = static_cast<float>(pAntiVehicles->m_nFlowRate);
	strValue.Format(_T("Flow Rate litres (USG/min): %d"),pAntiVehicles->m_nFlowRate);
	HTREEITEM hAnFlowRateItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hAnVehiclesItem);
	m_wndTreeCtrl.SetItemData(hAnFlowRateItem,(DWORD_PTR)pNodeData);
}

void CDlgDe_icingAndAnti_icingStrategies::InitPriorityItem(HTREEITEM hItem,CDeiceAndAnticeInitionalSurfaceCondition* pSurface)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CTreeNodeData* pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pSurface;
	pNodeData->emType = NOTYPE;
	HTREEITEM hFluidItem = m_wndTreeCtrl.InsertItem(_T("Fluid application parking position"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hFluidItem,(DWORD_PTR)pNodeData);

	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pSurface;
	pNodeData->emType = NOTYPE;
	HTREEITEM hPriorityItem1 = m_wndTreeCtrl.InsertItem(("Priority1"),cni,FALSE,FALSE,hFluidItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem1,(DWORD_PTR)pNodeData);
	PriorityType emType = pSurface->getPriotity(1);
	SetPriorityNodeContent(hPriorityItem1,emType,pSurface);

	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pSurface;
	pNodeData->emType = NOTYPE;
	HTREEITEM hPriorityItem2 = m_wndTreeCtrl.InsertItem(("Priority2"),cni,FALSE,FALSE,hFluidItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem2,(DWORD_PTR)pNodeData);
	emType = pSurface->getPriotity(2);
	SetPriorityNodeContent(hPriorityItem2,emType,pSurface);

	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pSurface;
	pNodeData->emType = NOTYPE;
	HTREEITEM hPriorityItem3 = m_wndTreeCtrl.InsertItem(("Priority3"),cni,FALSE,FALSE,hFluidItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem3,(DWORD_PTR)pNodeData);
	emType = pSurface->getPriotity(3);
	SetPriorityNodeContent(hPriorityItem3,emType,pSurface);

	pNodeData = new CTreeNodeData();
	pNodeData->dwData = (DWORD_PTR)pSurface;
	pNodeData->emType = NOTYPE;
	HTREEITEM hPriorityItem4 = m_wndTreeCtrl.InsertItem(("Priority4"),cni,FALSE,FALSE,hFluidItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem4,(DWORD_PTR)pNodeData);
	emType = pSurface->getPriotity(4);
	SetPriorityNodeContent(hPriorityItem4,emType,pSurface);
}

void CDlgDe_icingAndAnti_icingStrategies::SetPriorityNodeContent(HTREEITEM hItem,PriorityType emType,CDeiceAndAnticeInitionalSurfaceCondition* pSurface)
{
	CString strValue = _T("");
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CTreeNodeData* pNodeData = new CTreeNodeData();
	switch(emType)
	{
	case DeicePad_Type:
		{
			DeicePadPriority* pDeicePad = pSurface->getDeicePad();

			pNodeData->dwData = (DWORD_PTR)pDeicePad;
			pNodeData->emType = DEICE_PAD;
			cni.nt = NT_NORMAL;
			cni.net = NET_SHOW_DIALOGBOX;
			cni.unMenuID = IDR_MENU_PRIORITY;
			if(pDeicePad->m_deicePad == -1)
				strValue.Format(_T("Deice pad: All"));
			else
			{
				ALTObjectGroup altGroup;
				altGroup.ReadData(pDeicePad->m_deicePad);
				strValue.Format(_T("Deice pad: %s"),altGroup.getName().GetIDString());
			}
			HTREEITEM hDeiceItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hDeiceItem,(DWORD_PTR)pNodeData);
			m_vDeicePad.push_back(hDeiceItem);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDeicePad;
			pNodeData->emType = NOTYPE;
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = 0;
			HTREEITEM hConItem = m_wndTreeCtrl.InsertItem(_T("Change conditions"),cni,FALSE,FALSE,hDeiceItem);
			m_wndTreeCtrl.SetItemData(hConItem,(DWORD_PTR)pNodeData);
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			cni.fMinValue = static_cast<float>(pDeicePad->m_nDeicePadQueue);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDeicePad;
			pNodeData->emType = PAD_QUEUE;
			strValue.Format(_T("Deiceing pad queue greater than aircraft or: %d"),pDeicePad->m_nDeicePadQueue);
			HTREEITEM hQueueItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hQueueItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDeicePad;
			pNodeData->emType = PAD_WAITTIME;
			strValue.Format(_T("Wait time exceeds minutes: %d"),pDeicePad->m_nWaittime);
			cni.fMinValue = static_cast<float>(pDeicePad->m_nWaittime);
			HTREEITEM hWaitItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hWaitItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDeicePad;
			pNodeData->emType = PAD_METHOD;
			strValue.Format(_T("Position method: %s"),strPosType[pDeicePad->m_emType]);
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			HTREEITEM hPosItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hPosItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDeicePad;
			pNodeData->emType = PAD_STATE;
			strValue.Format(_T("Engine state: %s"),strStateType[pDeicePad->m_emEngineState]);
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			HTREEITEM hStateItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hStateItem,(DWORD_PTR)pNodeData);
		}
		break;
	case DepStand_Type:
		{
			DepStandPriority* pDepStand = pSurface->getDepStand();
			pNodeData->dwData = (DWORD_PTR)pDepStand;
			pNodeData->emType = DEP_STAND;
			cni.net = NET_SHOW_DIALOGBOX;
			cni.unMenuID = IDR_MENU_PRIORITY;
			if(pDepStand->m_Stand == -1)
				strValue.Format(_T("Dep Stand: All"));
			else
			{
				ALTObjectGroup altGroup;
				altGroup.ReadData(pDepStand->m_Stand);
				strValue.Format(_T("Dep Stand: %s"),altGroup.getName().GetIDString());
			}
			HTREEITEM hDepItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hDepItem,(DWORD_PTR)pNodeData);
			m_vDepStand.push_back(hDepItem);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDepStand;
			pNodeData->emType = NOTYPE;
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = 0;
			HTREEITEM hConItem = m_wndTreeCtrl.InsertItem(_T("Change conditions"),cni,FALSE,FALSE,hDepItem);
			m_wndTreeCtrl.SetItemData(hConItem,(DWORD_PTR)pNodeData);
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDepStand;
			pNodeData->emType = DEPSTAND_WITHINTIME;
			strValue.Format(_T("Stand needed within mins: %d"),pDepStand->m_nStandWithinTime);
			cni.fMinValue = static_cast<float>(pDepStand->m_nStandWithinTime);
			HTREEITEM hWithinItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hWithinItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDepStand;
			pNodeData->emType = DEPSTAND_FLUIDTIME;
			strValue.Format(_T("Wait for fluid vehicles greater than mins: %d"),pDepStand->m_nWaitFluidTime);
			cni.fMinValue = static_cast<float>(pDepStand->m_nWaitFluidTime);
			HTREEITEM hFluidItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hFluidItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDepStand;
			pNodeData->emType = DEPSTAND_INSPECTIONTIME;
			strValue.Format(_T("Wait fro inspection greater than mins: %d"),pDepStand->m_nWaitInspectionTime);
			cni.fMinValue = static_cast<float>(pDepStand->m_nWaitInspectionTime);
			HTREEITEM hInspectionItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hInspectionItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pDepStand;
			pNodeData->emType = DEPSTAND_STATE;
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			strValue.Format(_T("Engine state: %s"),strStateType[pDepStand->m_emEngineState]);
			HTREEITEM hStateItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hStateItem,(DWORD_PTR)pNodeData);
		}
		break;
	case LeadOut_Type:
		{
			LeadOutPriority* pLeadOut = pSurface->getLeadOut();
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			pNodeData->dwData = (DWORD_PTR)pLeadOut;
			pNodeData->emType = LEADOUT_DISTIANCE;
			cni.unMenuID = IDR_MENU_PRIORITY;
			strValue.Format(_T("Lead Out Line from parking position: %d"),pLeadOut->m_nDistance);
			cni.fMinValue = static_cast<float>(pLeadOut->m_nDistance);
			HTREEITEM hLeadOutItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hLeadOutItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pLeadOut;
			pNodeData->emType = NOTYPE;
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = 0;
			HTREEITEM hConItem = m_wndTreeCtrl.InsertItem(_T("Change conditions"),cni,FALSE,FALSE,hLeadOutItem);
			m_wndTreeCtrl.SetItemData(hConItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pLeadOut;
			pNodeData->emType = LEADOUT_WITHINTIME;
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			strValue.Format(_T("Stand needed within mins: %d"),pLeadOut->m_nStandWithinTime);
			cni.fMinValue = static_cast<float>(pLeadOut->m_nStandWithinTime);
			HTREEITEM hWithinItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hWithinItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pLeadOut;
			pNodeData->emType = LEADOUT_FLUIDTIME;
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			strValue.Format(_T("Wait for fluid vehicles greater than mins: %d"),pLeadOut->m_nWaitFluidTime);
			cni.fMinValue = static_cast<float>(pLeadOut->m_nWaitFluidTime);
			HTREEITEM hFluidItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hFluidItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pLeadOut;
			pNodeData->emType = LEADOUT_INSPECTIONTIME;
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			strValue.Format(_T("Wait fro inspection greater than mins: %d"),pLeadOut->m_nWaitInspectionTime);
			cni.fMinValue = static_cast<float>(pLeadOut->m_nWaitInspectionTime);
			HTREEITEM hInspectionItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hInspectionItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pLeadOut;
			pNodeData->emType = LEADOUT_STATE;
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			strValue.Format(_T("Engine state: %s"),strStateType[pLeadOut->m_emEngineState]);
			HTREEITEM hStateItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hStateItem,(DWORD_PTR)pNodeData);
		}
	    break;
	case RemoteStand_Type:
		{
			RemoteStandPriority* pRemoteStand = pSurface->getRemoteStand();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = REMOTE_STAND;
			cni.net = NET_SHOW_DIALOGBOX;
			cni.unMenuID = IDR_MENU_PRIORITY;
			if(pRemoteStand->m_Stand == -1)
				strValue.Format(_T("Remote stand: All"));
			else
			{
				ALTObjectGroup altGroup;
				altGroup.ReadData(pRemoteStand->m_Stand);
				strValue.Format(_T("Remote stand: %s"),altGroup.getName().GetIDString());
			}

			HTREEITEM hRemoteItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hRemoteItem,(DWORD_PTR)pNodeData);
			m_vRemoteStand.push_back(hRemoteItem);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = NOTYPE;
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = 0;
			HTREEITEM hConItem = m_wndTreeCtrl.InsertItem(_T("Change conditions"),cni,FALSE,FALSE,hRemoteItem);
			m_wndTreeCtrl.SetItemData(hConItem,(DWORD_PTR)pNodeData);
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = RESTAND_WITHINTIME;
			strValue.Format(_T("Stand needed within mins: %d"),pRemoteStand->m_nStandWithinTime);
			cni.fMinValue = static_cast<float>(pRemoteStand->m_nStandWithinTime);
			HTREEITEM hWithinItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hWithinItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = RESTAND_FLUIDTIME;
			strValue.Format(_T("Wait for fluid vehicles greater than mins: %d"),pRemoteStand->m_nWaitFluidTime);
			cni.fMinValue = static_cast<float>(pRemoteStand->m_nWaitFluidTime);
			HTREEITEM hFluidItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hFluidItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = RESTAND_INSPECTIONTIME;
			strValue.Format(_T("Wait fro inspection greater than mins: %d"),pRemoteStand->m_nWaitInspectionTime);
			cni.fMinValue = static_cast<float>(pRemoteStand->m_nWaitInspectionTime);
			HTREEITEM hInspectionItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hConItem);
			m_wndTreeCtrl.SetItemData(hInspectionItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = RESTAND_METHOD;
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			strValue.Format(_T("Position method: %s"),strPosType[pRemoteStand->m_emType]);
			HTREEITEM hPosItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hPosItem,(DWORD_PTR)pNodeData);
			pNodeData = new CTreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pRemoteStand;
			pNodeData->emType = RESTAND_STATE;
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			strValue.Format(_T("Engine state: %s"),strStateType[pRemoteStand->m_emEngineState]);
			HTREEITEM hStateItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hStateItem,(DWORD_PTR)pNodeData);
		}
	    break;
	default:
	    break;
	}
}


LRESULT CDlgDe_icingAndAnti_icingStrategies::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			CString strData = _T("");
			switch(pNodeData->emType)
			{
			case DE_NUMBER:
				{
					IceVehicles* pData = (IceVehicles*)pNodeData->dwData;
					pData->m_nNumber = atoi(strValue);
					strData.Format(_T("Number: %s"),strValue);
				}
				break;
			case DE_FLOWRATE:
				{
					IceVehicles* pData = (IceVehicles*)pNodeData->dwData;
					pData->m_nFlowRate = atoi(strValue);
					strData.Format(_T("Flow Rate litres (USG/min): %s"),strValue);
				}
				break;
			case ANTI_NUMBER:
				{
					IceVehicles* pData = (IceVehicles*)pNodeData->dwData;
					pData->m_nNumber = atoi(strValue);
					strData.Format(_T("Number: %s"),strValue);
				}
			    break;
			case ANTI_FLOWRATE:
				{
					IceVehicles* pData = (IceVehicles*)pNodeData->dwData;
					pData->m_nFlowRate = atoi(strValue);
					strData.Format(_T("Flow Rate litres (USG/min): %s"),strValue);
				}
			    break;
			case PAD_QUEUE:
				{
					DeicePadPriority* pData = (DeicePadPriority*)pNodeData->dwData;
					pData->m_nDeicePadQueue = atoi(strValue);
					strData.Format(_T("Deiceing pad queue greater than aircraft or: %s"),strValue);
				}
				break;
			case PAD_WAITTIME:
				{
					DeicePadPriority* pData = (DeicePadPriority*)pNodeData->dwData;
					pData->m_nWaittime = atoi(strValue);
					strData.Format(_T("Wait time exceeds minutes: %s"),strValue);
				}
				break;
			case DEPSTAND_WITHINTIME:
				{
					DepStandPriority* pData = (DepStandPriority*)pNodeData->dwData;
					pData->m_nStandWithinTime = atoi(strValue);
					strData.Format(_T("Stand needed within mins: %s"),strValue);
				}
			    break;
			case DEPSTAND_FLUIDTIME:
				{
					DepStandPriority* pData = (DepStandPriority*)pNodeData->dwData;
					pData->m_nWaitFluidTime = atoi(strValue);
					strData.Format(_T("Wait for fluid vehicles greater than mins: %s"),strValue);
				}
			    break;
			case DEPSTAND_INSPECTIONTIME:
				{
					DepStandPriority* pData = (DepStandPriority*)pNodeData->dwData;
					pData->m_nWaitInspectionTime = atoi(strValue);
					strData.Format(_T("Wait fro inspection greater than mins: %s"),strValue);
				}
				break;
			case LEADOUT_DISTIANCE:
				{
					LeadOutPriority* pData = (LeadOutPriority*)pNodeData->dwData;
					pData->m_nDistance = atoi(strValue);
					strData.Format(_T("Lead Out Line from parking position: %s"),strValue);
				}
				break;
			case LEADOUT_WITHINTIME:
				{
					LeadOutPriority* pData = (LeadOutPriority*)pNodeData->dwData;
					pData->m_nStandWithinTime = atoi(strValue);
					strData.Format(_T("Stand needed within mins: %s"),strValue);
				}
				break;
			case LEADOUT_FLUIDTIME:
				{
					LeadOutPriority* pData = (LeadOutPriority*)pNodeData->dwData;
					pData->m_nWaitFluidTime = atoi(strValue);
					strData.Format(_T("Wait for fluid vehicles greater than mins: %s"),strValue);
				}
				break;
			case LEADOUT_INSPECTIONTIME:
				{
					LeadOutPriority* pData = (LeadOutPriority*)pNodeData->dwData;
					pData->m_nWaitInspectionTime = atoi(strValue);
					strData.Format(_T("Wait fro inspection greater than mins: %s"),strValue);
				}
				break;
			case RESTAND_WITHINTIME:
				{
					RemoteStandPriority* pData = (RemoteStandPriority*)pNodeData->dwData;
					pData->m_nStandWithinTime = atoi(strValue);
					strData.Format(_T("Stand needed within mins: %s"),strValue);
				}
				break;
			case RESTAND_FLUIDTIME:
				{
					RemoteStandPriority* pData = (RemoteStandPriority*)pNodeData->dwData;
					pData->m_nWaitFluidTime = atoi(strValue);
					strData.Format(_T("Wait for fluid vehicles greater than mins: %s"),strValue);
				}
				break;
			case RESTAND_INSPECTIONTIME:
				{
					RemoteStandPriority* pData = (RemoteStandPriority*)pNodeData->dwData;
					pData->m_nWaitInspectionTime = atoi(strValue);
					strData.Format(_T("Wait fro inspection greater than mins: %s"),strValue);
				}
				break;
			default:
			    break;
			}
			cni->fMinValue = static_cast<float>(atoi(strValue));
			m_wndTreeCtrl.SetItemText(hItem,strData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		case UM_CEW_COMBOBOX_BEGIN:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				switch(pNodeData->emType)
				{
				case PAD_METHOD:
				case RESTAND_METHOD:
					{
						CreatePositionMethod(hItem);
					}
					break;
				case PAD_STATE:
				case DEPSTAND_STATE:
				case LEADOUT_STATE:
				case RESTAND_STATE:
					{
						CreateEngineState(hItem);
					}
				default:
				    break;
				}
			}
			break;

		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				switch(pNodeData->emType)
				{
				case PAD_METHOD:
				case RESTAND_METHOD:
					{
						HandlePositionMethod(hItem);
					}
					break;
				case PAD_STATE:
				case DEPSTAND_STATE:
				case LEADOUT_STATE:
				case RESTAND_STATE:
					{
						HandleEngineState(hItem);
					}
				default:
					break;
				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				OnLButtonDblClk(wParam,lParam);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				return 0;
			}
			break;
	default:
		break;
	}

	 return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}

void CDlgDe_icingAndAnti_icingStrategies::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnUpdateToolbar();
}

void CDlgDe_icingAndAnti_icingStrategies::OnUpdateToolbar()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,FALSE);
	}
	else if (m_wndTreeCtrl.GetParentItem(hItem))
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,FALSE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,TRUE);
	}
}

void CDlgDe_icingAndAnti_icingStrategies::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(point, &uFlags);
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (std::find(m_vDepartureStand.begin(),m_vDepartureStand.end(),hItem) != m_vDepartureStand.end())
	{
		CDlgStandFamily dlgSelect(m_nProjID);

		if(dlgSelect.DoModal()==IDOK)
		{
			CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			CDeiceAndAnticeFlightType* pFlightType = (CDeiceAndAnticeFlightType*)pParentData->dwData;
			CDeiceAndAnticeStand* pStand = (CDeiceAndAnticeStand*)pNodeData->dwData;
			CString strNodeTitle = _T("");
			if(dlgSelect.GetSelStandFamilyID() == -1)
				strNodeTitle.Format(_T("Departure Stand(s): All"));
			else
				strNodeTitle.Format(_T("Departure Stand(s): %s"),dlgSelect.GetSelStandFamilyName());
			int nCount = pFlightType->getCount();
			for (int i = 0; i < nCount; i++)
			{
				CDeiceAndAnticeStand* pItem = pFlightType->getItem(i);
				if (dlgSelect.GetSelStandFamilyID() == pItem->getStand() && pItem != pStand)
				{
					::AfxMessageBox(_T("The stand is exist"));
					return;
				}
			}
			pStand->setStand( dlgSelect.GetSelStandFamilyID() );
			m_wndTreeCtrl.SetItemText(hItem,strNodeTitle);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pStand);
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	else if (std::find(m_vDeicePad.begin(),m_vDeicePad.end(),hItem) != m_vDeicePad.end())
	{
		CDlgDeicePadFamily dlgSelect(m_nProjID,this);
		COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
		if(dlgSelect.DoModal()==IDOK && cni->net == NET_SHOW_DIALOGBOX)
		{
			DeicePadPriority* pDeicePad = (DeicePadPriority*)pNodeData->dwData;
			CString strNodeTitle = _T("");
			if(dlgSelect.GetSelStandFamilyID() == -1)
				strNodeTitle.Format(_T("Deice pad: All"));
			else
				strNodeTitle.Format(_T("Deice pad: %s"),dlgSelect.GetSelStandFamilyName());
			pDeicePad->m_deicePad = dlgSelect.GetSelStandFamilyID();
			m_wndTreeCtrl.SetItemText(hItem,strNodeTitle);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pDeicePad);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
	else if (std::find(m_vDepStand.begin(),m_vDepStand.end(),hItem) != m_vDepStand.end())
	{
		CDlgStandFamily dlgSelect(m_nProjID);
		COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
		if(dlgSelect.DoModal()==IDOK && cni->net == NET_SHOW_DIALOGBOX)
		{
			DepStandPriority* pDepStand = (DepStandPriority*)pNodeData->dwData;
			CString strNodeTitle = _T("");
			if(dlgSelect.GetSelStandFamilyID() == -1)
				strNodeTitle.Format(_T("Dep Stand: All"));
			else
				strNodeTitle.Format(_T("Dep Stand: %s"),dlgSelect.GetSelStandFamilyName());
			pDepStand->m_Stand = dlgSelect.GetSelStandFamilyID();
			m_wndTreeCtrl.SetItemText(hItem,strNodeTitle);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pDepStand);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
	else if (std::find(m_vRemoteStand.begin(),m_vRemoteStand.end(),hItem) != m_vRemoteStand.end())
	{
		CDlgStandFamily dlgSelect(m_nProjID);
		COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
		if(dlgSelect.DoModal()==IDOK && cni->net == NET_SHOW_DIALOGBOX)
		{
			RemoteStandPriority* pRemoteStand = (RemoteStandPriority*)pNodeData->dwData;
			CString strNodeTitle = _T("");
			if(dlgSelect.GetSelStandFamilyID() == -1)
				strNodeTitle.Format(_T("Remote Stand: All"));
			else
				strNodeTitle.Format(_T("Remote Stand: %s"),dlgSelect.GetSelStandFamilyName());
			pRemoteStand->m_Stand = dlgSelect.GetSelStandFamilyID();
			m_wndTreeCtrl.SetItemText(hItem,strNodeTitle);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pRemoteStand);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

void CDlgDe_icingAndAnti_icingStrategies::CreateEngineState(HTREEITEM hItem)
{
	CRect rectWnd;
	CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
	m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
	pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
	CComboBox* pCB=(CComboBox*)pWnd;
	pCB->ResetContent();
	if (pCB->GetCount() == 0)
	{
		pCB->AddString(strStateType[0]);
		pCB->AddString(strStateType[1]);
	}
}

void CDlgDe_icingAndAnti_icingStrategies::CreatePositionMethod(HTREEITEM hItem)
{
	CRect rectWnd;
	CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
	m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
	pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
	CComboBox* pCB=(CComboBox*)pWnd;
	pCB->ResetContent();
	if (pCB->GetCount() == 0)
	{
		pCB->AddString(strPosType[0]);
		pCB->AddString(strPosType[1]);
		pCB->AddString(strPosType[2]);
	}
}

void CDlgDe_icingAndAnti_icingStrategies::HandlePositionMethod(HTREEITEM hItem)
{
	int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CString strValue = _T("");
	switch(pNodeData->emType)
	{
	case PAD_METHOD:
		{
			DeicePadPriority* pDeicePad = (DeicePadPriority*)pNodeData->dwData;
			pDeicePad->m_emType = (PostionMethod)nSel;
		}
		break;
	case RESTAND_METHOD:
		{
			RemoteStandPriority* pRemoteStand = (RemoteStandPriority*)pNodeData->dwData;
			pRemoteStand->m_emType = (PostionMethod)nSel;
		}
		break;
	default:
	    break;
	}
	strValue.Format(_T("Position method: %s"),strPosType[nSel]);
	m_wndTreeCtrl.SetItemText(hItem,strValue);
}

void CDlgDe_icingAndAnti_icingStrategies::HandleEngineState(HTREEITEM hItem)
{
	int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CString strValue = _T("");
	switch(pNodeData->emType)
	{
	case PAD_STATE:
		{
			DeicePadPriority* pDeicePad = (DeicePadPriority*)pNodeData->dwData;
			pDeicePad->m_emEngineState = (EngineState)nSel;
		}
		break;
	case DEPSTAND_STATE:
		{
			DepStandPriority* pDepStand = (DepStandPriority*)pNodeData->dwData;
			pDepStand->m_emEngineState = (EngineState)nSel;
		}
		break;
	case LEADOUT_STATE:
		{
			LeadOutPriority* pLeadOut = (LeadOutPriority*)pNodeData->dwData;
			pLeadOut->m_emEngineState = (EngineState)nSel;
		}
	    break;
	case RESTAND_STATE:
		{
			RemoteStandPriority* pRemoteStand = (RemoteStandPriority*)pNodeData->dwData;
			pRemoteStand->m_emEngineState = (EngineState)nSel;
		}
	    break;
	default:
	    break;
	}
	strValue.Format(_T("Engine state: %s"),strStateType[nSel]);
	m_wndTreeCtrl.SetItemText(hItem,strValue);
}

void CDlgDe_icingAndAnti_icingStrategies::ChangeDeicePad()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	CDeiceAndAnticeInitionalSurfaceCondition* pSurface = (CDeiceAndAnticeInitionalSurfaceCondition*)pParentData->dwData;
	DeicePadPriority* pDeicePad = pSurface->getDeicePad();
	switch(pNodeData->emType)
	{
	case DEP_STAND:
		{
			DepStandPriority* pDepStand = (DepStandPriority*)pNodeData->dwData;
			int nPriority = pDepStand->m_nPriority;
			pDepStand->m_nPriority = pDeicePad->m_nPriority;
			pDeicePad->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pDeicePad->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pDepStand->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pDepStand->m_nPriority),pSurface);
		}
		break;
	case LEADOUT_DISTIANCE:
		{
			LeadOutPriority* pLeadOut = (LeadOutPriority*)pNodeData->dwData;
			int nPriority = pLeadOut->m_nPriority;
			pLeadOut->m_nPriority = pDeicePad->m_nPriority;
			pDeicePad->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pDeicePad->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pLeadOut->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pLeadOut->m_nPriority),pSurface);
		}
	    break;
	case REMOTE_STAND:
		{
			RemoteStandPriority* pRemoteStand = (RemoteStandPriority*)pNodeData->dwData;
			int nPriority = pRemoteStand->m_nPriority;
			pRemoteStand->m_nPriority = pDeicePad->m_nPriority;
			pDeicePad->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pDeicePad->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pRemoteStand->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pRemoteStand->m_nPriority),pSurface);
		}
	    break;
	default:
	    break;
	}
}

void CDlgDe_icingAndAnti_icingStrategies::ChangeDepStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	CDeiceAndAnticeInitionalSurfaceCondition* pSurface = (CDeiceAndAnticeInitionalSurfaceCondition*)pParentData->dwData;
	DepStandPriority* pDepStand = pSurface->getDepStand();
	switch(pNodeData->emType)
	{
	case DEICE_PAD:
		{
			DeicePadPriority* pDeicePad = (DeicePadPriority*)pNodeData->dwData;
			int nPriority = pDeicePad->m_nPriority;
			pDeicePad->m_nPriority = pDepStand->m_nPriority;
			pDepStand->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pDepStand->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pDeicePad->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pDeicePad->m_nPriority),pSurface);
		}
		break;
	case LEADOUT_DISTIANCE:
		{
			LeadOutPriority* pLeadOut = (LeadOutPriority*)pNodeData->dwData;
			int nPriority = pLeadOut->m_nPriority;
			pLeadOut->m_nPriority = pDepStand->m_nPriority;
			pDepStand->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pDepStand->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pLeadOut->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pLeadOut->m_nPriority),pSurface);
		}
		break;
	case REMOTE_STAND:
		{
			RemoteStandPriority* pRemoteStand = (RemoteStandPriority*)pNodeData->dwData;
			int nPriority = pRemoteStand->m_nPriority;
			pRemoteStand->m_nPriority = pDepStand->m_nPriority;
			pDepStand->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pDepStand->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pRemoteStand->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pRemoteStand->m_nPriority),pSurface);
		}
		break;
	default:
		break;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::ChangeLeadOutLine()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	CDeiceAndAnticeInitionalSurfaceCondition* pSurface = (CDeiceAndAnticeInitionalSurfaceCondition*)pParentData->dwData;
	LeadOutPriority* pLeadOut = pSurface->getLeadOut();
	switch(pNodeData->emType)
	{
	case DEICE_PAD:
		{
			DeicePadPriority* pDeicePad = (DeicePadPriority*)pNodeData->dwData;
			int nPriority = pDeicePad->m_nPriority;
			pDeicePad->m_nPriority = pLeadOut->m_nPriority;
			pLeadOut->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pLeadOut->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pDeicePad->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pDeicePad->m_nPriority),pSurface);
		}
		break;
	case DEP_STAND:
		{
			DepStandPriority* pDepStand = (DepStandPriority*)pNodeData->dwData;
			int nPriority = pDepStand->m_nPriority;
			pDepStand->m_nPriority = pLeadOut->m_nPriority;
			pLeadOut->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pLeadOut->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pDepStand->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pDepStand->m_nPriority),pSurface);
		}
		break;
	case REMOTE_STAND:
		{
			RemoteStandPriority* pRemoteStand = (RemoteStandPriority*)pNodeData->dwData;
			int nPriority = pRemoteStand->m_nPriority;
			pRemoteStand->m_nPriority = pLeadOut->m_nPriority;
			pLeadOut->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pLeadOut->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pRemoteStand->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pRemoteStand->m_nPriority),pSurface);
		}
		break;
	default:
		break;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::ChangeRemote()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
	CTreeNodeData* pNodeData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CTreeNodeData* pParentData = (CTreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	CDeiceAndAnticeInitionalSurfaceCondition* pSurface = (CDeiceAndAnticeInitionalSurfaceCondition*)pParentData->dwData;
	RemoteStandPriority* pRemoteStand = pSurface->getRemoteStand();
	switch(pNodeData->emType)
	{
	case DEICE_PAD:
		{
			DeicePadPriority* pDeicePad = (DeicePadPriority*)pNodeData->dwData;
			int nPriority = pDeicePad->m_nPriority;
			pDeicePad->m_nPriority = pRemoteStand->m_nPriority;
			pRemoteStand->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pRemoteStand->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pDeicePad->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pDeicePad->m_nPriority),pSurface);
		}
		break;
	case LEADOUT_DISTIANCE:
		{
			LeadOutPriority* pLeadOut = (LeadOutPriority*)pNodeData->dwData;
			int nPriority = pLeadOut->m_nPriority;
			pLeadOut->m_nPriority = pRemoteStand->m_nPriority;
			pRemoteStand->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pRemoteStand->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pLeadOut->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pLeadOut->m_nPriority),pSurface);
		}
		break;
	case DEP_STAND:
		{
			DepStandPriority* pDepStand = (DepStandPriority*)pNodeData->dwData;
			int nPriority = pDepStand->m_nPriority;
			pDepStand->m_nPriority = pRemoteStand->m_nPriority;
			pRemoteStand->m_nPriority = nPriority;
			DeletePriorityItems(hParentItem);
			SetPriorityNodeContent(hParentItem,pSurface->getPriotity(pRemoteStand->m_nPriority),pSurface);
			HTREEITEM hPriorityItem = GetPriorityItem(hGrandItem,pDepStand->m_nPriority);
			DeletePriorityItems(hPriorityItem);
			SetPriorityNodeContent(hPriorityItem,pSurface->getPriotity(pDepStand->m_nPriority),pSurface);
		}
		break;
	default:
		break;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDe_icingAndAnti_icingStrategies::DeletePriorityItems(HTREEITEM hItem)
{
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while(hChildItem)
	{
		m_wndTreeCtrl.DeleteItem(hChildItem);
		hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	}
}

HTREEITEM CDlgDe_icingAndAnti_icingStrategies::GetPriorityItem(HTREEITEM hItem, int nPriority)
{
	CString strValue = _T("");
	CString strData = _T("");
	strValue.Format(_T("Priority%d"),nPriority);
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while(hChildItem)
	{
		strData = m_wndTreeCtrl.GetItemText(hChildItem);
		if (!strData.CompareNoCase(strValue))
		{
			return hChildItem;
		}
		hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hChildItem);
	}
	return NULL;
}
