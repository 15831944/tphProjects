// FacilityDataPage.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "FacilityDataPage.h"
#include ".\facilitydatapage.h"
#include "../Landside/InputLandside.h"
#include "DlgSelectLandsideObject.h"
#include "../Landside/FacilityBehavior.h"
#include "ProcesserDialog.h"
#include "../Inputs/PROCIDList.h"
#include "Landside\LandsideParkingLot.h"
// CFacilityDataPage dialog

IMPLEMENT_DYNAMIC(CFacilityDataPage, CResizablePage)
CFacilityDataPage::CFacilityDataPage(InputLandside *pInputLandside,
									 InputTerminal* _pInTerm,
									 CFacilityBehaviorList *pBehaviorDB,
									 enum ALTOBJECT_TYPE enumFacType,
									 UINT nIDCaption,
									 int nProjectID)
	: CResizablePage(IDD_PROPPAGE_FACILITYDATA, nIDCaption)
{
	m_enumType			= enumFacType;
	m_nProjectID		= nProjectID;
	m_pInputLandside	= pInputLandside;
	m_pBehaviorDB		= pBehaviorDB;
	m_pInTerm			= _pInTerm;


	m_hTerminalProc = NULL;
	m_hParkingLot	= NULL;
	m_hCapacity		= NULL;
	m_hServeTaxi    = NULL;
	m_hLeadToParkingLot = NULL;
}

CFacilityDataPage::~CFacilityDataPage()
{
}

void CFacilityDataPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcDataPage)
	DDX_Control(pDX, IDC_STATIC_LINKAGE, m_staticLinkage);
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeData);
	DDX_Control(pDX, IDC_LIST_PROCESSOR, m_listboxFacility);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CFacilityDataPage, CResizablePage)
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


// CFacilityDataPage message handlers

BOOL CFacilityDataPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	//comment out, for duplicated with CFacilityDataPage::OnOK()
	//try
	//{
	//	m_pBehaviorDB->SaveData(m_nProjectID);
	//	SetModified(FALSE);
	//}
	//catch (CADOException& e)
	//{
	//	CString strMsg = e.ErrorMessage();
	//	MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
	//}
	return CResizablePage::OnApply();
}

void CFacilityDataPage::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		m_pBehaviorDB->SaveData(m_nProjectID);
	}
	catch (CADOException& e)
	{
		CString strMsg = e.ErrorMessage();
		MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
		return;
	}
	CResizablePage::OnOK();
}

void CFacilityDataPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CResizablePage::OnCancel();
}



BOOL CFacilityDataPage::OnInitDialog()
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
void CFacilityDataPage::InitToolbar()
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
void CFacilityDataPage::InitSpiltter()
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
void CFacilityDataPage::SetSplitterRange()
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
int CFacilityDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CFacilityDataPage::OnSize(UINT nType, int cx, int cy)
{
	CResizablePage::OnSize(nType, cx, cy);
	SetSplitterRange();
	// TODO: Add your message handler code here
}

void CFacilityDataPage::OnButtonAdd()
{
	CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
	dlg.AddObjType(m_enumType);
	if( dlg.DoModal() == IDCANCEL )
		return;
	
	ALTObjectID selectFac = dlg.getSelectObject();

	//check if the Facility existed
	if(m_pBehaviorDB->IsExisted(selectFac,m_enumType))
	{
		MessageBox("The selected object has been existed.",_T("Facility Behavior"), MB_OK);
		return;
	}

	CFacilityBehavior *pNewBehavior = m_pBehaviorDB->AddNewFacility(m_enumType);
	pNewBehavior->setFacilityName(selectFac);
	int nIndex = AddFacilityToList(pNewBehavior);
	m_listboxFacility.SetCurSel(nIndex);

	LoadTreeProperties();
	UpdateFacilityToolbarState();
	
	SetModified();
}

