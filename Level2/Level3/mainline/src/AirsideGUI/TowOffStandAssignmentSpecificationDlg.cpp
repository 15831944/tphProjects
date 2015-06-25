// TowOffStandAssignmentSpecificationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TowOffStandAssignmentSpecificationDlg.h"
#include "../InputAirside/TowOffStandAssignmentDataList.h"
#include "../InputAirside/TemporaryParkingCriteria.h"
#include "../InputAirside/stand.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../Database/DBElementCollection_Impl.h"
#include "SelectIntermediateStandsDlg.h"
#include "../Common/AirsideFlightType.h"
#include "DlgTimeRange.h"

const static LPCTSTR sTow[] = {_T("No"),_T("Yes")};
const static LPCTSTR sTowoffCriteria[] = {_T("Reposition for Departure"),_T("Free up Stand")};
const static LPCTSTR sTimeLeaveArrStandType[] = {_T("Before Departure"),_T("After Arrival")};
const static LPCTSTR sBackToArrivalStand[] = {_T("Yes"),_T("No")};
const static LPCTSTR sPreferredTowoff[] = {_T("Yes"),_T("No")};
namespace
{
	const UINT ID_NEW     = 10;
	const UINT ID_EDIT    = 11;
	const UINT ID_DEL     = 12;

	const UINT IDC_STATIC_SPLITTER = 13;
}

// CTowOffStandAssignmentSpecificationDlg dialog

IMPLEMENT_DYNAMIC(CTowOffStandAssignmentSpecificationDlg, CXTResizeDialogEx)
CTowOffStandAssignmentSpecificationDlg::CTowOffStandAssignmentSpecificationDlg(int nProjID, PSelectFlightType pSelectFlightType,PSelectNewFlightType pSelectNewFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent /*=NULL*/)
	: CXTResizeDialogEx(CXTResizeDialogEx::styleUseInitSizeAsMinSize, CTowOffStandAssignmentSpecificationDlg::IDD, pParent)
	, m_nProjID(nProjID)
	, m_pSelectNewFlightType(pSelectNewFlightType)
	,m_pSelectFlightType(pSelectFlightType)
	, m_pAirportDatabase(pAirPortdb)
{
	m_towoffStrategyConstainer.SetAirportDatabase(pAirPortdb);
	m_towoffStrategyConstainer.ReadData(-1);
}

CTowOffStandAssignmentSpecificationDlg::~CTowOffStandAssignmentSpecificationDlg()
{
	std::vector<TowNodeData*>::iterator ite = m_listNodeData.begin();
	std::vector<TowNodeData*>::iterator iteEn = m_listNodeData.end();
	for (;ite!=iteEn;ite++)
	{
		delete (*ite);
	}
}

void CTowOffStandAssignmentSpecificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TOWOFFSTANDASSIGN, m_wndTreeCtrl);
	DDX_Control(pDX,IDC_LIST_NAME,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CTowOffStandAssignmentSpecificationDlg, CXTResizeDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_NEW, OnCmdNew)
	ON_COMMAND(ID_DEL, OnCmdDelete)
	ON_COMMAND(ID_EDIT, OnCmdEdit)
	ON_COMMAND(ID_TOOLBAR_ADD, OnAddStrategy)
	ON_COMMAND(ID_TOOLBAR_DEL, OnDeleteStrategy)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TOWOFFSTANDASSIGN, OnTvnSelchangedTreeItem)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_NAME, OnLvnEndlabeleditListContents)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NAME, OnLvnItemchangedList)
END_MESSAGE_MAP()


// CTowOffStandAssignmentSpecificationDlg message handlers
int CTowOffStandAssignmentSpecificationDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
		CRect(0,0,0,0),IDR_TOOLBAR_ADD_DEL) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,
		CRect(0,0,0,0),IDR_TOOLBAR_ADD_DEL_EDIT) || !m_ToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_STATIC_SPLITTER);
	m_wndSplitter.SetStyle(SPS_HORIZONTAL);

	CToolBarCtrl& toolbar = m_ToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW);
	toolbar.SetCmdID(1, ID_DEL);
	toolbar.SetCmdID(2, ID_EDIT);

	return 0;
}

BOOL CTowOffStandAssignmentSpecificationDlg::OnInitDialog()
{
	CXTResizeDialogEx::OnInitDialog();

	InitToolBar();
	InitListCtrlHeader();
	LoadListContent();

	m_wndSplitter.SetBuddies(GetDlgItem(IDC_STATIC_NAME), GetDlgItem(IDC_STATIC_GROUPBOX));

	SetAllControlsResize();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CTowOffStandAssignmentSpecificationDlg::SetAllControlsResize()
{
	CXTResizeDialogEx::Init();
	SetResize(IDC_STATIC_NAME,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_NAME,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT);
	SetResize(m_wndSplitter.GetDlgCtrlID(), SZ_MIDDLE_LEFT, SZ_MIDDLE_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUPBOX, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_TREE_TOWOFFSTANDASSIGN, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);
}

void CTowOffStandAssignmentSpecificationDlg::DoResize(int delta,UINT nIDSplitter)
{
	CSplitterControl::ChangeHeight(GetDlgItem(IDC_STATIC_NAME), delta, CW_TOPALIGN);
	CSplitterControl::ChangeHeight(&m_wndListCtrl, delta, CW_TOPALIGN);

	CSplitterControl::ChangeHeight(GetDlgItem(IDC_STATIC_GROUPBOX), -delta, CW_BOTTOMALIGN);
	CSplitterControl::ChangePos(&m_ToolBar, 0,delta);

	CSplitterControl::ChangeHeight(GetDlgItem(IDC_TREE_TOWOFFSTANDASSIGN), -delta, CW_BOTTOMALIGN);

	SetAllControlsResize();
}

BOOL CTowOffStandAssignmentSpecificationDlg::SetTreeCtrlItemData(HTREEITEM hItem, TowNodeData* pNodeData)
{
	// push all the nodes into a vector, and will release the data in ctor
	m_listNodeData.push_back(pNodeData);
	return m_wndTreeCtrl.SetItemData(hItem, (DWORD)pNodeData);
}

void CTowOffStandAssignmentSpecificationDlg::InitToolBar(void)
{
	CRect topRect;
	m_wndListCtrl.GetWindowRect(&topRect);
	ScreenToClient(&topRect);
	topRect.top -= 26;
	topRect.bottom = topRect.top + 22;
	topRect.left += 4;
	m_wndToolBar.MoveWindow(&topRect);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_ADD, TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DEL, FALSE);

	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBar.MoveWindow(&rect);

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_EDIT, FALSE );
}

