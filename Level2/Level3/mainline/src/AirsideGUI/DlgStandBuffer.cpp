// DlgStandBuffer.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "DlgStandBuffer.h"
#include "DlgSelectALTObject.h"
#include "DlgStandFamily.h"

static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;
static const LPCTSTR AddArray[] = {_T("Add Stand Group..."),_T("Add Departing Flight Type..."),_T("Add Arriving Flight Type...")};
static const LPCTSTR EditArray[] = { _T("Edit This Stand Group..."),_T("Edit This Departing Flight Type...")\
,_T("Edit This Arriving Flight Type..."),_T("Edit Buffer Time")};
static const LPCTSTR DelArray[] = { _T("Delete This Stand Group..."),_T("Delete This Departing Flight Type..."),_T("Delete This Arriving Flight Type...")};

IMPLEMENT_DYNAMIC(CDlgStandBuffer, CXTResizeDialog)

CDlgStandBuffer::CDlgStandBuffer(int nProjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirportDatabase , CWnd* pParent /*=NULL*/)
 : CXTResizeDialog(IDD_STANDBUFFER, pParent)
 ,m_psbStdBuffer(nProjID,pAirportDatabase)
 ,m_pSelectFlightType(pSelectFlightType)
 , m_nProjID(nProjID)
 ,m_pAirportDatabase(pAirportDatabase) 
{
}

CDlgStandBuffer::~CDlgStandBuffer()
{
}

void CDlgStandBuffer::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STANDBUFFER, m_wndTreeCtrl);
}

BEGIN_MESSAGE_MAP(CDlgStandBuffer, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_STANDBUFFER_ADD, OnNewItem)
	ON_COMMAND(ID_STANDBUFFER_DEL, OnDeleteItem)
	ON_COMMAND(ID_STANDBUFFER_EDIT, OnEditItem)
	ON_COMMAND(IDC_BUTTON_SAVE,OnSave)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText ) 
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_STANDBUFFER, OnNMDblclkTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDBUFFER, OnTvnSelchangedTree)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CDlgStandBuffer::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndTreeCtrl.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitToolbar();

	m_psbStdBuffer.ReadData(m_nProjID);
		
	RefreshTreeCtrl();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgStandBuffer::OnOK()
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgStandBuffer::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

int CDlgStandBuffer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS) ||	!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_STANDBUFFER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}


void CDlgStandBuffer::InitToolbar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolBar);
	ScreenToClient(rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);

// 	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
// 	toolbar.SetCmdID(0, ID_NEW_ITEM);
// 	toolbar.SetCmdID(1, ID_DEL_ITEM);
// 	toolbar.SetCmdID(2, ID_EDIT_ITEM);
// 
// 	toolbar.EnableButton(ID_NEW_ITEM, TRUE);
// 	toolbar.EnableButton(ID_DEL_ITEM, TRUE);
// 	toolbar.EnableButton(ID_EDIT_ITEM,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_EDIT, TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, TRUE);
}

