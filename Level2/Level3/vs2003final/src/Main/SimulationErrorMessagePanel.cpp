#include "StdAfx.h"
#include "SimulationErrorMessagePanel.h"
#include "Common/EngineDiagnoseEx.h"
#include "Common/OnBoardException.h"
#include "common\enginediagnose.h"
#include "inputs\in_term.h"
#include "termplandoc.h"
#include "trackwnd.h"
#include ".\SimulationErrorMessagePanel.h"
#include "Common\SimAndReportManager.h"


enum
{
	IDC_LIST_TERMINAL = 0x13045,
	IDC_LIST_AIRSIDE,
	IDC_LIST_LANDSIDE,
	IDC_LIST_ONBOARD,
	IDC_LIST_CARGO,
	IDC_LIST_ENVIRONMENT,
	IDC_LIST_FINANCIAL,

	IDC_SIMERRMSG_PANEL,
};

enum
{
	TAB_INDEX_AIRSIDE = 0,
	TAB_INDEX_LANSIDE,
	TAB_INDEX_TERMINAL,
	TAB_INDEX_ONBOARD,
	TAB_INDEX_CARGO,
	TAB_INDEX_ENVIRONMENT,
	TAB_INDEX_FINANCIAL,

	TAB_ITEM_COUNT,
};


SimulationErrorMessagePanel::SimulationErrorMessagePanel(void)
	: m_bShowErrorInfo(FALSE)
	, m_pTrackWnd(NULL)
{
	m_tipWindow.SetMargins(CSize(1,1));
	m_tipWindow.SetLineSpace(10);
}

SimulationErrorMessagePanel::~SimulationErrorMessagePanel(void)
{
	if (m_pTrackWnd)
	{
		if (m_pTrackWnd->GetSafeHwnd())
		{
			m_pTrackWnd->DestroyWindow();
		}
		delete m_pTrackWnd;
		m_pTrackWnd = NULL;
	}
}

BEGIN_MESSAGE_MAP(SimulationErrorMessagePanel, CXTTabCtrl)
	ON_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED,OnXListCtrlCheckBoxClicked)
	ON_MESSAGE(WM_TRACKWND_END,OnTrackWndDestroy)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TERMINAL, OnColumnclickListErrorMsgTerminal)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_AIRSIDE, OnColumnclickListErrorMsgAirside)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_LANDSIDE, OnColumnclickListErrorMsgLandSide)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TERMINAL, OnColumnclickListErrorMsgOnBoard)
END_MESSAGE_MAP()

void SimulationErrorMessagePanel::OnColumnclickListErrorMsgTerminal(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrlTerminal.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrlTerminal.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrlTerminal.SaveSortList();
	}	
	*pResult = 0;
}
void SimulationErrorMessagePanel::OnColumnclickListErrorMsgAirside(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrlAirside.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrlAirside.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrlAirside.SaveSortList();
	}	
	*pResult = 0;
}

void SimulationErrorMessagePanel::OnColumnclickListErrorMsgLandSide(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrlLandside.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrlLandside.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrlLandside.SaveSortList();
	}	
	*pResult = 0;
}

void SimulationErrorMessagePanel::OnColumnclickListErrorMsgOnBoard(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrlTerminal.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrlOnBoard.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrlOnBoard.SaveSortList();
	}	
	*pResult = 0;
}

BOOL SimulationErrorMessagePanel::InsertNewTab(CXListCtrl& ctrl, CSortableHeaderCtrl& headerCtrl, int nCtrlID, int nTabIndex, LPCTSTR strTabName,
										 const char* const colLabels[], const int nFormats[], const int nDefWidths[], const EDataType colDataType[], int nColNum)
{
	static const DWORD dwListCtrlStype = LVS_SHOWSELALWAYS|LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_TABSTOP;
	static const CRect rcListCtrl(0,0,300,188);

	if(!ctrl.Create( dwListCtrlStype, rcListCtrl,this, nCtrlID))
		return FALSE;

	DWORD dwStyle = ctrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ctrl.SetExtendedStyle( dwStyle );

	headerCtrl.SubclassWindow( ctrl.GetHeaderCtrl()->GetSafeHwnd() );

	for( int i=0; i<nColNum; i++ )
	{
		ctrl.InsertColumn( i, colLabels[i], nFormats[i], nDefWidths[i] );
		headerCtrl.SetDataType( i, colDataType[i] );
	}

	if (!AddControl(strTabName, &ctrl, nTabIndex))
	{
		return FALSE;
	}
	AddToolTip(nTabIndex, strTabName);
	return TRUE;
}