LRESULT CTowOffStandAssignmentSpecificationDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case UM_CEW_EDITSPIN_END:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
				CTowOffStandAssignmentData* pData = (CTowOffStandAssignmentData*)pNodeData->dwData;
				CString strData = _T("");
				CString strCaption = _T("");
				switch(pNodeData->emNodeType)
				{
				case ND_DEPLANETIME:
					{
						strCaption.LoadString(IDS_DEPLANEMENTTIME);
						pData->SetDeplanementTime(atoi(strValue));
						strData.Format("%s: %d",strCaption,pData->GetDeplanementTime());
					}
					break;
				case ND_ENPLANETIME:
					{
						strCaption.LoadString(IDS_ENPLANEMENTTIME);
						pData->SetEnplanementTime(atoi(strValue));
						strData.Format("%s: %d",strCaption,pData->GetEnplanementTime());
					}
					break;
				case ND_LEAVE_ARR_STAND_TIME:
					{
						strCaption.LoadString(IDS_LEAVE_ARR_STAND_TIME);
						pData->SetTimeLeaveArrStand(atoi(strValue));
						strData.Format("%s: %d",strCaption,pData->GetTimeLeaveArrStand());
					}
					break;
				case ND_TOWOFFCRITERIA_TIME:
					{
						strCaption.LoadString(IDS_TOWOFFCRITERIA);
						pData->SetTowOffCriteria(atoi(strValue));
						strData.Format("%s: %d",strCaption,pData->GetTowOffCriteria());
					}
				    break;
				case ND_RETURNTO_STAND_EMPLANEMENT_TIME:
					{
						strCaption.LoadString(IDS_RETURNTO_STAND_EMPLANEMENT_TIME);
						CTowOffStandReturnToStandEntry* pReturnToStand = (CTowOffStandReturnToStandEntry*)pNodeData->dwExtraData;
						ASSERT(pReturnToStand);
						pReturnToStand->SetTimeBeforeEmplanement(atoi(strValue));
						strData.Format("%s: %d",strCaption, pReturnToStand->GetTimeBeforeEmplanement());
					}
					break;
				case ND_CONDITION_OPERATION:
					{
						PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)pNodeData->dwExtraData;
						ASSERT(pPreferredTowoffData);
						pPreferredTowoffData->SetOperationNum(atoi(strValue));
						strData.Format(_T("No ops number: %d"),pPreferredTowoffData->GetOperationNum());
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							strCaption.Format(_T("When no ops < %d within %d mins of flight type"),pPreferredTowoffData->GetOperationNum(),pPreferredTowoffData->GetTimeMins());
							m_wndTreeCtrl.SetItemText(hParentItem,strCaption);
						}
					}
					break;
				case ND_CONDITION_TIME:
					{
						PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)pNodeData->dwExtraData;
						ASSERT(pPreferredTowoffData);
						pPreferredTowoffData->SetTimeMins(atoi(strValue));
						strData.Format(_T("Mins: %d"),pPreferredTowoffData->GetTimeMins());
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							strCaption.Format(_T("When no ops < %d within %d mins of flight type"),pPreferredTowoffData->GetOperationNum(),pPreferredTowoffData->GetTimeMins());
							m_wndTreeCtrl.SetItemText(hParentItem,strCaption);
						}
					}
					break;
				default:
					ASSERT(FALSE);
				    break;
				}
				cni->fMinValue = static_cast<float>(atoi(strValue));
				m_wndTreeCtrl.SetItemText(hItem,strData);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
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
				if (pCB->GetCount() == 0)
				{
					TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
					switch (pNodeData->emNodeType)
					{
					case ND_TOWORNOT:
						{
							pCB->AddString(sTow[0]);
							pCB->AddString(sTow[1]);
						}
						break;
					case ND_TOWOFFCRITERIA:
						{
							pCB->AddString(sTowoffCriteria[0]);
							pCB->AddString(sTowoffCriteria[1]);
						}
						break;
					case ND_LEAVE_ARR_STAND_TIME_TYPE:
						{
							pCB->AddString(sTimeLeaveArrStandType[0]);
							pCB->AddString(sTimeLeaveArrStandType[1]);
						}
						break;
					case ND_BACKTO_ARR_STAND:
						{
							pCB->AddString(sBackToArrivalStand[0]);
							pCB->AddString(sBackToArrivalStand[1]);
						}
						break;
					case ND_PREFERRED_TOWOFF:
						{
							pCB->AddString(sPreferredTowoff[0]);
							pCB->AddString(sPreferredTowoff[1]);
						}
						break;
					default:
						ASSERT(FALSE);
						break;
					}
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
				TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				CString strValue = _T("");
				CTowOffStandAssignmentData* pData = (CTowOffStandAssignmentData*)pNodeData->dwData;

				switch (pNodeData->emNodeType)
				{
				case ND_TOWORNOT:
					{
						BOOL bNewTowoff = (1 == nSel);
						strValue.Format(_T("Tow or Not: %s"),sTow[nSel]);
						m_wndTreeCtrl.SetItemText(hItem,strValue);
						if (pData->IsTow() != bNewTowoff)
						{
							pData->SetTow(bNewTowoff);
							DeleteAllChildItemsOfTreeCtrl(hItem);
							COOLTREE_NODE_INFO cni;
							CCoolTree::InitNodeInfo(this, cni);
							AddItemsOfTowOrNotToTreeCtrl(hItem, cni, pData);
						}
					}
					break;
				case ND_TOWOFFCRITERIA:
					{
						BOOL bNewRepositionOrFreeup = (0 == nSel);
						CString strCaption;
						strCaption.LoadString(IDS_TOWOFFCRITERIA);
						strValue.Format(_T("%s: %s"), strCaption, sTowoffCriteria[nSel]);
						m_wndTreeCtrl.SetItemText(hItem,strValue);
						if (pData->GetRepositionOrFreeup() != bNewRepositionOrFreeup)
						{
							pData->SetRepositionOrFreeup(bNewRepositionOrFreeup);

							DeleteAllChildItemsOfTreeCtrl(hItem);
							COOLTREE_NODE_INFO cni;
							CCoolTree::InitNodeInfo(this, cni);
							AddItemsOfTowoffCriteriaToTreeCtrl(hItem, cni, pData);
						}
					}
					break;
				case ND_LEAVE_ARR_STAND_TIME_TYPE:
					{
						pData->SetTimeLeaveArrStandType((CTowOffStandAssignmentData::TimeLeaveArrStandType)nSel);
						strValue = sTimeLeaveArrStandType[nSel];
						m_wndTreeCtrl.SetItemText(hItem,strValue);
					}
					break;
				case ND_BACKTO_ARR_STAND:
					{
						BOOL bNewBackToArrStand = (0 == nSel);
						CString strCaption;
						strCaption.LoadString(IDS_BACKTO_ARR_STAND);
						pData->SetBackToArrivalStand(bNewBackToArrStand);
						strValue.Format(_T("%s: %s"), strCaption, sBackToArrivalStand[nSel]);
						m_wndTreeCtrl.SetItemText(hItem,strValue);
					}
					break;
				case ND_PREFERRED_TOWOFF:
					{
						bool bPreferred = (0 == nSel ? true : false);
						pData->SetPreferredTowoff(bPreferred);
						strValue.Format(_T("Preferred tow off: %s"),bPreferred ? sPreferredTowoff[0]:sPreferredTowoff[1]);
						m_wndTreeCtrl.SetItemText(hItem,strValue);

						DeleteAllChildItemsOfTreeCtrl(hItem);
						COOLTREE_NODE_INFO cni;
						CCoolTree::InitNodeInfo(this, cni);
						if (bPreferred)
						{
							AddPreferredTowoffToTreeCtrl(hItem,cni,pData);
						}
					}
					break;
				default:
					{
						ASSERT(FALSE);
					}
					break;
				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		case UM_CEW_SHOW_DIALOGBOX_BEGIN:
			{
				CString strData = _T("");
				HTREEITEM hItem = (HTREEITEM)wParam;
				TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				CTowOffStandAssignmentData* pData = (CTowOffStandAssignmentData*)pNodeData->dwData;
				switch(pNodeData->emNodeType)
				{
				case ND_FLTTYPE:
					{
						CString strFlightType(_T(""));
						if (m_pSelectNewFlightType)
						{
							AirsideFlightType fltType;
							if( (*m_pSelectNewFlightType)(NULL,fltType))
							{
								strFlightType = fltType.getPrintString() ;
								
								CString strCaption(_T(""));
								strCaption.LoadString(IDS_FLIGHTTYPE);
								strData.Format(_T("%s: %s"),strCaption,strFlightType);
								pData->SetFlightType(fltType);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
							}
							else
							{
								strData = m_wndTreeCtrl.GetItemText(hItem);
							}
						}
					}
					break;
				case ND_PARKSTAND:
					{
						CSelectIntermediateStandsDlg dlg(m_nProjID, pData, pNodeData->nStandID,
							CSelectIntermediateStandsDlg::ParkStand);
						if (IDOK == dlg.DoModal())
						{
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
						strData.Format(_T("Parked stand: %s"),pData->GetParStandNameString());
						pNodeData->nStandID = pData->GetParkStandsID();
					}
				    break;
				case ND_PRIOIRITY:
					{
						CTowOffPriorityEntry* pPriorityEntry = pData->FindPriorityItem(pNodeData->nStandID);
						ASSERT(pPriorityEntry);
						CSelectIntermediateStandsDlg dlg(m_nProjID, pData, pNodeData->nStandID,
							CSelectIntermediateStandsDlg::PriorityStand);
						if (IDOK == dlg.DoModal())
						{
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
						int nCurStandID = pPriorityEntry->GetStandID();
						int nIdx = pData->FindPriorityItemIndex(nCurStandID);
						ASSERT(-1 != nIdx);
						strData.Format(_T("Priority %d: %s"),nIdx+1,pPriorityEntry->GetStandNameString());
						pNodeData->nStandID = nCurStandID;
					}
					break;
				case ND_RETURNTO_STAND:
					{
						CTowOffStandReturnToStandEntry* pReturnToStandEntry = (CTowOffStandReturnToStandEntry*)pNodeData->dwExtraData;
						ASSERT(pReturnToStandEntry);
						CTowOffPriorityEntry* pPriorityEntry = (CTowOffPriorityEntry*)pNodeData->dwExtraParentData;
						ASSERT(pPriorityEntry);
						CSelectIntermediateStandsDlg dlg(m_nProjID, pData, pNodeData->nStandID,
							CSelectIntermediateStandsDlg::ReturnToStand, pPriorityEntry->GetStandID());
						if (IDOK == dlg.DoModal())
						{
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
						int nCurStandID = pReturnToStandEntry->GetStandID();
						strData.Format(_T("Return to Stand: %s"), pReturnToStandEntry->GetStandNameString());
						pNodeData->nStandID = nCurStandID;
					}
					break;
				case ND_TIMERANGE_STAND:
					{
						PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)pNodeData->dwExtraData;
						ASSERT(pPreferredTowoffData);
						TimeRange timeRange = pPreferredTowoffData->GetTimeRange();
						CDlgTimeRange dlg(timeRange.GetStartTime(),timeRange.GetEndTime());
						if (dlg.DoModal() == IDOK)
						{
							pPreferredTowoffData->SetTimeRange(dlg.GetStartTime(),dlg.GetEndTime());
							strData.Format(_T("In time window: %s"),pPreferredTowoffData->GetTimeRangeString());
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
						else
						{
							strData.Format(_T("In time window: %s"),pPreferredTowoffData->GetTimeRangeString());
						}
					}
					break;
				case ND_FLIGHTTYPE_ITEM:
					{
						CString strFlightType(_T(""));
						PreferTowoffFlightType* pPreferredTowoffFlightType = (PreferTowoffFlightType*)pNodeData->dwExtraParentData;
						if (m_pSelectFlightType)
						{
							FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

							char szBuffer[1024] = {0};
							fltType.screenPrint(szBuffer);
							
							strData = szBuffer;
							pPreferredTowoffFlightType->SetFlightType(fltType);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
						}
					}
					break;
				default:
					ASSERT(FALSE);
				    break;
				}
				m_wndTreeCtrl.SetItemText(hItem,strData);
			}
			break;
		case UM_CEW_STATUS_CHANGE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)(pNodeData->dwExtraData);
				if (pNodeData->emNodeType == ND_TIMERANGE_STAND )
				{
					BOOL bCheck = m_wndTreeCtrl.IsRadioItem(hItem);
					pPreferredTowoffData->UseTimeRange(bCheck?true:false);
				}
				else if (pNodeData->emNodeType == ND_CONDITION_STAND)
				{
					BOOL bCheck = m_wndTreeCtrl.IsRadioItem(hItem);
					pPreferredTowoffData->UseTimeRange(bCheck?false:true);
				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
			}
			break;
		case WM_NOTIFY:
			{
				if (wParam == IDC_STATIC_SPLITTER)
				{
					SPC_NMHDR* pHdr = (SPC_NMHDR*)lParam;
					DoResize(pHdr->delta, wParam);
				}
			}
			break;
		default:
			{
// 				ASSERT(FALSE);
			}
			break;

	}
	return CXTResizeDialogEx::DefWindowProc(message, wParam, lParam);
}


void CTowOffStandAssignmentSpecificationDlg::UpdateToolBar(void)
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	BOOL bNewEnabled = -1;
	BOOL bDelEnabled = -1;
	BOOL bEditEnabled = -1;

	if (hItem)
	{
		TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		CTowOffStandAssignmentData* pData = (CTowOffStandAssignmentData*)pNodeData->dwData;
		bNewEnabled = TRUE;
		bDelEnabled = FALSE;
		bEditEnabled = TRUE;
		switch (pNodeData->emNodeType)
		{
		case ND_FLTTYPE:
			{
				bDelEnabled = (pData != NULL);
			}
			break;
		case ND_PRIOIRITY:
			{
				bDelEnabled = pData->GetPriorityCount() > 1;
			}
			break;
		case ND_RETURNTO_STAND:
			{
				CTowOffPriorityEntry* pPriorityEntry = (CTowOffPriorityEntry*)pNodeData->dwExtraParentData;
				ASSERT(pPriorityEntry);
				bDelEnabled = pPriorityEntry->GetReturnToStandCout() > 1;
			}
			break;
		case ND_FLIGHTTYPE_STAND:
			{
				bEditEnabled = FALSE;
			}
			break;
		case ND_FLIGHTTYPE_ITEM:
			{
				PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)pNodeData->dwExtraData;
				bDelEnabled = pPreferredTowoffData->GetItemCount() > 0;
			}
			break;
		default:
			{
				bNewEnabled = FALSE;
			}
			break;
		}
	}
	else
	{
		bNewEnabled = TRUE;
		bDelEnabled = FALSE;
		bEditEnabled = FALSE;
	}

	ASSERT(-1 != bNewEnabled && -1 != bDelEnabled && -1 != bEditEnabled);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, bNewEnabled);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, bDelEnabled );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_EDIT, bEditEnabled );
}
void CTowOffStandAssignmentSpecificationDlg::AddFlightTypeNode()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	AirsideFlightType fltType;
	ASSERT(m_pSelectNewFlightType);
	if( (*m_pSelectNewFlightType)(NULL,fltType))
	{
		CTowOffStandAssignmentData* pTowOffStandAssign = new CTowOffStandAssignmentData(ContentNonNull);
		pTowOffStandAssign->SetFlightType(fltType);
		TowoffStandStrategy* pTowoffStrategy = m_towoffStrategyConstainer.GetItem(nSel);
		CTowOffStandAssignmentDataList* pTowOffStandAssignmentDataList = pTowoffStrategy->GetTowoffStandDataList();
		pTowOffStandAssignmentDataList->AddNewItem(pTowOffStandAssign);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this, cni);
		AddOneTowOffStandAssignmentItemToTreeCtrl(cni, pTowOffStandAssign);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CTowOffStandAssignmentSpecificationDlg::AddPriorityNode(HTREEITEM hItem)
{
	ASSERT(hItem);
	CString strData(_T(""));
	TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	CTowOffStandAssignmentData* pData = (CTowOffStandAssignmentData*)pNodeData->dwData;
	int nCount = pData->GetPriorityCount();
	CSelectIntermediateStandsDlg dlg(m_nProjID, pData,-1);
	if(dlg.DoModal() == IDOK)
	{
		if (nCount != pData->GetPriorityCount())
		{
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.nt = NT_NORMAL;
			cni.net = NET_SHOW_DIALOGBOX;
			int nStandID = pData->GetPriorityItem(pData->GetPriorityCount() - 1)->GetStandID();
			pNodeData = new TowNodeData();
			pNodeData->dwData = (DWORD)pData;
			pNodeData->emNodeType = ND_PRIOIRITY;
			pNodeData->nStandID = nStandID;
			strData.Format(_T("Priority %d: %s"),pData->GetPriorityCount(),dlg.GetStandName());
			HTREEITEM hPriorityItem = m_wndTreeCtrl.InsertItem(strData,cni,FALSE,FALSE,hItem,TVI_LAST);
			SetTreeCtrlItemData(hPriorityItem, pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

void CTowOffStandAssignmentSpecificationDlg::OnCmdNew()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
 		AddFlightTypeNode();
	}
	else
	{
		TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		CTowOffStandAssignmentData* pTowOffStandAssign = (CTowOffStandAssignmentData*)pNodeData->dwData;
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this, cni);
		switch (pNodeData->emNodeType)
		{
		case ND_FLTTYPE:
			{
				AddFlightTypeNode();
			}
			break;
		case ND_PRIOIRITY:
			{
				CSelectIntermediateStandsDlg dlg(m_nProjID, pTowOffStandAssign, -1, CSelectIntermediateStandsDlg::PriorityStand);
				if (IDOK == dlg.DoModal())
				{
					CTowOffPriorityEntry* pPriorityEntry = (CTowOffPriorityEntry*)dlg.GetNewCreatedData();
					ASSERT(pPriorityEntry);
					int nNewIndex = pTowOffStandAssign->GetPriorityCount() - 1;
					HTREEITEM hTowoffItem = m_wndTreeCtrl.GetParentItem(hItem);
					AddOnePriorityItemToTreeCtrl(hTowoffItem, cni, pPriorityEntry, pTowOffStandAssign, nNewIndex);
				}
			}
			break;
		case ND_RETURNTO_STAND:
			{
				CTowOffPriorityEntry* pPriorityEntry = (CTowOffPriorityEntry*)pNodeData->dwExtraParentData;
				ASSERT(pPriorityEntry);
				CSelectIntermediateStandsDlg dlg(m_nProjID, pTowOffStandAssign, -1,
					CSelectIntermediateStandsDlg::ReturnToStand, pPriorityEntry->GetStandID());
				if (IDOK == dlg.DoModal())
				{
					CTowOffStandReturnToStandEntry* pReturnToStandEntry = (CTowOffStandReturnToStandEntry*)dlg.GetNewCreatedData();
					ASSERT(pReturnToStandEntry);
					HTREEITEM hPriorityItem = m_wndTreeCtrl.GetParentItem(hItem);
					AddOneReturnToStandItemToTreeCtrl(hPriorityItem, cni, pReturnToStandEntry, pPriorityEntry, pTowOffStandAssign);
				}
			}
			break;
		case ND_FLIGHTTYPE_STAND:
			{
				PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)pNodeData->dwExtraData;
				ASSERT(pPreferredTowoffData);
				CTowOffStandAssignmentData* pData = (CTowOffStandAssignmentData*)pNodeData->dwData;
	
				FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
				{
					PreferTowoffFlightType* pPreferredTowffFlightType = new PreferTowoffFlightType();
					pPreferredTowffFlightType->SetFlightType(fltType);
					pPreferredTowoffData->AddNewItem(pPreferredTowffFlightType);
					HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
					AddOnePreferredTypeToTreeCtrl(hItem,cni,pPreferredTowffFlightType,pPreferredTowoffData,pTowOffStandAssign);
					m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
				}
				
			}
			break;
		default:
			{
				// do nothing
			}
			break;
		}
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}


void CTowOffStandAssignmentSpecificationDlg::OnCmdDelete()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	ASSERT(hItem);
	TowNodeData* pNodeData = NULL;
	pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData)
	{
		CTowOffStandAssignmentData* pTowOffAssign = (CTowOffStandAssignmentData*)pNodeData->dwData;
		switch(pNodeData->emNodeType)
		{
		case ND_FLTTYPE:
			{
				int nSel = m_wndListCtrl.GetCurSel();
				if (nSel != LB_ERR)
				{
					TowoffStandStrategy* pTowoffStrategy = m_towoffStrategyConstainer.GetItem(nSel);
					CTowOffStandAssignmentDataList* pTowOffStandAssignmentDataList = pTowoffStrategy->GetTowoffStandDataList();
					if (pTowOffStandAssignmentDataList)
					{
						pTowOffStandAssignmentDataList->DeleteItem(pTowOffAssign);
					}
				}
			}
			break;
		case ND_PRIOIRITY:
			{
				CString strData(_T(""));
				int nIdx = pTowOffAssign->FindPriorityItemIndex(pNodeData->nStandID);
				int nCount = pTowOffAssign->GetPriorityCount();
				HTREEITEM hNextItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
				while (hNextItem && nIdx < nCount - 1)
				{
					strData.Format(_T("Priority %d: %s"),nIdx+1,
						pTowOffAssign->GetPriorityItem(nIdx+1)->GetStandName());
					m_wndTreeCtrl.SetItemText(hNextItem,strData);
					hNextItem = m_wndTreeCtrl.GetNextSiblingItem(hNextItem);
					nIdx++;
				}
				pTowOffAssign->RemovePriorityItem(pNodeData->nStandID);
			}
			break;
		case ND_RETURNTO_STAND:
			{
				CTowOffPriorityEntry* pPriorityEntry = (CTowOffPriorityEntry*)pNodeData->dwExtraParentData;
				ASSERT(pPriorityEntry);
				CTowOffStandReturnToStandEntry* pReturnToStandEntry = pPriorityEntry->FindReturnToStandItem(pNodeData->nStandID);
				ASSERT(pReturnToStandEntry);
				pPriorityEntry->RemoveReturnToStandItem(pReturnToStandEntry);
			}
			break;
		case ND_FLIGHTTYPE_ITEM:
			{
				PreferTowoffFlightType* pPreferTowoffType = (PreferTowoffFlightType*)pNodeData->dwExtraParentData;
				ASSERT(pPreferTowoffType);
				PreferrdTowoffStandData* pPreferredTowoffData = (PreferrdTowoffStandData*)pNodeData->dwExtraData;
				ASSERT(pPreferredTowoffData);
				pPreferredTowoffData->DeleteItem(pPreferTowoffType);
			}
			break;
		default:
			ASSERT(FALSE);
		    break;
		}
		m_wndTreeCtrl.DeleteItem(hItem);
	}

	UpdateToolBar();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CTowOffStandAssignmentSpecificationDlg::OnCmdEdit()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem)
	{
		m_wndTreeCtrl.DoEdit(hItem);
	}
}

