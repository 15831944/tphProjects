// DlgVehicleBehaviorDistritbution.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgVehicleBehaviorDistritbution.h"
#include ".\dlgvehiclebehaviordistritbution.h"
#include "../Landside/VehicleBehaviorDistributions.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgSelectLandsideObject.h"
#include "Common\ALT_TYPE.h"
#include "landside/LandsideLayoutObject.h"
#include "landside/InputLandside.h"
#include "../Inputs/IN_TERM.H"
#include "../common/ProbDistEntry.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"
#include "DlgProbDist.h"


// CDlgVehicleBehaviorDistritbution dialog
namespace
{
	const UINT ID_RESOURCE_NEW = 10;
	const UINT ID_VEHICLE_NEW = 11;
	const UINT ID_RESVEHI_DEL = 12;
}


IMPLEMENT_DYNAMIC(CDlgVehicleBehaviorDistritbution, CXTResizeDialog)
CDlgVehicleBehaviorDistritbution::CDlgVehicleBehaviorDistritbution(InputLandside* pInLandside, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgVehicleBehaviorDistritbution::IDD, pParent)
	, m_pInLandside(pInLandside)
	,m_pVehicleDistriution(NULL)
{

}

CDlgVehicleBehaviorDistritbution::~CDlgVehicleBehaviorDistritbution()
{
}

void CDlgVehicleBehaviorDistritbution::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgVehicleBehaviorDistritbution, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_RESOURCE_NEW,OnNewResource)
	ON_COMMAND(ID_VEHICLE_NEW,OnNewVehicle)
	ON_COMMAND(ID_RESVEHI_DEL,OnDelResVehi)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
END_MESSAGE_MAP()


// CDlgVehicleBehaviorDistritbution message handlers
int CDlgVehicleBehaviorDistritbution::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_ADDADDDELTOOLBAR))
	{
		return -1;
	}

	return 0;
}

BOOL CDlgVehicleBehaviorDistritbution::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	delete m_pVehicleDistriution;
	m_pVehicleDistriution = new VehicleBehaviorDistributions;
	m_pVehicleDistriution->ReadData(0);

	OnInitToolbar();
	UpdateToolBarState();
	LoadTreeContent();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	SetModified(FALSE);

	// TODO:  Add extra initialization here

	m_wndTreeCtrl.SetFocus();
	return FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgVehicleBehaviorDistritbution::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 2;
	m_wndToolbar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolbar.GetToolBarCtrl();
	spotBarCtrl.SetCmdID(0,ID_RESOURCE_NEW);
	spotBarCtrl.SetCmdID(1,ID_VEHICLE_NEW);
	spotBarCtrl.SetCmdID(2,ID_RESVEHI_DEL);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_RESOURCE_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_VEHICLE_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_RESVEHI_DEL, FALSE);



}

void CDlgVehicleBehaviorDistritbution::UpdateToolBarState()
{
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_RESOURCE_NEW,TRUE);
	
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_RESVEHI_DEL,FALSE);
		return;
	}
	
	
	VBNode* pNodeData = (VBNode*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_RESVEHI_DEL,FALSE);
		return;
	}

	if (pNodeData->nodeType == NodeType_Resource)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_RESVEHI_DEL,TRUE);
	}
	else if(pNodeData->nodeType == NodeType_VehicleType )
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_RESVEHI_DEL,TRUE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_RESVEHI_DEL,FALSE);
	}
}
	
void CDlgVehicleBehaviorDistritbution::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: Add your message handler code here
}



