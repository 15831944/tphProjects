// FacilityDataPage.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TaxiPoolFacilityDataPage.h"
#include "Landside/InputLandside.h"
#include "../Inputs/IN_TERM.H"
#include "DlgSelectLandsideObject.h"
#include "Landside/FacilityTaxiPoolBehavior.h"
#include "DlgSelectLandsideVehicleType.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"
#include "DlgProbDist.h"
// CTaxiPoolFacilityDataPage dialog

IMPLEMENT_DYNAMIC(CTaxiPoolFacilityDataPage, CResizablePage)
CTaxiPoolFacilityDataPage::CTaxiPoolFacilityDataPage(InputLandside *pInputLandside,
									 InputTerminal* _pInTerm,
									 CFacilityBehaviorList *pBehaviorDB,
									 enum ALTOBJECT_TYPE enumFacType,
									 UINT nIDCaption,
									 int nProjectID)
									 : CResizablePage(CTaxiPoolFacilityDataPage::IDD, nIDCaption)
{
	m_enumType			= enumFacType;
	m_nProjectID		= nProjectID;
	m_pInputLandside	= pInputLandside;
	m_pTaxiPoolBehaviorDB		= pBehaviorDB;
	m_pInTerm			= _pInTerm;


	m_hVehicleType      = NULL;
	m_hRecordLevel	    = NULL;
	m_hRecordQuantity	= NULL;
	m_hCallFromExternal = NULL;
}

CTaxiPoolFacilityDataPage::~CTaxiPoolFacilityDataPage()
{
}

void CTaxiPoolFacilityDataPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcDataPage)
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeData);
	DDX_Control(pDX, IDC_LIST_PROCESSOR, m_listboxFacility);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CTaxiPoolFacilityDataPage, CResizablePage)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_PROCESSOR_DATA_ADD, OnButtonAdd)
	ON_BN_CLICKED(ID_PROCESSOR_DATA_DEL, OnButtonDel)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_LBN_SELCHANGE(IDC_LIST_PROCESSOR, OnSelchangeListFacility)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeData)
END_MESSAGE_MAP()


// CTaxiPoolFacilityDataPage message handlers

BOOL CTaxiPoolFacilityDataPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class
	int nCount = m_pTaxiPoolBehaviorDB->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CFacilityTaxiPoolBehavior *pTaxiPoolBehavior = (CFacilityTaxiPoolBehavior*)m_pTaxiPoolBehaviorDB->GetItem(i);
		if (pTaxiPoolBehavior == NULL)
			continue;
		if (pTaxiPoolBehavior->getFacilityType() == ALT_LTAXIPOOL)
		{
			if (!pTaxiPoolBehavior->checkTotals())
			{
				MessageBox( "All branches must total 100%", "Error", MB_OK );
				return NULL;
			}
		}
		
	}	
	try
	{
		m_pTaxiPoolBehaviorDB->SaveData(m_nProjectID);
		SetModified(FALSE);
	}
	catch (CADOException& e)
	{
		CString strMsg = e.ErrorMessage();
		MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
	}
	return CResizablePage::OnApply();
}

void CTaxiPoolFacilityDataPage::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		m_pTaxiPoolBehaviorDB->SaveData(m_nProjectID);
	}
	catch (CADOException& e)
	{
		CString strMsg = e.ErrorMessage();
		MessageBox(_T("Save data failed."),_T("ARCport"), MB_OK);
		return;
	}
	CResizablePage::OnOK();
}

void CTaxiPoolFacilityDataPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CResizablePage::OnCancel();
}