void CTowOffStandAssignmentSpecificationDlg::SetTreeContent(void)
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	TowoffStandStrategy* pStrategy = m_towoffStrategyConstainer.GetItem(nSel);
	if (pStrategy == NULL)
		return;
	
	CTowOffStandAssignmentDataList* pTowOffStandAssignmentDataList = pStrategy->GetTowoffStandDataList();

	AddTowOffStandAssignmentItemsToTreeCtrl(cni,pTowOffStandAssignmentDataList);
}


void CTowOffStandAssignmentSpecificationDlg::DeleteAllChildItemsOfTreeCtrl(HTREEITEM hParent)
{
	if (m_wndTreeCtrl.ItemHasChildren(hParent))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hParent);

		while (hChildItem != NULL)
		{
			hNextItem = m_wndTreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
			m_wndTreeCtrl.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}

}
void CTowOffStandAssignmentSpecificationDlg::AddTowOffStandAssignmentItemsToTreeCtrl(COOLTREE_NODE_INFO& cniInitialized,
											 CTowOffStandAssignmentDataList* pTowOffStandAssignList
											 )
{
	for (int i=0; i<(int)pTowOffStandAssignList->GetElementCount(); i++)
	{
		CTowOffStandAssignmentData *pTowOffStandAssign = pTowOffStandAssignList->GetItem(i);
		AddOneTowOffStandAssignmentItemToTreeCtrl(cniInitialized, pTowOffStandAssign);
	}
}

