#include "stdafx.h"
#include "ARCportTabCtrl.h"
#include "Resource.h"

enum
{
	IDC_TAB_PANEL,
	IDC_LIST_SCHEDULE,
	IDC_LIST_ROSTER
};

const static COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT);
const static COLORREF crBackground = ::GetSysColor(COLOR_WINDOW);

const static COLORREF crValidText = RGB(255,62,62);
const static COLORREF crValidBackground = ::GetSysColor(COLOR_WINDOW);

CARCportTabCtrl::CARCportTabCtrl(AODBImportManager* aodbImportMgr)
:m_pAodbImportMgr(aodbImportMgr)
{
	m_tipWindow.SetMargins(CSize(1,1));
	m_tipWindow.SetLineSpace(10);
}

CARCportTabCtrl::~CARCportTabCtrl()
{

}

BEGIN_MESSAGE_MAP(CARCportTabCtrl, CXTTabCtrl)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ROSTER, OnLvnEndlabeleditRosterListData)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_SCHEDULE, OnLvnEndlabeleditScheduleListData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ROSTER, OnColumnclickListRoster)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SCHEDULE, OnColumnclickListSchedule)
	ON_MESSAGE(WM_XLISTCTRL_DBCLICKED,OnDBClickListCtrl)
END_MESSAGE_MAP()

LRESULT CARCportTabCtrl::OnDBClickListCtrl(WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

void CARCportTabCtrl::OnColumnclickListRoster(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrlRoster.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrlRoster.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrlRoster.SaveSortList();
	}	
	*pResult = 0;
}

void CARCportTabCtrl::OnColumnclickListSchedule(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrlSchedule.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrlSchedule.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrlSchedule.SaveSortList();
	}	
	*pResult = 0;
}

BOOL CARCportTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_MOUSEMOVE && pMsg->wParam == 0)
	{
		int id = ::GetDlgCtrlID(pMsg->hwnd);
		CPoint point;
		point.x = LOWORD(pMsg->lParam);
		point.y = HIWORD(pMsg->lParam);
		if(id == IDC_LIST_ROSTER || id == IDC_LIST_SCHEDULE)
			MouseOverListCtrl(id,point);
	}

	return CXTTabCtrl::PreTranslateMessage(pMsg);
}