LRESULT CDlgVehicleBehaviorDistritbution::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			VBNode* pNodeData = (VBNode*)m_wndTreeCtrl.GetItemData(hItem);
			if(pNodeData->nodeType == NodeType_Resource)
			{
				CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
				dlg.AddObjType(ALT_LTOLLGATE);
				dlg.AddObjType(ALT_LCURBSIDE);
				dlg.AddObjType(ALT_LSTOPSIGN);
				dlg.AddObjType(ALT_LYIELDSIGN);
				dlg.AddObjType(ALT_LPARKINGLOT);
				dlg.AddObjType(ALT_LTAXIQUEUE);
				if(dlg.DoModal() == IDOK)
				{
					ALTObjectID ltObj = dlg.getSelectObject();
					pNodeData->pObject->setObjectID(ltObj);

					UpdateItemText(NodeType_Resource, pNodeData->pObject, pNodeData->pServiceTime, hItem);
					UpdateToolBarState();
					SetModified(TRUE);
				}
			}
			else if(pNodeData->nodeType == NodeType_VehicleType)
			{
				CDlgSelectLandsideVehicleType dlg;
				if(dlg.DoModal() == IDOK)
				{
					CString strGroupName = dlg.GetName();
					if (!strGroupName.IsEmpty())//non empty
					{
						//create new item
						pNodeData->pServiceTime->setVehicleType(strGroupName);
						UpdateItemText(NodeType_VehicleType, pNodeData->pObject, pNodeData->pServiceTime, hItem);						
						UpdateToolBarState();
						SetModified(TRUE);
					}
			}
			else
			{
				ASSERT(0);
			}
		}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			UpdateToolBarState();
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			VBNode* pNodeData = (VBNode*)m_wndTreeCtrl.GetItemData(hItem);
			if(pNodeData->nodeType == NodeType_ServiceTime)
			{
				CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;		

				if(pCB->GetCount()!=0)
				{
					pCB->ResetContent();
				}

				CString s;
				s.LoadString(IDS_STRING_NEWDIST);
				pCB->AddString(s);

				int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
				for( int m=0; m<nCount; m++ )
				{
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( m );		
					pCB->AddString(pPBEntry->m_csName);
				}



			}
			else if (pNodeData->nodeType == NodeType_Unit)
			{
				CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;		

				if(pCB->GetCount()!=0)
				{
					pCB->ResetContent();
				}

				pCB->AddString(_T("Seconds"));
				pCB->AddString(_T("Minutes"));
				pCB->AddString(_T("Hours"));

			}

		}
		break;
	case  UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			VBNode* pNodeData = (VBNode*)m_wndTreeCtrl.GetItemData(hItem);
			int iSelCombox = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);

			if(pNodeData->nodeType == NodeType_ServiceTime)
			{
				if(iSelCombox == 0)
				{
					CDlgProbDist dlg(  m_pInLandside->getInputTerminal()->m_pAirportDB, true );
					if(dlg.DoModal()==IDOK) 
					{
						int idxPD = dlg.GetSelectedPDIdx();
						ASSERT(idxPD!=-1);
						CProbDistEntry* pde = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem(idxPD);

						pNodeData->pServiceTime->setDstTimet(pde);
						UpdateItemText(NodeType_ServiceTime, pNodeData->pObject, pNodeData->pServiceTime, hItem);
						SetModified();

					}
				}
				else
				{
					ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getCount()) );
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getItem( iSelCombox-1 );

					pNodeData->pServiceTime->setDstTimet(pPBEntry);
					UpdateItemText(NodeType_ServiceTime, pNodeData->pObject, pNodeData->pServiceTime, hItem);
					SetModified();

				}
			}
			else if(pNodeData->nodeType == NodeType_Unit)
			{
				if(iSelCombox != LB_ERR)
				{
					pNodeData->pServiceTime->setTimeUnit((VBDistributionObjectServiceTime::TimeUnit) iSelCombox);
					UpdateItemText(NodeType_Unit, pNodeData->pObject, pNodeData->pServiceTime, hItem);
					SetModified();
				}
			}

		}
		break;

	case  UM_CEW_COMBOBOX_END:
		{

		}
		break;

	default:
		{
			//ASSERT(0);
		}
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}
void CDlgVehicleBehaviorDistritbution::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pVehicleDistriution->SaveData(0);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	SetModified(FALSE);
}
void CDlgVehicleBehaviorDistritbution::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	try
	{
		CADODatabase::BeginTransaction();
		m_pVehicleDistriution->SaveData(0);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}


	CXTResizeDialog::OnOK();
}

void CDlgVehicleBehaviorDistritbution::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}

void CDlgVehicleBehaviorDistritbution::SetModified( BOOL bModified /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bModified);
}