void CTowOffStandAssignmentSpecificationDlg::AddOneTowOffStandAssignmentItemToTreeCtrl(COOLTREE_NODE_INFO& cniInitialized,
											   CTowOffStandAssignmentData *pTowOffStandAssign
											   )
{
	AirsideFlightType flightType = pTowOffStandAssign->GetFlightType();

	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_SHOW_DIALOGBOX;
	CString strCaption(_T(""));
	CString strFltType(_T(""));
	strCaption.LoadString(IDS_FLIGHTTYPE);

	TowNodeData* pNodeData = new TowNodeData();
	strFltType.Format(_T("%s: %s"),strCaption,flightType.getPrintString());
	HTREEITEM hFltItem = m_wndTreeCtrl.InsertItem(strFltType,cniInitialized,FALSE,FALSE,TVI_ROOT,TVI_LAST);
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_FLTTYPE;
	SetTreeCtrlItemData(hFltItem, pNodeData);

	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_EDIT_INT;
	cniInitialized.fMinValue = static_cast<float>(pTowOffStandAssign->GetDeplanementTime());
	strCaption.LoadString(IDS_DEPLANEMENTTIME);
	CString strDeplanementTime(_T(""));
	strDeplanementTime.Format("%s: %d",strCaption,pTowOffStandAssign->GetDeplanementTime());
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_DEPLANETIME;
	HTREEITEM hDepItem = m_wndTreeCtrl.InsertItem(strDeplanementTime,cniInitialized,FALSE,FALSE,hFltItem,TVI_LAST);
	SetTreeCtrlItemData(hDepItem, pNodeData);

	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_EDIT_INT;
	cniInitialized.fMinValue = static_cast<float>(pTowOffStandAssign->GetEnplanementTime());
	strCaption.LoadString(IDS_ENPLANEMENTTIME);
	CString strEnplanementTime(_T(""));
	strEnplanementTime.Format("%s: %d", strCaption,pTowOffStandAssign->GetEnplanementTime());
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_ENPLANETIME;
	HTREEITEM hEnpItem = m_wndTreeCtrl.InsertItem(strEnplanementTime,cniInitialized,FALSE,FALSE,hFltItem,TVI_LAST);
	SetTreeCtrlItemData(hEnpItem, pNodeData);

	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_SHOW_DIALOGBOX;
	CString strStandPark(_T(""));
	strStandPark.Format(_T("Parked stand: %s"),pTowOffStandAssign->GetParStandNameString());
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_PARKSTAND;
	HTREEITEM hParkItem = m_wndTreeCtrl.InsertItem(strStandPark,cniInitialized,FALSE,FALSE,hFltItem,TVI_LAST);
	SetTreeCtrlItemData(hParkItem, pNodeData);

	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_COMBOBOX;
	CString strToworNot(_T(""));
	strToworNot.Format(_T("Tow or Not: %s"),sTow[pTowOffStandAssign->IsTow()]);
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_TOWORNOT;
	HTREEITEM hTowItem = m_wndTreeCtrl.InsertItem(strToworNot,cniInitialized,FALSE,FALSE,hFltItem,TVI_LAST);
	SetTreeCtrlItemData(hTowItem, pNodeData);

	AddItemsOfTowOrNotToTreeCtrl(hTowItem, cniInitialized, pTowOffStandAssign);

	m_wndTreeCtrl.Expand(hTowItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hFltItem,TVE_EXPAND);
}