void CARCportTabCtrl::MouseOverListCtrl(int id,CPoint point)
{
	int nItem, nCol;

	// Schedule list
	if (id == IDC_LIST_SCHEDULE && (nItem = m_wndScheduleListCtrl.HitTestEx(point, &nCol)) != -1)
	{
		int nOriginalCol = m_wndScheduleListCtrl.GetHeaderCtrl()->OrderToIndex(nCol);
		CString sItemText = m_wndScheduleListCtrl.GetItemText(nItem, nOriginalCol);
		int iTextWidth = m_wndScheduleListCtrl.GetStringWidth(sItemText) + 5; //5 pixed extra size
		AODB::ScheduleItem* item = (AODB::ScheduleItem*)m_wndScheduleListCtrl.GetItemData(nItem); 

		CRect rect;
		m_wndScheduleListCtrl.GetSubItemRect(nItem, nOriginalCol, LVIR_LABEL, rect);
		
		rect.top--;
		//Calculate the client coordinates of the dialog window
		m_wndScheduleListCtrl.ClientToScreen(&rect);
		m_wndScheduleListCtrl.ClientToScreen(&point);
		ScreenToClient(&rect);
		ScreenToClient(&point);

		AODB::MapField* pMapField = item->GetMapField((AODB::ScheduleItem::ScheduleItemType)nOriginalCol);
		if(pMapField && !(pMapField->IsValid())&& !item->IsSuccess())
		{
			CXTStringHelper sTemp(pMapField->GetErroDesc());
			if (!sTemp.IsEmpty())
			{
				sTemp.Replace(_T("\r\n"), _T("\n"));
				m_tipWindow.SetMargins(CSize(1,1));
				m_tipWindow.SetLineSpace(0);
				m_tipWindow.SetTipText(_T(""), sTemp);
				m_tipWindow.SetTipColors(::GetSysColor(COLOR_INFOBK),RGB(255,111,111));

				//Show the tip window
				UpdateData();
				m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, TRUE);
			}
			
		}
		else if(iTextWidth > rect.Width())
		{
			sItemText.Replace(_T("\r\n"), _T("\n"));
			m_tipWindow.SetMargins(CSize(1,1));
			m_tipWindow.SetLineSpace(0);
			m_tipWindow.SetTipText(_T(""), sItemText);
			m_tipWindow.SetTipColors(::GetSysColor(COLOR_INFOBK),::GetSysColor(COLOR_INFOTEXT));

			//Show the tip window
			UpdateData();
			m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, FALSE);
		}
	}

	// Roster
	if (id == IDC_LIST_ROSTER && (nItem = m_wndRosterListCtrl.HitTestEx(point, &nCol)) != -1)
	{
		int nOriginalCol = m_wndRosterListCtrl.GetHeaderCtrl()->OrderToIndex(nCol);
		CString sItemText = m_wndRosterListCtrl.GetItemText(nItem, nOriginalCol);
		int iTextWidth = m_wndRosterListCtrl.GetStringWidth(sItemText) + 5; //5 pixed extra size
		AODB::RosterItem* item = (AODB::RosterItem*)m_wndRosterListCtrl.GetItemData(nItem); 

		CRect rect;
		m_wndRosterListCtrl.GetSubItemRect(nItem, nOriginalCol, LVIR_LABEL, rect);
		
		
		rect.top--;
		//Calculate the client coordinates of the dialog window
		m_wndRosterListCtrl.ClientToScreen(&rect);
		m_wndRosterListCtrl.ClientToScreen(&point);
		ScreenToClient(&rect);
		ScreenToClient(&point);

		AODB::MapField* pMapFiled = item->GetMapFile((AODB::RosterItem::RosterItemType)nOriginalCol);
		if(pMapFiled && !(pMapFiled->IsValid()))
		{
			CXTStringHelper sTemp(pMapFiled->GetErroDesc());
			if (!sTemp.IsEmpty())
			{
				sTemp.Replace(_T("\r\n"), _T("\n"));
				m_tipWindow.SetMargins(CSize(1,1));
				m_tipWindow.SetLineSpace(0);
				m_tipWindow.SetTipText(_T(""), sTemp);
				m_tipWindow.SetTipColors(::GetSysColor(COLOR_INFOBK),RGB(255,111,111));

				//Show the tip window
				UpdateData();
				m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, TRUE);
			}
			
		}
		else if(iTextWidth > rect.Width())
		{
			sItemText.Replace(_T("\r\n"), _T("\n"));
			m_tipWindow.SetMargins(CSize(1,1));
			m_tipWindow.SetLineSpace(0);
			m_tipWindow.SetTipText(_T(""), sItemText);
			m_tipWindow.SetTipColors(::GetSysColor(COLOR_INFOBK),::GetSysColor(COLOR_INFOTEXT));

			//Show the tip window
			UpdateData();
			m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, FALSE);
		}
	}
}

void CARCportTabCtrl::SetTitle(const CString& szSchedule, const CString& szRoster)
{
	m_strRosterTitle = szRoster;
	m_strScheduleTitle = szSchedule;
}

BOOL CARCportTabCtrl::Create(CWnd* pWnd, const CRect& rect)
{
	if (!CXTTabCtrl::Create(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_TABSTOP, rect, pWnd, IDC_TAB_PANEL))
		return FALSE;

	SetPadding(CSize(6, 3));

	// Clip children of the tab control from paint routines to reduce flicker.
	ModifyStyle(0L, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	m_tipWindow.Create(this);
	m_tipWindow.ShowWindow(SW_HIDE);

	InitScheduleList();
	InitRosterList();

	SetActiveView(0);

	if (m_imgList.m_hImageList == NULL)
	{
		m_imgList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
	}
	m_imgList.Add(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_SUCCESS) ));
	m_imgList.Add(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FAIL) ));

	m_wndScheduleListCtrl.SetImageList(&m_imgList,LVSIL_SMALL);

	m_wndRosterListCtrl.SetImageList(&m_imgList,LVSIL_SMALL);
	return TRUE;
}