BOOL SimulationErrorMessagePanel::Create(CWnd* pWnd, CPoint ptPos)
{
	CRect tabRC(0 + ptPos.x, 0 + ptPos.y, 600 + ptPos.x, 260 + ptPos.y);
	if (!CXTTabCtrl::Create(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_TABSTOP, tabRC, pWnd, IDC_SIMERRMSG_PANEL))
		return FALSE ;

	SetPadding(CSize(6, 3));

	// Set the tab to auto condense (shrink) its tabs when sized.
	// 	SetAutoCondense(m_bAutoCondense);

	// Clip children of the tab control from paint routines to reduce flicker.
	ModifyStyle(0L, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);


	m_tipWindow.Create(this);
	m_tipWindow.ShowWindow(SW_HIDE);

	//-----------------------------------------------------------------------------------------------------
	//Airside
	{
		const char* const colLabelAirside[] = { "Time", "ErrorType", "ID",	"Details Information" };
		const int DefaultColWidthAirside[] = { 150, 120, 200, 300 };
		const int nFormatAirside[] = { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT,LVCFMT_LEFT };
		const EDataType typeAirside[] = { dtDATETIME, dtSTRING, dtINT, dtSTRING };

		InsertNewTab(m_wndAirsideListCtrl, m_ctlHeaderCtrlAirside, IDC_LIST_AIRSIDE, TAB_INDEX_AIRSIDE, _T("Airside"),
			colLabelAirside, nFormatAirside, DefaultColWidthAirside, typeAirside, 4);
	}

	//-----------------------------------------------------------------------------------------------------
	//Landside
	{
		const char* const colLabelLandside[] = { "Time", "ErrorType", "ID",	"Details Information"};
		const int nFormatLandside[] = { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT };
		const int DefaultColWidthLandside[] = { 150, 120, 200, 300 };
		const EDataType typeLandside[] = { dtDATETIME, dtSTRING, dtINT, dtSTRING };

		InsertNewTab(m_wndLandsideListCtrl, m_ctlHeaderCtrlLandside, IDC_LIST_LANDSIDE, TAB_INDEX_LANSIDE, _T("Landside"),
			colLabelLandside, nFormatLandside, DefaultColWidthLandside, typeLandside, 4);
	}


	//-----------------------------------------------------------------------------------------------------
	//Terminal
	{
		const char* colLabelTerminal[] = { "Time", "ErrorType", "ProcessorID",	"PaxID", "FlightID", "MobileType", "Other" };
		const int nFormatTerminal[] = { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER };
		const int DefaultColumnWidthTerminal[] = { 150, 120, 100, 60, 100, 220, 120 };
		const EDataType typeTerminal[] = { dtDATETIME, dtSTRING, dtSTRING, dtINT, dtSTRING, dtSTRING, dtSTRING };

		InsertNewTab(m_wndTerminalListCtrl, m_ctlHeaderCtrlTerminal, IDC_LIST_TERMINAL, TAB_INDEX_TERMINAL, _T("Terminal"),
			colLabelTerminal, nFormatTerminal, DefaultColumnWidthTerminal, typeTerminal, 7);
	}

	//-----------------------------------------------------------------------------------------------------
	//OnBoard
	{
		const char* const colLabelOnBoard[] = { "Time", "ErrorType","Flight ID", "Pax ID", "Details Information" };
		const int nFormatOnBoard[] = { LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT };
		const int DefaultColWidthOnBoard[] = { 60, 200,60, 60, 300 };
		const EDataType typeOnBoard[] = { dtDATETIME, dtSTRING, dtINT, dtINT,dtSTRING };

		InsertNewTab(m_wndOnBoardListCtrl, m_ctlHeaderCtrlOnBoard, IDC_LIST_ONBOARD, TAB_INDEX_ONBOARD, _T("Onboard"),
			colLabelOnBoard, nFormatOnBoard, DefaultColWidthOnBoard, typeOnBoard, 5);
	}

	//-----------------------------------------------------------------------------------------------------
	//Cargo
	{
		const char* colLabelCargo[] = { "Time", "ErrorType", "ProcessorID",	"PaxID", "FlightID", "MobileType", "Other" };
		const int nFormatCargo[] = { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER };
		const int DefaultColumnWidthCargo[] = { 150, 120, 100, 60, 100, 220, 120 };
		const EDataType typeCargo[] = { dtDATETIME, dtSTRING, dtSTRING, dtINT, dtSTRING, dtSTRING, dtSTRING };

		InsertNewTab(m_wndCargoListCtrl, m_ctlHeaderCtrlCargo, IDC_LIST_CARGO, TAB_INDEX_CARGO, _T("Cargo"),
			colLabelCargo, nFormatCargo, DefaultColumnWidthCargo, typeCargo, 7);
	}

	//-----------------------------------------------------------------------------------------------------
	//Environment
	{
		const char* colLabelEnvironment[] = { "Time", "ErrorType", "ProcessorID",	"PaxID", "FlightID", "MobileType", "Other" };
		const int nFormatEnvironment[] = { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER };
		const int DefaultColumnWidthEnvironment[] = { 150, 120, 100, 60, 100, 220, 120 };
		const EDataType typeEnvironment[] = { dtDATETIME, dtSTRING, dtSTRING, dtINT, dtSTRING, dtSTRING, dtSTRING };

		InsertNewTab(m_wndEnvironmentListCtrl, m_ctlHeaderCtrlEnvironment, IDC_LIST_ENVIRONMENT, TAB_INDEX_ENVIRONMENT, _T("Environment"),
			colLabelEnvironment, nFormatEnvironment, DefaultColumnWidthEnvironment, typeEnvironment, 7);
	}

	//-----------------------------------------------------------------------------------------------------
	//Financial
	{
		const char* colLabelFinancial[] = { "Time", "ErrorType", "ProcessorID",	"PaxID", "FlightID", "MobileType", "Other" };
		const int nFormatFinancial[] = { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER };
		const int DefaultColumnWidthFinancial[] = { 150, 120, 100, 60, 100, 220, 120 };
		const EDataType typeFinancial[] = { dtDATETIME, dtSTRING, dtSTRING, dtINT, dtSTRING, dtSTRING, dtSTRING };

		InsertNewTab(m_wndFinancialListCtrl, m_ctlHeaderCtrlFinancial, IDC_LIST_FINANCIAL, TAB_INDEX_FINANCIAL, _T("Financial"),
			colLabelFinancial, nFormatFinancial, DefaultColumnWidthFinancial, typeFinancial, 7);
	}


	return TRUE;
}