BOOL CTaxiPoolFacilityDataPage::OnInitDialog()
{
	CResizablePage::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolbar();
	//Left Wnd
	AddAnchor(m_ToolBarLeft, CSize(0,0),CSize(40,0));
	AddAnchor(IDC_LIST_PROCESSOR, TOP_LEFT,CSize(40,100));

	//Right Wnd
	AddAnchor(IDC_STATIC_LINKAGE,CSize(40,0),CSize(100,0));
	AddAnchor(m_ToolBar, CSize(40,0),CSize(100,0));
	AddAnchor(IDC_TREE_DATA, CSize(40,0),CSize(100,100));

	InitTaxiPoolFacilityList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CTaxiPoolFacilityDataPage::InitToolbar()
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

int CTaxiPoolFacilityDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CResizablePage::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_TAXIPOOLFACILITYBEHAVIOR))
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

void CTaxiPoolFacilityDataPage::OnSize(UINT nType, int cx, int cy)
{
	CResizablePage::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
}

void CTaxiPoolFacilityDataPage::OnButtonAdd()
{
	CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
	dlg.AddObjType(m_enumType);
	if( dlg.DoModal() == IDCANCEL )
		return;

	ALTObjectID selectFac = dlg.getSelectObject();

	//check if the Facility existed
	if(m_pTaxiPoolBehaviorDB->IsExisted(selectFac,m_enumType))
	{
		MessageBox("The selected object has been existed.",_T("Taxi Pool Facility Behavior"), MB_OK);
		return;
	}

	CFacilityTaxiPoolBehavior *pNewTaxiPoolBehavior = (CFacilityTaxiPoolBehavior*)m_pTaxiPoolBehaviorDB->AddNewFacility(m_enumType);
	pNewTaxiPoolBehavior->setFacilityName(selectFac);
	int nIndex = AddTaxiPoolFacilityToList(pNewTaxiPoolBehavior);
	m_listboxFacility.SetCurSel(nIndex);

	LoadTreeProperties();
	UpdateFacilityToolbarState();

	SetModified();
}

void CTaxiPoolFacilityDataPage::OnButtonDel()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return;

	CFacilityTaxiPoolBehavior *pThisBehavior =(CFacilityTaxiPoolBehavior *)m_listboxFacility.GetItemData(nIdx);
	m_pTaxiPoolBehaviorDB->DeleteItem(pThisBehavior);
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

void CTaxiPoolFacilityDataPage::OnToolbarbuttonadd()
{
	CFacilityTaxiPoolBehavior * pCurrentTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
	if(pCurrentTaxiPoolBehavior == NULL)
		return;
	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if (hSelItem == m_hCallFromExternal)
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
		pCurrentTaxiPoolBehavior->getCallFromExternal()->AddItem(pCallFromExternal);
		EvenPercent();
		LoadTreeProperties();
		m_TreeData.SelectItem(m_hCallFromExternal);
		UpdateFacilityToolbarState();
	}
	
}

void CTaxiPoolFacilityDataPage::OnToolbarbuttondel()
{
	CFacilityTaxiPoolBehavior * pCurrentBehavior = GetCurrentTaxiPoolBehavior();
	if(pCurrentBehavior == NULL)
		return;
	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	if(m_hCallFromExternal == m_TreeData.GetParentItem(hSelItem))
	{
		CCallFromExternal* pCallExternal = (CCallFromExternal*)m_TreeData.GetItemData(hSelItem);
		if (pCallExternal == NULL)
			return;

// 		HTREEITEM hNextSelItem = m_TreeData.GetNextSiblingItem(hSelItem);
// 		if(hNextSelItem == NULL)
// 			hNextSelItem = m_TreeData.GetPrevSiblingItem(hSelItem);
// 		if(hNextSelItem == NULL)
// 			hNextSelItem = m_TreeData.GetParentItem(hSelItem);

		m_TreeData.DeleteItem(hSelItem);
		pCurrentBehavior->getCallFromExternal()->DeleteItem(pCallExternal);

// 		if(hNextSelItem)
// 			m_TreeData.SelectItem(hNextSelItem);
		EvenPercent();
		LoadTreeProperties();
		m_TreeData.SelectItem(m_hCallFromExternal);
		UpdateFacilityToolbarState();
		SetModified();
	}
}

