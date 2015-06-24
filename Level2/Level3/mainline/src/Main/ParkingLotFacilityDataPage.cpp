// FacilityDataPage.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ParkingLotFacilityDataPage.h"
#include ".\facilitydatapage.h"
#include "../Landside/InputLandside.h"
#include "DlgSelectLandsideObject.h"
#include "../Landside/FacilityParkingLotBehavior.h"
#include "ProcesserDialog.h"
#include "../Inputs/PROCIDList.h"
#include "Landside\LandsideParkingLot.h"
// CParkingLotFacilityDataPage dialog

IMPLEMENT_DYNAMIC(CParkingLotFacilityDataPage, CResizablePage)
CParkingLotFacilityDataPage::CParkingLotFacilityDataPage(InputLandside *pInputLandside,
									 InputTerminal* _pInTerm,
									 CFacilityBehaviorList *pBehaviorDB,
									 enum ALTOBJECT_TYPE enumFacType,
									 UINT nIDCaption,
									 int nProjectID)
									 : CResizablePage(CParkingLotFacilityDataPage::IDD, nIDCaption)
{
	m_enumType			= enumFacType;
	m_nProjectID		= nProjectID;
	m_pInputLandside	= pInputLandside;
	m_pBehaviorDB		= pBehaviorDB;
	m_pInTerm			= _pInTerm;

	m_hCapacity		= NULL;
}

CParkingLotFacilityDataPage::~CParkingLotFacilityDataPage()
{
	std::vector<ParkingLotTreeNodeData*>::iterator iter = m_vParkingLotTreeNode.begin();
	for(; iter != m_vParkingLotTreeNode.end(); iter++)
	{
		delete *iter;
	}
	m_vParkingLotTreeNode.clear();
}

void CParkingLotFacilityDataPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcDataPage)
	DDX_Control(pDX, IDC_STATIC_LINKAGE, m_staticLinkage);
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeData);
	DDX_Control(pDX, IDC_LIST_PROCESSOR, m_listboxFacility);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CParkingLotFacilityDataPage, CResizablePage)
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


// CParkingLotFacilityDataPage message handlers

BOOL CParkingLotFacilityDataPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	try
	{
		m_pBehaviorDB->SaveData(m_nProjectID);
		SetModified(FALSE);
	}
	catch (CADOException& e)
	{
		CString strMsg = e.ErrorMessage();
		MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
	}
	return CResizablePage::OnApply();
}

void CParkingLotFacilityDataPage::OnOK()
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

void CParkingLotFacilityDataPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CResizablePage::OnCancel();
}



BOOL CParkingLotFacilityDataPage::OnInitDialog()
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
void CParkingLotFacilityDataPage::InitToolbar()
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
void CParkingLotFacilityDataPage::InitSpiltter()
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
void CParkingLotFacilityDataPage::SetSplitterRange()
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
int CParkingLotFacilityDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CParkingLotFacilityDataPage::OnSize(UINT nType, int cx, int cy)
{
	CResizablePage::OnSize(nType, cx, cy);
	SetSplitterRange();
	// TODO: Add your message handler code here
}

void CParkingLotFacilityDataPage::OnButtonAdd()
{
	CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
	dlg.AddObjType(m_enumType);
	if( dlg.DoModal() == IDCANCEL )
		return;

	ALTObjectID selectFac = dlg.getSelectObject();

	if (selectFac.GetIDString().Compare("All") == 0)
	{
		MessageBox(_T("Pelease select one specified parking lot."), _T("Facility Behavior"), MB_OK);
		return;
	}
	LandsideParkingLot* pParkinglot = (LandsideParkingLot*)m_pInputLandside->getObjectList().getObjectByName(selectFac);
	if(pParkinglot == NULL)
	{
		MessageBox(_T("Pelease select one specified parking lot instead of a group."), _T("Facility Behavior"), MB_OK);
		return;
	}


	//check if the Facility existed
	if(m_pBehaviorDB->IsExisted(selectFac,m_enumType))
	{
		MessageBox("The selected object has been existed.",_T("Facility Behavior"), MB_OK);
		return;
	}

	CFacilityParkingLotBehavior* pNewBehavior = (CFacilityParkingLotBehavior*)m_pBehaviorDB->AddNewFacility(m_enumType);
	pNewBehavior->setFacilityName(selectFac);
	int nIndex = AddFacilityToList(pNewBehavior);
	m_listboxFacility.SetCurSel(nIndex);

	LoadTreeProperties();
	UpdateFacilityToolbarState();

	SetModified();
}

