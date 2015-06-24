// DlgSeatAssignment.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatAssignment.h"
//#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../InputOnBoard/Deck.h"
#include "../InputOnBoard/Cabin.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard\CInputOnboard.h"
#include "DlgOnboardFlightList.h"

#include "..\InputOnboard\AircaftLayOut.h"
#include "..\InputOnboard\AircraftPlacements.h"


// CDlgSeatAssignment dialog

IMPLEMENT_DYNAMIC(CDlgSeatAssignment, CXTResizeDialog)
CDlgSeatAssignment::CDlgSeatAssignment(InputOnboard* pInputOnboard,Terminal* pTerm,int nProjID,FlightType flightType,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSeatAssignment::IDD, pParent)
	,m_pInputOnboard(pInputOnboard)
	,m_nProjID(nProjID)
	,m_pTerminal(pTerm)
	,m_eFlightType(flightType)
{
}

CDlgSeatAssignment::~CDlgSeatAssignment()
{
}

void CDlgSeatAssignment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
	DDX_Control(pDX,IDC_LIST_FLIGHT,m_listFlight);
}


BEGIN_MESSAGE_MAP(CDlgSeatAssignment, CXTResizeDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
	ON_BN_CLICKED(ID_FLIGHT_ADD,OnNewFlight)
	ON_BN_CLICKED(ID_FLIGHT_DEL,OnDelFlight)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT,OnSelChangeFlight)
END_MESSAGE_MAP()


// CDlgSeatAssignment message handlers

