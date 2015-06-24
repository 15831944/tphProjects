// FacilityDataPage.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TaxiQFacilityDataPage.h"
#include "../Landside/InputLandside.h"
#include "DlgSelectLandsideObject.h"
#include "ProcesserDialog.h"
#include "../Inputs/PROCIDList.h"
#include "DlgSelectLandsideVehicleType.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"
#include "DlgProbDist.h"
// CTaxiQFacilityDataPage dialog

IMPLEMENT_DYNAMIC(CTaxiQFacilityDataPage, CResizablePage)
CTaxiQFacilityDataPage::CTaxiQFacilityDataPage(InputLandside *pInputLandside,
									 InputTerminal* _pInTerm,
									 CFacilityBehaviorList *pBehaviorDB,
									 enum ALTOBJECT_TYPE enumFacType,
									 UINT nIDCaption,
									 int nProjectID)
									 : CResizablePage(CTaxiQFacilityDataPage::IDD, nIDCaption)
{
	m_enumType			= enumFacType;
	m_nProjectID		= nProjectID;
	m_pInputLandside	= pInputLandside;
	m_pTaxiQBehaviorDB		= pBehaviorDB;
	m_pInTerm			= _pInTerm;



}

CTaxiQFacilityDataPage::~CTaxiQFacilityDataPage()
{
}

void CTaxiQFacilityDataPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcDataPage)
	DDX_Control(pDX, IDC_STATIC_LINKAGE, m_staticLinkage);
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeData);
	DDX_Control(pDX, IDC_LIST_PROCESSOR, m_listboxFacility);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CTaxiQFacilityDataPage, CResizablePage)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_PROCESSOR_DATA_ADD, OnButtonAdd)
	ON_BN_CLICKED(ID_PROCESSOR_DATA_DEL, OnButtonDel)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_LINKAGE_ONETOONE, OnLinkageOnetoone)
	ON_LBN_SELCHANGE(IDC_LIST_PROCESSOR, OnSelchangeListFacility)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeData)
END_MESSAGE_MAP()


// CTaxiQFacilityDataPage message handlers

BOOL CTaxiQFacilityDataPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class
	int nCount = m_pTaxiQBehaviorDB->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CFacilityTaxiQBehavior *pTaxiQBehavior = (CFacilityTaxiQBehavior*)m_pTaxiQBehaviorDB->GetItem(i);
		if (pTaxiQBehavior == NULL)
			continue;
		if (pTaxiQBehavior->getFacilityType() == ALT_LTAXIQUEUE)
		{
			if (!pTaxiQBehavior->checkTotals())
			{
				MessageBox( "All branches must total 100%", "Error", MB_OK );
				return NULL;
			}
		}

	}	
	try
	{
		m_pTaxiQBehaviorDB->SaveData(m_nProjectID);
		SetModified(FALSE);
	}
	catch (CADOException& e)
	{
		CString strMsg = e.ErrorMessage();
		MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
	}
	return CResizablePage::OnApply();
}

void CTaxiQFacilityDataPage::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		m_pTaxiQBehaviorDB->SaveData(m_nProjectID);
	}
	catch (CADOException& e)
	{
		CString strMsg = e.ErrorMessage();
		MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
		return;
	}
	CResizablePage::OnOK();
}

void CTaxiQFacilityDataPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CResizablePage::OnCancel();
}



BOOL CTaxiQFacilityDataPage::OnInitDialog()
{
	CResizablePage::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolbar();
	InitSpiltter();
	//Left Wnd
	AddAnchor(m_ToolBarLeft, CSize(0,0),CSize(40,0));
	AddAnchor(IDC_LIST_PROCESSOR, TOP_LEFT,CSize(40,100));

	//Right Wnd
	AddAnchor(m_wndSplitter,CSize(40,0),CSize(40,100));
	AddAnchor(IDC_STATIC_LINKAGE,CSize(40,0),CSize(100,0));
	AddAnchor(m_ToolBar, CSize(40,0),CSize(100,0));
	AddAnchor(IDC_TREE_DATA, CSize(40,0),CSize(100,100));

	InitFacilityList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CTaxiQFacilityDataPage::InitToolbar()
{
	CRect rc;
	m_TreeData.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom=rc.top+26;
	rc.OffsetRect(0,-26);

	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);

	m_listboxFacility.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom=rc.top+26;
	rc.OffsetRect(0,-26);

	m_ToolBarLeft.MoveWindow(&rc);
	m_ToolBarLeft.ShowWindow(SW_SHOW);
	m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_ADD );
	m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_DEL );

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );		
	//if(m_nProcDataType==Elevator)
	//{
	//	m_ToolBar.GetToolBarCtrl().HideButton( ID_LINKAGE_ONETOONE);		
	//}
}
void CTaxiQFacilityDataPage::InitSpiltter()
{
	CRect rcList,rcTree,rect;

	CWnd* pWnd = GetDlgItem(IDC_LIST_PROCESSOR);
	pWnd->GetWindowRect(rcList);
	ScreenToClient(rcList);

	pWnd=GetDlgItem(IDC_TREE_DATA);
	pWnd->GetWindowRect(rcTree);
	ScreenToClient(rcTree);
	rect=CRect(rcList.right,rcList.top,rcTree.left,rcList.bottom);
	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, rect, this, IDC_STATIC_SPLITTER);
	SetSplitterRange();

}
void CTaxiQFacilityDataPage::SetSplitterRange()
{
	if(m_TreeData.m_hWnd&&m_wndSplitter.m_hWnd)
	{
		CRect rect;
		m_TreeData.GetWindowRect(rect);
		ScreenToClient(rect);
		m_wndSplitter.SetRange(20,rect.right-180);
		Invalidate();
	}
}
int CTaxiQFacilityDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CResizablePage::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_LINKAGETOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_ToolBarLeft.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBarLeft.LoadToolBar(IDR_ADDDEL_PROCESSOR_DATA))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CTaxiQFacilityDataPage::OnSize(UINT nType, int cx, int cy)
{
	CResizablePage::OnSize(nType, cx, cy);
	SetSplitterRange();
	// TODO: Add your message handler code here
}