void CDlgStandBuffer::RefreshTreeCtrl(void)
{
	HTREEITEM hRoot = 0;
	HTREEITEM hFirstGrade = 0;
	HTREEITEM hSecondGrade = 0;
	HTREEITEM hThirdGrade = 0;
	HTREEITEM hFourthGrade = 0;
	CString strStdGrp = _T("");
	CString strDepFltType = _T("");
	CString strArrFltType = _T("");
	long lBufferTime = 0L;

	long lStdGrpCount = 0;
	long lDepFltTypeCount = 0;
	long lArrFltTypeCount = 0;	

	m_wndTreeCtrl.SetRedraw(FALSE);
	m_wndTreeCtrl.DeleteAllItems();
	
	COOLTREE_NODE_INFO cni;
	CVehiclePoolsAndDeploymentCoolTree::InitNodeInfo(this,cni);	/*
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	cni.bAutoSetItemText = false;*/

	lStdGrpCount = m_psbStdBuffer.GetParkingStandgroupCount();
	m_ayDepFltType.RemoveAll();
	m_ayArrFltType.RemoveAll();
	hRoot = m_wndTreeCtrl.InsertItem(_T("Parking Stand Buffer"),cni,FALSE,FALSE);
	m_wndTreeCtrl.SelectItem(hRoot);
	CString strBufferNode = _T("");
	for (long lStdGrpIndex = 0;lStdGrpIndex < lStdGrpCount;lStdGrpIndex++)
	{
		PSBStdGrp * pPSBStdGrp = m_psbStdBuffer.GetParkingStandgroup(lStdGrpIndex);
		if(-1 != pPSBStdGrp->m_nStandGroupID)
		{ 
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(pPSBStdGrp->m_nStandGroupID);	
			strStdGrp = altObjGroup.getName().GetIDString(); 
			strBufferNode.Format(_T("Stand Group : %s"),strStdGrp);
		}
		else
			strBufferNode.Format(_T("Stand Group : %s"),_T("All"));
		CVehiclePoolsAndDeploymentCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		cni.bAutoSetItemText = false;
		hFirstGrade = m_wndTreeCtrl.InsertItem(strBufferNode,cni,FALSE,FALSE,hRoot);
		m_wndTreeCtrl.SetItemData(hFirstGrade,pPSBStdGrp->m_nStandGroupID);
		lDepFltTypeCount = pPSBStdGrp->GetDepartingFltTypeCount();
		for (long lDepFltTypeIndex = 0;lDepFltTypeIndex < lDepFltTypeCount;lDepFltTypeIndex++)
		{
			PSBDepartingFltType * pPSBDepartingFltType = pPSBStdGrp->GetDepartingFltType(lDepFltTypeIndex);
// 			strDepFltType = pPSBDepartingFltType->m_strFltTypeDeparting;
// 			m_ayDepFltType.Add(strDepFltType);
// 			FlightConstraint flightTypeDep;
// 			if(!m_pAirportDatabase)return;
// 			flightTypeDep.SetAirportDB(m_pAirportDatabase);
// 			flightTypeDep.setConstraintWithVersion(strDepFltType);			
	//		flightTypeDep.screenPrint(strDepFltType);
			strDepFltType = _T("");
			pPSBDepartingFltType->m_fltconstrain.screenPrint(strDepFltType);
			strBufferNode.Format(_T("Departing Flight Type : %s"),strDepFltType);
			CVehiclePoolsAndDeploymentCoolTree::InitNodeInfo(this,cni);
			cni.nt = NT_NORMAL;
			cni.net = NET_SHOW_DIALOGBOX;
			cni.bAutoSetItemText = false;
			hSecondGrade = m_wndTreeCtrl.InsertItem(strBufferNode,cni,FALSE,FALSE,hFirstGrade);			
			m_wndTreeCtrl.SetItemData(hSecondGrade,(DWORD_PTR)(&pPSBDepartingFltType->m_fltconstrain));
			lArrFltTypeCount = pPSBDepartingFltType->GetArrivingFltTypeCount();
			for (long lArrFltTypeIndex = 0;lArrFltTypeIndex < lArrFltTypeCount;lArrFltTypeIndex++)
			{
				PSBArrivingFltType * pPSBArrivingFltType = pPSBDepartingFltType->GetArrivingFltType(lArrFltTypeIndex);				
// 				strArrFltType = pPSBArrivingFltType->m_strFltTypeArriving;
// 				m_ayArrFltType.Add(strArrFltType);
// 				FlightConstraint flightTypeArr;
// 				if(!m_pAirportDatabase)return;
// 				flightTypeArr.SetAirportDB(m_pAirportDatabase);
// 				flightTypeArr.setConstraintWithVersion(strArrFltType);
//				flightTypeArr.screenPrint(strArrFltType);
				strArrFltType = _T("");
				pPSBArrivingFltType->m_fltconstrain.screenPrint(strArrFltType);
				lBufferTime = pPSBArrivingFltType->m_lBufferTime;
				
				strBufferNode.Format(_T("Arriving Flight Type : %s"),strArrFltType);
				CVehiclePoolsAndDeploymentCoolTree::InitNodeInfo(this,cni);
				cni.nt = NT_NORMAL;
				cni.net = NET_SHOW_DIALOGBOX;
				cni.bAutoSetItemText = false;
				hThirdGrade = m_wndTreeCtrl.InsertItem(strBufferNode,cni,FALSE,FALSE,hSecondGrade);	 
				 
				CVehiclePoolsAndDeploymentCoolTree::InitNodeInfo(this,cni);
				cni.nt = NT_NORMAL;
				cni.net = NET_EDITSPIN_WITH_VALUE;
				cni.bAutoSetItemText = false;
				strBufferNode.Format(_T("Buffer Time [ %d ] mins"),lBufferTime);
				hFourthGrade = m_wndTreeCtrl.InsertItem(strBufferNode,cni,FALSE,FALSE,hThirdGrade);
				m_wndTreeCtrl.SetItemData(hFourthGrade,(DWORD_PTR)(lBufferTime));
				m_wndTreeCtrl.Expand(hThirdGrade,TVE_EXPAND);
				
				m_wndTreeCtrl.SetItemData(hThirdGrade,(DWORD_PTR)(&pPSBArrivingFltType->m_fltconstrain));
			}
			m_wndTreeCtrl.Expand(hSecondGrade,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hFirstGrade,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hRoot,TVE_EXPAND);
	m_wndTreeCtrl.SetRedraw(TRUE);
}

void CDlgStandBuffer::OnNewItem(void)
{
	HTREEITEM hSelItem = 0;
	HTREEITEM hSelItemParent = 0;
	HTREEITEM hSelItemChild = 0;
	HTREEITEM hItemTest = 0;
	hSelItem = m_wndTreeCtrl.GetSelectedItem(); 
	if(!hSelItem)
		return;
	hSelItemParent = m_wndTreeCtrl.GetParentItem(hSelItem);
	hSelItemChild  = m_wndTreeCtrl.GetChildItem(hSelItem);

	CString strParentItemTitle = _T("");
	CString strItemTitle = _T("");
	CString strItemChildrenTitle = _T("");
	char szBuffer[1024] = {0};

	if (!hSelItemParent)
	{
		//stand group
		CDlgStandFamily  dlg(m_nProjID);
		if(IDOK == dlg.DoModal())
		{	
			m_psbStdBuffer.AddStandGroup(dlg.GetSelStandFamilyID(),FlightConstraint(),FlightConstraint());
			RefreshTreeCtrl();
		}
	}
	else 
	{
		hItemTest = m_wndTreeCtrl.GetParentItem(hSelItemParent);	
		if(hItemTest == 0)
		{
			//departing flight type
			if (m_pSelectFlightType)
			{
				FlightConstraint fltType = (*m_pSelectFlightType)(NULL);	
//				fltType.screenPrint(szBuffer);
//				fltType.screenPrint(strItemChildrenTitle);
//				strItemChildrenTitle = szBuffer;

				//int nSelCorrectFltType= strItemChildrenTitle.Find(_T("DEPARTING"));
				//if(-1 == nSelCorrectFltType)
				//{
				//	AfxMessageBox(_T("Flight type should be: 'DEPARTING'"));
				//	return;
				//}

				int nParentStdGrpID = (int) m_wndTreeCtrl.GetItemData(hSelItem);
				m_psbStdBuffer.AddStandGroup(nParentStdGrpID,fltType,FlightConstraint(),0L);
				RefreshTreeCtrl();
			}
		}
		else 
		{
			//arriving flight type
			hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent)); 
			if(hItemTest == 0)
			{
				if (m_pSelectFlightType)
				{
					FlightConstraint fltType = (*m_pSelectFlightType)(NULL);	
//					fltType.printConstraint(szBuffer);
//					strItemChildrenTitle = szBuffer;
	//				fltType.screenPrint(strItemChildrenTitle);

					//int nSelCorrectFltType = strItemChildrenTitle.Find(_T("ARRIVING"));
					//if(-1 == nSelCorrectFltType)
					//{
					//	AfxMessageBox(_T("Flight type should be: 'ARRIVING'"));
					//	return;
					//}


					FlightConstraint* DepFlightType = (FlightConstraint*) m_wndTreeCtrl.GetItemData(hSelItem);
				//	strItemTitle = m_ayDepFltType.GetAt(nParentDepFltTypeIndex); 
					int nStdGrpID = (int) (m_wndTreeCtrl.GetItemData(hSelItemParent)); 
					//m_psbStdBuffer.AddStandGroup(nStdGrpID,fltType,*DepFlightType,10);
					PSBDepartingFltType* pDepData = m_psbStdBuffer.GetDepartingFltType(nStdGrpID,*DepFlightType);
					pDepData->AddArrivingFltType(fltType,10);
					RefreshTreeCtrl();
				}
			}
		}
	}
	
}