void CTaxiPoolFacilityDataPage::InitTaxiPoolFacilityList()
{
	int nCount = m_pTaxiPoolBehaviorDB->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CFacilityTaxiPoolBehavior *pTaxiPoolBehavior = (CFacilityTaxiPoolBehavior*)m_pTaxiPoolBehaviorDB->GetItem(nItem);
		if(pTaxiPoolBehavior && pTaxiPoolBehavior->getFacilityType() == m_enumType)
		{
			AddTaxiPoolFacilityToList(pTaxiPoolBehavior);
		}
	}
	
	if(nCount > 0)
		m_listboxFacility.SetCurSel(0);

	LoadTreeProperties();
	UpdateFacilityToolbarState();
}

int CTaxiPoolFacilityDataPage::AddTaxiPoolFacilityToList( CFacilityTaxiPoolBehavior *pBehavior )
{
	int nIndex = m_listboxFacility.AddString(pBehavior->getFacilityName().GetIDString()); ;
	m_listboxFacility.SetItemData(nIndex, (DWORD_PTR)pBehavior);
	return nIndex;
}

void CTaxiPoolFacilityDataPage::UpdateFacilityToolbarState()
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

void CTaxiPoolFacilityDataPage::OnSelchangeListFacility()
{
	LoadTreeProperties();
}

void CTaxiPoolFacilityDataPage::LoadTreeProperties()
{

	m_TreeData.DeleteAllItems();

	int nCurSel = m_listboxFacility.GetCurSel();
	if(nCurSel == LB_ERR)
	{
		return;
	}

	CFacilityTaxiPoolBehavior *pCurTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
	if(pCurTaxiPoolBehavior == NULL)
	{
		return;
	}

	//vehicle
	CString strVehicle ;
	strVehicle.Format("Vehicle Type: %s",pCurTaxiPoolBehavior->GetVehicleType());
	COOLTREE_NODE_INFO Vehiclecni;
	CCoolTree::InitNodeInfo(this,Vehiclecni);
	Vehiclecni.net = NET_SHOW_DIALOGBOX;
	m_hVehicleType = m_TreeData.InsertItem(strVehicle,Vehiclecni,FALSE,FALSE,TVI_ROOT);
	//m_TreeData.SetItemData(m_hVehicleType,(DWORD_PTR)pCurTaxiPoolBehavior->GetVehicleType());

	//recorder level
	COOLTREE_NODE_INFO levelcni;
	CCoolTree::InitNodeInfo(this,levelcni);
	levelcni.net = NET_EDITSPIN_WITH_VALUE;
	CString strRecordLevel;
	strRecordLevel.Format(_T("Reorder Level: %d"),pCurTaxiPoolBehavior->GetRecordLevel());
	m_hRecordLevel = m_TreeData.InsertItem(strRecordLevel,levelcni,FALSE,FALSE,TVI_ROOT);
	m_TreeData.SetItemData(m_hRecordLevel,(DWORD_PTR)pCurTaxiPoolBehavior->GetRecordLevel());

	//recorder quantity
	COOLTREE_NODE_INFO Quantitycni;
	CCoolTree::InitNodeInfo(this,Quantitycni);
	Quantitycni.net = NET_EDITSPIN_WITH_VALUE;
	CString srtRecordQuantity;
	srtRecordQuantity.Format(_T("Reorder Quantity: %d"),pCurTaxiPoolBehavior->GetRecordQuantity());
	m_hRecordQuantity = m_TreeData.InsertItem(srtRecordQuantity,Quantitycni,FALSE,FALSE,TVI_ROOT);
	m_TreeData.SetItemData(m_hRecordQuantity,(DWORD_PTR)pCurTaxiPoolBehavior->GetRecordQuantity());

	//call from external
	COOLTREE_NODE_INFO Externalcni;
	CCoolTree::InitNodeInfo(this,Externalcni);
	Externalcni.net = NET_NORMAL;
	m_hCallFromExternal = m_TreeData.InsertItem(_T("Call From External"),Externalcni,FALSE,FALSE,TVI_ROOT);
	CCallFromExternalist* CallExternalist = pCurTaxiPoolBehavior->getCallFromExternal();
	int nCount = CallExternalist->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		//object
		CCallFromExternal* CallExternal = CallExternalist->GetItem(i);
		COOLTREE_NODE_INFO objectcni;
		CCoolTree::InitNodeInfo(this,objectcni);
		objectcni.net = NET_EDIT_INT;
		objectcni.fMinValue = static_cast<float>(CallExternal->Getpercent());
		CString sPercent;
		sPercent.Format("( %d%% )", CallExternal->Getpercent());	
		CString entry = CallExternal->GetEntry().GetIDString();
		HTREEITEM hEntry = m_TreeData.InsertItem(entry+sPercent,objectcni,FALSE,FALSE,m_hCallFromExternal);
		m_TreeData.SetItemData(hEntry,(DWORD_PTR)CallExternal);
		
		//offset
		COOLTREE_NODE_INFO Offsetcni;
		CCoolTree::InitNodeInfo(this,Offsetcni);
		Offsetcni.net = NET_COMBOBOX;
		CString offset;
		offset.Format(_T("Entry Offset(min): %s"),CallExternal->GetOffset()->getDistName());
		HTREEITEM hOffset = m_TreeData.InsertItem(offset,Offsetcni,FALSE,FALSE,hEntry);
		m_TreeData.SetItemData(hOffset,(DWORD_PTR)EntryNodeType_Offset);

		//delay
		COOLTREE_NODE_INFO delaycni;
		CCoolTree::InitNodeInfo(this,delaycni);
		delaycni.net = NET_COMBOBOX;
		CString delay;
		delay.Format(_T("Delay Distribution(min): %s"),CallExternal->GetDelay()->getDistName());
		HTREEITEM hDelay = m_TreeData.InsertItem(delay,delaycni,FALSE,FALSE,hEntry);
		m_TreeData.SetItemData(hDelay,(DWORD_PTR)EntryNodeType_Delay);
		
		m_TreeData.Expand(hEntry, TVE_EXPAND); 
	}
	m_TreeData.Expand(m_hCallFromExternal, TVE_EXPAND);
}