void CTaxiQFacilityDataPage::OnButtonAdd()
{
	CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
	dlg.AddObjType(m_enumType);
	if( dlg.DoModal() == IDCANCEL )
		return;

	ALTObjectID selectFac = dlg.getSelectObject();

	//check if the Facility existed
	if(m_pTaxiQBehaviorDB->IsExisted(selectFac,m_enumType))
	{
		MessageBox("The selected object has been existed.",_T("Facility Behavior"), MB_OK);
		return;
	}

	CFacilityTaxiQBehavior *pNewBehavior = (CFacilityTaxiQBehavior*)m_pTaxiQBehaviorDB->AddNewFacility(m_enumType);
	pNewBehavior->setFacilityName(selectFac);
	int nIndex = AddFacilityToList(pNewBehavior);
	m_listboxFacility.SetCurSel(nIndex);

	LoadTreeProperties();
	UpdateFacilityToolbarState();

	SetModified();
}

void CTaxiQFacilityDataPage::OnButtonDel()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return;

	CFacilityTaxiQBehavior *pThisBehavior =(CFacilityTaxiQBehavior *)m_listboxFacility.GetItemData(nIdx);
	m_pTaxiQBehaviorDB->DeleteItem(pThisBehavior);
	m_listboxFacility.DeleteString( nIdx );

	//select next item
	if(nIdx > 0)
	{
		m_listboxFacility.SetCurSel(nIdx - 1);
		LoadTreeProperties();
	}
		
	else
	{
		if(m_listboxFacility.GetCount())
		{
			m_listboxFacility.SetCurSel(0);
			LoadTreeProperties();
		}
		else
		{
			LoadTreeProperties();
		}
	}

	SetModified();

	UpdateFacilityToolbarState();

}

void CTaxiQFacilityDataPage::OnToolbarbuttonadd()
{
	CFacilityTaxiQBehavior * pCurrentTaxiQBehavior = GetCurrentTaxiQBehavior();
	if(pCurrentTaxiQBehavior == NULL)
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	TaxiQTreeNodeData* pTaxiQNodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (pTaxiQNodeData == NULL)
		return;
	
	if(pTaxiQNodeData->m_emType == TaxiQTreeNodeData::Teminal_Node)
	{
		CProcesserDialog dlg( m_pInTerm );
		if( dlg.DoModal() == IDCANCEL )
			return;	

		ProcessorIDList idList;
		if( !dlg.GetProcessorIDList(idList) )
			return;	
		int nIDcount = idList.getCount();
		for(int i = 0; i < nIDcount; i++)
		{
			ProcessorID id = *(idList.getID(i));			

			CString strProc = id.GetIDString();
			//if( pProcID->getOne2OneFlag() )
			//	strcat(str ,"{1:1}");
			if(IsItemDuplicate(hSelItem, strProc))
			{
				MessageBox(_T("The processor selected has existed in the list."), _T("ARCPORT"), MB_OK);
				return;
			}
			CFacilityBehaviorLinkage *pLinkage  = new CFacilityBehaviorLinkage;
			pLinkage->setType(CFacilityBehaviorLinkage::enumLinkType_Terminal);
			pLinkage->setName(strProc);
			pCurrentTaxiQBehavior->getLinkageList()->AddItem(pLinkage);
			
			TaxiQTreeNodeData* pLinkageNodeData = new TaxiQTreeNodeData();
			pLinkageNodeData->m_emType = TaxiQTreeNodeData::TeminalLinkage_Node;
			pLinkageNodeData->m_dwData = (DWORD)pLinkage;
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hItem = m_TreeData.InsertItem( strProc,cni,FALSE,FALSE, hSelItem );
			m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkageNodeData );
			SetModified();
		}
		m_TreeData.Expand(hSelItem, TVE_EXPAND);

	}
	else if(pTaxiQNodeData->m_emType == TaxiQTreeNodeData::ParkingLot_Node)
	{
		CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LPARKINGLOT);
		if( dlg.DoModal() == IDCANCEL )
			return;

		ALTObjectID altObj = dlg.getSelectObject();
		CString strObj = altObj.GetIDString();
		if(IsItemDuplicate(hSelItem, strObj))
		{
			MessageBox(_T("The object selected has existed in the list."), _T("ARCPORT"), MB_OK);
			return;
		}

		CFacilityBehaviorLinkage *pLinkage  = new CFacilityBehaviorLinkage;
		pLinkage->setType(CFacilityBehaviorLinkage::enumLinkType_ParkingLot);
		pLinkage->setName(strObj);
		pCurrentTaxiQBehavior->getLinkageList()->AddItem(pLinkage);
		
		TaxiQTreeNodeData* pLinkageNodeData = new TaxiQTreeNodeData();
		pLinkageNodeData->m_emType = TaxiQTreeNodeData::ParkingLotLinkage_Node;
		pLinkageNodeData->m_dwData = (DWORD)pLinkage;
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hItem = m_TreeData.InsertItem( strObj,cni,FALSE,FALSE, hSelItem );
		m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkage );

		m_TreeData.Expand(hSelItem, TVE_EXPAND);
		SetModified();

	}
	else if(pTaxiQNodeData->m_emType == TaxiQTreeNodeData::External_Node)
	{
		CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LEXT_NODE);
		if( dlg.DoModal() == IDCANCEL )
			return;

		ALTObjectID altObj = dlg.getSelectObject();
		CString strObj = altObj.GetIDString();
		if(IsItemDuplicate(hSelItem, strObj))
		{
			MessageBox(_T("The object selected has existed in the list."), _T("ARCPORT"), MB_OK);
			return;
		}
		CCallFromExternal* pCallFromExternal = new CCallFromExternal;
		pCallFromExternal->SetEntry(altObj);
		pCurrentTaxiQBehavior->getCallFromExternal()->AddItem(pCallFromExternal);
		EvenPercent();
		LoadTreeProperties();
	/*	m_TreeData.SelectItem(hSelItem);*/
		UpdateFacilityToolbarState();
	}
}