void CTowOffStandAssignmentSpecificationDlg::AddItemsOfTowOrNotToTreeCtrl(HTREEITEM hTowItem, COOLTREE_NODE_INFO& cniInitialized,
								  CTowOffStandAssignmentData *pTowOffStandAssign
								  )
{
	if (pTowOffStandAssign->IsTow())
	{
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_COMBOBOX;
		CString strCaption;
		strCaption.LoadString(IDS_TOWOFFCRITERIA);
		CString strTowOffCriteria;
		strTowOffCriteria.Format(_T("%s: %s"), strCaption,
			pTowOffStandAssign->GetRepositionOrFreeup()?sTowoffCriteria[0]:sTowoffCriteria[1]);
		TowNodeData* pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->emNodeType = ND_TOWOFFCRITERIA;
		HTREEITEM hTowOffItem = m_wndTreeCtrl.InsertItem(strTowOffCriteria,cniInitialized,FALSE,FALSE,hTowItem,TVI_LAST);
		SetTreeCtrlItemData(hTowOffItem, pNodeData);
		m_wndTreeCtrl.Expand(hTowItem, TVE_EXPAND);

		AddItemsOfTowoffCriteriaToTreeCtrl(hTowOffItem, cniInitialized, pTowOffStandAssign);
	}
}