void CParkingLotFacilityDataPage::OnButtonDel()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return;

	CFacilityParkingLotBehavior *pThisBehavior =(CFacilityParkingLotBehavior *)m_listboxFacility.GetItemData(nIdx);
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

void CParkingLotFacilityDataPage::OnToolbarbuttonadd()
{
	CFacilityParkingLotBehavior * pCurrentBehavior = GetCurrentBehavior();
	if(pCurrentBehavior == NULL)
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	ParkingLotTreeNodeData* pLinkageNodeData = (ParkingLotTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (pLinkageNodeData == NULL)
		return;

	if(pLinkageNodeData->m_emType == ParkingLotTreeNodeData::TeminalProcessor_Node)
	{
		HTREEITEM hParkingLotLevelItem = m_TreeData.GetParentItem(hSelItem);

		LandsideParkingLotLevel* parkinglotlevel = (LandsideParkingLotLevel*)m_TreeData.GetItemData(hParkingLotLevelItem);

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
			CFacilityParkingLotBehaviorLinkage *pLinkage  = new CFacilityParkingLotBehaviorLinkage;
			pLinkage->setType(CFacilityParkingLotBehaviorLinkage::enumLinkType_Terminal);
			pLinkage->setName(strProc);
			pLinkage->SetLevelID(parkinglotlevel->m_nLevelId);
			pCurrentBehavior->GetParkingLotLinkageList()->AddItem(pLinkage);

			ParkingLotTreeNodeData* pLinkageNodeData = new ParkingLotTreeNodeData();
			pLinkageNodeData->m_dwData = (DWORD)pLinkage;
			pLinkageNodeData->m_emType = ParkingLotTreeNodeData::TeminalLinkage_Node;
			m_vParkingLotTreeNode.push_back(pLinkageNodeData);
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hItem = m_TreeData.InsertItem( strProc,cni,FALSE,FALSE, hSelItem );
			m_TreeData.SetItemData( hItem, (DWORD_PTR)pLinkageNodeData );
			SetModified();
		}
		m_TreeData.Expand(hSelItem, TVE_EXPAND);

	}
}