void CDlgStandBuffer::OnDeleteItem(void)
{
	HTREEITEM hSelItem = 0;
	HTREEITEM hSelItemParent = 0;
	HTREEITEM hSelItemChild = 0;
	HTREEITEM hItemTest = 0;
	hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (!hSelItem)
		return;

	hSelItemParent = m_wndTreeCtrl.GetParentItem(hSelItem);
	hSelItemChild  = m_wndTreeCtrl.GetChildItem(hSelItem);

	CString strParentItemTitle = _T("");
	CString strItemTitle = _T("");
	CString strItemChildrenTitle = _T("");
	int nStdGrpID = -2;
 
	hItemTest = m_wndTreeCtrl.GetParentItem(hSelItemParent); 
	if(hItemTest == 0)
	{
		//stand group 
		int nStdGrpID = m_wndTreeCtrl.GetItemData(hSelItem);
		m_psbStdBuffer.DeleteStandGroup(nStdGrpID);
		
	}
	else
	{			
		hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent)); 
		if(hItemTest == 0)
		{ 
			//departing flight type
			nStdGrpID = (int) (m_wndTreeCtrl.GetItemData(hSelItemParent));
			FlightConstraint* DepFltType = (FlightConstraint*) m_wndTreeCtrl.GetItemData(hSelItem) ;					
			m_psbStdBuffer.DeleteDepartingFltType(nStdGrpID,*DepFltType);
		}
		else
		{
			hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent))); 
			if(hItemTest == 0)
			{
				nStdGrpID = (int) m_wndTreeCtrl.GetItemData(m_wndTreeCtrl.GetParentItem(hSelItemParent));

				FlightConstraint* DepFltType = (FlightConstraint*) (m_wndTreeCtrl.GetItemData(hSelItemParent));

				FlightConstraint* ArrFltType = (FlightConstraint*) (m_wndTreeCtrl.GetItemData(hSelItem));
				m_psbStdBuffer.DeleteArrivingFltType(nStdGrpID,*DepFltType,*ArrFltType);
			}
		}
	}

	RefreshTreeCtrl();
}