BOOL CARCportTabCtrl::InitRosterList()
{
	DWORD dwListCtrlStype = LVS_SHOWSELALWAYS|LVS_SINGLESEL|LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_TABSTOP;
	CRect rcListCtrl;
	GetClientRect(&rcListCtrl);

	if(!m_wndRosterListCtrl.Create( dwListCtrlStype, rcListCtrl,this,IDC_LIST_ROSTER))
		return FALSE;

	DWORD dwStyle = m_wndRosterListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndRosterListCtrl.SetExtendedStyle( dwStyle );

	m_ctlHeaderCtrlRoster.SubclassWindow( m_wndRosterListCtrl.GetHeaderCtrl()->GetSafeHwnd() );

	const char* colLabelEnvironment[] = { "RESOURCES", "START TIME(MM/DD/YYYY HH:MM:SS)", "END TIME(MM/DD/YYYY HH:MM:SS)","AIRLINES", "FLIGHT NUMBER"};
	const int nFormatEnvironment[] = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT};
	const int DefaultColumnWidthEnvironment[] = { 120, 150, 150, 60, 100};
	const EDataType typeEnvironment[] = { dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtINT};

	int nColNum = 5;
	for( int i=0; i<nColNum; i++ )
	{
		m_wndRosterListCtrl.InsertColumn( i,colLabelEnvironment[i], nFormatEnvironment[i], DefaultColumnWidthEnvironment[i]);
		m_ctlHeaderCtrlRoster.SetDataType( i, typeEnvironment[i] );
	}

	if (!AddControl(m_strRosterTitle, &m_wndRosterListCtrl, 1))
	{
		return FALSE;
	}

	AddToolTip(1, m_strRosterTitle);
	return TRUE;
}

BOOL CARCportTabCtrl::InitScheduleList()
{
	DWORD dwListCtrlStype = LVS_SHOWSELALWAYS|LVS_SINGLESEL|LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_TABSTOP;
	CRect rcListCtrl;
	GetClientRect(&rcListCtrl);

	if(!m_wndScheduleListCtrl.Create( dwListCtrlStype, rcListCtrl,this,IDC_LIST_SCHEDULE))
		return FALSE;

	DWORD dwStyle = m_wndScheduleListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndScheduleListCtrl.SetExtendedStyle( dwStyle );

	m_ctlHeaderCtrlSchedule.SubclassWindow( m_wndScheduleListCtrl.GetHeaderCtrl()->GetSafeHwnd() );

	const char* colLabelEnvironment[] = { "SDA(MM/DD/YYYY)", //1
		"AIRLINE", //2
		"ARR ID",//3
		"INT STOP",//4
		"ORIGIN", //5
		"STA(HH:MM:SS)",//6 
		"ATA(HH:MM:SS)",//7
		"ADA(MM/DD/YYYY)",//8
		"DEP ID", //9
		"INT STOP OUT",//10
		"DEST", //11
		"STD(HH:MM:SS)",//12
		"SDD(MM/DD/YYYY)",//13
		"ATD(HH:MM:SS)",//14
		"ADD(MM/DD/YYYY)",//15
		"ACTYPE", //16
		"ARR STAND",//17
		"T TOW OFF(HH:MM:SS)",//18
		"D TOW OFF(MM/DD/YYYY)", //19
		"DEP STAND",//20
		"INT STAND",//21
		"T EX INT STAND(HH:MM:SS)",//22
		"D EX INT STAND(MM/DD/YYYY)",//23
		"STAY TIME(HH:MM:SS)", //24
		"ARR GATE", //25
		"DEP GATE",//26
		"CAROUSEL",//27
		"ARR LOAD FACTOR",//28 
		"DEP LOAD FACTOR",//29
		"AC SEATS"//30
	};
	const int nFormatEnvironment[] = { LVCFMT_LEFT,//1
		LVCFMT_LEFT, //2
		LVCFMT_LEFT, //3
		LVCFMT_LEFT, //4
		LVCFMT_LEFT,//5
		LVCFMT_LEFT,//6
		LVCFMT_LEFT,//7
		LVCFMT_LEFT,//8
		LVCFMT_LEFT,//9
		LVCFMT_LEFT,//10
		LVCFMT_LEFT,//11
		LVCFMT_LEFT,//12
		LVCFMT_LEFT,//13
		LVCFMT_LEFT,//14
		LVCFMT_LEFT,//15
		LVCFMT_LEFT,//16
		LVCFMT_LEFT,//17
		LVCFMT_LEFT,//18
		LVCFMT_LEFT,//19
		LVCFMT_LEFT,//20
		LVCFMT_LEFT,//21
		LVCFMT_LEFT,//22
		LVCFMT_LEFT,//23
		LVCFMT_LEFT,//24
		LVCFMT_LEFT,//25
		LVCFMT_LEFT,//26
		LVCFMT_LEFT,//27
		LVCFMT_LEFT,//28
		LVCFMT_LEFT,//29
		LVCFMT_LEFT//30
	};
	const int DefaultColumnWidthEnvironment[] = { 100,//1
		60,//2
		50,//3
		60,//4
		60,//5
		100,//6
		100,//7
		100,//8
		50,//9
		100,//10 
		60,//11
		100,//12
		100,//13
		100,//14
		100,//15
		60,//16
		100,//17
		100,//18
		100,//19
		100,//20
		100,//21
		100,//22
		100,//23
		100,//24
		100,//25
		100,//26
		100,//27
		120,//28
		120,//29
		60//30
	};
	const EDataType typeEnvironment[] = { dtSTRING,
		dtSTRING,
		dtSTRING, 
		dtSTRING, 
		dtSTRING,
		dtSTRING, 
		dtSTRING, 
		dtSTRING,
		dtSTRING, 
		dtSTRING,
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING,
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING,
		dtSTRING, 
		dtSTRING, 
		dtSTRING, 
		dtSTRING,
		dtINT,
		dtINT,
		dtINT
	};

	int nColNum = 30;
	for( int i=0; i<nColNum; i++ )
	{
		m_wndScheduleListCtrl.InsertColumn( i, colLabelEnvironment[i], nFormatEnvironment[i], DefaultColumnWidthEnvironment[i],-1 );
		m_ctlHeaderCtrlSchedule.SetDataType( i, typeEnvironment[i] );
	}

	if (!AddControl(m_strScheduleTitle, &m_wndScheduleListCtrl, 0))
	{
		return FALSE;
	}

	AddToolTip(0, m_strScheduleTitle);
	return TRUE;
}