void CTaxiQFacilityDataPage::OnToolbarbuttondel()
{
	CFacilityTaxiQBehavior * pCurrentBehavior = GetCurrentTaxiQBehavior();
	if(pCurrentBehavior == NULL)
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	TaxiQTreeNodeData* pTaxiQNodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (pTaxiQNodeData == NULL)
		return;
	if (pTaxiQNodeData->m_emType == TaxiQTreeNodeData::TeminalLinkage_Node || pTaxiQNodeData->m_emType == TaxiQTreeNodeData::ParkingLotLinkage_Node)
	{
		CFacilityBehaviorLinkage *pLinkage = (CFacilityBehaviorLinkage *)pTaxiQNodeData->m_dwData;
		if(pLinkage == NULL)
			return;

		HTREEITEM hNextSelItem = m_TreeData.GetNextSiblingItem(hSelItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_TreeData.GetPrevSiblingItem(hSelItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_TreeData.GetParentItem(hSelItem);

		m_TreeData.DeleteItem(hSelItem);
		pCurrentBehavior->getLinkageList()->DeleteItem(pLinkage);

		if(hNextSelItem)
			m_TreeData.SelectItem(hNextSelItem);

		UpdateFacilityToolbarState();
		SetModified();
	} 
	else if (pTaxiQNodeData->m_emType == TaxiQTreeNodeData::Entry_Node)
	{
		CCallFromExternal* pCallExternal = (CCallFromExternal*)pTaxiQNodeData->m_dwData;
		if (pCallExternal == NULL)
			return;
		
		m_TreeData.DeleteItem(hSelItem);
		pCurrentBehavior->getCallFromExternal()->DeleteItem(pCallExternal);

		EvenPercent();
		LoadTreeProperties();
		UpdateFacilityToolbarState();
		SetModified();
	}
}

void CTaxiQFacilityDataPage::OnLinkageOnetoone()
{
	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	TaxiQTreeNodeData* pTaxiQNodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (pTaxiQNodeData == NULL)
		return;
	CFacilityBehaviorLinkage *pLinkage = (CFacilityBehaviorLinkage *)pTaxiQNodeData->m_dwData;
	if(pLinkage == NULL)
		return;

	pLinkage->set1to1(!pLinkage->is1To1());
	CString strItemText = pLinkage->getName();

	if(pLinkage->is1To1())
		strItemText +=(_T("(1:1)"));

	m_TreeData.SetItemText(hSelItem,strItemText);

	SetModified();
}

void CTaxiQFacilityDataPage::InitFacilityList()
{
	int nCount = m_pTaxiQBehaviorDB->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CFacilityTaxiQBehavior *pBehavior = (CFacilityTaxiQBehavior*)m_pTaxiQBehaviorDB->GetItem(nItem);
		if (pBehavior && pBehavior->getFacilityType() == m_enumType)
		{
			AddFacilityToList(pBehavior);
		}
				
	}
	if(nCount > 0)
		m_listboxFacility.SetCurSel(0);

	LoadTreeProperties();
	UpdateFacilityToolbarState();
}

int CTaxiQFacilityDataPage::AddFacilityToList( CFacilityTaxiQBehavior *pBehavior )
{
	int nIndex = m_listboxFacility.AddString(pBehavior->getFacilityName().GetIDString());
	m_listboxFacility.SetItemData(nIndex, (DWORD_PTR)pBehavior);
	return nIndex;
}

void CTaxiQFacilityDataPage::UpdateFacilityToolbarState()
{
	if(m_listboxFacility.GetCurSel() == LB_ERR)
	{
		m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_DEL, FALSE );
	}
	else
	{
		m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_DEL, TRUE );
	}

	UpdateTreeToolbarState();
}

void CTaxiQFacilityDataPage::OnSelchangeListFacility()
{
	LoadTreeProperties();
}