void CDlgStandBuffer::OnSave(void)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_psbStdBuffer.SaveData();
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
}


void CDlgStandBuffer::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgStandBuffer::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hSelItem = 0;
	HTREEITEM hSelItemParent = 0;
	HTREEITEM hSelItemChild = 0;
	HTREEITEM hItemTest = 0; 

	hSelItem = m_wndTreeCtrl.GetSelectedItem();
	hSelItemParent = m_wndTreeCtrl.GetParentItem(hSelItem);
	hSelItemChild  = m_wndTreeCtrl.GetChildItem(hSelItem);
	
	InitToolbar(); 	
 
	if(!hSelItemParent)
	{
		//root ID_STANDBUFFER_ADD  ID_STANDBUFFER_DEL
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_EDIT, FALSE);
	}
	else 
	{
		hItemTest = m_wndTreeCtrl.GetParentItem(hSelItemParent); 
		if(hItemTest == 0)
		{
			//stand group
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_EDIT, FALSE); 
		}
		else
		{			
			hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent)); 
			if(hItemTest == 0)
			{ 
				//departing flight type
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_EDIT, FALSE); 
			}
			else
			{
				hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent))); 
				if(hItemTest == 0)
				{
					//arriving flight type
					m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_EDIT, FALSE); 
					m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, FALSE);
				}
				else
				{
					//buffer time
					m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_ADD, FALSE);
					m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STANDBUFFER_DEL, FALSE);
				}
			}
		}	 
	}
	*pResult = 0;
}

