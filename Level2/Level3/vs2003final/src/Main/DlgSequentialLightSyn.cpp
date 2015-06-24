// DlgSequentialLightSyn.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSequentialLightSyn.h"
#include "Common/UnitPiece.h"
#include "DlgSelectLandsideObject.h"
#include "landside/LandsideLayoutObject.h"
#include "Landside/LandsideIntersectionNode.h"
#include "TermPlanDoc.h"
#include "landside/InputLandside.h"
// CDlgSequentialLightSyn dialog
namespace
{
	const UINT ID_SEQUENTIAL_NEW = WM_USER + 100;
	const UINT ID_SEQUENTIAL_DEL = WM_USER + 101;

	const UINT ID_MENU_ADD = WM_USER + 102;
	const UINT ID_MENU_DEL = WM_USER + 103;
}

IMPLEMENT_DYNAMIC(CDlgSequentialLightSyn, CDialog)
CDlgSequentialLightSyn::CDlgSequentialLightSyn(CTermPlanDoc *tmpDoc,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSequentialLightSyn::IDD, pParent)
	,m_pDoc(tmpDoc)
{
}

CDlgSequentialLightSyn::~CDlgSequentialLightSyn()
{
	ClearTreeNodeData();
}

void CDlgSequentialLightSyn::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSequentialLightSyn, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_COMMAND(ID_SEQUENTIAL_NEW,OnCmdNewItem)
	ON_COMMAND(ID_SEQUENTIAL_DEL,OnCmdDeleteItem)
	ON_COMMAND(ID_MENU_ADD,OnAddIntersect)
	ON_COMMAND(ID_MENU_DEL,OnDeleteIntersection)
END_MESSAGE_MAP()


// CDlgSequentialLightSyn message handlers
BOOL CDlgSequentialLightSyn::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_sequentialLightSyn.ReadData(-1);

	OnInitToolbar();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	/////	
	CUnitPiece::InitializeUnitPiece(GetProjectID(),UP_ID_1003,this);

	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndTreeCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Velocity_InUse);
	/////

	LoadTreeContent();
	UpdateToolBarState();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgSequentialLightSyn::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}
	return 0;
}

void CDlgSequentialLightSyn::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_sequentialLightSyn.SaveData(-1);
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