void CARCportTabCtrl::LoadListData(AODBImportManager& aodbImportManager)
{
	SetTabText(0, m_strScheduleTitle);
	SetTabText(1, m_strRosterTitle);
	FillScheduleList(aodbImportManager);
	FillRosterList(aodbImportManager);
}

void CARCportTabCtrl::FillScheduleList(AODBImportManager& aodbImportManager)
{
	AODB::ScheduleFile& scheduleData = aodbImportManager.GetScheduleFile();

	m_wndScheduleListCtrl.DeleteAllItems();
	for (int i = 0; i < scheduleData.GetCount(); i++)
	{
		AODB::ScheduleItem* item = scheduleData.GetItem(i);
		{
			//arr day
			CString strArrDay(_T(""));
			strArrDay.Format(_T("Day %d"),item->m_arrDate.GetDay());
			m_wndScheduleListCtrl.InsertItem(i,strArrDay);
			SetScheduleItemColor(i,0,item->m_arrDate);
		
			//airline
			m_wndScheduleListCtrl.SetItemText(i,1,item->m_strAirline.GetMapValue());
			SetScheduleItemColor(i,1,item->m_strAirline);

			//arr id
			m_wndScheduleListCtrl.SetItemText(i,2,item->m_strArrID.GetMapValue());
			SetScheduleItemColor(i,2,item->m_strArrID);

			//Int stop
			m_wndScheduleListCtrl.SetItemText(i,3,item->m_strArrIntStop.GetMapValue());
			SetScheduleItemColor(i,3,item->m_strArrIntStop);

			//Origin
			m_wndScheduleListCtrl.SetItemText(i,4,item->m_strOrigin.GetMapValue());
			SetScheduleItemColor(i,4,item->m_strOrigin);

			//ARR TIME STA
			m_wndScheduleListCtrl.SetItemText(i,5,item->m_arrTime.GetMapValue());
			SetScheduleItemColor(i,5,item->m_arrTime);

			//ATA
			m_wndScheduleListCtrl.SetItemText(i,6,item->m_ataTime.GetMapValue());
			SetScheduleItemColor(i,6,item->m_ataTime);

			//ADA
			CString strADADay(_T(""));
			strADADay.Format(_T("Day %d"),item->m_ataDay.GetDay());
			m_wndScheduleListCtrl.SetItemText(i,7,strADADay);
			SetScheduleItemColor(i,7,item->m_ataDay);

			//DEP ID
			m_wndScheduleListCtrl.SetItemText(i,8,item->m_strDepID.GetMapValue());
			SetScheduleItemColor(i,8,item->m_strDepID);

			//INT STOP OUT
			m_wndScheduleListCtrl.SetItemText(i,9,item->m_strDepIntStop.GetMapValue());
			SetScheduleItemColor(i,9,item->m_strDepIntStop);

			//DEST
			m_wndScheduleListCtrl.SetItemText(i,10,item->m_strDest.GetMapValue());
			SetScheduleItemColor(i,10,item->m_strDest);

			//DEP TIME STD
			m_wndScheduleListCtrl.SetItemText(i,11,item->m_depTime.GetMapValue());
			SetScheduleItemColor(i,11,item->m_depTime);

			//DEP DATE SDD
			CString strDepDay(_T(""));
			strDepDay.Format(_T("Day %d"),item->m_depDate.GetDay());
			m_wndScheduleListCtrl.SetItemText(i,12,strDepDay);
			SetScheduleItemColor(i,12,item->m_depDate);

			//ATD
			m_wndScheduleListCtrl.SetItemText(i,13,item->m_atdTime.GetMapValue());
			SetScheduleItemColor(i,13,item->m_atdTime);

			//ADD
			CString strADDDay(_T(""));
			strADDDay.Format(_T("Day %d"),item->m_atdDay.GetDay());
			m_wndScheduleListCtrl.SetItemText(i,14,strADDDay);
			SetScheduleItemColor(i,14,item->m_atdDay);

			//ACTYPE
			m_wndScheduleListCtrl.SetItemText(i,15,item->m_strACType.GetMapValue());
			SetScheduleItemColor(i,15,item->m_strACType);

			//ARR STAND
			m_wndScheduleListCtrl.SetItemText(i,16,item->m_arrStand.GetMapValue());
			SetScheduleItemColor(i,16,item->m_arrStand);

			//T TOW OFF
			m_wndScheduleListCtrl.SetItemText(i,17,item->m_towOffTime.GetMapValue());
			SetScheduleItemColor(i,17,item->m_towOffTime);

			//D TOW OFF
			CString strTowoffDay(_T(""));
			strTowoffDay.Format(_T("Day %d"),item->m_towOffDay.GetDay());
			m_wndScheduleListCtrl.SetItemText(i,18,strTowoffDay);
			SetScheduleItemColor(i,18,item->m_towOffDay);

			//DEP STAND
			m_wndScheduleListCtrl.SetItemText(i,19,item->m_depStand.GetMapValue());
			SetScheduleItemColor(i,19,item->m_depStand);

			//INT STAND
			m_wndScheduleListCtrl.SetItemText(i,20,item->m_intStand.GetMapValue());
			SetScheduleItemColor(i,20,item->m_intStand);

			//T EX INT STAND
			m_wndScheduleListCtrl.SetItemText(i,21,item->m_towOffExTime.GetMapValue());
			SetScheduleItemColor(i,21,item->m_towOffExTime);

			//D EX INT STAND
			CString strTowoffExDay(_T(""));
			strTowoffExDay.Format(_T("Day %d"),item->m_towOffExDay.GetDay());
			m_wndScheduleListCtrl.SetItemText(i,22,strTowoffExDay);
			SetScheduleItemColor(i,22,item->m_towOffExDay);

			//STAY TIME
			m_wndScheduleListCtrl.SetItemText(i,23,item->m_stayTime.GetMapValue());
			SetScheduleItemColor(i,23,item->m_stayTime);

			//ARR GATE
			m_wndScheduleListCtrl.SetItemText(i,24,item->m_arrGate.GetMapValue());
			SetScheduleItemColor(i,24,item->m_arrGate);

			//DEP GATE
			m_wndScheduleListCtrl.SetItemText(i,25,item->m_depGate.GetMapValue());
			SetScheduleItemColor(i,25,item->m_depGate);

			//CAROUSEL
			m_wndScheduleListCtrl.SetItemText(i,26,item->m_bageDevice.GetMapValue());
			SetScheduleItemColor(i,26,item->m_bageDevice);

			//ARR LOAD FACTOR
			m_wndScheduleListCtrl.SetItemText(i,27,item->m_dArrLoadFactor.GetMapValue());
			SetScheduleItemColor(i,27,item->m_dArrLoadFactor);

			//DEP LOAD FACTOR
			m_wndScheduleListCtrl.SetItemText(i,28,item->m_deDepLoadFactor.GetMapValue());
			SetScheduleItemColor(i,28,item->m_deDepLoadFactor);

			//AC SEATS
			m_wndScheduleListCtrl.SetItemText(i,29,item->m_nSeats.GetMapValue());
			SetScheduleItemColor(i,29,item->m_nSeats);

			if (item->IsSuccess())
			{
				m_wndScheduleListCtrl.SetItemImage(i,0,0);
			}
			else
			{
				m_wndScheduleListCtrl.SetItemImage(i,0,1);
			}
			m_wndScheduleListCtrl.SetItemData(i,(DWORD_PTR)item);
		}	
	}
}