void SimulationErrorMessagePanel::AppendTerminalDiagnose(TerminalDiagnose* pDiagnose)
{
	ASSERT(pDiagnose != NULL);


}

void SimulationErrorMessagePanel::AppendAirsideDiagnose(AirsideDiagnose* pDiagnose)
{
	ASSERT(pDiagnose != NULL);
	int nCount = m_wndAirsideListCtrl.GetItemCount();
	int nIndex = m_wndAirsideListCtrl.InsertItem(nCount, pDiagnose->GetTime());
	m_wndAirsideListCtrl.SetItemText(nIndex, 1, pDiagnose->GetErrorType());
	m_wndAirsideListCtrl.SetItemText(nIndex, 2,pDiagnose->GetMobElementID());
	m_wndAirsideListCtrl.SetItemText(nIndex, 3, pDiagnose->GetDetails());
}

void SimulationErrorMessagePanel::AppendLandsideDiagnose(LandsideDiagnose* pDiagnose)
{
	ASSERT(pDiagnose != NULL);
	if(!pDiagnose)
		return;

	CString strID ;
	strID.Format("%d",pDiagnose->GetMobElementID());
	int nCount = m_wndLandsideListCtrl.GetItemCount();
	int nIndex = m_wndLandsideListCtrl.InsertItem(nCount, pDiagnose->GetTime());
	m_wndLandsideListCtrl.SetItemText(nIndex, 1, pDiagnose->GetErrorType());
	m_wndLandsideListCtrl.SetItemText(nIndex, 2, strID);
	m_wndLandsideListCtrl.SetItemText(nIndex, 3, pDiagnose->GetDetails());
}