void CDlgSequentialLightSyn::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_sequentialLightSyn.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgSequentialLightSyn::OnCmdNewItem()
{
	SequentialLightRule* pRule = new SequentialLightRule();
	m_sequentialLightSyn.AddNewItem(pRule);
	InsertRuleItem(TVI_ROOT,pRule);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSequentialLightSyn::OnCmdDeleteItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	SequentialLightRule* pRule = (SequentialLightRule*)(pNodeData->m_dwData);
	m_sequentialLightSyn.DeleteItem(pRule);
	DeleteTreeNodeData(pNodeData);

	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

LRESULT CDlgSequentialLightSyn::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			if (hItem)
			{
				TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				if (pNodeData)
				{
					if (pNodeData->m_emType == TreeNodeData::RuleName_Node)
					{
						SequentialLightRule* pRule = (SequentialLightRule*)(pNodeData->m_dwData);
						if (!strValue.IsEmpty() && strValue.CompareNoCase( pRule->GetName()))
						{
							if (m_sequentialLightSyn.ExistSameName(strValue))
							{
								MessageBox(_T("Exist the same name!!."),_T("Warning"),MB_OK);
							}
							else
							{
								pRule->SetName(strValue);
								CString strName;
								strName.Format(_T("Name: %s"),strValue);
								strName.MakeUpper();
								m_wndTreeCtrl.SetItemText(hItem,strName);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
							}
						}
					}
				}
			}
		}
		break;
	case UM_CEW_EDITSPIN_BEGIN:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				cni->fMinValue = 0.0;
			}
		}
		break;
	case  UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			double dValue = atof(strValue);
			if (pNodeData)
			{
				double dSpeed = CARCVelocityUnit::ConvertVelocity(CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,dValue);
				SequentialLightRule* pRule = (SequentialLightRule*)(pNodeData->m_dwData);
				pRule->SetSpeed(dSpeed);
				CString strSpeed;
				strSpeed.Format(_T("Syncronization Speed(%s): %.2f"),CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()),dValue);
				cni->fMinValue = static_cast<float>(dValue);
				m_wndTreeCtrl.SetItemText(hItem,strSpeed);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::Intersection_Node)
			{
				SequetailLightNodeRoute* pNode= (SequetailLightNodeRoute*)(pNodeData->m_dwData);
				if (pNode)
				{
					CDlgSelectLandsideObject* pDlgSelectLandsideObject = new CDlgSelectLandsideObject(GetLandsideLayoutObjectList(),this);
					pDlgSelectLandsideObject->AddObjType(ALT_LINTERSECTION);
					pCNI->pEditWnd = pDlgSelectLandsideObject;
				}
			}

		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);

			if (pNodeData && pNodeData->m_emType == TreeNodeData::Intersection_Node)
			{
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				if (hParentItem)
				{
					TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
					CDlgSelectLandsideObject* pDlgSelectLandsideObject = (CDlgSelectLandsideObject*)pCNI->pEditWnd;
					SequetailLightNodeRoute* pNode= (SequetailLightNodeRoute*)(pNodeData->m_dwData);

					if (pSuperNode)
					{
					
						SequetailLightNodeRoute* pParentNode = (SequetailLightNodeRoute*)(pSuperNode->m_dwData);
						bool bEqual = pNode->GetIntersection() == pDlgSelectLandsideObject->getSelectObject() ? true : false;
						if (!bEqual)
						{
							if (pParentNode && !pParentNode->HasIntersection(pDlgSelectLandsideObject->getSelectObject()))
							{
								LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
								if (pLayoutList)
								{
									LandsideFacilityLayoutObject* pObject = pLayoutList->getObjectByName(pDlgSelectLandsideObject->getSelectObject());
									if (pObject)
									{
										LandsideIntersectionNode* pIntersectNode = (LandsideIntersectionNode*)pObject;
										if (pIntersectNode->getNodeType() != LandsideIntersectionNode::_Signalized)
										{
											MessageBox(_T("Can't select intersection node without traffic light!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
										}
										else
										{
											if (pLayoutList->getObjectByName(pDlgSelectLandsideObject->getSelectObject()) == NULL)
											{
												MessageBox(_T("Can't select group of intersection node!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
											}
											else
											{
												pNode->SetIntersection(pDlgSelectLandsideObject->getSelectObject());
												m_wndTreeCtrl.SetItemText(hItem,pNode->GetIntersection().GetIDString());
												GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
											}
										}
									}
								}
							}
							else
							{
								MessageBox(_T("Exist the same intersection node!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
							}
						}
						
					}
				}
			}
		}
		break;
	default:
		break;
	}
		
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgSequentialLightSyn::OnContextMenu( CWnd* pWnd, CPoint point )
{
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
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hRClickItem);
	if (pNodeData == NULL)
		return;

	CMenu menuPopup; 
	menuPopup.CreatePopupMenu();
	switch (pNodeData->m_emType)
	{
	case TreeNodeData::IntersectionList_Node:
		{
			if (!m_wndTreeCtrl.ItemHasChildren(hRClickItem))
			{
				menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Add Intersection Node"));
				menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
			}
		}
		break;
	case TreeNodeData::Intersection_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Add Intersection Node"));
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Intersection Node"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgSequentialLightSyn::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 2;
	m_wndToolbar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolbar.GetToolBarCtrl();
	spotBarCtrl.SetCmdID(0,ID_SEQUENTIAL_NEW);
	spotBarCtrl.SetCmdID(1,ID_SEQUENTIAL_DEL);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_SEQUENTIAL_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_SEQUENTIAL_DEL, FALSE);
}

void CDlgSequentialLightSyn::UpdateToolBarState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_DEL,FALSE);
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_NEW,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_DEL,FALSE);
		}
		else
		{
			if (pNodeData->m_emType == TreeNodeData::RuleName_Node)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_DEL,TRUE);
			}
			else
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_DEL,FALSE);
			}
		}
	}
}

void CDlgSequentialLightSyn::LoadTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	for (int i = 0; i < m_sequentialLightSyn.GetItemCount(); i++)
	{
		SequentialLightRule* pRule = m_sequentialLightSyn.GetItem(i);
		InsertRuleItem(TVI_ROOT,pRule);
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
	}
}

bool CDlgSequentialLightSyn::ConvertUnitFromDBtoGUI( void )
{
	return (false);
}

bool CDlgSequentialLightSyn::RefreshGUI( void )
{
	LoadTreeContent();
	return (true);
}

bool CDlgSequentialLightSyn::ConvertUnitFromGUItoDB( void )
{
	return (false);
}