void CTaxiQFacilityDataPage::LoadTreeProperties()
{

	m_TreeData.DeleteAllItems();

	int nCurSel = m_listboxFacility.GetCurSel();
	if(nCurSel == LB_ERR)
	{
		return;
	}

	CFacilityTaxiQBehavior *pCurBehavior = GetCurrentTaxiQBehavior();
	if(pCurBehavior == NULL)
	{
		return;
	}

	CFacilityBehaviorLinkageList *pLinkageList = pCurBehavior->getLinkageList();
	//linkage 
	TaxiQTreeNodeData* pLinkageNodeData = new TaxiQTreeNodeData();
	pLinkageNodeData->m_emType = TaxiQTreeNodeData::Linkage_Node;
	pLinkageNodeData->m_dwData = (DWORD)pLinkageList;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hLinkage = m_TreeData.InsertItem( _T("Linkage"),cni,FALSE,FALSE, TVI_ROOT);
	m_TreeData.SetItemData(hLinkage, (DWORD_PTR)pLinkageNodeData);

	//Terminal processors
	TaxiQTreeNodeData* pTeminalNodeData = new TaxiQTreeNodeData();
	pTeminalNodeData->m_emType = TaxiQTreeNodeData::Teminal_Node;
	pTeminalNodeData->m_dwData = (DWORD)pLinkageList;
	HTREEITEM hTerminalProc = m_TreeData.InsertItem(_T("Terminal Processors"), cni,FALSE,FALSE, hLinkage);
	m_TreeData.SetItemData(hTerminalProc, (DWORD_PTR)pTeminalNodeData);

	//parking lot
	TaxiQTreeNodeData* pParkingLotNodeData = new TaxiQTreeNodeData();
	pParkingLotNodeData->m_emType = TaxiQTreeNodeData::ParkingLot_Node;
	pParkingLotNodeData->m_dwData = (DWORD)pLinkageList;
	HTREEITEM hParkingLot = m_TreeData.InsertItem(_T("Parking Lot"), cni,FALSE,FALSE, hLinkage);
	m_TreeData.SetItemData(hParkingLot, (DWORD_PTR)pParkingLotNodeData);	

	int nLinkCount =  pLinkageList->GetItemCount();
	for (int nLink = 0 ; nLink < nLinkCount; ++ nLink)
	{
		CFacilityBehaviorLinkage *pLinkage = pLinkageList->GetItem(nLink);

		CString strItemText = pLinkage->getName();

		if(pLinkage->is1To1())
			strItemText +=(_T("(1:1)"));
		
		TaxiQTreeNodeData* pLinkObjectNodeData = new TaxiQTreeNodeData();		
		pLinkObjectNodeData->m_dwData = (DWORD)pLinkage;
		HTREEITEM hParentItem = NULL;
		if(pLinkage->getType() == CFacilityBehaviorLinkage::enumLinkType_Terminal)
		{
			hParentItem = hTerminalProc;
			pLinkObjectNodeData->m_emType = TaxiQTreeNodeData::TeminalLinkage_Node;
		}
		else if(pLinkage->getType() == CFacilityBehaviorLinkage::enumLinkType_ParkingLot)
		{
			hParentItem = hParkingLot;
			pLinkObjectNodeData->m_emType = TaxiQTreeNodeData::ParkingLotLinkage_Node;
		}			
		else
		{
			hParentItem = NULL;
			ASSERT(0);
		}
		HTREEITEM hItem = m_TreeData.InsertItem( strItemText, cni,FALSE,FALSE, hParentItem);	
		m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkObjectNodeData);
	}
	m_TreeData.Expand(hLinkage, TVE_EXPAND);
	m_TreeData.Expand(hParkingLot, TVE_EXPAND);
	m_TreeData.Expand(hTerminalProc, TVE_EXPAND);

	//capacity
	cni.nt	=	NT_CHECKBOX;
	cni.net =	NET_EDITSPIN_WITH_VALUE;	
// 	TaxiQTreeNodeData* pCapacityNodeData = new TaxiQTreeNodeData();
// 	pCapacityNodeData->m_emType = TaxiQTreeNodeData::Capacity_Node;
// 	pCapacityNodeData->m_dwData = (DWORD)pCurBehavior->getCapacity();
	CString strCapacity;
	strCapacity.Format(_T("Capacity: %d"), pCurBehavior->getCapacity());
	m_hCapacity = m_TreeData.InsertItem(strCapacity,cni,FALSE,FALSE, TVI_ROOT);
	if(pCurBehavior->isCapacityLimit())
	{
		m_TreeData.SetCheckStatus(m_hCapacity,  TRUE);
	}
	else
	{
		m_TreeData.SetCheckStatus(m_hCapacity,  FALSE);
	}
	m_TreeData.SetItemData(m_hCapacity,(DWORD_PTR)pCurBehavior->getCapacity());

	//serve taxi
