// DlgStandCriteriaAssignment.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgStandCriteriaAssignment.h"
#include "../Common/AirportDatabase.h"
#include "./InputAirside/StandCriteriaAssignment.h"
#include "./InputAirside/ALTObject.h"
#include "./InputAirside/ALTAirport.h"
#include "./InputAirside/Stand.h"

// DlgStandCriteriaAssignment dialog

IMPLEMENT_DYNAMIC(DlgStandCriteriaAssignment, CXTResizeDialog)
DlgStandCriteriaAssignment::DlgStandCriteriaAssignment(int nProjID,CAirportDatabase *pAirPortdb,PSelectFlightType pSelectFlightType,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgStandCriteriaAssignment::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pAirportDB(pAirPortdb)
{
	m_pStandCriteriaList = new StandCriteriaAssignmentList();
	m_pSelectFlightType = pSelectFlightType; 
}

DlgStandCriteriaAssignment::~DlgStandCriteriaAssignment()
{
	if (m_pStandCriteriaList)
	{
		delete m_pStandCriteriaList;
		m_pStandCriteriaList = NULL;
	}
}

void DlgStandCriteriaAssignment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STAND_CRITERIA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(DlgStandCriteriaAssignment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,AddStandItem)
	ON_COMMAND(ID_TOOLBAR_DEL,RemoveStandItem)
	ON_COMMAND(ID_FLIGHTTYPE_ADD,AddFlightTypeItem)
	ON_COMMAND(ID_FLIGHTTYPE_EDIT,EditFlightItem)
	ON_COMMAND(ID_FLIGHTTYPE_REMOVE,RemoveFlightItem)
	ON_NOTIFY(TVN_SELCHANGED,IDC_STAND_CRITERIA, OnSelchangedTree)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,SaveData)
	ON_BN_CLICKED(IDOK,OnBnClickedOk)
END_MESSAGE_MAP()


// DlgStandCriteriaAssignment message handlers