void CTaxiPoolFacilityDataPage::UpdateTreeToolbarState()
{
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);

	if(m_listboxFacility.GetCurSel() == LB_ERR )
		return;

	HTREEITEM hSelItem = m_TreeData.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	if(hSelItem == m_hCallFromExternal)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE );
	}
	HTREEITEM hParentItem = m_TreeData.GetParentItem(hSelItem);
	if(hParentItem == m_hCallFromExternal)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE);	
	}
}

void CTaxiPoolFacilityDataPage::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateTreeToolbarState();
	*pResult = 0;
}

CFacilityTaxiPoolBehavior * CTaxiPoolFacilityDataPage::GetCurrentTaxiPoolBehavior()
{
	int nIdx = m_listboxFacility.GetCurSel();
	if( nIdx == LB_ERR )
		return NULL;

	CFacilityTaxiPoolBehavior *pThisBehavior =(CFacilityTaxiPoolBehavior *)m_listboxFacility.GetItemData(nIdx);
	return pThisBehavior;

}



LRESULT CTaxiPoolFacilityDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		if (hItem == m_hVehicleType)
		{
			CFacilityTaxiPoolBehavior *pCurTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strGroupName = dlg.GetName();
				if (!strGroupName.IsEmpty())//non empty
				{
					if (strGroupName.CompareNoCase(pCurTaxiPoolBehavior->GetVehicleType()) != 0)//not the same name as the old name
					{
						pCurTaxiPoolBehavior->SetVehicleType(strGroupName);
						CString strVehicle ;
						strVehicle.Format("Vehicle Type: %s",pCurTaxiPoolBehavior->GetVehicleType());
						m_TreeData.SetItemText(m_hVehicleType,strVehicle);
						SetModified();
					}
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
		COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
		pInfo->fMinValue = 0;
	}
	if(message==UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		
		CString strValue=*((CString*)lParam);
		int nValue=static_cast<int>(atoi(strValue));
		if(hItem == m_hRecordLevel)
		{
			CFacilityTaxiPoolBehavior *pCurTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
			if(pCurTaxiPoolBehavior != NULL)
			{
				pCurTaxiPoolBehavior->SetRecordLevel(nValue);
				CString strRecordLevel;
				strRecordLevel.Format(_T("Reorder Level: %d"), pCurTaxiPoolBehavior->GetRecordLevel());
				m_TreeData.SetItemText(m_hRecordLevel, strRecordLevel);
				m_TreeData.SetItemData(m_hRecordLevel,pCurTaxiPoolBehavior->GetRecordLevel());
				SetModified();
			}
		}
		if (hItem == m_hRecordQuantity)
		{
			CFacilityTaxiPoolBehavior *pCurTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
			if(pCurTaxiPoolBehavior != NULL)
			{
				pCurTaxiPoolBehavior->SetRecordQuantity(nValue);
				CString strRecordQuantity;
				strRecordQuantity.Format(_T("Reorder Quantity: %d"), pCurTaxiPoolBehavior->GetRecordQuantity());
				m_TreeData.SetItemText(m_hRecordQuantity, strRecordQuantity);
				m_TreeData.SetItemData(m_hRecordQuantity,pCurTaxiPoolBehavior->GetRecordQuantity());
				SetModified();
			}
		}
		if (m_TreeData.GetParentItem(hItem) == m_hCallFromExternal)
		{
			COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
			CFacilityTaxiPoolBehavior *pCurTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
			if(pCurTaxiPoolBehavior != NULL)
			{
				CCallFromExternal* pCallExternal = (CCallFromExternal*)m_TreeData.GetItemData(hItem);
				pCallExternal->SetPercent(nValue);

				CString sPercent;
				sPercent.Format("( %d%% )", pCallExternal->Getpercent());	
				CString entry = pCallExternal->GetEntry().GetIDString();
				pInfo->fMinValue = static_cast<float>(pCallExternal->Getpercent());
				m_TreeData.SetItemText(hItem,entry+sPercent);
				m_TreeData.SetItemData(hItem,(DWORD_PTR)pCallExternal);
			}
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
		HTREEITEM hItem=(HTREEITEM)wParam;
		HTREEITEM hParentItem = m_TreeData.GetParentItem(hItem);
		CCallFromExternal* pExternal = (CCallFromExternal*)m_TreeData.GetItemData(hParentItem);
		EntryNodeType NodeType = (EntryNodeType)m_TreeData.GetItemData(hItem);
		int iSelCombox = m_TreeData.GetCmbBoxCurSel(hItem);

		if(NodeType == EntryNodeType_Offset)
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
		else if(NodeType == EntryNodeType_Delay)
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

bool CTaxiPoolFacilityDataPage::IsItemDuplicate( HTREEITEM hParentItem,const CString& strName )
{
	HTREEITEM hItem = m_TreeData.GetChildItem(hParentItem);
	while(hItem)
	{

		CCallFromExternal *pCallFromExternal = (CCallFromExternal *)m_TreeData.GetItemData(hItem);
		if(pCallFromExternal == NULL)
			continue;
		if(pCallFromExternal->GetEntry().GetIDString().CompareNoCase(strName) == 0)
			return true;

		hItem = m_TreeData.GetNextSiblingItem(hItem);
	}

	return false;
}

void CTaxiPoolFacilityDataPage::EvenPercent()
{
	CFacilityTaxiPoolBehavior *pCurTaxiPoolBehavior = GetCurrentTaxiPoolBehavior();
	CCallFromExternalist* Externalist = pCurTaxiPoolBehavior->getCallFromExternal();
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