BOOL CDlgSeatAssignment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetResize(IDC_STATIC,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_FLIGHT,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_DATA,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	InitToolBar();
	m_steatingAssignmentCabinList.ReadData(m_nProjID,m_eFlightType);
	OnInitFlightList();
// 	OnInitTreeCtrl();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}
void CDlgSeatAssignment::InitToolBar()
{
	if (!m_toolBarFlight.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarFlight.LoadToolBar(IDR_ADDDEL_FLIGHT))
	{
		MessageBox("Can't create toolbar!");
		return ;
	}
	CRect rect;
	m_listFlight.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-28;
	rect.left=rect.left+4;
	rect.bottom=rect.top+22;
	rect.right=rect.right-20;
	m_toolBarFlight.MoveWindow(rect);
	m_toolBarFlight.ShowWindow(SW_SHOW);
	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,true);


}
void CDlgSeatAssignment::OnInitTreeCtrl()
{
	int nCount = m_pInputOnboard->GetOnBoardFlightCandiates()->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CFlightTypeCandidate* pFltData = m_pInputOnboard->GetOnBoardFlightCandiates()->getItem(i);
		char szBuff[1024] = {0};
		pFltData->GetFltType().screenPrint(szBuff);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		TreeNodeData* pNodeData = new TreeNodeData();
		HTREEITEM hItem = m_wndTreeCtrl.InsertItem(szBuff,cni,FALSE);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pNodeData);
		for (int j = 0; j < pFltData->getCount(); j++)
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			COnboardFlight* pFlight = pFltData->getItem(j);
			CString strFlight = _T("");
			if (pFlight->getFlightType() == ArrFlight)
			{
				strFlight.Format(_T("ARR ID: %s ACTYPE: %s"),pFlight->getFlightID(),pFlight->getACType());
			}
			else
			{
				strFlight.Format(_T("DEP ID: %s ACTYPE: %s"),pFlight->getFlightID(),pFlight->getACType());
			} 
			pNodeData = new TreeNodeData();
			HTREEITEM hChildItem = m_wndTreeCtrl.InsertItem(strFlight,cni,FALSE,FALSE,hItem);
			m_wndTreeCtrl.SetItemData(hChildItem,(DWORD_PTR)pNodeData);
			cni.nt = NT_RADIOBTN;
			cni.net = NET_NORMAL;
		
			pNodeData = new TreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pFlight;
			pNodeData->emSeatType = FREE_STEATING;
			HTREEITEM hFreeItem = m_wndTreeCtrl.InsertItem(_T("Free seating"),cni,FALSE,FALSE,hChildItem);
			m_wndTreeCtrl.SetItemData(hFreeItem,(DWORD_PTR)pNodeData);
			pNodeData = new TreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pFlight;
			pNodeData->emSeatType = PREFERENCE_BASES;
			HTREEITEM hPreItem = m_wndTreeCtrl.InsertItem(_T("Preference bases"),cni,FALSE,FALSE,hChildItem);
			m_wndTreeCtrl.SetItemData(hPreItem,(DWORD_PTR)pNodeData);
			pNodeData = new TreeNodeData();
			pNodeData->dwData = (DWORD_PTR)pFlight;
			pNodeData->emSeatType = WEIGHT_BALANCE_BASED;
			HTREEITEM hWightItem = m_wndTreeCtrl.InsertItem(_T("Weight balance based"),cni,TRUE,FALSE,hChildItem);
			m_wndTreeCtrl.SetItemData(hWightItem,(DWORD_PTR)pNodeData);
			m_wndTreeCtrl.Expand(hChildItem,TVE_EXPAND);
			switch(pFlight->getSeatAssignmentType())
			{
			case FREE_STEATING:
				m_wndTreeCtrl.DoRadio(hFreeItem);
				break;
			case PREFERENCE_BASES:
				m_wndTreeCtrl.DoRadio(hPreItem);
			    break;
			case WEIGHT_BALANCE_BASED:
				m_wndTreeCtrl.DoRadio(hWightItem);
				break;
			default:
			    break;
			}
			
			CAircaftLayOut* pLayOut = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlight->getID());
			if (pLayOut)
			{
				CDeckManager* pManager = pLayOut->GetDeckManager();
				if (pManager)
				{
					for (int k =0; k < pManager->getCount(); k++)
					{
						cni.nt = NT_NORMAL;
						cni.net = NET_NORMAL;
						CString strValue = _T("");
						CDeck* pDeck = pManager->getItem(k);
						strValue.Format(_T("Deck %s"),pDeck->GetName());
						pNodeData = new TreeNodeData();
						HTREEITEM hDeckItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hWightItem);
						m_wndTreeCtrl.SetItemData(hDeckItem,(DWORD_PTR)pNodeData);
						CCabinDataList& cabinList = pDeck->GetCabinList();
						for (int n = 0; n < cabinList.GetItemCount(); n++)
						{
							cni.nt = NT_NORMAL;
							cni.net = NET_NORMAL;
							CCabin* pCabin = cabinList.GetItem(n);
							CSeatingAssignmentCabinData* pItem = m_steatingAssignmentCabinList.getItem(pCabin->GetID());
							if (pItem == NULL)
							{
								pItem = new CSeatingAssignmentCabinData();
								pItem->setFlightAnalysisID(pFlight->getID());
								pItem->setCabinID(pCabin->GetID());
								m_steatingAssignmentCabinList.addItem(pItem);
							}
							strValue.Format(_T("Cabin %s"),pCabin->getName());
							pNodeData = new TreeNodeData();
							HTREEITEM hCabinItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hDeckItem);
							m_wndTreeCtrl.SetItemData(hCabinItem,(DWORD_PTR)pNodeData);
							pNodeData = new TreeNodeData();
							HTREEITEM hRowItem = m_wndTreeCtrl.InsertItem(pCabin->getRangeNumberString(),cni,FALSE,FALSE,hCabinItem);
							m_wndTreeCtrl.SetItemData(hRowItem,(DWORD_PTR)pNodeData);
							cni.nt = NT_NORMAL;
							cni.net = NET_EDIT_INT;
							pNodeData = new TreeNodeData();
							cni.fMinValue = static_cast<float>(pItem->getMinPax());
							pNodeData->dwData = (DWORD_PTR)pItem;
							pNodeData->emType = MINPAX;
							strValue.Format(_T("Min pax : %d"),pItem->getMinPax());
							HTREEITEM hMinItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hRowItem);
							m_wndTreeCtrl.SetItemData(hMinItem,(DWORD_PTR)pNodeData);
							pNodeData = new TreeNodeData();
							cni.fMinValue = static_cast<float>(pItem->getMaxPax());
							pNodeData->dwData = (DWORD_PTR)pItem;
							pNodeData->emType = MAXPAX;
							strValue.Format(_T("Max pax : %d"),pItem->getMaxPax());
							HTREEITEM hMaxItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hRowItem);
							m_wndTreeCtrl.SetItemData(hMaxItem,(DWORD_PTR)pNodeData);
							pNodeData = new TreeNodeData();
							cni.fMinValue = static_cast<float>(pItem->getPaxNumber());
							pNodeData->dwData = (DWORD_PTR)pItem;
							pNodeData->emType = NUMOFPAX;
							strValue.Format(_T("Number of passengers : %d"),pItem->getPaxNumber());
							HTREEITEM hNumPaxItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hRowItem);
							m_wndTreeCtrl.SetItemData(hNumPaxItem,(DWORD_PTR)pNodeData);
							m_wndTreeCtrl.Expand(hRowItem,TVE_EXPAND);
							m_wndTreeCtrl.Expand(hCabinItem,TVE_EXPAND);
						}
						m_wndTreeCtrl.Expand(hDeckItem,TVE_EXPAND);
					}
				}
			}
			m_wndTreeCtrl.Expand(hWightItem,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	}
}