void CTowOffStandAssignmentSpecificationDlg::AddOnePreferredTypeToTreeCtrl(HTREEITEM hItem,COOLTREE_NODE_INFO& cniInitialized, PreferTowoffFlightType* pPreferredFlightType, PreferrdTowoffStandData* pPreferredTowoffData, CTowOffStandAssignmentData* pTowOffStandAssign)
{
	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_SHOW_DIALOGBOX;
	TowNodeData* pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->dwExtraData = (DWORD)pPreferredTowoffData;
	pNodeData->dwExtraParentData = (DWORD)pPreferredFlightType;
	pNodeData->emNodeType = ND_FLIGHTTYPE_ITEM;
	char szBuffer[1024] = {0};
	pPreferredFlightType->GetFlightType().screenPrint(szBuffer);
	HTREEITEM hChildFlightTypeItem = m_wndTreeCtrl.InsertItem(szBuffer,cniInitialized,FALSE,FALSE,hItem,TVI_LAST);
	SetTreeCtrlItemData(hChildFlightTypeItem,pNodeData);
}

void CTowOffStandAssignmentSpecificationDlg::AddPreferredTowoffToTreeCtrl( HTREEITEM hPreferredItem,COOLTREE_NODE_INFO& cniInitialized, CTowOffStandAssignmentData* pTowOffStandAssign )
{
	PreferrdTowoffStandData* pPreferredTowoff = pTowOffStandAssign->GetPreferredTowoffData();
	if (pPreferredTowoff == NULL)
		return;
	
	cniInitialized.nt = NT_RADIOBTN;
	cniInitialized.net = NET_SHOW_DIALOGBOX;
	CString strCaption;
	TowNodeData* pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->dwExtraData = (DWORD)pPreferredTowoff;
	pNodeData->emNodeType = ND_TIMERANGE_STAND;
	strCaption.Format(_T("In time window: %s"), pPreferredTowoff->GetTimeRangeString());
	HTREEITEM hTimeItem = m_wndTreeCtrl.InsertItem(strCaption,cniInitialized,FALSE,FALSE,hPreferredItem,TVI_LAST);
	m_wndTreeCtrl.SetEnableStatus(hTimeItem,TRUE);
	SetTreeCtrlItemData(hTimeItem,pNodeData);
	
	cniInitialized.nt = NT_RADIOBTN;
	cniInitialized.net = NET_NORMAL;
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->dwExtraData = (DWORD)pPreferredTowoff;
	pNodeData->emNodeType = ND_CONDITION_STAND;
	strCaption.Format(_T("When no ops < %d within %d mins of flight type"),pPreferredTowoff->GetOperationNum(),pPreferredTowoff->GetTimeMins());
	HTREEITEM hConditionItem = m_wndTreeCtrl.InsertItem(strCaption,cniInitialized,TRUE,FALSE,hPreferredItem,TVI_LAST);
	m_wndTreeCtrl.SetEnableStatus(hConditionItem,TRUE);
	if (pPreferredTowoff->WhetherUseTimeRange())
	{
		m_wndTreeCtrl.SetRadioStatus(hTimeItem,TRUE);
	}
	else
	{
		m_wndTreeCtrl.SetRadioStatus(hConditionItem,TRUE);
	}
	SetTreeCtrlItemData(hConditionItem,pNodeData);
	{
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_EDIT_INT;
		cniInitialized.fMinValue = static_cast<float>(pPreferredTowoff->GetOperationNum());
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->dwExtraData = (DWORD)pPreferredTowoff;
		pNodeData->emNodeType = ND_CONDITION_OPERATION;
		strCaption.Format(_T("No ops number:  %d"),pPreferredTowoff->GetOperationNum());
		HTREEITEM hOperationItem = m_wndTreeCtrl.InsertItem(strCaption,cniInitialized,FALSE,FALSE,hConditionItem,TVI_LAST);
		SetTreeCtrlItemData(hOperationItem,pNodeData);

		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_EDIT_INT;
		cniInitialized.fMinValue = static_cast<float>(pPreferredTowoff->GetTimeMins());
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->dwExtraData = (DWORD)pPreferredTowoff;
		pNodeData->emNodeType = ND_CONDITION_TIME;
		strCaption.Format(_T("Mins:  %d"),pPreferredTowoff->GetTimeMins());
		HTREEITEM hTimeItem = m_wndTreeCtrl.InsertItem(strCaption,cniInitialized,FALSE,FALSE,hConditionItem,TVI_LAST);
		SetTreeCtrlItemData(hTimeItem,pNodeData);

		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_NORMAL;
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->dwExtraData = (DWORD)pPreferredTowoff;
		pNodeData->emNodeType = ND_FLIGHTTYPE_STAND;
		HTREEITEM hRootFlgihtTypeItem = m_wndTreeCtrl.InsertItem(_T("Flight type"),cniInitialized,FALSE,FALSE,hConditionItem,TVI_LAST);
		SetTreeCtrlItemData(hRootFlgihtTypeItem,pNodeData);
		{
			for (int i = 0; i < pPreferredTowoff->GetItemCount(); i++)
			{
				PreferTowoffFlightType* pPerferredTowoffFlightType = pPreferredTowoff->GetItem(i);
				AddOnePreferredTypeToTreeCtrl(hRootFlgihtTypeItem,cniInitialized,pPerferredTowoffFlightType,pPreferredTowoff,pTowOffStandAssign);
			}
			m_wndTreeCtrl.Expand(hRootFlgihtTypeItem,TVE_EXPAND);
		}

		m_wndTreeCtrl.Expand(hConditionItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hPreferredItem,TVE_EXPAND);
}

void CTowOffStandAssignmentSpecificationDlg::AddItemsOfTowoffCriteriaToTreeCtrl(HTREEITEM hTowoffItem, COOLTREE_NODE_INFO& cniInitialized,
										CTowOffStandAssignmentData *pTowOffStandAssign
										)
{
	CString strCaption;
	TowNodeData* pNodeData;
	if (pTowOffStandAssign->GetRepositionOrFreeup())
	{
		// Time Leave Arrival Stand
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_EDIT_INT;
		cniInitialized.fMinValue = static_cast<float>(pTowOffStandAssign->GetTimeLeaveArrStand());
		strCaption.LoadString(IDS_TIME_LEAVE_ARR_STAND);
		CString strTimeLeaveArrStand;
		strTimeLeaveArrStand.Format("%s: %d", strCaption, pTowOffStandAssign->GetTimeLeaveArrStand());
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->emNodeType = ND_LEAVE_ARR_STAND_TIME;
		HTREEITEM hTimeLeaveArrStand = m_wndTreeCtrl.InsertItem(strTimeLeaveArrStand,cniInitialized,FALSE,FALSE,hTowoffItem,TVI_LAST);
		SetTreeCtrlItemData(hTimeLeaveArrStand, pNodeData);

		// Time Leave Arrival Stand Type
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_COMBOBOX;
		CString strTimeLeaveArrStandType = sTimeLeaveArrStandType[pTowOffStandAssign->GetTimeLeaveArrStandType()];
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->emNodeType = ND_LEAVE_ARR_STAND_TIME_TYPE;
		HTREEITEM hTimeLeaveArrStandType = m_wndTreeCtrl.InsertItem(strTimeLeaveArrStandType,cniInitialized,FALSE,FALSE,hTowoffItem,TVI_LAST);
		SetTreeCtrlItemData(hTimeLeaveArrStandType, pNodeData);
		m_wndTreeCtrl.Expand(hTowoffItem, TVE_EXPAND);
	} 
	else
	{
		// Time (Tow off Criteria)
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_EDIT_INT;
		cniInitialized.fMinValue = static_cast<float>(pTowOffStandAssign->GetTowOffCriteria());
		strCaption.LoadString(IDS_TIME_TOWOFF_CRITERIA);
		CString strTimeTowoffCriteria;
		strTimeTowoffCriteria.Format(_T("%s: %d"), strCaption, pTowOffStandAssign->GetTowOffCriteria());
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->emNodeType = ND_TOWOFFCRITERIA_TIME;
		HTREEITEM hTimeTowoffCriteria = m_wndTreeCtrl.InsertItem(strTimeTowoffCriteria,cniInitialized,FALSE,FALSE,hTowoffItem,TVI_LAST);
		SetTreeCtrlItemData(hTimeTowoffCriteria, pNodeData);

		//Preferred tow off
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_COMBOBOX;
		strCaption.Format(_T("Preferred tow off: %s"),pTowOffStandAssign->GetPreferredTowoff() ? sPreferredTowoff[0] : sPreferredTowoff[1]);
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->emNodeType = ND_PREFERRED_TOWOFF;
		HTREEITEM hPreferredTowoff = m_wndTreeCtrl.InsertItem(strCaption,cniInitialized,FALSE,FALSE,hTowoffItem,TVI_LAST);
		SetTreeCtrlItemData(hPreferredTowoff,pNodeData);
		if(pTowOffStandAssign->GetPreferredTowoff())
		{
			AddPreferredTowoffToTreeCtrl(hPreferredTowoff,cniInitialized,pTowOffStandAssign);
		}
		// Back to Arrival Stand    ND_BACKTO_ARR_STAND
		cniInitialized.nt = NT_NORMAL;
		cniInitialized.net = NET_COMBOBOX;
		strCaption.LoadString(IDS_BACKTO_ARR_STAND);
		CString strBackToArrivalStand;
		strBackToArrivalStand.Format(_T("%s: %s"), strCaption,
			pTowOffStandAssign->GetBackToArrivalStand()?sBackToArrivalStand[0]:sBackToArrivalStand[1]);
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pTowOffStandAssign;
		pNodeData->emNodeType = ND_BACKTO_ARR_STAND;
		HTREEITEM hTimeLeaveArrStandType = m_wndTreeCtrl.InsertItem(strBackToArrivalStand,cniInitialized,FALSE,FALSE,hTowoffItem,TVI_LAST);
		SetTreeCtrlItemData(hTimeLeaveArrStandType, pNodeData);
		m_wndTreeCtrl.Expand(hTowoffItem, TVE_EXPAND);

		AddPriorityItemsToTreeCtrl(hTowoffItem, cniInitialized, pTowOffStandAssign);
	}
}

void CTowOffStandAssignmentSpecificationDlg::AddPriorityItemsToTreeCtrl(HTREEITEM hTowoffItem, COOLTREE_NODE_INFO& cniInitialized,
								CTowOffStandAssignmentData* pTowOffStandAssign
								)
{
	for (int j = 0; j < pTowOffStandAssign->GetPriorityCount();j++)
	{
		CTowOffPriorityEntry* pPriorityEntry = pTowOffStandAssign->GetPriorityItem(j);
		AddOnePriorityItemToTreeCtrl(hTowoffItem, cniInitialized, pPriorityEntry, pTowOffStandAssign, j);
	}
}

void CTowOffStandAssignmentSpecificationDlg::AddOnePriorityItemToTreeCtrl(HTREEITEM hTowoffItem, COOLTREE_NODE_INFO& cniInitialized,
																		  CTowOffPriorityEntry* pPriorityEntry,
																		  CTowOffStandAssignmentData* pTowOffStandAssign,
																		  int nIndex
																		  )
{
	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_SHOW_DIALOGBOX;
	CString strCaption;
	strCaption.LoadString(IDS_STAND_PRIORITY);
	CString strPriority(_T(""));
	strPriority.Format(_T("%s %d: %s"),strCaption,nIndex+1, pPriorityEntry->GetStandNameString());
	TowNodeData* pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_PRIOIRITY;
	pNodeData->nStandID = pPriorityEntry->GetStandID();
	pNodeData->dwExtraData = (DWORD)pPriorityEntry;
	HTREEITEM hPriorityItem = m_wndTreeCtrl.InsertItem(strPriority,cniInitialized,FALSE,FALSE,hTowoffItem,TVI_LAST);
	SetTreeCtrlItemData(hPriorityItem, pNodeData);
	m_wndTreeCtrl.Expand(hPriorityItem, TVE_EXPAND);

	AddReturnToStandItemsToTreeCtrl(hPriorityItem, cniInitialized, pPriorityEntry, pTowOffStandAssign);
}

void CTowOffStandAssignmentSpecificationDlg::AddReturnToStandItemsToTreeCtrl(HTREEITEM hPriorityItem, COOLTREE_NODE_INFO& cniInitialized,
																			 CTowOffPriorityEntry* pPriorityEntry,
																			 CTowOffStandAssignmentData* pTowOffStandAssign
																			 )
{
	// Return to Stand
	for (int k =0;k < pPriorityEntry->GetReturnToStandCout();k++)
	{
		CTowOffStandReturnToStandEntry* pReturnToStandEntry = pPriorityEntry->GetReturnToStandItem(k);
		AddOneReturnToStandItemToTreeCtrl(hPriorityItem, cniInitialized, pReturnToStandEntry, pPriorityEntry, pTowOffStandAssign);
	}
}

void CTowOffStandAssignmentSpecificationDlg::AddOneReturnToStandItemToTreeCtrl(HTREEITEM hPriorityItem, COOLTREE_NODE_INFO& cniInitialized,
																			   CTowOffStandReturnToStandEntry* pReturnToStandEntry,
																			   CTowOffPriorityEntry* pPriorityEntry,
																			   CTowOffStandAssignmentData* pTowOffStandAssign
																			   )
{
	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_SHOW_DIALOGBOX;
	CString strCaption;
	strCaption.LoadString(IDS_RETURN_TO_STAND);
	CString strReturnToStand;
	strReturnToStand.Format(_T("%s: %s"),strCaption, pReturnToStandEntry->GetStandNameString());
	TowNodeData* pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_RETURNTO_STAND;
	pNodeData->nStandID = pReturnToStandEntry->GetStandID();
	pNodeData->dwExtraData = (DWORD)pReturnToStandEntry;
	pNodeData->dwExtraParentData = (DWORD)pPriorityEntry;
	HTREEITEM hReturnToStand = m_wndTreeCtrl.InsertItem(strReturnToStand,cniInitialized,FALSE,FALSE,hPriorityItem,TVI_LAST);
	SetTreeCtrlItemData(hReturnToStand, pNodeData);

	// Time Before Enplanement
	cniInitialized.nt = NT_NORMAL;
	cniInitialized.net = NET_EDIT_INT;
	cniInitialized.fMinValue = static_cast<float>(pReturnToStandEntry->GetTimeBeforeEmplanement());
	strCaption.LoadString(IDS_TIME_BEFORE_ENPLANEMENT);
	CString strTimeBeforeEnplanement;
	strTimeBeforeEnplanement.Format(_T("%s: %d"), strCaption, pReturnToStandEntry->GetTimeBeforeEmplanement());
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pTowOffStandAssign;
	pNodeData->emNodeType = ND_RETURNTO_STAND_EMPLANEMENT_TIME;
	pNodeData->nStandID = pReturnToStandEntry->GetStandID();
	pNodeData->dwExtraData = (DWORD)pReturnToStandEntry;
	pNodeData->dwExtraParentData = (DWORD)pPriorityEntry;
	HTREEITEM hTimeBeforeEnplanement = m_wndTreeCtrl.InsertItem(strTimeBeforeEnplanement,cniInitialized,FALSE,FALSE,hReturnToStand,TVI_LAST);
	SetTreeCtrlItemData(hTimeBeforeEnplanement, pNodeData);
	m_wndTreeCtrl.Expand(hPriorityItem, TVE_EXPAND);
	m_wndTreeCtrl.Expand(hReturnToStand, TVE_EXPAND);
}


void CTowOffStandAssignmentSpecificationDlg::OnOK()
{
	if(SaveData())
		CDialog::OnOK();
}

void CTowOffStandAssignmentSpecificationDlg::OnBnClickedSave()
{
	if(SaveData())
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CTowOffStandAssignmentSpecificationDlg::OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateToolBar();
}

void CTowOffStandAssignmentSpecificationDlg::OnAddStrategy()
{
	int nCount = m_wndListCtrl.GetItemCount();
	m_wndListCtrl.InsertItem(nCount,_T(""));

	TowoffStandStrategy* pStrategy = new TowoffStandStrategy();
	m_towoffStrategyConstainer.AddItem(pStrategy);

	m_wndListCtrl.EditSubLabel( nCount, 0);
}

void CTowOffStandAssignmentSpecificationDlg::OnDeleteStrategy()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	m_towoffStrategyConstainer.DeleteItem(nSel);
	m_wndListCtrl.DeleteItem(nSel);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CTowOffStandAssignmentSpecificationDlg::InitListCtrlHeader()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	CStringList strList;

	//Index
	LV_COLUMNEX	lvc;
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_EDIT;
	lvc.pszText = _T("Strategy Name");
	lvc.cx = 500;
	m_wndListCtrl.InsertColumn(0,&lvc);
}