void SimulationErrorMessagePanel::AppendOnBoardDiagnose(OnBoardDiagnose* pDiagnose)
{
	ASSERT(pDiagnose != NULL);

	ASSERT(pDiagnose != NULL);
	int nCount = m_wndOnBoardListCtrl.GetItemCount();
	int nIndex = m_wndOnBoardListCtrl.InsertItem(nCount, pDiagnose->GetTime());
	m_wndOnBoardListCtrl.SetItemText(nIndex, 0,pDiagnose->GetTime());
	m_wndOnBoardListCtrl.SetItemText(nIndex, 1, pDiagnose->GetErrorType());
	m_wndOnBoardListCtrl.SetItemText(nIndex, 2, pDiagnose->GetFltID());
	CString strPaxID;
	strPaxID.Format(_T("%d"),pDiagnose->GetMobElementID());
	m_wndOnBoardListCtrl.SetItemText(nIndex, 3,strPaxID);
	m_wndOnBoardListCtrl.SetItemText(nIndex, 4, pDiagnose->GetDetails());

}


void SimulationErrorMessagePanel::AddSimStatusFormatExMsg(const FORMATMSGEX * msgEx)
{
	int _nCount = m_wndTerminalListCtrl.GetItemCount();
	int _nItemIndex = m_wndTerminalListCtrl.InsertItem( _nCount, msgEx->strTime );

	m_wndTerminalListCtrl.SetItemText( _nItemIndex, 1, msgEx->strMsgType );
	m_wndTerminalListCtrl.SetItemText( _nItemIndex, 2, msgEx->strProcessorID );
	m_wndTerminalListCtrl.SetItemText( _nItemIndex, 3, msgEx->strPaxID );
	m_wndTerminalListCtrl.SetItemText( _nItemIndex, 4, msgEx->strFlightID );
	m_wndTerminalListCtrl.SetItemText( _nItemIndex, 5, msgEx->strMobileType );
	m_wndTerminalListCtrl.SetItemText( _nItemIndex, 6, msgEx->strOther );

	// set item data
	if( msgEx->diag_entry.eType != NON_DIAGNOSE )	// have diagnose info
	{
		//MessageBox("test",NULL,MB_OK);
		DiagnoseEntry* p_entry = new DiagnoseEntry;
		*p_entry = msgEx->diag_entry;
		//m_vDiagnoseEntryList.push_back( p_entry );

		m_wndTerminalListCtrl.SetItemData( _nItemIndex, (DWORD_PTR)p_entry );
		m_wndTerminalListCtrl.SetCheckbox(_nItemIndex,3,FALSE);
	}
	else
	{
		m_wndTerminalListCtrl.SetItemData( _nItemIndex, -1 );
	}

	m_wndTerminalListCtrl.EnsureVisible(_nItemIndex,FALSE);
	return;
}



LRESULT SimulationErrorMessagePanel::OnXListCtrlCheckBoxClicked(WPARAM wParam,LPARAM lParam)
{
	int nItem=wParam;
	int nSubItem=lParam;
	if (FALSE == m_bShowErrorInfo)
	{
		m_wndTerminalListCtrl.SetCheckbox(nItem, nSubItem, FALSE);
		return FALSE;
	}

	CRect rectSubItem;
	m_wndTerminalListCtrl.GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,rectSubItem);
	m_wndTerminalListCtrl.ClientToScreen(rectSubItem);

	ASSERT(m_wndTerminalListCtrl.GetCheckbox((int)wParam, (int)lParam) == 1/*checked*/); // must have already been checked
	long _lItemData = m_wndTerminalListCtrl.GetItemData(nItem);
	if( _lItemData <0 )
		return TRUE;
	DiagnoseEntry* pEntry = ( DiagnoseEntry* ) _lItemData;

	switch( pEntry->eType) 
	{
	case MISS_FLIGHT:		// miss flight
	case PAXDEST_DIAGNOS:	// pax destination diagnose
		{
			if (NULL == m_pTrackWnd)
			{
				m_pTrackWnd = new CTrackWnd((Terminal*)m_pTerm, GetProjPath(), this);
			}
			if (m_pTrackWnd)
			{
				m_pTrackWnd->SetWindowPos(NULL,rectSubItem.left,rectSubItem.bottom,0,0,SWP_NOSIZE);
				reloadLogIfNeed( pEntry );
				m_pTrackWnd->ShowInfo(pEntry, nItem, nSubItem);
			}
			break;
		}
	default:
		break;
	}
	return TRUE;
}



void SimulationErrorMessagePanel::Move(int cx, int cy)
{
}



//----------------------------------------------