void CParkingLotFacilityDataPage::OnToolbarbuttondel()
{
	CFacilityParkingLotBehavior * pCurrentBehavior = GetCurrentBehavior();
	if(pCurrentBehavior == NULL)
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	ParkingLotTreeNodeData* ParkingLotLinkageTreeNode = (ParkingLotTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (ParkingLotLinkageTreeNode == NULL)
		return;
	CFacilityParkingLotBehaviorLinkage *pLinkage = (CFacilityParkingLotBehaviorLinkage *)ParkingLotLinkageTreeNode->m_dwData;
	if(pLinkage == NULL)
		return;

	HTREEITEM hNextSelItem = m_TreeData.GetNextSiblingItem(hSelItem);
	if(hNextSelItem == NULL)
		hNextSelItem = m_TreeData.GetPrevSiblingItem(hSelItem);
	if(hNextSelItem == NULL)
		hNextSelItem = m_TreeData.GetParentItem(hSelItem);

	m_TreeData.DeleteItem(hSelItem);
	pCurrentBehavior->GetParkingLotLinkageList()->DeleteItem(pLinkage);

	if(hNextSelItem)
		m_TreeData.SelectItem(hNextSelItem);

	UpdateFacilityToolbarState();
	SetModified();
	

}

void CParkingLotFacilityDataPage::OnLinkageOnetoone()
{
	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;

	ParkingLotTreeNodeData* ParkingLotLinkageTreeNode = (ParkingLotTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (ParkingLotLinkageTreeNode == NULL)
		return;
	CFacilityParkingLotBehaviorLinkage *pLinkage = (CFacilityParkingLotBehaviorLinkage *)ParkingLotLinkageTreeNode->m_dwData;
	if(pLinkage == NULL)
		return;

	pLinkage->set1to1(!pLinkage->is1To1());
	CString strItemText = pLinkage->getName();

	if(pLinkage->is1To1())
		strItemText +=(_T("(1:1)"));

	m_TreeData.SetItemText(hSelItem,strItemText);

	SetModified();
}

void CParkingLotFacilityDataPage::InitFacilityList()
{
	int nCount = m_pBehaviorDB->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CFacilityParkingLotBehavior* pBehavior = (CFacilityParkingLotBehavior*)m_pBehaviorDB->GetItem(nItem);
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

int CParkingLotFacilityDataPage::AddFacilityToList( CFacilityParkingLotBehavior *pBehavior )
{
	int nIndex = m_listboxFacility.AddString(pBehavior->getFacilityName().GetIDString());
	m_listboxFacility.SetItemData(nIndex, (DWORD_PTR)pBehavior);
	return nIndex;
}

void CParkingLotFacilityDataPage::UpdateFacilityToolbarState()
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

void CParkingLotFacilityDataPage::OnSelchangeListFacility()
{
	LoadTreeProperties();
}

void CParkingLotFacilityDataPage::LoadTreeProperties()
{

	m_TreeData.DeleteAllItems();

	int nCurSel = m_listboxFacility.GetCurSel();
	if(nCurSel == LB_ERR)
	{
		return;
	}

	CFacilityParkingLotBehavior *pCurBehavior = GetCurrentBehavior();
	if(pCurBehavior == NULL)
	{
		return;
	}

	CFacilityParkingLotBehaviorLinkageList *pLinkageList = pCurBehavior->GetParkingLotLinkageList();

	LandsideParkingLot* parkinglot = (LandsideParkingLot*)m_pInputLandside->getObjectList().getObjectByName(pCurBehavior->getFacilityName());
	
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hLinkage = m_TreeData.InsertItem( _T("Linkage"),cni,FALSE,FALSE,TVI_ROOT );
	m_TreeData.SetItemData(hLinkage, (DWORD_PTR)pLinkageList);


	for (int i = 0; i < parkinglot->GetLevelCount(); i++)
	{		
		LandsideParkingLotLevel* parkinglotlevel = parkinglot->GetLevel(i);	
		CString levelname = parkinglotlevel->m_name;

		HTREEITEM m_hParkingLotLevel = m_TreeData.InsertItem(levelname, cni,FALSE,FALSE, hLinkage );
		m_TreeData.SetItemData(m_hParkingLotLevel,(DWORD_PTR)parkinglotlevel);

		//Terminal processors
		ParkingLotTreeNodeData* pParkingLotTreeNode = new ParkingLotTreeNodeData();
		pParkingLotTreeNode->m_emType = ParkingLotTreeNodeData::TeminalProcessor_Node;

		HTREEITEM hTerminalProc = m_TreeData.InsertItem(_T("Terminal Processors"), cni,FALSE,FALSE, m_hParkingLotLevel);
		m_TreeData.SetItemData(hTerminalProc, (DWORD_PTR)pParkingLotTreeNode);
		m_vParkingLotTreeNode.push_back(pParkingLotTreeNode);

		std::vector<CFacilityParkingLotBehaviorLinkage*> vParkingLotLinkage;
		pLinkageList->GetLevelLinkageList(parkinglotlevel->m_nLevelId,vParkingLotLinkage);

		int nLinkCount =  static_cast<int>(vParkingLotLinkage.size());
		for (int nLink = 0 ; nLink < nLinkCount; ++ nLink)
		{
			CFacilityParkingLotBehaviorLinkage *pLinkage = vParkingLotLinkage.at(nLink);

			CString strItemText = pLinkage->getName();

			if(pLinkage->is1To1())
				strItemText +=(_T("(1:1)"));

			HTREEITEM hParentItem = NULL;
			if(pLinkage->getType() == CFacilityParkingLotBehaviorLinkage::enumLinkType_Terminal)
				hParentItem = hTerminalProc;
			else
			{
				hParentItem = NULL;
				ASSERT(0);
			}
			ParkingLotTreeNodeData* pParkingLotLinkageTreeNode = new ParkingLotTreeNodeData();
			pParkingLotLinkageTreeNode->m_emType = ParkingLotTreeNodeData::TeminalLinkage_Node;
			pParkingLotLinkageTreeNode->m_dwData = (DWORD)pLinkage;
			m_vParkingLotTreeNode.push_back(pParkingLotLinkageTreeNode);
			HTREEITEM hItem = m_TreeData.InsertItem( strItemText, cni,FALSE,FALSE, hParentItem);	
			m_TreeData.SetItemData( hItem, (DWORD_PTR)pParkingLotLinkageTreeNode);
		}
		m_TreeData.Expand(hLinkage, TVE_EXPAND);
		m_TreeData.Expand(hTerminalProc, TVE_EXPAND);
		m_TreeData.Expand(m_hParkingLotLevel, TVE_EXPAND);
	}


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

}

void CParkingLotFacilityDataPage::UpdateTreeToolbarState()
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

	ParkingLotTreeNodeData* pParkingLotTreeNode = (ParkingLotTreeNodeData*)m_TreeData.GetItemData(hSelItem);
	if (pParkingLotTreeNode == NULL)
		return;
	if(pParkingLotTreeNode->m_emType == ParkingLotTreeNodeData::TeminalProcessor_Node)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE );
	}
	if(pParkingLotTreeNode->m_emType == ParkingLotTreeNodeData::TeminalLinkage_Node)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);	
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, TRUE);
	}
}