void CTowOffStandAssignmentSpecificationDlg::LoadListContent()
{
	for (int i = 0; i < m_towoffStrategyConstainer.GetItemCount(); i++)
	{
		TowoffStandStrategy* pStrategy = m_towoffStrategyConstainer.GetItem(i);
		m_wndListCtrl.InsertItem(i,pStrategy->GetStrategyName());
	}

	int nCount = m_wndListCtrl.GetItemCount();
	if (nCount)
	{
		int nSel = m_towoffStrategyConstainer.GetSelectStrategy();
		if (nSel <0 || nSel >= nCount)
		{
			m_wndListCtrl.SetCurSel(0);
		}
		else
		{
			m_wndListCtrl.SetCurSel(nSel);
		}
	}

	SetTreeContent();
}

void CTowOffStandAssignmentSpecificationDlg::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	*pResult = 0;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0 || nItem>=m_wndListCtrl.GetItemCount())
		return;

	TowoffStandStrategy* pStrategy = m_towoffStrategyConstainer.GetItem(nItem);
	if (pStrategy == NULL)
		return;

	CString strValue = m_wndListCtrl.GetItemText(nItem,nSubItem);
	if (strValue.IsEmpty())
	{
		if (pStrategy->GetStrategyName().IsEmpty())
		{
			MessageBox(_T("Please define strategy name!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
			m_towoffStrategyConstainer.DeleteItem(nItem);
			m_wndListCtrl.DeleteItem(nItem);
			return;
		}
	}
	else
	{
		pStrategy->SetStrategyName(strValue);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
}

void CTowOffStandAssignmentSpecificationDlg::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);

			m_ToolBar.GetToolBarCtrl().EnableButton(ID_NEW,FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT,FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEL,FALSE);

			m_wndListCtrl.SetCurSel(-1);
			SetTreeContent();
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndListCtrl.SetCurSel(pnmv->iItem);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_NEW,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEL,FALSE);
		SetTreeContent();
	}
	else
		ASSERT(0);
}