BOOL DlgStandCriteriaAssignment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
 
	m_pStandCriteriaList->SetAirportDatabase(m_pAirportDB);
	m_pStandCriteriaList->ReadData(m_nProjID);

	OnInitToolbar();
	OnInitTreeCtrl();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetResize(IDC_STATIC_SHOW,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STAND_CRITERIA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

int DlgStandCriteriaAssignment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolbar.LoadToolBar(IDR_STANDCRITERIA_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void DlgStandCriteriaAssignment::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.left += 2;
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
}

void DlgStandCriteriaAssignment::AddStandItem()
{
	ALTObjectList vObject;
	int nCount = 0;
	ALTAirport::GetStandList(m_nProjID,vObject);
	nCount = (int)vObject.size();
	if (vObject.size() == 0)
	{
		AfxMessageBox(_T("No stand to be selected"));
		return;
	}
	HTREEITEM hItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	cni.unMenuID = IDR_MENU_STAND;
	hItem = m_wndTreeCtrl.InsertItem(_T(""),cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
	StandCriteriaAssignment* pStandCriteria = NULL;
	pStandCriteria = new StandCriteriaAssignment();
	m_pStandCriteriaList->addItem(pStandCriteria);
	m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pStandCriteria);
	m_wndTreeCtrl.DoEdit(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void DlgStandCriteriaAssignment::AddFlightTypeItem()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while(hChildItem)
	{
		CString strFlightType = _T("");
		strFlightType = m_wndTreeCtrl.GetItemText(hChildItem);
		if (!strFlightType.Compare(szBuffer))
		{
			MessageBox(_T("The Flight Type already exists!"));
			return;
		}
		hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hChildItem);
	}
	FlightTypeStandAssignment* pFlightType = NULL;
	pFlightType = new FlightTypeStandAssignment();
	pFlightType->SetAirportDatabase(m_pAirportDB);
	pFlightType->SetFlightType(fltType);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.unMenuID = IDR_MENU_FLIGHTTYPE;
	hChildItem = m_wndTreeCtrl.InsertItem(szBuffer,cni,FALSE,FALSE,hItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hChildItem,(DWORD_PTR)pFlightType);
	StandCriteriaAssignment* pStandCriteria = NULL;
	pStandCriteria = (StandCriteriaAssignment*)m_wndTreeCtrl.GetItemData(hItem);
	if (pStandCriteria)
	{
		pStandCriteria->addItem(pFlightType);
		Stand standObj;
		standObj.ReadObject(pStandCriteria->getStandID());
		if (standObj.GetLeadInLineList().GetItemCount() > 0&& standObj.GetLeadOutLineList().GetItemCount() > 0)
		{
			COOLTREE_NODE_INFO LeadCni;
			CCoolTree::InitNodeInfo(this,LeadCni);
			LeadCni.nt = NT_NORMAL;
			LeadCni.net = NET_COMBOBOX;
			CString strLeadinLineName = _T("");
			strLeadinLineName =  _T("EntyVia: LeadIn") + standObj.GetLeadInLineList().ItemAt(0).GetName();
			pFlightType->setLeadInLineID(standObj.GetLeadInLineList().ItemAt(0).GetID());
			HTREEITEM hLeadInItem = m_wndTreeCtrl.InsertItem(strLeadinLineName,LeadCni,FALSE,FALSE,hChildItem,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hLeadInItem,(DWORD)TREENODE_LEADINLINE);

			CString strLeadOutLineName = _T("");
			if(standObj.GetLeadOutLineList().GetItemCount()> 0)
			{
				strLeadOutLineName = _T("ExitVia: LeadOut") + standObj.GetLeadOutLineList().ItemAt(0).GetName();
				pFlightType->setLeadOutLineID(standObj.GetLeadOutLineList().ItemAt(0).GetID());
				HTREEITEM hLeadOutItem = m_wndTreeCtrl.InsertItem(strLeadOutLineName,LeadCni,FALSE,FALSE,hChildItem,TVI_LAST);
				m_wndTreeCtrl.SetItemData(hLeadOutItem,(DWORD)TREENODE_LEADOUTLINE);
			}
			m_wndTreeCtrl.Expand(hChildItem,TVE_EXPAND);
		}
		else
		{
			pStandCriteria->removeItem(pFlightType);
			m_wndTreeCtrl.DeleteItem(hChildItem);
		}
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	}
}

void DlgStandCriteriaAssignment::EditFlightItem()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	//HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	//HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	//while(hChildItem)
	//{
	//	hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
	//	if (hChildItem)
	//	{
	//		CString strFlightType = _T("");
	//		strFlightType = m_wndTreeCtrl.GetItemText(hChildItem);
	//		if (!strFlightType.Compare(szBuffer))
	//		{
	//			MessageBox(_T("The Flight Type already exists!"));
	//			return;
	//		}
	//	}
	//}
	FlightTypeStandAssignment* pFlightType = NULL;
	pFlightType = (FlightTypeStandAssignment*)m_wndTreeCtrl.GetItemData(hItem);
	if (pFlightType)
	{
		pFlightType->SetAirportDatabase(m_pAirportDB);
		pFlightType->SetFlightType(fltType);
		m_wndTreeCtrl.SetItemText(hItem,szBuffer);
	}
}

void DlgStandCriteriaAssignment::RemoveFlightItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	StandCriteriaAssignment* pStandCriteria = NULL;
	pStandCriteria = (StandCriteriaAssignment*)m_wndTreeCtrl.GetItemData(hParentItem);
	FlightTypeStandAssignment* pFlightType = NULL;
	pFlightType = (FlightTypeStandAssignment*)m_wndTreeCtrl.GetItemData(hItem);
	
	if (pStandCriteria)
	{
		pStandCriteria->removeItem(pFlightType);
		m_wndTreeCtrl.DeleteItem(hItem);
	}
}