void SimulationErrorMessagePanel::reloadLogIfNeed( const DiagnoseEntry* _pEntry )
{
	Terminal* _pTerm = (Terminal*)m_pTerm;

	static long _iCallCount=0l;
	if( _iCallCount++ == 0 )	// the first call the function // ???
	{
		_pTerm->clearLogs();
	}

	if( _pEntry->iSimResultIndex == _pTerm->getCurrentSimResult() )
		return;

	_pTerm->clearLogs();
	_pTerm->setCurrentSimResult( _pEntry->iSimResultIndex );
	_pTerm->GetSimReportManager()->SetCurrentSimResult( _pEntry->iSimResultIndex );
}

CString SimulationErrorMessagePanel::GetProjPath()
{
	ASSERT(m_pTermPlanDoc != NULL);
	return m_pTermPlanDoc->m_ProjInfo.path;
}

// LRESULT SimulationErrorMessagePanel::NotifyOwner(WPARAM wParam, LPARAM lParam)
// {
// 
// 	switch((const UINT)wParam)
// 	{
// 	case XTP_TPN_GROUPEXPANDING:
// 		{
// 			XTP_TPNGROUPEXPANDING* pGroupExpanding = (XTP_TPNGROUPEXPANDING *)lParam;
// 			CXTPTaskPanelGroup* pGroup = pGroupExpanding->pGroup;
// 			if (pGroup == NULL)
// 			{
// 				return XTP_ACTION_CANCEL;
// 			}
// 			//if (pGroup != NULL)
// 			//{
// 			//	if (GetActiveGroup() == pGroup)
// 			//	{
// 			//		if(pGroup->IsExpanded() == TRUE)
// 			//			return CXTTabCtrl::NotifyOwner(wParam, lParam);
// 			//		else
// 			//			ExpandGroup(pGroup,TRUE);
// 			//	}
// 			//	else
// 			//	{
// 			//		if (pGroupExpanding->bExpanding == TRUE)
// 			//		{
// 			//		
// 			//			ExpandGroup(GetActiveGroup(),FALSE);
// 			//			ExpandGroup(pGroup,TRUE);
// 			//		}
// 			//	}
// 			//}
// 
// 			 //SetRedraw(FALSE);
// 			if (pGroupExpanding->bExpanding == TRUE)//collapse
// 			{
// 				int nItemCount = GetGroups()->GetCount();
// 				for (int i =0; i <nItemCount ;++i)
// 				{
// 					CXTPTaskPanelGroup* pTempGroup = GetAt(i);
// 
// 					if( pTempGroup != pGroup)
// 					{
// 						if(pTempGroup->IsExpanded() == TRUE)
// 						{
// 						
// 							ExpandGroup(pTempGroup,FALSE);
// 						}
// 					}
// 				}
// 				//ExpandGroup(pGroup,TRUE);
// 			}
// 
// 			//SetRedraw(TRUE);
// 
// 			if (pGroupExpanding->bExpanding == FALSE)
// 			{
// 				//CXTPTaskPanelGroup* pActiveGroup = GetActiveGroup();
// 				CXTPTaskPanelGroup* pActiveGroup = reinterpret_cast<CXTPTaskPanelGroup *>(GetFocusedItem());
// 				if (pActiveGroup == pGroup)
// 				{
// 					Reposition();
// 					return XTP_ACTION_CANCEL;
// 				}
// 			}
// 
// 
// 
// 			//}
// 
// 		}
// 		break;
// 
// 	case XTP_TPN_GROUPEXPANDED:
// 		{
// 
// 		}
// 		break;
// 
// 	case XTP_TPN_CLICK:
// 		{
// 			//int i =0;
// 		}
// 		break;
// 	default:
// 		break;
// 	}
// 	return CXTTabCtrl::NotifyOwner(wParam, lParam);
// }
// 
void SimulationErrorMessagePanel::ExpandAirside()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_AIRSIDE);
}
void SimulationErrorMessagePanel::ExpandLandside()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_LANSIDE);
}
void SimulationErrorMessagePanel::ExpandTerminal()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_TERMINAL);
}
void SimulationErrorMessagePanel::ExpandOnBoard()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_ONBOARD);
}
void SimulationErrorMessagePanel::ExpandCargo()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_CARGO);
}
void SimulationErrorMessagePanel::ExpandEnvironment()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_ENVIRONMENT);
}
void SimulationErrorMessagePanel::ExpandFinancial()
{
	ASSERT(TAB_ITEM_COUNT == GetItemCount());
	SetActiveView(TAB_INDEX_FINANCIAL);
}