// 	TaxiQTreeNodeData* pServeTaxiNodeData = new TaxiQTreeNodeData();
// 	pServeTaxiNodeData->m_emType = TaxiQTreeNodeData::ServeTaxi_Node;
// 	pServeTaxiNodeData->m_dwData = (DWORD)pCurBehavior->GetServeTaxi();
	cni.nt = NT_NORMAL;
	CString ServeTaxi;
	ServeTaxi.Format(_T("Simultaneous Serve Taxi: %d"), pCurBehavior->GetServeTaxi());
	m_hServeTaxi = m_TreeData.InsertItem(ServeTaxi,cni,FALSE,FALSE, TVI_ROOT);
	m_TreeData.SetItemData(m_hServeTaxi,(DWORD_PTR)pCurBehavior->GetServeTaxi());
	
	//Vehicle type
	TaxiQTreeNodeData* pVehicleTypeNodeData = new TaxiQTreeNodeData();
	pVehicleTypeNodeData->m_emType = TaxiQTreeNodeData::VehicleType_Node;
	pVehicleTypeNodeData->m_dwData = (DWORD)(LPCTSTR)pCurBehavior->GetVehicleType();
	CString strVehicle ;
	strVehicle.Format("Vehicle Type: %s",pCurBehavior->GetVehicleType());
	cni.net = NET_SHOW_DIALOGBOX;
	HTREEITEM hVehicleType = m_TreeData.InsertItem(strVehicle,cni,FALSE,FALSE,TVI_ROOT);
	m_TreeData.SetItemData(hVehicleType,(DWORD_PTR)pVehicleTypeNodeData);

	//source
	TaxiQTreeNodeData* psourceNodeData = new TaxiQTreeNodeData();
	psourceNodeData->m_emType = TaxiQTreeNodeData::Source_Node;
	cni.net = NET_NORMAL;
	HTREEITEM hsource = m_TreeData.InsertItem( _T("Source"),cni,FALSE,FALSE, TVI_ROOT);
	m_TreeData.SetItemData(hsource, (DWORD_PTR)psourceNodeData);

	//have pool
	TaxiQTreeNodeData* pHavePoolNodeData = new TaxiQTreeNodeData();
	pHavePoolNodeData->m_emType = TaxiQTreeNodeData::HavePool_Node;
	HTREEITEM hHavePool = m_TreeData.InsertItem( _T("Case 1: have pool"),cni,FALSE,FALSE, hsource);
	m_TreeData.SetItemData(hHavePool, (DWORD_PTR)pHavePoolNodeData);

	//pool rules
	TaxiQTreeNodeData* pPoolRuleNodeData = new TaxiQTreeNodeData();
	pPoolRuleNodeData->m_emType = TaxiQTreeNodeData::PoolRule_Node;
	HTREEITEM hPoolRule = m_TreeData.InsertItem( _T("One or more pools with rules"),cni,FALSE,FALSE, hHavePool);
	m_TreeData.SetItemData(hPoolRule, (DWORD_PTR)pPoolRuleNodeData);

	//fifo
	TaxiQTreeNodeData* pFifoNodeData = new TaxiQTreeNodeData();
	pFifoNodeData->m_emType = TaxiQTreeNodeData::FiFo_Node;
	cni.nt = NT_RADIOBTN;
	HTREEITEM hFifo = m_TreeData.InsertItem( _T("FIFO"),cni,FALSE,FALSE, hPoolRule);
	m_TreeData.SetItemData(hFifo, (DWORD_PTR)pFifoNodeData);
	if (pCurBehavior->GetTaxiPoolType() == CFacilityTaxiQBehavior::TaxiPoolType::TaxiPool_FiFo)
	{
		m_TreeData.SetRadioStatus(hFifo,TRUE);
	} 
	else
	{
		m_TreeData.SetRadioStatus(hFifo,FALSE);
	}

	//longest queue
	TaxiQTreeNodeData* pQueueNodeData = new TaxiQTreeNodeData();
	pQueueNodeData->m_emType = TaxiQTreeNodeData::LongQueue_Node;
	cni.nt = NT_RADIOBTN;
	HTREEITEM hqueue = m_TreeData.InsertItem( _T("Longest queue"),cni,FALSE,FALSE, hPoolRule);
	m_TreeData.SetItemData(hqueue, (DWORD_PTR)pQueueNodeData);
	if (pCurBehavior->GetTaxiPoolType() == CFacilityTaxiQBehavior::TaxiPoolType::TaxiPool_LongQueue)
	{
		m_TreeData.SetRadioStatus(hqueue,TRUE);
	} 
	else
	{
		m_TreeData.SetRadioStatus(hqueue,FALSE);
	}

	//dedicted source
	TaxiQTreeNodeData* pdedictedNodeData = new TaxiQTreeNodeData();
	pdedictedNodeData->m_emType = TaxiQTreeNodeData::Dedicted_Node;
	cni.nt = NT_RADIOBTN;
	cni.net = NET_SHOW_DIALOGBOX;
	CString dedicted;
	dedicted.Format( _T("Dedicted source: %s"),pCurBehavior->GetTaxiPool().GetIDString());
	HTREEITEM hdedicted = m_TreeData.InsertItem( dedicted,cni,FALSE,FALSE, hPoolRule);
	m_TreeData.SetItemData(hdedicted, (DWORD_PTR)pdedictedNodeData);
	if (pCurBehavior->GetTaxiPoolType() == CFacilityTaxiQBehavior::TaxiPoolType::TaxiPool_Dedicted)
	{
		m_TreeData.SetRadioStatus(hdedicted,TRUE);
	} 
	else
	{
		m_TreeData.SetRadioStatus(hdedicted,FALSE);
	}

	//no pool
	TaxiQTreeNodeData* pNoPoolNodeData = new TaxiQTreeNodeData();
	pNoPoolNodeData->m_emType = TaxiQTreeNodeData::NoPool_Node;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hNoPool = m_TreeData.InsertItem( _T("Case 2: No pool"),cni,FALSE,FALSE, hsource);
	m_TreeData.SetItemData(hNoPool, (DWORD_PTR)pNoPoolNodeData);

	//call from external
	TaxiQTreeNodeData* pexternalNodeData = new TaxiQTreeNodeData();
	pexternalNodeData->m_emType = TaxiQTreeNodeData::External_Node;
	HTREEITEM hexternal = m_TreeData.InsertItem( _T("Call from external"),cni,FALSE,FALSE, hNoPool);
	m_TreeData.SetItemData(hexternal, (DWORD_PTR)pexternalNodeData);

	int nCount = pCurBehavior->getCallFromExternal()->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CCallFromExternal* External = pCurBehavior->getCallFromExternal()->GetItem(i);
		//entry 
		TaxiQTreeNodeData* pentryNodeData = new TaxiQTreeNodeData();
		pentryNodeData->m_emType = TaxiQTreeNodeData::Entry_Node;
		pentryNodeData->m_dwData = (DWORD)External;		
		cni.net = NET_EDIT_INT;	
		cni.fMinValue = static_cast<float>(External->Getpercent());
		CString sPercent;
		sPercent.Format("( %d%% )", External->Getpercent());	
		CString entry = External->GetEntry().GetIDString();
		HTREEITEM hentry = m_TreeData.InsertItem(entry+sPercent,cni,FALSE,FALSE,hexternal);
		m_TreeData.SetItemData(hentry, (DWORD_PTR)pentryNodeData);

		//offset
		TaxiQTreeNodeData* poffsetNodeData = new TaxiQTreeNodeData();
		poffsetNodeData->m_emType = TaxiQTreeNodeData::Offset_Node;
		poffsetNodeData->m_dwData = (DWORD)External->GetOffset();
		cni.net = NET_COMBOBOX;
		CString offset;
		offset.Format(_T("Entry Offset(min): %s"),External->GetOffset()->getDistName());
		HTREEITEM hOffset = m_TreeData.InsertItem(offset,cni,FALSE,FALSE,hentry);
		m_TreeData.SetItemData(hOffset,(DWORD_PTR)poffsetNodeData);

		//delay
		TaxiQTreeNodeData* pdelayNodeData = new TaxiQTreeNodeData();
		pdelayNodeData->m_emType = TaxiQTreeNodeData::Delay_Node;
		pdelayNodeData->m_dwData = (DWORD)External->GetDelay();
		CString delay;
		delay.Format(_T("Delay Distribution(min): %s"),External->GetDelay()->getDistName());
		HTREEITEM hDelay = m_TreeData.InsertItem(delay,cni,FALSE,FALSE,hentry);
		m_TreeData.SetItemData(hDelay,(DWORD_PTR)pdelayNodeData);

		m_TreeData.Expand(hentry,TVE_EXPAND);
	}
	m_TreeData.Expand(hsource,TVE_EXPAND);
	m_TreeData.Expand(hHavePool,TVE_EXPAND);
	m_TreeData.Expand(hPoolRule,TVE_EXPAND);
	m_TreeData.Expand(hNoPool,TVE_EXPAND);
	m_TreeData.Expand(hexternal,TVE_EXPAND);

}