void CARCportTabCtrl::SetScheduleItemColor(int nItem, int nSubItem,const AODB::MapField& pMapField)
{
	if (!pMapField.IsValid())
	{
		m_wndScheduleListCtrl.SetItemText(nItem,nSubItem,pMapField.GetOriginal(),crValidText,crValidBackground);
	}
	else
	{
		m_wndScheduleListCtrl.SetItemText(nItem,nSubItem,pMapField.GetMapValue(),crText,crBackground);
	}
}

void CARCportTabCtrl::SetRosterItemColor(int nItem, int nSubItem,const AODB::MapField& pMapField)
{
	if (!pMapField.IsValid())
	{
		m_wndRosterListCtrl.SetItemText(nItem,nSubItem,pMapField.GetOriginal(),crValidText,crValidBackground);
	}
	else
	{
		m_wndRosterListCtrl.SetItemText(nItem,nSubItem,pMapField.GetMapValue(),crText,crBackground);
	}
}

void CARCportTabCtrl::FillRosterList(AODBImportManager& aodbImportManager)
{
	AODB::RosterFile& rosterData = aodbImportManager.GetRosterFile();

	m_wndRosterListCtrl.DeleteAllItems();
	for (int i = 0; i < rosterData.GetCount(); i++)
	{
		AODB::RosterItem* item = rosterData.GetItem(i);

		//RESOURCES
		m_wndRosterListCtrl.InsertItem(i,item->m_proc.GetMapValue());
		SetRosterItemColor(i,0,item->m_proc);

		//START TIME
		m_wndRosterListCtrl.SetItemText(i,1,item->m_startTime.GetMapValue());
		SetRosterItemColor(i,1,item->m_startTime);
	
		//END TIME
		m_wndRosterListCtrl.SetItemText(i,2,item->m_endTime.GetMapValue());
		SetRosterItemColor(i,2,item->m_endTime);

		//AIRLINES
		m_wndRosterListCtrl.SetItemText(i,3,item->m_strAirline.GetMapValue());
		SetRosterItemColor(i,3,item->m_strAirline);

		//FLIGHT NUMBER
		m_wndRosterListCtrl.SetItemText(i,4,item->m_strFltNumber.GetMapValue());
		SetRosterItemColor(i,4,item->m_strFltNumber);

		if (item->valid())
		{
			m_wndRosterListCtrl.SetItemImage(i,0,0);
		}
		else
		{
			m_wndRosterListCtrl.SetItemImage(i,0,1);
		}
		m_wndRosterListCtrl.SetItemData(i,(DWORD_PTR)item);
	}
}