LRESULT CDlgStandBuffer::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		CString strValue = *((CString*)lParam);
		int nValue = _ttoi(strValue);

		HTREEITEM hSelItemParentParentParent = 0;
		HTREEITEM hSelItemParentParent = 0;
		HTREEITEM hSelItemParent = 0; 

		hSelItemParent = m_wndTreeCtrl.GetParentItem(hSelItem);//arriving flight type
		hSelItemParentParent  = m_wndTreeCtrl.GetParentItem(hSelItemParent);//departing flight type
		hSelItemParentParentParent  = m_wndTreeCtrl.GetParentItem(hSelItemParentParent);//stand group

		int nStdGrpID = (int) m_wndTreeCtrl.GetItemData(hSelItemParentParentParent);
		FlightConstraint* DepFltType = (FlightConstraint*) (m_wndTreeCtrl.GetItemData(hSelItemParentParent));
		FlightConstraint* ArrFltType = (FlightConstraint*) (m_wndTreeCtrl.GetItemData(hSelItemParent));

		m_psbStdBuffer.AddStandGroup(nStdGrpID,*DepFltType,*ArrFltType,nValue);

		RefreshTreeCtrl();
	}
	else if(message == UM_CEW_EDITSPIN_BEGIN)
	{
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		long lBufferTime = m_wndTreeCtrl.GetItemData(hSelItem);
		m_wndTreeCtrl.SetWidth(lBufferTime);
	}
	else if(message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		OnEditItem();
	}
	else if (message == UM_CEW_SHOW_DIALOGBOX_END)
	{
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgStandBuffer::OnEditItem(void)
{
	HTREEITEM hSelItem = 0;
	HTREEITEM hSelItemParent = 0;
	HTREEITEM hSelItemChild = 0;
	HTREEITEM hItemTest = 0;
	hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (!hSelItem)
		return;

	hSelItemParent = m_wndTreeCtrl.GetParentItem(hSelItem);
	hSelItemChild  = m_wndTreeCtrl.GetChildItem(hSelItem);

	CString strParentItemTitle = _T("");
	CString strItemTitle = _T("");
	CString strItemChildrenTitle = _T("");
	int nStdGrpID = -2;
	char szBuffer[1024] = {0};

	hItemTest = m_wndTreeCtrl.GetParentItem(hSelItemParent); 
	if(hItemTest == 0)
	{
		//stand group
		CDlgStandFamily  dlg(m_nProjID);
		if(IDOK == dlg.DoModal())
		{	
			int iResult = m_psbStdBuffer.UpdateStandGroup((int)m_wndTreeCtrl.GetItemData(hSelItem),dlg.GetSelStandFamilyID());
			if(2 == iResult)
			{
				MessageBox(_T("can not edit to an existent stand group."));
				return;
			}
			else if(0 == iResult)
				RefreshTreeCtrl();
		}

	}
	else
	{			
		hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent)); 
		if(hItemTest == 0)
		{  						
			//departing flight type
			if (m_pSelectFlightType)
			{
				FlightConstraint fltType = (*m_pSelectFlightType)(NULL);	
	//			fltType.screenPrint(szBuffer);
		//		fltType.screenPrint(strItemChildrenTitle);
	//			strItemChildrenTitle = szBuffer;

				//int nSelCorrectFltType= strItemChildrenTitle.Find(_T("DEPARTING"));
				//if(-1 == nSelCorrectFltType)
				//{
				//	AfxMessageBox(_T("Flight type should be: 'DEPARTING'"));
				//	return;
				//}



				nStdGrpID = (int) (m_wndTreeCtrl.GetItemData(hSelItemParent));
				FlightConstraint* DepFltType = (FlightConstraint*) m_wndTreeCtrl.GetItemData(hSelItem);	 
				int iResult = m_psbStdBuffer.UpdateDepartingFltType(nStdGrpID,*DepFltType,fltType); 
				if(2 == iResult)
				{
					MessageBox(_T("can not edit to an existent departing flight type."));
					return;
				}
				else if(0 == iResult)
					RefreshTreeCtrl();
			}
		}
		else
		{
			hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent))); 
			if(hItemTest == 0)
			{
				nStdGrpID = (int) m_wndTreeCtrl.GetItemData(m_wndTreeCtrl.GetParentItem(hSelItemParent));

				int nIndex = -1;
				FlightConstraint* DepFltType = (FlightConstraint*) (m_wndTreeCtrl.GetItemData(hSelItemParent));
		//		strItemTitle = m_ayDepFltType.GetAt(nIndex);

				nIndex = -1;
				FlightConstraint* ArrFltType = (FlightConstraint*) (m_wndTreeCtrl.GetItemData(hSelItem));
		//		strItemChildrenTitle = m_ayArrFltType.GetAt(nIndex); 

				if (m_pSelectFlightType)
				{
					FlightConstraint fltType = (*m_pSelectFlightType)(NULL);	
//					fltType.printConstraint(szBuffer);
//					strParentItemTitle = szBuffer; 

			//		fltType.screenPrint(strParentItemTitle);
					//int nSelCorrectFltType = strParentItemTitle.Find(_T("ARRIVING"));
					//if(-1 == nSelCorrectFltType)
					//{
					//	AfxMessageBox(_T("Flight type should be: 'ARRIVING'"));
					//	return;
					//}

					int iResult = m_psbStdBuffer.UpdateArrivingFltType(nStdGrpID,*DepFltType,*ArrFltType,fltType);
					if(2 == iResult)
					{
						MessageBox(_T("can not edit to an existent arriving flight type."));
						return;
					}
					else if(0 == iResult)
						RefreshTreeCtrl();
				}
			}
			else
			{
				hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent)))); 
				if(hItemTest == 0)
				{
					long lBufferTime = m_wndTreeCtrl.GetItemData(hSelItem);
					m_wndTreeCtrl.SetWidth(lBufferTime);
					COOLTREE_NODE_INFO* pCNI = 0;
					pCNI = m_wndTreeCtrl.GetItemNodeInfo(hSelItem);
					if(!pCNI) return;
					m_wndTreeCtrl.ShowEditSpinWnd(hSelItem,pCNI);
				}
			}
		}
	}  
}