void CFacilityDataPage::OnButtonDel()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return;
	
	CFacilityBehavior *pThisBehavior =(CFacilityBehavior *)m_listboxFacility.GetItemData(nIdx);
	m_pBehaviorDB->DeleteItem(pThisBehavior);
	m_listboxFacility.DeleteString( nIdx );

	//select next item
	if(nIdx > 0)
		m_listboxFacility.SetCurSel(nIdx - 1);
	else
	{
		if(m_listboxFacility.GetCount())
			m_listboxFacility.SetCurSel(0);
	}

	SetModified();

	UpdateFacilityToolbarState();
	
}

void CFacilityDataPage::OnToolbarbuttonadd()
{
	CFacilityBehavior * pCurrentBehavior = GetCurrentBehavior();
	if(pCurrentBehavior == NULL)
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == m_hTerminalProc)
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
			pCurrentBehavior->getLinkageList()->AddItem(pLinkage);

			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hItem = m_TreeData.InsertItem( strProc,cni,FALSE,FALSE, m_hTerminalProc );
			m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkage );
			SetModified();
		}
		m_TreeData.Expand(m_hTerminalProc, TVE_EXPAND);

	}
	else if(hSelItem == m_hParkingLot)
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
		pCurrentBehavior->getLinkageList()->AddItem(pLinkage);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hItem = m_TreeData.InsertItem( strObj,cni,FALSE,FALSE, m_hParkingLot );
		m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkage );

		m_TreeData.Expand(m_hParkingLot, TVE_EXPAND);
		SetModified();

	}
	else if(hSelItem == m_hLeadToParkingLot)
	{
		CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LPARKINGLOT);
		if( dlg.DoModal() == IDCANCEL )
			return;
		LandsideParkingLot* parkinglot = (LandsideParkingLot*)dlg.objectList->getObjectByName(dlg.getSelectObject());
		CLandsideLeadToParkingLot* leadToParkingLot = new CLandsideLeadToParkingLot();
		leadToParkingLot->SetparkingLotId(parkinglot->getID());
		if(pCurrentBehavior->findLeadToParkingLot(leadToParkingLot))
		{
			MessageBox(_T("The item is already exit!"),NULL,MB_OK);
			return;
		}
			
		pCurrentBehavior->getParkingLeadToLotList()->AddItem(leadToParkingLot);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		CString name = parkinglot->getName().GetIDString();
		HTREEITEM m_hParkingLots = m_TreeData.InsertItem(name, cni,FALSE,FALSE, m_hLeadToParkingLot);
		m_TreeData.SetItemData(m_hParkingLots, (DWORD_PTR)leadToParkingLot);
		for (int j = 0; j < (int)parkinglot->GetLevelCount(); j++)
		{
			LandsideParkingLotLevel* parkinglotlevel = parkinglot->GetLevel(j);
			CString levelname = parkinglotlevel->m_name;
			

			cni.nt = NT_CHECKBOX;
			HTREEITEM m_hParkingLotLevel = m_TreeData.InsertItem(levelname, cni,FALSE,FALSE, m_hParkingLots);
			m_TreeData.SetItemData(m_hParkingLotLevel,(DWORD_PTR)j);
		}
		m_TreeData.Expand(m_hParkingLots,TVE_EXPAND);
		m_TreeData.Expand(m_hLeadToParkingLot,TVE_EXPAND);		
	}
}