void SimulationErrorMessagePanel::ClearErrorMessage()
{
	m_wndAirsideListCtrl.DeleteAllItems();
	m_wndLandsideListCtrl.DeleteAllItems();
	m_wndTerminalListCtrl.DeleteAllItems();
	m_wndOnBoardListCtrl.DeleteAllItems();
	m_wndCargoListCtrl.DeleteAllItems();
	m_wndEnvironmentListCtrl.DeleteAllItems();
	m_wndFinancialListCtrl.DeleteAllItems();
}

void SimulationErrorMessagePanel::SaveErrorMessage(FILE* pFile)
{
	//terminal messages
	fputs(_T("\nTerminal Error Messages .....\n"),pFile);
	SaveTerminalErrorMessage(m_wndTerminalListCtrl,pFile);

	fputs(_T("\nAirside Error Messages .....\n"),pFile);
	SaveAirsideErrorMessage(m_wndAirsideListCtrl,pFile);

	fputs(_T("\nLandside Error Messages .....\n"),pFile);
	SaveLandsideErrorMessage(m_wndLandsideListCtrl,pFile);

	fputs(_T("\nOnBoard Error Messages .....\n"),pFile);
	SaveOnBoardErrorMessage(m_wndOnBoardListCtrl,pFile);
}

void SimulationErrorMessagePanel::SaveAirsideErrorMessage(CXListCtrl& pListControl,FILE* f)
{
	int nCount =  pListControl.GetItemCount();
	if( nCount>0 )
		fputs("\n===============================================================\n",f);

	for( int j=0; j<nCount; j++ )
	{
		CString csStr;

		// time item
		csStr += pListControl.GetItemText( j, 0 );
		csStr += "\t";

		// error type
		csStr += pListControl.GetItemText( j, 1 );
		csStr += "\t";

		// ID
		csStr += pListControl.GetItemText( j, 2 );
		csStr += "\n";

		//details information
		csStr += pListControl.GetItemText( j, 3 );

		fputs( csStr, f );
		fputs( "\n", f );
	}
}

void SimulationErrorMessagePanel::SaveTerminalErrorMessage(CXListCtrl& pListControl,FILE* f)
{
	int nCount =  pListControl.GetItemCount();
	if( nCount>0 )
		fputs("\n===============================================================\n",f);

	for( int j=0; j<nCount; j++ )
	{
		CString csStr,strItem;

		// time item
		csStr += pListControl.GetItemText( j, 0 );
		csStr += "\t";

		// error type
		csStr += pListControl.GetItemText( j, 1 );
		csStr += ":\t";

		// process ID
		strItem = pListControl.GetItemText( j, 2 );
		if( !strItem.IsEmpty() )
		{
			csStr += " Processor: ";
			csStr = csStr + strItem + "\t";
		}

		// mob type
		// pax id
		strItem = pListControl.GetItemText( j, 3 );
		strItem.Remove(' ');
		if( !strItem.IsEmpty() )
		{
			csStr += "Passenger: ";
			csStr = csStr + strItem + " ";
		}
		// flight id
		strItem = pListControl.GetItemText( j, 4 );
		if( !strItem.IsEmpty() )
			csStr = csStr + strItem + " ";
		// mobile type
		strItem = pListControl.GetItemText( j, 5 );
		if( !strItem.IsEmpty() )
			csStr = csStr + strItem + "\t";

		// other info
		strItem = pListControl.GetItemText( j , 6 );
		if( !strItem.IsEmpty() )
			csStr += strItem;

		fputs( csStr, f );
		fputs( "\n", f );
	}


}

void SimulationErrorMessagePanel::SaveLandsideErrorMessage(CXListCtrl& pListControl,FILE* f)
{

}

void SimulationErrorMessagePanel::SaveOnBoardErrorMessage(CXListCtrl& pListControl,FILE* f)
{
	int nCount =  m_wndOnBoardListCtrl.GetItemCount();
	if( nCount>0 )
		fputs("\n===============================================================\n",f);

	for( int j=0; j<nCount; j++ )
	{
		CString csStr;

		// time item
		csStr += pListControl.GetItemText( j, 0 );
		csStr += "\t";

		// error type
		csStr += pListControl.GetItemText( j, 1 );
		csStr += "\t";

		// ID
		csStr += pListControl.GetItemText( j, 2 );
		csStr += "\n";

		//details information
		csStr += pListControl.GetItemText( j, 3 );

		fputs( csStr, f );
		fputs( "\n", f );
	}
}

