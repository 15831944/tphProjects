// StationSheet.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "StationSheet.h"
#include ".\stationsheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationSheet property page



CStationSheet* CStationSheet::m_sheetCurrentInstance = NULL;


IMPLEMENT_DYNCREATE(CStationSheet, CPropertyPage)

CStationSheet::CStationSheet(TrainTrafficGraph* _pTrafficGraph,CAllCarSchedule* _pAllCarSchedule )
		 : CPropertyPage(CStationSheet::IDD ,IDS_STRING_STATION_SCHEDULE )
{
	m_pTrafficGraph = _pTrafficGraph;
	m_pAllCarSchedule = _pAllCarSchedule;

	ASSERT(NULL == m_sheetCurrentInstance); // you should not create more than one instance at the same time
	m_sheetCurrentInstance = this;
}

CStationSheet::~CStationSheet()
{
	m_sheetCurrentInstance = NULL;
}

void CStationSheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationSheet)
	DDX_Control(pDX, IDC_LIST_TIMETABLE, m_listTimeTable);
	DDX_Control(pDX, IDC_LIST_STATION, m_listStation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationSheet, CPropertyPage)
	//{{AFX_MSG_MAP(CStationSheet)
	ON_LBN_SELCHANGE(IDC_LIST_STATION, OnSelchangeListStation)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_TIMETABLE, OnGetdispinfoListTimetable)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TIMETABLE, OnColumnclickListTimetable)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_IMPORT_TIME_TABLE, OnBnClickedBtnImportTimeTable)
	ON_BN_CLICKED(IDC_BTN_EXPORT_TIME_TABLE, OnBnClickedBtnExportTimeTable)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationSheet message handlers

BOOL CStationSheet::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	InitStationListBox();
	InitTimeTableList();

	EnableControls(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CStationSheet::InitStationListBox()
{
	STATIONANDRALLWAY::const_iterator iter= m_pTrafficGraph->GetTrafficGraph().begin();
	STATIONANDRALLWAY::const_iterator iterEnd= m_pTrafficGraph->GetTrafficGraph().end();
	IntegratedStation* pStation;
	char sStationName [64];
	for( ; iter!=iterEnd; ++iter)
	{
		pStation = (*iter)->GetStation();
		pStation->getID()->printID( sStationName );
		m_listStation.SetItemData( m_listStation.AddString( sStationName ) , (DWORD)pStation );
	}
}

void CStationSheet::OnSelchangeListStation() 
{
	int iSelect = m_listStation.GetCurSel();
	ASSERT( iSelect >=0 );
	IntegratedStation* pStation = (IntegratedStation*)m_listStation.GetItemData( iSelect );
	SetListTimeTable( pStation );

	EnableControls(NULL != pStation);
}

void CStationSheet::SetListTimeTable( IntegratedStation* _pStation)
{
	m_vTimeTable.clear();
	m_listTimeTable.DeleteAllItems();

	m_pAllCarSchedule->GetStationTimeTable( _pStation,m_vTimeTable);

	TimeTableVector::const_iterator iter = m_vTimeTable.begin();
	TimeTableVector::const_iterator iterEnd = m_vTimeTable.end();
		
		for(int i=0; iter!=iterEnd ; ++iter,++i) 
		{
			LV_ITEM lvi;
			lvi.mask = LVIF_TEXT |  LVIF_PARAM;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.iImage = 0;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			// lvi.lParam = (LPARAM) &(*iter);
			lvi.lParam = (LPARAM) i;
			m_listTimeTable.InsertItem( &lvi );
		}

}
void CStationSheet::InitTimeTableList()
{
	CRect listRcct;
	CString colName;
	m_listTimeTable.GetClientRect( &listRcct );

	int width = listRcct.Width()/7;
	colName.LoadString(IDS_STRING_TRAIN_INDEX);
	m_listTimeTable.InsertColumn(0, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_STRING_SCHEDULE_NAME);
	m_listTimeTable.InsertColumn(1, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_ARRIVE_DAY);
	m_listTimeTable.InsertColumn(2, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_ARRIVE_TIME);
	m_listTimeTable.InsertColumn(3, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_DEPART_DAY);
	m_listTimeTable.InsertColumn(4, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_DEPARTTIME);
	m_listTimeTable.InsertColumn(5, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_DESTSTATION);
	m_listTimeTable.InsertColumn(6, colName, LVCFMT_LEFT, width);
	m_listTimeTable.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	
	
	ListView_SetExtendedListViewStyle(m_listTimeTable.GetSafeHwnd(),LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES );


}

void CStationSheet::OnGetdispinfoListTimetable(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(pDispInfo->item.mask & LVIF_TEXT) 
	{
		TIMETABLE* pInfo = &m_vTimeTable[(size_t)(pDispInfo->item.lParam)];
		switch(pDispInfo->item.iSubItem)
		{
			case 0://train index
			{
				CString sIndex;
				sIndex.Format( "%d",pInfo->m_nTrainIndex);
				lstrcpy(pDispInfo->item.pszText, sIndex);
				break;;
			}
			case 1: //schedule name
				lstrcpy(pDispInfo->item.pszText, pInfo->m_sScheduleName);
				break;
			case 2: // Arrive Day
				{
					CString sIndex;
					sIndex.Format( _T("%d"),pInfo->m_arrDay);
					lstrcpy(pDispInfo->item.pszText, sIndex);
				}
				break;
			case 3: //arrive time
				lstrcpy(pDispInfo->item.pszText, pInfo->m_arrTime);
				break;
			case 4: // Departure Day
				{
					CString sIndex;
					sIndex.Format( _T("%d"),pInfo->m_depDay);
					lstrcpy(pDispInfo->item.pszText, sIndex);
				}
				break;
			case 5: //depart time
				lstrcpy(pDispInfo->item.pszText, pInfo->m_depTime);
				break;
			case 6://dest station
				lstrcpy(pDispInfo->item.pszText, pInfo->m_sDestStationName);
				break;
		
		}
	}
	*pResult = 0;
}

void CStationSheet::OnColumnclickListTimetable(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_listTimeTable.SortItems(CompareFunc, pNMListView->iSubItem);	
	*pResult = 0;
}
int CALLBACK CStationSheet::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//static int sortorder = 0;
	ASSERT(m_sheetCurrentInstance);
	TIMETABLE* pItem1 = &m_sheetCurrentInstance->m_vTimeTable[(size_t)(lParam1)];
	TIMETABLE* pItem2 = &m_sheetCurrentInstance->m_vTimeTable[(size_t)(lParam2)];
	int nResult;
	switch(lParamSort) 
	{
		case 0://train index
		{
			CString sIndex1;
			CString sIndex2;
			sIndex1.Format( "%d",pItem1->m_nTrainIndex);
			sIndex2.Format( "%d",pItem2->m_nTrainIndex);
			nResult = sIndex1.CompareNoCase( sIndex2 );
			
			break;;
		}
		case 1: // sort by schedule name
			nResult = pItem1->m_sScheduleName.CompareNoCase(pItem2->m_sScheduleName);
			break;
		case 2: // sort by arrive day
			nResult = pItem1->m_arrDay - pItem2->m_arrDay;
			break;
		case 3: // sort by arrive time
			nResult = pItem1->m_arrTime.CompareNoCase(pItem2->m_arrTime);
			break;
		case 4: // sort by depart day
			nResult = pItem1->m_depDay - pItem2->m_depDay;
			break;
		case 5: // sort by depart time
			nResult = pItem1->m_depTime.CompareNoCase(pItem2->m_depTime);
			break;
		case 6: // sort by dest station
			nResult = pItem1->m_sDestStationName.CompareNoCase(pItem2->m_sDestStationName);
			break;
		default:
			ASSERT(FALSE);
			break;
		
	}

	return nResult;
}