void CTowOffStandAssignmentSpecificationDlg::UpdateSplitterRange()
{
	const int nLeftMargin = 100;
	const int nRightMargin = 250;
	CRect rc;
	GetClientRect(rc);
	m_wndSplitter.SetRange(nLeftMargin, rc.right - nRightMargin);
}

void CTowOffStandAssignmentSpecificationDlg::OnSize( UINT nType, int cx, int cy )
{
	CXTResizeDialogEx::OnSize(nType,cx,cy);
	UpdateSplitterRange();
}

bool CTowOffStandAssignmentSpecificationDlg::IsHaveDefaultTowOffStrategy()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if( nSel == LB_ERR)
		return true;

	TowoffStandStrategy* pTowOffStrategy = m_towoffStrategyConstainer.GetItem(nSel);
	if (pTowOffStrategy == NULL)
		return true;

	CTowOffStandAssignmentDataList* pTowOffStandAssignmentList = pTowOffStrategy->GetTowoffStandDataList();
	for (int i = 0; i < (int)pTowOffStandAssignmentList->GetElementCount(); i++)
	{
		CTowOffStandAssignmentData* pTowOffStandAssignment = pTowOffStandAssignmentList->GetItem(i);
		AirsideFlightType FlightType = pTowOffStandAssignment->GetFlightType();
		if (FlightType.IsDefaultFlightType())
		{
			return true;
		}
	}				
	return false;
}

bool CTowOffStandAssignmentSpecificationDlg::SaveData()
{
	if (!IsHaveDefaultTowOffStrategy())
	{
		CString strwaring = _T("The tow off strategy defined might not cover all the flights in the schedule. This might lead to some flights not being assigned to a stand. We recommend to define a DEFAULT tow off criteria at the end of each strategy.\
							   \r\n Are you sure to continue?");
		if(MessageBox(strwaring,NULL,MB_YESNO) == IDNO)
		{
			return false;
		}
	}

	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_towoffStrategyConstainer.SetSelectStrategy(nSel);
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_towoffStrategyConstainer.SaveData(-1);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		CString str = e.ErrorMessage();
		return false;
	}

	return true;
}