BOOL SimulationErrorMessagePanel::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_MOUSEMOVE && pMsg->wParam == 0)
	{
		int id = ::GetDlgCtrlID(pMsg->hwnd);
		CPoint point;
		point.x = LOWORD(pMsg->lParam);
		point.y = HIWORD(pMsg->lParam);
		if(id == IDC_LIST_AIRSIDE || id == IDC_LIST_ONBOARD)
			MouseOverListCtrl(point);
		MouseOverCtrl(point, pMsg->hwnd);
	}

	return CXTTabCtrl::PreTranslateMessage(pMsg);
}

void SimulationErrorMessagePanel::MouseOverListCtrl(CPoint point)
{
	int nItem, nCol;

	// Airside
	if ((nItem = m_wndAirsideListCtrl.HitTestEx(point, &nCol)) != -1)
	{
		int nOriginalCol = m_wndAirsideListCtrl.GetHeaderCtrl()->OrderToIndex(nCol);
		CString sItemText = m_wndAirsideListCtrl.GetItemText(nItem, nOriginalCol);
		int iTextWidth = m_wndAirsideListCtrl.GetStringWidth(sItemText) + 5; //5 pixed extra size

		CRect rect;
		m_wndAirsideListCtrl.GetSubItemRect(nItem, nOriginalCol, LVIR_LABEL, rect);
		if(iTextWidth > rect.Width())
		{
			rect.top--;

			if(nOriginalCol == 3/*details information*/)
			{
				CXTStringHelper sTemp(sItemText);
				sTemp.Replace(_T("\\n"), _T("\n"));
				m_tipWindow.SetTipText(_T("Details Information:"), sTemp);
			}
			else
			{
				sItemText.Replace(_T("\r\n"), _T("\n"));
				m_tipWindow.SetMargins(CSize(1,1));
				m_tipWindow.SetLineSpace(0);
				m_tipWindow.SetTipText(_T(""), sItemText);
			}

			//Calculate the client coordinates of the dialog window
			m_wndAirsideListCtrl.ClientToScreen(&rect);
			m_wndAirsideListCtrl.ClientToScreen(&point);
			ScreenToClient(&rect);
			ScreenToClient(&point);

			//Show the tip window
			UpdateData();
			m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, FALSE);
		}

	}

	// OnBoard
	if ((nItem = m_wndOnBoardListCtrl.HitTestEx(point, &nCol)) != -1)
	{
		int nOriginalCol = m_wndOnBoardListCtrl.GetHeaderCtrl()->OrderToIndex(nCol);
		CString sItemText = m_wndOnBoardListCtrl.GetItemText(nItem, nOriginalCol);
		int iTextWidth = m_wndOnBoardListCtrl.GetStringWidth(sItemText) + 5; //5 pixed extra size

		CRect rect;
		m_wndOnBoardListCtrl.GetSubItemRect(nItem, nOriginalCol, LVIR_LABEL, rect);
		if(iTextWidth > rect.Width())
		{
			rect.top--;

			if(nOriginalCol == 3/*details information*/)
			{
				CXTStringHelper sTemp(sItemText);
				sTemp.Replace(_T("\\n"), _T("\n"));
				m_tipWindow.SetTipText(_T("Details Information:"), sTemp);
			}
			else
			{
				sItemText.Replace(_T("\r\n"), _T("\n"));
				m_tipWindow.SetMargins(CSize(1,1));
				m_tipWindow.SetLineSpace(0);
				m_tipWindow.SetTipText(_T(""), sItemText);
			}

			//Calculate the client coordinates of the dialog window
			m_wndOnBoardListCtrl.ClientToScreen(&rect);
			m_wndOnBoardListCtrl.ClientToScreen(&point);
			ScreenToClient(&rect);
			ScreenToClient(&point);

			//Show the tip window
			UpdateData();
			m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, FALSE);
		}
	}
}

void SimulationErrorMessagePanel::MouseOverCtrl(CPoint point, HWND hWnd)
{
	CWnd* pWnd = FromHandle(hWnd);
	if(!pWnd)
		return;
	int id = pWnd->GetDlgCtrlID();

}

LRESULT SimulationErrorMessagePanel::OnTrackWndDestroy(WPARAM wParam,LPARAM lParam)
{
	m_wndTerminalListCtrl.SetCheckbox((int)wParam,(int)lParam,FALSE);
	return TRUE;
}
