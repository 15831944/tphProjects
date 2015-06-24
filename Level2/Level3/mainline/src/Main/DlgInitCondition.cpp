// DlgInitCondition.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgInitCondition.h"
#include "../Common/ProbDistManager.h"
#include "DlgTimeRange.h"
#include "Landside/InputLandside.h"
#include "Inputs/PROBAB.H"
#include "DlgProbDist.h"
#include "inputs/IN_TERM.H"
#include "DlgSelectLandsideObject.h"
#include "TermPlanDoc.h"
#include "DlgSelectLandsideVehicleType.h"
// CDlgInitCondition dialog
namespace
{
	const UINT ID_OBJECT_NEW = 30;
	const UINT ID_OBJECT_DEL = 31;

	const UINT ID_MENU_ADD = 15;
	const UINT ID_MENU_DEL = 16;
}

const static char* sLandsideObjectString[] = {"Parking lot","Taxi Queue","Taxi Pool"};
const static int  landsideObjectType[] = {ALT_LPARKINGLOT,ALT_LTAXIQUEUE,ALT_LTAXIPOOL};
IMPLEMENT_DYNAMIC(CDlgInitCondition, CDialog)
CDlgInitCondition::CDlgInitCondition(InputLandside* pInputLandside,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgInitCondition::IDD, pParent)
	,m_pInLandside(pInputLandside)
{
	m_initCondition.ReadData(-1);
}

CDlgInitCondition::~CDlgInitCondition()
{
}

void CDlgInitCondition::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgInitCondition, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_COMMAND(ID_OBJECT_NEW,OnCmdNewItem)
	ON_COMMAND(ID_OBJECT_DEL,OnCmdDeleteItem)
	ON_COMMAND(ID_MENU_ADD,OnAddInitCondition)
	ON_COMMAND(ID_MENU_DEL,OnDeleteCondition)
END_MESSAGE_MAP()

// CDlgInitCondition message handlers
BOOL CDlgInitCondition::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	OnInitToolbar();
	UpdateToolBarState();
	LoadTreeContent();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgInitCondition::OnCreate( LPCREATESTRUCT lpCreateStruct )
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

void CDlgInitCondition::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 2;
	m_wndToolbar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolbar.GetToolBarCtrl();
	spotBarCtrl.SetCmdID(0,ID_OBJECT_NEW);
	spotBarCtrl.SetCmdID(1,ID_OBJECT_DEL);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_OBJECT_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_OBJECT_DEL, FALSE);
}

void CDlgInitCondition::UpdateToolBarState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_DEL,FALSE);
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_NEW,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_DEL,FALSE);
		}
		else
		{
			if (pNodeData->m_emType == TreeNodeData::Type_Node)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_DEL,TRUE);
			}
			else
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_DEL,FALSE);
			}
		}
	}
}

void CDlgInitCondition::LoadTreeContent()
{
	//reset tree 
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	//load tree
	for (int i = 0; i < m_initCondition.GetItemCount(); i++)
	{
		InitConditionType* pConditionType = m_initCondition.GetItem(i);
		InsertConditionTypeTreeItem(TVI_ROOT,pConditionType);
	}

	//make first tree item visible
	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
	}
}