void CFacilityDataPage::OnToolbarbuttondel()
{
	CFacilityBehavior * pCurrentBehavior = GetCurrentBehavior();
	if(pCurrentBehavior == NULL)
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;

	if (m_TreeData.GetParentItem(hSelItem) == m_hLeadToParkingLot)
	{
		CLandsideLeadToParkingLot* leadparkinglot = (CLandsideLeadToParkingLot*)m_TreeData.GetItemData(hSelItem);
		if (leadparkinglot == NULL)
			return;
		HTREEITEM hNextSelItem = m_TreeData.GetNextSiblingItem(hSelItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_TreeData.GetPrevSiblingItem(hSelItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_TreeData.GetParentItem(hSelItem);

		m_TreeData.DeleteItem(hSelItem);
		pCurrentBehavior->getParkingLeadToLotList()->DeleteItem(leadparkinglot);
		
		if(hNextSelItem)
			m_TreeData.SelectItem(hNextSelItem);

		UpdateFacilityToolbarState();
		SetModified();
	} 
	else
	{
		CFacilityBehaviorLinkage *pLinkage = (CFacilityBehaviorLinkage *)m_TreeData.GetItemData(hSelItem);
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
	
}

void CFacilityDataPage::OnLinkageOnetoone()
{
	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;

	CFacilityBehaviorLinkage *pLinkage = (CFacilityBehaviorLinkage *)m_TreeData.GetItemData(hSelItem);
	if(pLinkage == NULL)
		return;

	pLinkage->set1to1(!pLinkage->is1To1());
	CString strItemText = pLinkage->getName();

	if(pLinkage->is1To1())
		strItemText +=(_T("(1:1)"));

	m_TreeData.SetItemText(hSelItem,strItemText);
	
	SetModified();
}

void CFacilityDataPage::InitFacilityList()
{
	int nCount = m_pBehaviorDB->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CFacilityBehavior *pBehavior = m_pBehaviorDB->GetItem(nItem);
		if(pBehavior && pBehavior->getFacilityType() == m_enumType)
		{
			AddFacilityToList(pBehavior);
		}
	}
	if(nCount > 0)
		m_listboxFacility.SetCurSel(0);

	LoadTreeProperties();
	UpdateFacilityToolbarState();
}

int CFacilityDataPage::AddFacilityToList( CFacilityBehavior *pBehavior )
{
	int nIndex = m_listboxFacility.AddString(pBehavior->getFacilityName().GetIDString());
	m_listboxFacility.SetItemData(nIndex, (DWORD_PTR)pBehavior);
	return nIndex;
}

void CFacilityDataPage::UpdateFacilityToolbarState()
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

void CFacilityDataPage::OnSelchangeListFacility()
{
	LoadTreeProperties();
}

void CFacilityDataPage::LoadTreeProperties()
{

	m_TreeData.DeleteAllItems();

	int nCurSel = m_listboxFacility.GetCurSel();
	if(nCurSel == LB_ERR)
	{
		return;
	}

	CFacilityBehavior *pCurBehavior = GetCurrentBehavior();
	if(pCurBehavior == NULL)
	{
		return;
	}

	CFacilityBehaviorLinkageList *pLinkageList = pCurBehavior->getLinkageList();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hLinkage = m_TreeData.InsertItem( _T("Linkage"),cni,FALSE,FALSE, TVI_ROOT);
	m_TreeData.SetItemData(hLinkage, (DWORD_PTR)pLinkageList);
	
	//Terminal processors
	m_hTerminalProc = m_TreeData.InsertItem(_T("Terminal Processors"), cni,FALSE,FALSE, hLinkage);
	m_TreeData.SetItemData(m_hTerminalProc, (DWORD_PTR)pLinkageList);
	
	m_hParkingLot = m_TreeData.InsertItem(_T("Parking Lot"), cni,FALSE,FALSE, hLinkage);
	m_TreeData.SetItemData(m_hParkingLot, (DWORD_PTR)pLinkageList);	

	int nLinkCount =  pLinkageList->GetItemCount();
	for (int nLink = 0 ; nLink < nLinkCount; ++ nLink)
	{
		CFacilityBehaviorLinkage *pLinkage = pLinkageList->GetItem(nLink);

		CString strItemText = pLinkage->getName();

		if(pLinkage->is1To1())
			strItemText +=(_T("(1:1)"));
		
		HTREEITEM hParentItem = NULL;
		if(pLinkage->getType() == CFacilityBehaviorLinkage::enumLinkType_Terminal)
			hParentItem = m_hTerminalProc;
		else if(pLinkage->getType() == CFacilityBehaviorLinkage::enumLinkType_ParkingLot)
			hParentItem = m_hParkingLot;
		else
		{
			hParentItem = NULL;
			ASSERT(0);
		}
		HTREEITEM hItem = m_TreeData.InsertItem( strItemText, cni,FALSE,FALSE, hParentItem);	
		m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkage);
	}
	m_TreeData.Expand(hLinkage, TVE_EXPAND);
	m_TreeData.Expand(m_hParkingLot, TVE_EXPAND);
	m_TreeData.Expand(m_hTerminalProc, TVE_EXPAND);

	//capacity
	cni.nt	=	NT_CHECKBOX;
	cni.net =	NET_EDITSPIN_WITH_VALUE;

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
	m_TreeData.SetItemData(m_hCapacity, pCurBehavior->getCapacity());
	
	//Lead to Parking Lot
	COOLTREE_NODE_INFO cccni;
	CCoolTree::InitNodeInfo(this,cccni);
	m_hLeadToParkingLot = m_TreeData.InsertItem( _T("Lead To"),cccni,FALSE,FALSE, TVI_ROOT);	
	int nCount = pCurBehavior->getParkingLeadToLotList()->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CLandsideLeadToParkingLot* pLeadToParkingLot = pCurBehavior->getParkingLeadToLotList()->GetItem(i);
		LandsideParkingLot* pLayout = (LandsideParkingLot*)m_pInputLandside->getObjectList().getObjectByID(pLeadToParkingLot->GetparkingLotId());
		if (pLayout == NULL)
		{
			continue;
		}
		CString name = pLayout->getName().GetIDString();
		HTREEITEM m_hParkingLots = m_TreeData.InsertItem(name, cccni,FALSE,FALSE, m_hLeadToParkingLot);
		m_TreeData.SetItemData(m_hParkingLots, (DWORD_PTR)pLeadToParkingLot);
		for (int j = 0; j < (int)pLayout->GetLevelCount(); j++)
		{
			LandsideParkingLotLevel* ParkingLotLevel = pLayout->GetLevel(j);
			CString levelname = ParkingLotLevel->m_name;
			
			COOLTREE_NODE_INFO clevleni;
			CCoolTree::InitNodeInfo(this,clevleni);
			clevleni.nt = NT_CHECKBOX;
			HTREEITEM m_hParkingLotLevel = m_TreeData.InsertItem(levelname, clevleni,FALSE,FALSE, m_hParkingLots);
			m_TreeData.SetItemData(m_hParkingLotLevel,(DWORD_PTR)j);
			for (int k =0; k < (int)pLeadToParkingLot->GetLevelList().size(); k++)
			{
				if (j == pLeadToParkingLot->GetLevelList().at(k))
				{
					m_TreeData.SetCheckStatus(m_hParkingLotLevel,TRUE);
				}
			}			
		}
		m_TreeData.Expand(m_hParkingLots, TVE_EXPAND);
	}
	m_TreeData.Expand(m_hLeadToParkingLot, TVE_EXPAND);
	
}