void CDlgSequentialLightSyn::InsertRuleItem( HTREEITEM hItem,SequentialLightRule* pRule )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::RuleName_Node;
	pNodeData->m_dwData = (DWORD)pRule;
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	CString strRuleName;
	strRuleName.Format(_T("Name: %s"),pRule->GetName());
	HTREEITEM hRuleItem = m_wndTreeCtrl.InsertItem(strRuleName,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hRuleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//node list
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::IntersectionList_Node;
	pNodeData->m_dwData = (DWORD)pRule->GetNodeList();
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hListItem = m_wndTreeCtrl.InsertItem(_T("Intersections"),cni,FALSE,FALSE,hRuleItem);
	m_wndTreeCtrl.SetItemData(hListItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	//intersection node list
	{
		for (int i = 0; i < pRule->GetNodeRouteCount(); i++)
		{
			SequetailLightNodeRoute* pRoute = pRule->GetNodeRouteItem(i);
			InsertNodeItem(hListItem,pRoute);
		}
		m_wndTreeCtrl.Expand(hListItem,TVE_EXPAND);
	}

	//speed item
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Speed_Node;
	pNodeData->m_dwData = (DWORD)pRule;
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	CString strSpeed;
	double dSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),pRule->GetSpeed());
	cni.fMinValue = static_cast<float>(dSpeed);
	strSpeed.Format(_T("Syncronization Speed(%s): %.2f"),CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()),dSpeed);
	HTREEITEM hSpeedItem = m_wndTreeCtrl.InsertItem(strSpeed,cni,FALSE,FALSE,hRuleItem);
	m_wndTreeCtrl.SetItemData(hSpeedItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	
	m_wndTreeCtrl.Expand(hRuleItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgSequentialLightSyn::InsertNodeItem( HTREEITEM hItem,SequetailLightNodeRoute* pNode )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Intersection_Node;
	pNodeData->m_dwData = (DWORD)pNode;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;

	HTREEITEM hIntersectItem = m_wndTreeCtrl.InsertItem(pNode->GetIntersection().GetIDString(),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hIntersectItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pNode->GetItemCount(); i++)
	{
		SequetailLightNodeRoute* pChildNode = pNode->GetItem(i);
		InsertNodeItem(hIntersectItem,pChildNode);
	}
	m_wndTreeCtrl.Expand(hIntersectItem,TVE_EXPAND);
}

void CDlgSequentialLightSyn::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem)
	{
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::RuleName_Node) 
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_DEL,TRUE);
				return;
			}

		}
	}

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SEQUENTIAL_DEL,FALSE);
}

void CDlgSequentialLightSyn::OnAddIntersect()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;
	
	SequetailLightNodeRoute* pNode = (SequetailLightNodeRoute*)(pNodeData->m_dwData);
	LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
	if (pLayoutList)
	{
		CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
		dlg.AddObjType(ALT_LINTERSECTION);
		if(dlg.DoModal() == IDOK)
		{
			LandsideFacilityLayoutObject* pObject = pLayoutList->getObjectByName(dlg.getSelectObject());
			if (pObject)
			{
				LandsideIntersectionNode* pIntersectNode = (LandsideIntersectionNode*)pObject;
				if (pIntersectNode->getNodeType() != LandsideIntersectionNode::_Signalized)
				{
					MessageBox(_T("Can't select intersection node without traffic light!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					if (!pNode->HasIntersection(dlg.getSelectObject()))
					{
						SequetailLightNodeRoute* pChildeNode = new SequetailLightNodeRoute();
						pChildeNode->SetIntersection(dlg.getSelectObject());
						pNode->AddItem(pChildeNode);

						InsertNodeItem(hItem,pChildeNode);
						m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the same intersection node!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
					}
				}
				
			}
			else
			{
				MessageBox(_T("Can't select group of intersection node!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
			}
		}	
	}
}

void CDlgSequentialLightSyn::OnDeleteIntersection()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;
	
	TreeNodeData* pParentNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	if (pParentNodeData == NULL)
		return;
	
	SequetailLightNodeRoute* pParentData = (SequetailLightNodeRoute*)(pParentNodeData->m_dwData);
	SequetailLightNodeRoute* pNode = (SequetailLightNodeRoute*)(pNodeData->m_dwData);
	pParentData->DeleteItem(pNode);
	

	m_wndTreeCtrl.DeleteItem(hItem);
	DeleteTreeNodeData(pNodeData);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgSequentialLightSyn::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgSequentialLightSyn::ClearTreeNodeData()
{
	for (unsigned i = 0; i < m_vNodeData.size(); i++)
	{
		delete m_vNodeData[i];
	}
	m_vNodeData.clear();
}

LandsideFacilityLayoutObjectList* CDlgSequentialLightSyn::GetLandsideLayoutObjectList()
{
	InputLandside* pInputLandside = m_pDoc->getARCport()->GetInputLandside();
	if (pInputLandside == NULL)
		return NULL;

	return &pInputLandside->getObjectList();
}

int CDlgSequentialLightSyn::GetProjectID() const
{
	return m_pDoc->GetProjectID();
}