LRESULT CDlgInitCondition::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_EDITSPIN_BEGIN:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			cni->fMinValue = 0.0;
		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			int nValue = atoi(strValue);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::NumLot_Node)
			{
				InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
				pData->SetNum(nValue);
				CString strNumLot;
				cni->fMinValue = static_cast<float>(nValue);
				strNumLot.Format(_T("Number in lot: %d"),pData->GetNum());
				m_wndTreeCtrl.SetItemText(hItem,strNumLot);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::TimeRange_Node)
				{
					InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
					if (pData)
					{
						CDlgTimeRange* pDlgTimeRange = new CDlgTimeRange(pData->GetTimeRange().GetStartTime(),pData->GetTimeRange().GetEndTime());
						pCNI->pEditWnd = pDlgTimeRange;
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::Vehicle_Node)
				{
					CDlgSelectLandsideVehicleType* pDlgVehicle = new CDlgSelectLandsideVehicleType();
					pCNI->pEditWnd = pDlgVehicle;
				}
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::TimeRange_Node)
				{
					CDlgTimeRange* pDlg = (CDlgTimeRange*)pCNI->pEditWnd;
					InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
					if (pData)
					{
						TimeRange timeRange;
						timeRange.SetStartTime(pDlg->GetStartTime());
						timeRange.SetEndTime(pDlg->GetEndTime());

						pData->SetTimeRange(timeRange);
						CString strTimeRange;
						strTimeRange.Format(_T("Startup data and time: %s"),timeRange.GetString());
						m_wndTreeCtrl.SetItemText(hItem,strTimeRange);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::Vehicle_Node)
				{
					CDlgSelectLandsideVehicleType* pDlg = (CDlgSelectLandsideVehicleType*)pCNI->pEditWnd;
					HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
					TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
					if (pSuperNode)
					{
						CString strVehicle = pDlg->GetName();
						if (!strVehicle.IsEmpty())//non empty
						{
							InitConditionType* pType = (InitConditionType*)(pSuperNode->m_dwData);
							ASSERT(pType);
							InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
							ASSERT(pData);
							if (pType->ExistSameVehicleType(strVehicle) && pData->GetVehicleType() != strVehicle)
							{
								MessageBox(_T("Exist the same Vehicle Type!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
							}
							else
							{
								pData->SetVehicleType(strVehicle);

								CString strText;
								strText.Format(_T("Vehicle Type: %s"),strVehicle);
								m_wndTreeCtrl.SetItemText(hItem,strText);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
							}
							
						}
					}
					
					
				}
			}
			if (pCNI->pEditWnd)
			{
				delete pCNI->pEditWnd;
				pCNI->pEditWnd = NULL;
			}
			
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;
			CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
			m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::Type_Node)
				{
					pCB->AddString(_T("Parking lot"));
					pCB->AddString(_T("TaxiQ"));
					pCB->AddString(_T("Taxi pool"));
					pCB->SetCurSel(0);
				}
				else if (pNodeData->m_emType == TreeNodeData::DepartureDist_Node)
				{
					CStringList strList;
					CString s(_T("New Probability Distribution..."));
					pCB->AddString(s);
					int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
					for( int m=0; m<nCount; m++ )
					{
						CProbDistEntry* pPBEntry =_g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( m );			
						pCB->AddString(pPBEntry->m_csName);
					}
				}
			}
		}
		break;
	case UM_CEW_COMBOBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			
			
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::Type_Node)
				{
					int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);				
					LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
					if (pLayoutList)
					{
						CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
						dlg.AddObjType(landsideObjectType[nIndexSeled]);
						if(dlg.DoModal() == IDOK)
						{							
							CString strObject = dlg.getSelectObject().GetIDString();
							InitConditionType* pType = (InitConditionType*)(pNodeData->m_dwData);

							InitConditionType::ConditionType emType = InitConditionType::ConditionType(nIndexSeled);
							if (m_initCondition.ExistSameCondition(strObject,emType) && \
								(pType->GetName().GetIDString() != strObject || pType->GetType() != emType))
							{
								CString strError;
								strError.Format(_T("Exist the same %s!!."),sLandsideObjectString[nIndexSeled]);
								MessageBox(strError,_T("Warning"),MB_OK | MB_ICONINFORMATION);
								if (pType->GetName().IsBlank())
								{
									m_wndTreeCtrl.DeleteItem(hItem);
									DeleteTreeNodeData(pNodeData);
									m_initCondition.DeleteItem(pType);	
								}								
							}
							else
							{									
								pType->SetName(dlg.getSelectObject());
								pType->SetType(InitConditionType::ConditionType(nIndexSeled));
								CString strText;
								strText.Format(_T("%s: %s"),sLandsideObjectString[nIndexSeled],strObject);
								m_wndTreeCtrl.SetItemText(hItem,strText);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
							}
							
						}	
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::DepartureDist_Node)
				{
					InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
					CProbDistEntry* pEntry = ChangeProbDist(hItem,strValue);
					if (pEntry)
					{
						pData->SetProDist(pEntry);
						CString strDist;
						strDist.Format(_T("Departing distribution: %s"),pData->GetProDist().getPrintDist());
						m_wndTreeCtrl.SetItemText(hItem,strDist);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
				}
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::Type_Node)
				{
					InitConditionType* pType = (InitConditionType*)(pNodeData->m_dwData);
					CString strObject = pType->GetName().GetIDString();
					
					CString strText;
					strText.Format(_T("%s: %s"),sLandsideObjectString[pType->GetType()],strObject);
					m_wndTreeCtrl.SetItemText(hItem,strText);
				}
				else if (pNodeData->m_emType == TreeNodeData::DepartureDist_Node)
				{
					InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
					CProbDistEntry* pEntry = ChangeProbDist(hItem,strValue);
					if (pEntry)
					{
						pData->SetProDist(pEntry);
						CString strDist;
						strDist.Format(_T("Departing distribution: %s"),pData->GetProDist().getPrintDist());
						m_wndTreeCtrl.SetItemText(hItem,strDist);
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

CProbDistEntry* CDlgInitCondition::ChangeProbDist( HTREEITEM hItem,const CString& strDist)
{
	int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
	CString strValue = _T("New Probability Distribution...");

	CProbDistEntry* pPDEntry = NULL;
	if( strcmp( strValue, strDist ) == 0 )
	{
		CDlgProbDist dlg(  m_pInLandside->getInputTerminal()->m_pAirportDB, true );
		if(dlg.DoModal()==IDOK) 
		{
			int idxPD = dlg.GetSelectedPDIdx();
			ASSERT(idxPD!=-1);
			pPDEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem(idxPD);
		}
	}
	else
	{
		int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry =_g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getItem( i );
			if( strcmp( pPDEntry->m_csName, strDist ) == 0 )
				break;
		}
	}

	return pPDEntry;
}

HTREEITEM CDlgInitCondition::InsertConditionTypeTreeItem( HTREEITEM hItem,InitConditionType* pConditionType )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Type_Node;
	pNodeData->m_dwData = (DWORD)pConditionType;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strType;
	CString strObject = pConditionType->GetName().GetIDString();
	if (pConditionType->GetType() == InitConditionType::sp_ParkingLot)
	{
		strType.Format(_T("Parking Lot: %s"),strObject);

	}
	else if (pConditionType->GetType() == InitConditionType::sp_TaxiQ)
	{
		strType.Format(_T("TaxiQ: %s"),strObject);
	}
	else if (pConditionType->GetType() == InitConditionType::sp_TaxiPool)
	{
		strType.Format(_T("Taxi Pool: %s"),strObject);
	}
	
	HTREEITEM hTypeItem = m_wndTreeCtrl.InsertItem(strType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hTypeItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pConditionType->GetCount(); i++)
	{
		InitConditionData* pData = pConditionType->GetItem(i);
		InsertConditionDataTreeItem(hTypeItem,pData);
	}

	
	m_wndTreeCtrl.Expand(hTypeItem,TVE_EXPAND);
	return hTypeItem;
}