void CFacilityDataPage::UpdateTreeToolbarState()
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

	if(hSelItem == m_hParkingLot || hSelItem == m_hTerminalProc || hSelItem == m_hLeadToParkingLot)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE );
	}
	HTREEITEM hParentItem = m_TreeData.GetParentItem(hSelItem);
	if(hParentItem == m_hParkingLot || hParentItem == m_hTerminalProc)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);	
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, TRUE);
	}
	if (hParentItem == m_hLeadToParkingLot)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);
	}
}

void CFacilityDataPage::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	UpdateTreeToolbarState();
	*pResult = 0;
}

CFacilityBehavior * CFacilityDataPage::GetCurrentBehavior()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return NULL;

	CFacilityBehavior *pThisBehavior =(CFacilityBehavior *)m_listboxFacility.GetItemData(nIdx);
	return pThisBehavior;

}



LRESULT CFacilityDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		//COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
		CString strValue=*((CString*)lParam);
		int nValue=static_cast<int>(atoi(strValue));
		if(hItem == m_hCapacity)
		{
			CFacilityBehavior *pCurBehavior = GetCurrentBehavior();
			if(pCurBehavior != NULL)
			{
				pCurBehavior->setCapacity(nValue);
				CString strCapacity;
				strCapacity.Format(_T("Capacity: %d"), pCurBehavior->getCapacity());
				m_TreeData.SetItemText(m_hCapacity, strCapacity);
				if(pCurBehavior->isCapacityLimit())
				{
					m_TreeData.SetCheckStatus(m_hCapacity,  TRUE);
				}
				else
				{
					m_TreeData.SetCheckStatus(m_hCapacity,  FALSE);				
				}
				m_TreeData.SetItemData(m_hCapacity, pCurBehavior->getCapacity());
				SetModified();
			}
		}
	}
	if(message == UM_CEW_STATUS_CHANGE)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		if (hItem == m_hCapacity)
		{
			BOOL b = m_TreeData.IsCheckItem(m_hCapacity);
			CFacilityBehavior *pCurBehavior = GetCurrentBehavior();
			if(pCurBehavior != NULL)
			{
				pCurBehavior->setCapacityLimit(b==TRUE);
				if(pCurBehavior->isCapacityLimit())
				{
					m_TreeData.SetCheckStatus(m_hCapacity,  TRUE);
				}
				else
				{
					m_TreeData.SetCheckStatus(m_hCapacity,  FALSE);
				}

				SetModified();
			}
		} 
		else
		{
			HTREEITEM hparentitem = m_TreeData.GetParentItem(hItem);
			if (m_hLeadToParkingLot == m_TreeData.GetParentItem(hparentitem))
			{
				SelectLevelItem(hItem);
				SetModified();
			}
			
		}
	}
	return CResizablePage::DefWindowProc(message, wParam, lParam);
}