void CParkingLotFacilityDataPage::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateTreeToolbarState();
	*pResult = 0;
}

CFacilityParkingLotBehavior * CParkingLotFacilityDataPage::GetCurrentBehavior()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return NULL;

	CFacilityParkingLotBehavior *pThisBehavior =(CFacilityParkingLotBehavior *)m_listboxFacility.GetItemData(nIdx);
	return pThisBehavior;

}



LRESULT CParkingLotFacilityDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
			CFacilityParkingLotBehavior *pCurBehavior = GetCurrentBehavior();
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
			CFacilityParkingLotBehavior *pCurBehavior = GetCurrentBehavior();
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
	}
	return CResizablePage::DefWindowProc(message, wParam, lParam);
}

bool CParkingLotFacilityDataPage::IsItemDuplicate( HTREEITEM hParentItem,const CString& strName )
{
	HTREEITEM hItem = m_TreeData.GetChildItem(hParentItem);
	while(hItem)
	{

		ParkingLotTreeNodeData* pParkingLotTreeNode = (ParkingLotTreeNodeData*)m_TreeData.GetItemData(hItem);
		if (pParkingLotTreeNode == NULL)
			return false;
		CFacilityParkingLotBehaviorLinkage *pLinkage = (CFacilityParkingLotBehaviorLinkage*)pParkingLotTreeNode->m_dwData;
		if(pLinkage == NULL)
			continue;
		if(pLinkage->getName().CompareNoCase(strName) == 0)
			return true;

		hItem = m_TreeData.GetNextSiblingItem(hItem);
	}

	return false;
}