void CTaxiQFacilityDataPage::UpdateTreeToolbarState()
{
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );

	if(m_listboxFacility.GetCurSel() == LB_ERR )
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;

	if (hSelItem == m_hServeTaxi)
		return;

	TaxiQTreeNodeData* pTaxiQNodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (pTaxiQNodeData == NULL)
		return;
	if(pTaxiQNodeData->m_emType ==  TaxiQTreeNodeData::Teminal_Node || pTaxiQNodeData->m_emType == TaxiQTreeNodeData::ParkingLot_Node 
		|| pTaxiQNodeData->m_emType == TaxiQTreeNodeData::External_Node)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE );
	}
	if(pTaxiQNodeData->m_emType ==  TaxiQTreeNodeData::TeminalLinkage_Node ||  pTaxiQNodeData->m_emType ==  TaxiQTreeNodeData::ParkingLotLinkage_Node )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);	
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, TRUE);
	}
	if (pTaxiQNodeData->m_emType ==  TaxiQTreeNodeData::Entry_Node )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);
	}
}

void CTaxiQFacilityDataPage::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateTreeToolbarState();
	*pResult = 0;
}

CFacilityTaxiQBehavior * CTaxiQFacilityDataPage::GetCurrentTaxiQBehavior()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return NULL;
	
	CFacilityTaxiQBehavior *pThisBehavior =(CFacilityTaxiQBehavior *)m_listboxFacility.GetItemData(nIdx);
	return pThisBehavior;

}