void CARCportTabCtrl::OnLvnEndlabeleditRosterListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_wndRosterListCtrl.OnEndlabeledit(pNMHDR, pResult);

	int nItem = pDispInfo->item.iItem;
	AODB::RosterItem* pItem = (AODB::RosterItem*)m_wndRosterListCtrl.GetItemData(nItem);

	CString szText = m_wndRosterListCtrl.GetItemText(nItem,pDispInfo->item.iSubItem);
	AODB::RosterItem::RosterItemType type = (AODB::RosterItem::RosterItemType)pDispInfo->item.iSubItem;
	AODB::MapField* pMapField = pItem->GetMapFile(type);
	if (pMapField)
	{
		pMapField->SetValue(szText,m_pAodbImportMgr->GetConverParameter());
		
		if (pMapField->IsValid() && (type == AODB::RosterItem::START_TIME_TYPE || type == AODB::RosterItem::END_TIME_TYPE))
		{
			AODB::DateTimeMapField* pDateTime = (AODB::DateTimeMapField*)pMapField;
			if(m_pAodbImportMgr->SetStartTime(pDateTime->getDateTime()))
			{
				pMapField->SetValue(szText,m_pAodbImportMgr->GetConverParameter());
				pMapField->Convert(m_pAodbImportMgr->GetConverParameter());
				
				FillRosterList(*m_pAodbImportMgr);
				FillScheduleList(*m_pAodbImportMgr);
				*pResult = 0;
				return;
			}
		
		}
	
		pMapField->Convert(m_pAodbImportMgr->GetConverParameter());

		SetRosterItemColor(nItem,pDispInfo->item.iSubItem,*pMapField);
		if (pItem->IsSuccess())
		{
			m_wndRosterListCtrl.SetItemImage(nItem,0,0);
		}
		else
		{
			m_wndRosterListCtrl.SetItemImage(nItem,0,1);
		}
		m_wndRosterListCtrl.UpdateSubItem(nItem,pDispInfo->item.iSubItem);	
	}

	*pResult = 0;
}