bool CFacilityDataPage::IsItemDuplicate( HTREEITEM hParentItem,const CString& strName )
{
	HTREEITEM hItem = m_TreeData.GetChildItem(hParentItem);
	while(hItem)
	{

		CFacilityBehaviorLinkage *pLinkage = (CFacilityBehaviorLinkage *)m_TreeData.GetItemData(hItem);
		if(pLinkage == NULL)
			continue;
		if(pLinkage->getName().CompareNoCase(strName) == 0)
			return true;

		hItem = m_TreeData.GetNextSiblingItem(hItem);
	}

	return false;
}

void CFacilityDataPage::SelectLevelItem( HTREEITEM hItem )
{
	CLandsideLeadToParkingLot* LeadToParkingLot = (CLandsideLeadToParkingLot*)m_TreeData.GetItemData(m_TreeData.GetParentItem(hItem));
	int ParkingLotLevelInDex = (int)m_TreeData.GetItemData(hItem);
	CFacilityBehavior * pCurrentBehavior = GetCurrentBehavior();
	if(pCurrentBehavior == NULL)
		return;
	if (m_TreeData.IsCheckItem(hItem))
	{	

		if (pCurrentBehavior->findLeadToParkingLot(LeadToParkingLot))
		{						
			pCurrentBehavior->GetLeadToParkingLot(LeadToParkingLot->GetparkingLotId())->addnewlevel(ParkingLotLevelInDex);
		}
	
	} 
	else
	{
		
		if (pCurrentBehavior->findLeadToParkingLot(LeadToParkingLot))
		{
			pCurrentBehavior->GetLeadToParkingLot(LeadToParkingLot->GetparkingLotId())->removelevel(ParkingLotLevelInDex);
		}
		
	}
	
}