LRESULT CTaxiQFacilityDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		TaxiQTreeNodeData* NodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hItem);
		CFacilityTaxiQBehavior *pCurBehavior = GetCurrentTaxiQBehavior();
		if (NodeData->m_emType == TaxiQTreeNodeData::VehicleType_Node)
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strGroupName = dlg.GetName();
				if (!strGroupName.IsEmpty())//non empty
				{
					if (strGroupName.CompareNoCase(pCurBehavior->GetVehicleType()) != 0)//not the same name as the old name
					{
						pCurBehavior->SetVehicleType(strGroupName);
						CString strVehicle ;
						strVehicle.Format("Vehicle Type: %s",pCurBehavior->GetVehicleType());
						m_TreeData.SetItemText(hItem,strVehicle);
						SetModified();
					}
				}
			}
		}
		if (NodeData->m_emType == TaxiQTreeNodeData::Dedicted_Node)
		{
			CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
			dlg.AddObjType(ALT_LTAXIPOOL);
			if( dlg.DoModal() == IDOK )
			{
				ALTObjectID selectTaxiPool = dlg.getSelectObject();
				if (!selectTaxiPool.GetIDString().IsEmpty())
				{
					pCurBehavior->SetTaxiPool(selectTaxiPool);
					CString dedicted;
					dedicted.Format( _T("Dedicted source: %s"),pCurBehavior->GetTaxiPool().GetIDString());
					m_TreeData.SetItemText(hItem,dedicted);
					SetModified();
				}
			}

		}
	}
	if(message == UM_CEW_SHOW_DIALOGBOX_END)
	{
		UpdateTreeToolbarState();
	}
	if (message == UM_CEW_EDITSPIN_BEGIN)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo(hItem);
		pInfo->fMinValue = 0;
	}
	if(message==UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		CFacilityTaxiQBehavior *pCurBehavior = GetCurrentTaxiQBehavior();
		TaxiQTreeNodeData* NodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hItem);
		CString strValue=*((CString*)lParam);
		int nValue=static_cast<int>(atoi(strValue));
		if(hItem == m_hCapacity)
		{
			if(pCurBehavior != NULL)
			{
				pCurBehavior->setCapacity(nValue);
				CString strCapacity;
				strCapacity.Format(_T("Capacity: %d"), pCurBehavior->getCapacity());
				m_TreeData.SetItemText(hItem, strCapacity);
				m_TreeData.SetItemData(hItem, (DWORD_PTR)pCurBehavior->getCapacity());
				SetModified();
			}
		}
		else if(hItem == m_hServeTaxi )
		{
			if (pCurBehavior != NULL)
			{
				pCurBehavior->SetServeTaxi(nValue);
				CString strServeTaxi;
				strServeTaxi.Format(_T("Simultaneous Serve Taxi: %d"), pCurBehavior->GetServeTaxi());
				m_TreeData.SetItemText(hItem, strServeTaxi);
				m_TreeData.SetItemData(hItem, (DWORD_PTR)pCurBehavior->GetServeTaxi());
				SetModified();
			}
		}
		else if (NodeData->m_emType == TaxiQTreeNodeData::Entry_Node)
		{
			COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
			if(pCurBehavior != NULL)
			{
				CCallFromExternal* pCallExternal = (CCallFromExternal*)NodeData->m_dwData;
				pCallExternal->SetPercent(nValue);

				CString sPercent;
				sPercent.Format("( %d%% )", pCallExternal->Getpercent());	
				CString entry = pCallExternal->GetEntry().GetIDString();
				pInfo->fMinValue = static_cast<float>(pCallExternal->Getpercent());
				m_TreeData.SetItemText(hItem,entry+sPercent);
				TaxiQTreeNodeData* NodeData = new TaxiQTreeNodeData();
				NodeData->m_emType = TaxiQTreeNodeData::Entry_Node;
				NodeData->m_dwData = (DWORD)pCallExternal;
				m_TreeData.SetItemData(hItem,(DWORD_PTR)NodeData);
			}
		}
	}
	if(message == UM_CEW_STATUS_CHANGE)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		TaxiQTreeNodeData* NodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hItem);
		CFacilityTaxiQBehavior *pCurBehavior = GetCurrentTaxiQBehavior();
		if (hItem == m_hCapacity)
		{
			BOOL b = m_TreeData.IsCheckItem(hItem);			
			if(pCurBehavior != NULL)
			{
				if (b)
				{
					pCurBehavior->setCapacityLimit(TRUE);
				} 
				else
				{
					pCurBehavior->setCapacityLimit(FALSE);
				}
				SetModified();
			}
		} 
		if (NodeData->m_emType == TaxiQTreeNodeData::FiFo_Node)
		{
			pCurBehavior->SetTaxiPoolType(CFacilityTaxiQBehavior::TaxiPool_FiFo);
		}
		if (NodeData->m_emType == TaxiQTreeNodeData::LongQueue_Node)
		{
			pCurBehavior->SetTaxiPoolType(CFacilityTaxiQBehavior::TaxiPool_LongQueue);
		}
		if (NodeData->m_emType == TaxiQTreeNodeData::Dedicted_Node)
		{
			pCurBehavior->SetTaxiPoolType(CFacilityTaxiQBehavior::TaxiPool_Dedicted);
		}
	}
	if(message == UM_CEW_COMBOBOX_BEGIN)
	{
		CWnd* pWnd = m_TreeData.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		HTREEITEM hItem = (HTREEITEM)wParam;
		m_TreeData.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;		

		if(pCB->GetCount()!=0)
		{
			pCB->ResetContent();
		}

		CString s;
		s.LoadString(IDS_STRING_NEWDIST);
		pCB->AddString(s);

		int nCount = _g_GetActiveProbMan(  m_pInputLandside->getInputTerminal() )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInputLandside->getInputTerminal() )->getItem( m );		
			pCB->AddString(pPBEntry->m_csName);
		}
	}	
	if(message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		CFacilityTaxiQBehavior *pCurBehavior = GetCurrentTaxiQBehavior();

		HTREEITEM hItem=(HTREEITEM)wParam;
		
		int iSelCombox = m_TreeData.GetCmbBoxCurSel(hItem);
		ASSERT(iSelCombox >= 0);
		TaxiQTreeNodeData* NodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(hItem);
		
		HTREEITEM parentItem = m_TreeData.GetParentItem(hItem);
		
		TaxiQTreeNodeData* ParentNodeData = (TaxiQTreeNodeData*)m_TreeData.GetItemData(parentItem);
		
		CCallFromExternal* pExternal = (CCallFromExternal*)ParentNodeData->m_dwData;
		
		
		if(NodeData->m_emType == TaxiQTreeNodeData::Offset_Node)
		{
			if(iSelCombox == 0)
			{
				CDlgProbDist dlg(  m_pInputLandside->getInputTerminal()->m_pAirportDB, true );
				if(dlg.DoModal()==IDOK) 
				{
					int idxPD = dlg.GetSelectedPDIdx();
					ASSERT(idxPD!=-1);
					CProbDistEntry* pde = _g_GetActiveProbMan( m_pInputLandside->getInputTerminal() )->getItem(idxPD);
					pExternal->SetOffset(pde);
					CString delay;
					delay.Format(_T("Entry Offset(min): %s"),pExternal->GetOffset()->getDistName());
					m_TreeData.SetItemText(hItem,delay);
					SetModified();
				}
			}
			else
			{
				ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( m_pInputLandside->getInputTerminal()  )->getCount()) );
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInputLandside->getInputTerminal()  )->getItem( iSelCombox-1 );
				pExternal->SetOffset(pPBEntry);
				CString delay;
				delay.Format(_T("Entry Offset(min): %s"),pExternal->GetOffset()->getDistName());
				m_TreeData.SetItemText(hItem,delay);
				SetModified();
			}
		}
		if(NodeData->m_emType == TaxiQTreeNodeData::Delay_Node)
		{
			if(iSelCombox == 0)
			{
				CDlgProbDist dlg(  m_pInputLandside->getInputTerminal()->m_pAirportDB, true );
				if(dlg.DoModal()==IDOK) 
				{
					int idxPD = dlg.GetSelectedPDIdx();
					ASSERT(idxPD!=-1);
					CProbDistEntry* pde = _g_GetActiveProbMan( m_pInputLandside->getInputTerminal() )->getItem(idxPD);
					pExternal->SetDelay(pde);
					CString delay;
					delay.Format(_T("Delay Distribution(min): %s"),pExternal->GetDelay()->getDistName());
					m_TreeData.SetItemText(hItem,delay);
					SetModified();
				}
			}
			else
			{
				ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( m_pInputLandside->getInputTerminal()  )->getCount()) );
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInputLandside->getInputTerminal()  )->getItem( iSelCombox-1 );
				pExternal->SetDelay(pPBEntry);
				CString delay;
				delay.Format(_T("Delay Distribution(min): %s"),pExternal->GetDelay()->getDistName());
				m_TreeData.SetItemText(hItem,delay);
				SetModified();
			}	
		}

	}
	if(message == UM_CEW_COMBOBOX_END)
	{

	}
	return CResizablePage::DefWindowProc(message, wParam, lParam);
}