void CARCportTabCtrl::OnLvnEndlabeleditScheduleListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_wndScheduleListCtrl.OnEndlabeledit(pNMHDR, pResult);

	int nItem = pDispInfo->item.iItem;
	AODB::ScheduleItem* pItem = (AODB::ScheduleItem*)m_wndScheduleListCtrl.GetItemData(nItem);

	CString szText = m_wndScheduleListCtrl.GetItemText(nItem,pDispInfo->item.iSubItem);
	AODB::ScheduleItem::ScheduleItemType type = (AODB::ScheduleItem::ScheduleItemType)pDispInfo->item.iSubItem;
	AODB::MapField* pMapField = pItem->GetMapField(type);
	if (pMapField)
	{
		pMapField->SetValue(szText,m_pAodbImportMgr->GetConverParameter());
		
		if (pMapField->IsValid() && (type == AODB::ScheduleItem::ARRIVAL_DAY_TYPE || \
			type == AODB::ScheduleItem::DEPDAY_DAY_TYPE || \
			type == AODB::ScheduleItem::ADATIME_TYPE || \
			type == AODB::ScheduleItem::ADDTIME_TYPE || \
			type ==  AODB::ScheduleItem::DTOWOFF_TYPE || \
			type == AODB::ScheduleItem::DEXINT_STAND_TYPE))
		{
			AODB::DateMapField* pDayField = (AODB::DateMapField*)pMapField;
			if(m_pAodbImportMgr->SetStartTime(pDayField->getDate()))
			{
				pMapField->SetValue(szText,m_pAodbImportMgr->GetConverParameter());
				pMapField->Convert(m_pAodbImportMgr->GetConverParameter());

				FillScheduleList(*m_pAodbImportMgr);
				FillRosterList(*m_pAodbImportMgr);

				*pResult = 0;
				return;
			}
		}
		
		pMapField->Convert(m_pAodbImportMgr->GetConverParameter());
		
		SetScheduleItemColor(nItem,pDispInfo->item.iSubItem,*pMapField);

		pItem->ConvertToFlight(m_pAodbImportMgr->GetConverParameter());
		if (pItem->IsSuccess())
		{
			//pItem->Success(true);
			m_wndScheduleListCtrl.SetItemImage(nItem,0,0);
		}
		else
		{
			//pItem->Success(false);
			m_wndScheduleListCtrl.SetItemImage(nItem,0,1);
		}
		m_wndScheduleListCtrl.UpdateSubItem(nItem,pDispInfo->item.iSubItem);
		
	}

	*pResult = 0;
}

//////////tip window/////////////////////////////////////////////////////////////////////
CARCportTipWindow::CARCportTipWindow()
{

}

CARCportTipWindow::~CARCportTipWindow()
{

}

BEGIN_MESSAGE_MAP(CARCportTipWindow, CXTTipWindow)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CARCportTipWindow::ForwardMessage(UINT uiMsg, UINT nFlags, CPoint point)
{
	HideTipWindow();

 	CPoint pt(point);
 	ClientToScreen(&pt);
 	CWnd* pClick = WindowFromPoint(pt);

	if (!pClick->IsChild(this) && pClick != this)
 	{
 		LRESULT iHitTest = pClick->SendMessage(WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y));
 		if (AfxGetMainWnd() != NULL)
 			pClick->SendMessage(WM_MOUSEACTIVATE, (LPARAM)AfxGetMainWnd()->m_hWnd, MAKELPARAM(iHitTest , uiMsg));
 		pClick->ScreenToClient(&pt);
 		pClick->SendMessage(uiMsg, nFlags, MAKELPARAM(pt.x, pt.y));
 	}

	CXTTipWindow::OnLButtonDown(nFlags, point);
}

void CARCportTipWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	ForwardMessage(WM_LBUTTONDOWN, nFlags, point);
}