LRESULT DlgStandCriteriaAssignment::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_COMBOBOX_BEGIN)
	{
		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		HTREEITEM hItem = (HTREEITEM)wParam;
		m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;
		if (pCB->GetCount() != 0)
		{
			pCB->ResetContent();
		}

		if (pCB->GetCount() == 0)
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem)
			{
				TreeNodeType emNodeType = (TreeNodeType)m_wndTreeCtrl.GetItemData(hItem);
				HTREEITEM hStandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
				StandCriteriaAssignment* pStandCriteria = (StandCriteriaAssignment*)m_wndTreeCtrl.GetItemData(hStandItem);
				if (pStandCriteria)
				{
					if (emNodeType == TREENODE_LEADINLINE)
					{
						SetLineInComBoxItem(pCB,pStandCriteria->getStandID());
					}
					else if (emNodeType == TREENODE_LEADOUTLINE)
					{
						SetLineOutComBoxItem(pCB,pStandCriteria->getStandID());	
					}
				}
			}
			else
			{
				SetComboBoxItem(pCB);
			}
			pCB->SetCurSel(0);
		}
	}
	else if (message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		DisplayTreeItem((HTREEITEM)wParam);
	}
	else if (message == UM_CEW_COMBOBOX_END)
	{
		DisplayTreeItem((HTREEITEM)wParam);
	}
	else if (message == UM_CEW_COMBOBOX_SETWIDTH)
	{
		m_wndTreeCtrl.SetWidth(200);
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void DlgStandCriteriaAssignment::SetComboBoxItem(CComboBox* pCB)
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	ALTObject* altObject = NULL;
	ALTObjectList vObject;
	ALTObjectID objName;
	int nCount = 0;

	ALTAirport::GetStandList(m_nProjID,vObject);
	nCount = (int)vObject.size();
	for (int i = 0; i< nCount; i++)
	{
		altObject = vObject.at(i).get();
		altObject->getObjName(objName);
		int nItemIndex = pCB->AddString(objName.GetIDString());
		pCB->SetItemData(nItemIndex,(DWORD_PTR)altObject->getID());
	}
}
void DlgStandCriteriaAssignment::RemoveStandItem()
{
	HTREEITEM hItem = m_hRclickItem;
	if (hItem == NULL)
	{
		return;
	}
	StandCriteriaAssignment* pStandCriteria = NULL;
	pStandCriteria = (StandCriteriaAssignment*)m_wndTreeCtrl.GetItemData(hItem);
	if (pStandCriteria)
	{
		m_pStandCriteriaList->removeItem(pStandCriteria);
		m_wndTreeCtrl.DeleteItem(hItem);
		OnInitTreeCtrl();
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void DlgStandCriteriaAssignment::OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
 	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	m_hRclickItem = hItem;
 	if (m_wndTreeCtrl.GetParentItem(hItem))
 	{
 		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
 		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
 	}
 	else
 	{
 		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
 		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
 	}
}

void DlgStandCriteriaAssignment::SetLineInComBoxItem(CComboBox* pCB,int nObjID)
{
	Stand standObj;
	standObj.ReadObject(nObjID);
	for (int i = 0; i < (int)standObj.GetLeadInLineList().GetItemCount(); i++)
	{
		StandLeadInLine& theItem = standObj.GetLeadInLineList().ItemAt(i);
		CString strLeadInLineName = _T("");
		strLeadInLineName = _T("LeadIn") + theItem.GetName();
		pCB->AddString(strLeadInLineName);
		pCB->SetItemData(i,(DWORD)(theItem.GetID()));
	}
}

void DlgStandCriteriaAssignment::SetLineOutComBoxItem(CComboBox* pCB,int nObjID)
{
	Stand standObj;
	standObj.ReadObject(nObjID);

	for (int i = 0; i < (int)standObj.GetLeadOutLineList().GetItemCount(); i++)
	{
		StandLeadOutLine& theItem = standObj.GetLeadOutLineList().ItemAt(i);
		CString strLeadOutLineName = _T("");
		strLeadOutLineName = _T("LeadOut") + theItem.GetName();
		int nIndex = pCB->AddString(strLeadOutLineName);
		pCB->SetItemData(nIndex,(DWORD)(theItem.GetID()));
	}
}

void DlgStandCriteriaAssignment::OnInitTreeCtrl()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	HTREEITEM hItem;
	HTREEITEM hChildItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for(int i = 0; i < m_pStandCriteriaList->getCount(); i++)
	{
		StandCriteriaAssignment* pStandCriteria = m_pStandCriteriaList->getItem(i);
		if (pStandCriteria)
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			cni.unMenuID = IDR_MENU_STAND;
			Stand standObj;
			standObj.ReadObject(pStandCriteria->getStandID());
			hItem = m_wndTreeCtrl.InsertItem(standObj.GetObjNameString(),cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pStandCriteria);

			for (int j = 0; j < pStandCriteria->getCount(); j++)
			{
				FlightTypeStandAssignment* pFlightType = pStandCriteria->getItem(j);
				cni.nt = NT_NORMAL;
				cni.net = NET_NORMAL;
				cni.unMenuID = IDR_MENU_FLIGHTTYPE;
				if (pFlightType)
				{
					hChildItem = m_wndTreeCtrl.InsertItem(pFlightType->GetFlightTypeScreenString(),cni,FALSE,FALSE,hItem,TVI_LAST);
					m_wndTreeCtrl.SetItemData(hChildItem,(DWORD_PTR)pFlightType);

					cni.nt = NT_NORMAL;
					cni.net = NET_COMBOBOX;
					CString strLeadinLineName = _T("");
					StandLeadInLine* pLeadInLine = NULL;
					pLeadInLine = standObj.GetLeadInLineList().GetItemByID(pFlightType->getLeadInLineID());
					if (pLeadInLine)
					{
						strLeadinLineName =  _T("EntyVia: LeadIn") + pLeadInLine->GetName();
						HTREEITEM hLeadInItem = m_wndTreeCtrl.InsertItem(strLeadinLineName,cni,FALSE,FALSE,hChildItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(hLeadInItem,(DWORD)TREENODE_LEADINLINE);
					}

					CString strLeadOutLineName = _T("");
					StandLeadOutLine* pLeadOutLine = NULL;
				
					pLeadOutLine = standObj.GetLeadOutLineList().GetItemByID(pFlightType->getLeadOutLineID());
					if (pLeadOutLine)
					{
						strLeadOutLineName = _T("ExitVia: LeadOut") + pLeadOutLine->GetName();
						HTREEITEM hLeadOutItem = m_wndTreeCtrl.InsertItem(strLeadOutLineName,cni,FALSE,FALSE,hChildItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(hLeadOutItem,(DWORD)TREENODE_LEADOUTLINE);
					}
					
					m_wndTreeCtrl.Expand(hChildItem,TVE_EXPAND);
				}
			}
			m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		}
	}
}

void DlgStandCriteriaAssignment::OnBnClickedOk()
{
	SaveData();
	OnOK();
}

void DlgStandCriteriaAssignment::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pStandCriteriaList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void DlgStandCriteriaAssignment::DisplayTreeItem(HTREEITEM hItem)
{
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParentItem)
	{
		HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
		TreeNodeType emNodeType = (TreeNodeType)m_wndTreeCtrl.GetItemData(hItem);
		int nLineID = (int)m_wndTreeCtrl.GetCurSelCmbBoxItemData(hItem);
		FlightTypeStandAssignment* pFlightType = (FlightTypeStandAssignment*)m_wndTreeCtrl.GetItemData(hParentItem);
		StandCriteriaAssignment* pStandCriteria = (StandCriteriaAssignment*)m_wndTreeCtrl.GetItemData(hGrandItem);
		CString strValue = m_wndTreeCtrl.GetItemText(hItem);
		if (emNodeType == TREENODE_LEADINLINE)
		{
			CString strLineInName = _T("EntyVia: ") + strValue;
			pFlightType->setLeadInLineID(nLineID);
			m_wndTreeCtrl.SetItemText(hItem,strLineInName);
		}
		else if (emNodeType == TREENODE_LEADOUTLINE)
		{
			CString strLineOutName =  _T("ExitVia: ") + strValue;
			pFlightType->setLeadOutLineID(nLineID);
			m_wndTreeCtrl.SetItemText(hItem,strLineOutName);
		}
	}
	else
	{
		int nObjID = (int)m_wndTreeCtrl.GetCurSelCmbBoxItemData(hItem);
		if (nObjID != -1)
		{
			StandCriteriaAssignment* pStandCriteria = (StandCriteriaAssignment*)m_wndTreeCtrl.GetItemData(hItem);
			if (pStandCriteria->getStandID() == -1)
			{
				pStandCriteria->setStandID(nObjID);
				if(m_pStandCriteriaList->IsExistInStandCriteriaList(pStandCriteria))
				{
					m_pStandCriteriaList->removeItem(pStandCriteria);
					m_wndTreeCtrl.DeleteItem(hItem);
				}				
			}
			else
			{		
				int nStandID = pStandCriteria->getStandID();
				pStandCriteria->setStandID(nObjID);
				if(m_pStandCriteriaList->IsExistInStandCriteriaList(pStandCriteria))
				{
					Stand standObj;
					standObj.ReadObject(nStandID);
					pStandCriteria->setStandID(nStandID);
					m_wndTreeCtrl.SetItemText(hItem,standObj.GetObjNameString());
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					return;
				}
			}
			HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
			if (hChildItem)
			{
				pStandCriteria->clear();
				m_wndTreeCtrl.DeleteItem(hChildItem);
			}
		}
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}