bool CTaxiQFacilityDataPage::IsItemDuplicate( HTREEITEM hParentItem,const CString& strName )
{
	TaxiQTreeNodeData *NodeData = (TaxiQTreeNodeData *)m_TreeData.GetItemData(hParentItem);
	if (NodeData == NULL)
		return NULL;
	HTREEITEM hItem = m_TreeData.GetChildItem(hParentItem);
	if (NodeData->m_emType == TaxiQTreeNodeData::External_Node)
	{
		while(hItem)
		{

			TaxiQTreeNodeData *NodeData = (TaxiQTreeNodeData *)m_TreeData.GetItemData(hItem);
			if(NodeData == NULL)
				continue;
			CCallFromExternal *pCallFromExternal = (CCallFromExternal*)NodeData->m_dwData;
			if(pCallFromExternal == NULL)
				continue;
			if(pCallFromExternal->GetEntry().GetIDString().CompareNoCase(strName) == 0)
				return true;

			hItem = m_TreeData.GetNextSiblingItem(hItem);
		}
	}
	if (NodeData->m_emType == TaxiQTreeNodeData::Teminal_Node || NodeData->m_emType == TaxiQTreeNodeData::ParkingLot_Node)
	{
		while(hItem)
		{
			TaxiQTreeNodeData *NodeData = (TaxiQTreeNodeData *)m_TreeData.GetItemData(hItem);
			if(NodeData == NULL)
				continue;
			CFacilityBehaviorLinkage* pLinkage = (CFacilityBehaviorLinkage*)NodeData->m_dwData;
			if(pLinkage == NULL)
				continue;
			if(pLinkage->getName().CompareNoCase(strName) == 0)
				return true;

			hItem = m_TreeData.GetNextSiblingItem(hItem);
		}
	}
	return false;
}

void CTaxiQFacilityDataPage::EvenPercent()
{
	CFacilityTaxiQBehavior *pCurTaxiQBehavior = GetCurrentTaxiQBehavior();
	CCallFromExternalist* Externalist = pCurTaxiQBehavior->getCallFromExternal();
	int iSize = Externalist->GetItemCount();
	if( iSize > 0 )
	{		
		int iPercent = 100 / iSize ;
		if( 100 % iSize == 0 )
		{

			for( int i=0; i< iSize; ++i )
			{
				CCallFromExternal* External = Externalist->GetItem(i);
				External->SetPercent( iPercent );
			}
		}
		else
		{
			for( int i=0; i< iSize-1; ++i )
			{
				CCallFromExternal* External = Externalist->GetItem(i);
				External->SetPercent( iPercent );
			}
			Externalist->GetItem(iSize-1)->SetPercent( 100- iPercent*(iSize-1) );
		}		
	}
}