void CDlgStandBuffer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeCtrl.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeCtrl.SetFocus();
	CPoint pt = point;
	m_wndTreeCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeCtrl.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeCtrl.SelectItem(hRClickItem);

	CMenu menuPopup; 
	menuPopup.CreatePopupMenu();

	HTREEITEM hSelItem = 0;
	HTREEITEM hSelItemParent = 0;
	HTREEITEM hSelItemChild = 0;
	HTREEITEM hItemTest = 0; 

	hSelItem = hRClickItem;
	hSelItemParent = m_wndTreeCtrl.GetParentItem(hSelItem);
	hSelItemChild  = m_wndTreeCtrl.GetChildItem(hSelItem); 

	if(!hSelItemParent)
	{
		//root 
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_ADD,_T("Add Stand Group...") );
	}
	else 
	{
		hItemTest = m_wndTreeCtrl.GetParentItem(hSelItemParent); 
		if(hItemTest == 0)
		{
			//stand group
			menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_ADD, _T("Add Departing Flight Type...") ); 
			menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_EDIT, _T("Edit This Stand Group...") ); 
			menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_DEL, _T("Delete This Stand Group...") );
		}
		else
		{			
			hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent)); 
			if(hItemTest == 0)
			{ 
				//departing flight type
				menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_ADD, _T("Add Arriving Flight Type...") );
				menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_EDIT, _T("Edit This Departing Flight Type...") );
				menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_DEL, _T("Delete This Departing Flight Type...") );
			}
			else
			{
				hItemTest = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hSelItemParent))); 
				if(hItemTest == 0)
				{
					//arriving flight type 
					menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_EDIT, _T("Edit This Arriving Flight Type...") );
					menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_DEL, _T("Delete This Arriving Flight Type...") );
				}
				else
				{
					//buffer time
					menuPopup.AppendMenu(MF_POPUP, (UINT) ID_STANDBUFFER_EDIT, _T("Edit Buffer Time") );
				}
			}
		}	 
	}
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}

BOOL CDlgStandBuffer::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
	if (GetRoutingFrame() != NULL) return FALSE;

	// to be thorough we will need to handle UNICODE versions of the message also !!
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
//	TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool 
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (nID!= 0) // will be zero on a separator
	{
		int nCount = GetTreeLevel(hItem);
		switch(nID)
		{
		case ID_STANDBUFFER_ADD:
			{
				if (nCount < 3)
				{
					strTipText = AddArray[nCount];
				}
				else
				{
					strTipText = _T("");
				}
			}
			break;
		case ID_STANDBUFFER_EDIT:
			{
				if (nCount == 0)
				{
					strTipText = _T("");
				}
				else
				{
					strTipText = EditArray[nCount-1];
				}
			}
			break;
		case ID_STANDBUFFER_DEL:
			{
				if (nCount == 0)
				{
					strTipText = _T("");
				}
				else
				{
					strTipText = DelArray[nCount-1];
				}
			}
			break;
		default:
			break;
		}
// 		AfxLoadString(nID, szFullText);
// 		strTipText=szFullText;

#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif

		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}

int CDlgStandBuffer::GetTreeLevel(HTREEITEM hItem)
{
	int nCount = 0;
	if (hItem == NULL)
	{
		return -1;
	}
	while (m_wndTreeCtrl.GetParentItem(hItem))
	{
		nCount++;
		hItem = m_wndTreeCtrl.GetParentItem(hItem);
	}
	return nCount;
}