void CDlgSeatAssignment::OnInitFlightList()
{
	for (int i=0;i<m_steatingAssignmentCabinList.getItemCount();i++)
	{
		CSeatingAssignmentCabinData* pCurSeatAss = m_steatingAssignmentCabinList.getItemByIndex(i);
		COnboardFlight *onboardFlight=pCurSeatAss->getOnboardFlight();
		CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(onboardFlight->getID()) ;
		CDlgOnboardFlightList dlg(m_pTerminal);
		CString strLayout;
		if(_layout != NULL)
			strLayout = _layout->GetName() ;
		int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.FormatFlightIdNodeForCandidate(strLayout,onboardFlight));
		m_listFlight.SetItemData(tmpIndex,(DWORD)pCurSeatAss);
	}
	if (m_listFlight.GetCount()>0)
	{
		m_listFlight.SetCurSel(0);
		ResetTreeContent();
	}	
}
LRESULT CDlgSeatAssignment::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			CSeatingAssignmentCabinData* pItem = (CSeatingAssignmentCabinData*)(pNodeData->dwData);
			CString strData = _T("");
			switch(pNodeData->emType)
			{
			case MINPAX:
				{
					strData.Format(_T("Min pax : %s"),strValue);
					pItem->setMinPax(atoi(strValue));
				}
				break;
			case MAXPAX:
				{
					strData.Format(_T("Max pax : %s"),strValue);
					pItem->setMaxPax(atoi(strValue));
				}
				break;
			case NUMOFPAX:
				{
					strData.Format(_T("Number of passengers : %s"),strValue);
					pItem->setPaxNumber(atoi(strValue));
				}
			    break;
			default:
			    break;
			}
			cni->fMinValue = static_cast<float>(atoi(strValue));
			m_wndTreeCtrl.SetItemText(hItem,strData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	case UM_CEW_STATUS_CHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
 			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COnboardFlight* pFlight = getCurSeatingAss()->getOnboardFlight();
			pFlight->setSeatAssignmentType(pNodeData->emSeatType);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	default:
		break;
	}
	 return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}