// use this as the file filter type string
const CString CStationSheet::m_strFileFilterType = _T("Train Schedule File (*.skd)|*.skd;*.SKD|All type (*.*)|*.*|");


void CStationSheet::OnBnClickedBtnImportTimeTable()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,_T("skd"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE ,
		m_strFileFilterType, NULL, 0, FALSE  );
	if(filedlg.DoModal()!=IDOK)
	{
		return;
	}

	CString csFileName=filedlg.GetPathName();
	// dealing with importing

}


void CStationSheet::OnBnClickedBtnExportTimeTable()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,_T("csv"), NULL,
		OFN_PATHMUSTEXIST | OFN_SHAREAWARE ,
		m_strFileFilterType, NULL, 0, FALSE  );
	if(filedlg.DoModal()!=IDOK)
	{
		return;
	}

	// dealing with exporting
	CString csFileName=filedlg.GetPathName();
	ArctermFile file;
	file.openFile(csFileName,WRITE);
	file.writeField("Train Number");
	file.writeField("schedule Name");
	file.writeField("start Station");     
	file.writeField("Arrive Day");
	file.writeField("Arrive Time");
	file.writeField("Destination Station");
	file.writeField("Departure Day");
	file.writeField("Departure Time");
	file.writeLine();
	STATIONANDRALLWAY::const_iterator iter= m_pTrafficGraph->GetTrafficGraph().begin();
	STATIONANDRALLWAY::const_iterator iterEnd= m_pTrafficGraph->GetTrafficGraph().end();
	
	for( ; iter!=iterEnd; ++iter)
	{
		IntegratedStation* pStation;
		pStation = (*iter)->GetStation();
		m_pAllCarSchedule->GetStationTimeTable( pStation,m_vTimeTable);
		TimeTableVector::const_iterator iter = m_vTimeTable.begin();
		TimeTableVector::const_iterator iterEnd = m_vTimeTable.end();
// 		char sStationName [64];
// 		pStation->getID()->printID(sStationName);
// 		file.writeField(sStationName);
// 		file.writeLine();
		for(int i=0; iter!=iterEnd ; ++iter,++i) 
		{
			file.writeInt((*iter).m_nTrainIndex);
			file.writeField((*iter).m_sScheduleName);
			file.writeField((*iter).m_sCurStationName);
			file.writeInt((*iter).m_arrDay);
			file.writeField((*iter).m_arrTime);
			file.writeField((*iter).m_sDestStationName);
			file.writeInt((*iter).m_depDay);
			file.writeField((*iter).m_depTime);
			file.writeLine();	
		}
	}
	file.endFile();
	MessageBox(_T("Export station time table successfully!"),"Warning",MB_OK);
}

// enable related controls, current there are import and export buttons
void CStationSheet::EnableControls(bool bEnabled)
{
	// import and export button
	//GetDlgItem(IDC_BTN_IMPORT_TIME_TABLE)->EnableWindow( bEnabled );
	//GetDlgItem(IDC_BTN_EXPORT_TIME_TABLE)->EnableWindow( bEnabled );
}