void CDlgInitCondition::InsertConditionDataTreeItem( HTREEITEM hItem,InitConditionData* pConditionData )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Vehicle_Node;
	pNodeData->m_dwData = (DWORD)pConditionData;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type: %s"),pConditionData->GetVehicleType());
	HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_EDIT_INT;
		CString strNumLot;
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::NumLot_Node;
		pNodeData->m_dwData = (DWORD)pConditionData;
		strNumLot.Format(_T("Number in lot: %d"),pConditionData->GetNum());
		cni.fMinValue = static_cast<float>(pConditionData->GetNum());
		HTREEITEM hNumItem = m_wndTreeCtrl.InsertItem(strNumLot,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hNumItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::TimeRange_Node;
		pNodeData->m_dwData = (DWORD)pConditionData;
		CString strTime;
		strTime.Format(_T("Startup date and time: %s"),pConditionData->GetTimeRange().GetString());
		HTREEITEM hTimeItem = m_wndTreeCtrl.InsertItem(strTime,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hTimeItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		pNodeData= new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::DepartureDist_Node;
		pNodeData->m_dwData = (DWORD)pConditionData;
		CString strDepDis;
		strDepDis.Format(_T("Departing distribution: %s"),pConditionData->GetProDist().getPrintDist());
		HTREEITEM hDepItem = m_wndTreeCtrl.InsertItem(strDepDis,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hDepItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);
	}
	m_wndTreeCtrl.Expand(hVehicleItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgInitCondition::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgInitCondition::OnCmdNewItem()
{
	InitConditionType* pType = new InitConditionType();
	HTREEITEM hItem = InsertConditionTypeTreeItem(TVI_ROOT,pType);
	m_wndTreeCtrl.DoEdit(hItem);
	m_initCondition.AddNewItem(pType);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgInitCondition::OnCmdDeleteItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	InitConditionType* pType = (InitConditionType*)(pNodeData->m_dwData);
	m_initCondition.DeleteItem(pType);
	DeleteTreeNodeData(pNodeData);

	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgInitCondition::OnContextMenu( CWnd* pWnd, CPoint point )
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
	case TreeNodeData::Type_Node:
		{
			InitConditionType* pType = (InitConditionType*)(pNodeData->m_dwData);
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Select Vehicle Type"));
			if (pType->GetType() == InitConditionType::sp_ParkingLot)
			{
				menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Parking Lot"));
			}
			else if (pType->GetType() == InitConditionType::sp_TaxiQ)
			{
				menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete TaxiQ"));
			}
			else if (pType->GetType() == InitConditionType::sp_TaxiPool)
			{
				menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Taxi Pool"));
			}
			
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::Vehicle_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Vehicle Type"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	default:
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgInitCondition::OnAddInitCondition()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	if (pNodeData->m_emType == TreeNodeData::Type_Node)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strVehicle = dlg.GetName();
			if (!strVehicle.IsEmpty())//non empty
			{
				InitConditionType* pType = (InitConditionType*)(pNodeData->m_dwData);
				if (pType->ExistSameVehicleType(strVehicle))
				{
					MessageBox(_T("Exist the same Vehicle Type!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					InitConditionData* pData = new InitConditionData();
					pData->SetVehicleType(strVehicle);
					pType->AddItem(pData);
					InsertConditionDataTreeItem(hItem,pData);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
			}
		}
	}
}

void CDlgInitCondition::OnDeleteCondition()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
	{
		InitConditionType* pType = (InitConditionType*)(pNodeData->m_dwData);
		m_initCondition.DeleteItem(pType);
		DeleteTreeNodeData(pNodeData);

		m_wndTreeCtrl.DeleteItem(hItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		return;
	}

	TreeNodeData* pSuperData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	if (pSuperData == NULL)
		return;

	InitConditionType* pType = (InitConditionType*)(pSuperData->m_dwData);
	InitConditionData* pData = (InitConditionData*)(pNodeData->m_dwData);
	if (pType && pData)
	{
		pType->DeleteItem(pData);
		m_wndTreeCtrl.DeleteItem(hItem);
		DeleteTreeNodeData(pNodeData);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

LandsideFacilityLayoutObjectList* CDlgInitCondition::GetLandsideLayoutObjectList()
{
	if (m_pParentWnd == NULL)
		return NULL;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(((CView*)m_pParentWnd)->GetDocument());
	if (pDoc == NULL)
		return NULL;

	InputLandside* pInputLandside = pDoc->getARCport()->GetInputLandside();
	if (pInputLandside == NULL)
		return NULL;

	return &pInputLandside->getObjectList();
}

void CDlgInitCondition::OnOK()
{
	CString strError;

	try
	{
		CADODatabase::BeginTransaction();
		m_initCondition.SaveData(-1);
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

void CDlgInitCondition::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_initCondition.SaveData(-1);
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

void CDlgInitCondition::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem)
	{
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::Type_Node) 
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_DEL,TRUE);
				return;
			}

		}
	}

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_OBJECT_DEL,FALSE);
}