void CDlgSeatAssignment::OnNewFlight()
{
	CDlgOnboardFlightList dlg(m_pTerminal,(CDlgOnboardFlightList::FlightListType)m_eFlightType);
	if(dlg.DoModal()==IDOK)
	{
		COnboardFlight *onboardFlight=dlg.getOnboardFlight();

		if (FlightExist(onboardFlight))
		{
			MessageBox("Flight already exist.");
			return;
		}

		CSeatingAssignmentCabinData* pItem = new CSeatingAssignmentCabinData();
		pItem->setProjID(m_nProjID);
		pItem->setOnboardFlight(onboardFlight);

		COnBoardAnalysisCandidates *pOnBoardList = m_pInputOnboard->GetOnBoardFlightCandiates();
		COnboardFlight* pFlightData=pOnBoardList->GetExistOnboardFlight(onboardFlight->getFlightID(),onboardFlight->getACType(),onboardFlight->getFlightType(),onboardFlight->getDay());
		if (pFlightData!=NULL)
		{
			CAircaftLayOut* pLayout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID());
			if (pLayout!=NULL)
			{
				pItem->setLayoutID(pLayout->GetID());
			}
		}

		m_steatingAssignmentCabinList.addItem(pItem);
		int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.getFlightString());
		m_listFlight.SetItemData(tmpIndex,(DWORD)pItem);
		m_listFlight.SetCurSel(tmpIndex);
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,TRUE);
		ResetTreeContent();
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}	
}
void CDlgSeatAssignment::OnDelFlight()
{
		int curFlightIndex=m_listFlight.GetCurSel();
		if (curFlightIndex<0)
		{
			return;
		}	
		m_steatingAssignmentCabinList.removeItem(curFlightIndex);
		m_listFlight.DeleteString(curFlightIndex);	 
		if (curFlightIndex<m_listFlight.GetCount())
		{
			m_listFlight.SetCurSel(curFlightIndex);
			ResetTreeContent();
		}else if(m_listFlight.GetCount()>0)
		{
			m_listFlight.SetCurSel(m_listFlight.GetCount()-1);
			ResetTreeContent();
		}else
		{
			m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,false);
			ResetTreeContent();
		}	
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	
}
bool CDlgSeatAssignment::FlightExist(COnboardFlight *flight)
{
	COnboardFlight *curFlight;
	for (int i=0;i<m_listFlight.GetCount();i++)
	{
		curFlight=((CSeatingAssignmentCabinData *)m_listFlight.GetItemData(i))->getOnboardFlight();
		if (curFlight->getACType()==flight->getACType()
			&& curFlight->getFlightID()==flight->getFlightID()
			&& curFlight->getFlightType()==flight->getFlightType()
			&& curFlight->getDay()==flight->getDay())
		{			
			return true;
		}
	}
	return false;
}
void CDlgSeatAssignment::OnSelChangeFlight()
{
	if (m_listFlight.GetCurSel()<0)
	{
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,FALSE);
		return;
	}
	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,TRUE);
	ResetTreeContent();
}
void CDlgSeatAssignment::ResetTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	if (m_listFlight.GetCurSel()<0)
	{
		return;
	}	
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(_T("Seating Assignment"),cni,FALSE);

	cni.nt = NT_RADIOBTN;
	cni.net = NET_NORMAL;

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->emSeatType = FREE_STEATING;
	HTREEITEM hFreeItem = m_wndTreeCtrl.InsertItem(_T("Free seating"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hFreeItem,(DWORD_PTR)pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->emSeatType = PREFERENCE_BASES;
	HTREEITEM hPreItem = m_wndTreeCtrl.InsertItem(_T("Preference bases"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hPreItem,(DWORD_PTR)pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->emSeatType = WEIGHT_BALANCE_BASED;
	HTREEITEM hWightItem = m_wndTreeCtrl.InsertItem(_T("Weight balance based"),cni,TRUE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hWightItem,(DWORD_PTR)pNodeData);
	m_wndTreeCtrl.SetEnableStatus(hWightItem,false);
	
// 	m_wndTreeCtrl.Expand(hChildItem,TVE_EXPAND);
	COnboardFlight *pFlight=getCurSeatingAss()->getOnboardFlight();
	if (pFlight==NULL)
	{
		return;
	}
	switch(pFlight->getSeatAssignmentType())
	{
	case FREE_STEATING:
		m_wndTreeCtrl.DoRadio(hFreeItem);
		break;
	case PREFERENCE_BASES:
		m_wndTreeCtrl.DoRadio(hPreItem);
		break;
	case WEIGHT_BALANCE_BASED:
		m_wndTreeCtrl.DoRadio(hWightItem);
		break;
	default:
		break;
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);

}
CSeatingAssignmentCabinData* CDlgSeatAssignment::getCurSeatingAss()
{
	if (m_listFlight.GetCurSel()<0)
	{
		return NULL;
	}
	return (CSeatingAssignmentCabinData*)m_listFlight.GetItemData(m_listFlight.GetCurSel());
}
void CDlgSeatAssignment::OnOK()
{
    OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgSeatAssignment::OnSave()
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	try
	{
		CADODatabase::BeginTransaction() ;
// 		m_pInputOnboard->GetOnBoardFlightCandiates()->SaveData(m_nProjID);
		m_steatingAssignmentCabinList.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

BOOL CDlgSeatAssignment::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgSeatAssignment::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CXTResizeDialog::OnPaint() for painting messages
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CCoolTree)))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);

	CXTResizeDialog::OnPaint();
}

void CDlgSeatAssignment::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy);
// 	if (m_wndTreeCtrl.m_hWnd != NULL)
// 	{
// 		CRect rc;
// 		m_wndTreeCtrl.SetWindowPos(NULL,10,11,cx-20,cy-60,NULL);
// 		m_wndTreeCtrl.GetWindowRect(&rc);
// 		ScreenToClient(&rc);
// 		CRect rcBtn;
// 		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
// 		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rc.right - rcBtn.Width() - 2,rc.bottom + 10,rcBtn.Width(),rcBtn.Height(),NULL);
// 		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
// 		ScreenToClient(rcBtn);
// 		GetDlgItem(IDOK)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() - 16,rc.bottom + 10,rcBtn.Width(),rcBtn.Height(), NULL);
// 		GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
// 		ScreenToClient(rcBtn);
// 		CRect rcActype;
// 		GetDlgItem(IDC_BUTTON_SAVE)->GetWindowRect(&rcActype);
// 		GetDlgItem(IDC_BUTTON_SAVE)->SetWindowPos(NULL,rcBtn.left - rcActype.Width() - 16,rc.bottom+10,rcActype.Width(),rcActype.Height(),NULL);
// 	}
}