void CDlgVehicleBehaviorDistritbution::LoadTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	int nItemCount = m_pVehicleDistriution->GetItemCount();
	for (int nItem = 0; nItem < nItemCount; ++ nItem)
	{
		VBDistributionObject *pObject = m_pVehicleDistriution->GetItem(nItem);
		if(pObject)
		{
			HTREEITEM hNewItem = AddToTree(pObject);
			m_wndTreeCtrl.Expand(hNewItem, TVE_EXPAND);
		}
	}
	HTREEITEM hRoot = m_wndTreeCtrl.GetRootItem();
	if (hRoot)
	{
		m_wndTreeCtrl.SelectItem(hRoot);
	}
}
HTREEITEM CDlgVehicleBehaviorDistritbution::AddToTree( VBDistributionObject *pObject )
{

	HTREEITEM hObjItem = InsertTreeItem(NodeType_Resource, pObject, NULL, TVI_ROOT);
	int nServiceCount =  pObject->GetItemCount();
	for(int nService = 0; nService < nServiceCount; ++ nService)
	{
		VBDistributionObjectServiceTime *pVehicleTime = pObject->GetItem(nService);
		if(pVehicleTime)
		{
			AddToTree(pObject, pVehicleTime, hObjItem);
		}
	}
	m_wndTreeCtrl.Expand(hObjItem, TVE_EXPAND);

	return hObjItem;
}

HTREEITEM CDlgVehicleBehaviorDistritbution::AddToTree( VBDistributionObject *pObject, VBDistributionObjectServiceTime *pVehicleTime, HTREEITEM hParentItem )
{

	HTREEITEM hVehicleItem = InsertTreeItem(NodeType_VehicleType, pObject, pVehicleTime, hParentItem);

	InsertTreeItem(NodeType_ServiceTime, pObject, pVehicleTime, hVehicleItem);
	InsertTreeItem(NodeType_Unit, pObject, pVehicleTime, hVehicleItem);

	m_wndTreeCtrl.Expand(hVehicleItem, TVE_EXPAND);

	return hVehicleItem;
}

HTREEITEM CDlgVehicleBehaviorDistritbution::InsertTreeItem( NodeType nodeType, VBDistributionObject *pObject, VBDistributionObjectServiceTime *pServiceTime, HTREEITEM hParentItem )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;


	CString strItemText;
	if(nodeType == NodeType_Resource)
	{	
		cni.net = NET_SHOW_DIALOGBOX;
		strItemText.Format(_T("Facility: %s"), pObject->getNameString());
	}
	else if(nodeType == NodeType_VehicleType)
	{
		cni.net = NET_SHOW_DIALOGBOX;
		strItemText.Format(_T("Vehicle Type: %s"), pServiceTime->getVehicleType());
	}
	else if(nodeType == NodeType_ServiceTime)
	{
		cni.net = NET_COMBOBOX;
		strItemText.Format(_T("Service Time: %s"), pServiceTime->getDstTime()->getPrintDist());
	}
	else if(nodeType == NodeType_Unit)
	{
		cni.net = NET_COMBOBOX;
		strItemText.Format(_T("Unit: %s"), pServiceTime->getTimeUnitName());
	}
	else
	{
		ASSERT(0);
		return NULL;
	}
	HTREEITEM hNewItem = m_wndTreeCtrl.InsertItem(strItemText, cni, FALSE, FALSE, hParentItem);

	VBNode *pNode = new VBNode(nodeType, pObject, pServiceTime);
	m_wndTreeCtrl.SetItemData(hNewItem, (DWORD_PTR)pNode);
	m_vNodes.push_back(pNode);

	return hNewItem;

}

HTREEITEM CDlgVehicleBehaviorDistritbution::UpdateItemText( NodeType nodeType, VBDistributionObject *pObject, VBDistributionObjectServiceTime *pServiceTime, HTREEITEM hCurItem )
{
	CString strItemText;
	if(nodeType == NodeType_Resource)
	{	
		strItemText.Format(_T("Facility: %s"), pObject->getNameString());
	}
	else if(nodeType == NodeType_VehicleType)
	{
		strItemText.Format(_T("Vehicle Type: %s"), pServiceTime->getVehicleType());
	}
	else if(nodeType == NodeType_ServiceTime)
	{
		strItemText.Format(_T("Service Time: %s"), pServiceTime->getDstTime()->getPrintDist());
	}
	else if(nodeType == NodeType_Unit)
	{
		strItemText.Format(_T("Unit: %s"), pServiceTime->getTimeUnitName());
	}
	else
	{
		ASSERT(0);
		return NULL;
	}
	m_wndTreeCtrl.SetItemText(hCurItem, strItemText);

	return hCurItem;
}
void CDlgVehicleBehaviorDistritbution::OnNewResource()
{
	CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
	dlg.AddObjType(ALT_LTOLLGATE);
	dlg.AddObjType(ALT_LCURBSIDE);
	dlg.AddObjType(ALT_LSTOPSIGN);
	dlg.AddObjType(ALT_LYIELDSIGN);
	dlg.AddObjType(ALT_LPARKINGLOT);
	dlg.AddObjType(ALT_LTAXIQUEUE);
	if(dlg.DoModal() == IDOK)
	{
		ALTObjectID ltObj = dlg.getSelectObject();
		//check the object is existed or not

	 	HTREEITEM hFaclityItem = m_wndTreeCtrl.GetRootItem();
		while(hFaclityItem)
		{
			VBNode *pNode = (VBNode *)m_wndTreeCtrl.GetItemData(hFaclityItem);
			if(pNode != NULL && pNode->pObject != NULL)
			{
				if(pNode->pObject->getObjectID() == ltObj)
				{
					CString strMsg;
					strMsg.Format(_T("The facility %s has been existed."), ltObj.GetIDString());
					MessageBox(strMsg, _T("Warning"), MB_OK);
					m_wndTreeCtrl.SelectItem(hFaclityItem);
					return;
				}
			}

			hFaclityItem = m_wndTreeCtrl.GetNextSiblingItem(hFaclityItem);
		}



		VBDistributionObject *pObject = new VBDistributionObject;
		pObject->setObjectID(ltObj);
		m_pVehicleDistriution->AddItem(pObject);
		HTREEITEM hNewItem = AddToTree(pObject);
		if(hNewItem)
			m_wndTreeCtrl.SelectItem(hNewItem);
		UpdateToolBarState();
		SetModified();
	}
}

void CDlgVehicleBehaviorDistritbution::OnNewVehicle()
{
	HTREEITEM hSelectItem = m_wndTreeCtrl.GetSelectedItem();
	VBNode *pNode = (VBNode *)m_wndTreeCtrl.GetItemData(hSelectItem);
	if(pNode == NULL)
		return;
	if(pNode->nodeType != NodeType_Resource)
		return;


	CDlgSelectLandsideVehicleType dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString strGroupName = dlg.GetName();
		if (!strGroupName.IsEmpty())//non empty
		{

			HTREEITEM hVItem = m_wndTreeCtrl.GetChildItem(hSelectItem);
			while(hVItem)
			{
				VBNode *pNode = (VBNode *)m_wndTreeCtrl.GetItemData(hVItem);
				if(pNode != NULL && pNode->pServiceTime != NULL)
				{
					if(pNode->pServiceTime->getVehicleType().CompareNoCase(strGroupName) == 0)
					{
						CString strMsg;
						strMsg.Format(_T("The vehicle type %s has been existed."), strGroupName);
						MessageBox(strMsg, _T("Warning"), MB_OK);
						m_wndTreeCtrl.SelectItem(hVItem);
						return;
					}
				}

				hVItem = m_wndTreeCtrl.GetNextSiblingItem(hVItem);
			}


			//create new item
			VBDistributionObjectServiceTime *pNewItem = new VBDistributionObjectServiceTime;
			pNewItem->setVehicleType(strGroupName);
			if(pNode->pObject)
				pNode->pObject->AddItem(pNewItem);

			HTREEITEM hNewItem = AddToTree(pNode->pObject, pNewItem, hSelectItem);
			m_wndTreeCtrl.SelectItem(hNewItem);
			UpdateToolBarState();
			SetModified(TRUE);
		}
	}

}

void CDlgVehicleBehaviorDistritbution::OnDelResVehi()
{
	HTREEITEM hSelectItem = m_wndTreeCtrl.GetSelectedItem();
	VBNode *pNode = (VBNode *)m_wndTreeCtrl.GetItemData(hSelectItem);
	if(pNode == NULL)
		return;
	if(pNode->nodeType == NodeType_Resource)
	{
		m_pVehicleDistriution->DeleteItem(pNode->pObject);
		m_wndTreeCtrl.DeleteItem(hSelectItem);
		SetModified();
	}
	else if(pNode->nodeType == NodeType_VehicleType)
	{
		pNode->pObject->DeleteItem(pNode->pServiceTime);
		m_wndTreeCtrl.DeleteItem(hSelectItem);
		SetModified();
	}



}
LandsideFacilityLayoutObjectList* CDlgVehicleBehaviorDistritbution::GetLandsideLayoutObjectList()
{
	if (m_pInLandside == NULL)
		return NULL;

	return &m_pInLandside->getObjectList();
}

void CDlgVehicleBehaviorDistritbution::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	UpdateToolBarState();
}








