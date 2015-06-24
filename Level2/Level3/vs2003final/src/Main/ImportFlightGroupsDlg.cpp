// ImportFlightGroupsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ImportFlightGroupsDlg.h"
#include ".\importflightgroupsdlg.h"
#include "..\Inputs\IN_TERM.H"
#include "..\Inputs\flight.h"
#include "..\common\FlightManager.h"
#include "..\Inputs\schedule.h"
#include "../Common/AirportDatabaseList.h"
#include "../Common/AirportDatabase.h"


// CImportFlightGroupsDlg dialog
const char fileheader[] = "ARCTerm Flight Groups File";
static char* columnLabel[] = {
	"Flight Group",
	"Airline", 
	"ID",
	"Day"		
};

#define  ID_WND_SPLITTERCONTROL1 1
#define  ID_WND_SPLITTERCONTROL2 2

IMPLEMENT_DYNAMIC(CImportFlightGroupsDlg, CDialog)
CImportFlightGroupsDlg::CImportFlightGroupsDlg(InputTerminal* _pInTerm,CWnd* pParent /*=NULL*/)
	: CDialog(CImportFlightGroupsDlg::IDD, pParent)
	,m_pInterm(_pInTerm)
	,m_bFlightGroupFile(FALSE)
{
}

CImportFlightGroupsDlg::~CImportFlightGroupsDlg()
{
	for (size_t i = 0; i < m_vFlightGroups.size(); i++)
	{
		if (m_vFlightGroups[i])
		{
			delete m_vFlightGroups[i];
			m_vFlightGroups[i] = NULL;
		}
	}
}

void CImportFlightGroupsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListData);
	DDX_Control(pDX,IDC_LIST_GROUP,m_wndListGroup);
	DDX_Control(pDX,IDC_EDIT_ERROR,m_editResultInfo);
	DDX_Control(pDX,IDC_LIST_FLIGHTGROUP,m_wndListBox);
}


BEGIN_MESSAGE_MAP(CImportFlightGroupsDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_OPEN,OnBtnOpenFile)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT,OnBtnConvert)
	ON_BN_CLICKED(IDC_BUTTON_APPEND,OnBtnAppend)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE,OnBtnReplace)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHTGROUP, OnLbnSelchangeListGroups)
	ON_MESSAGE(UM_HEADERCTRL_CLICK_COLUMN,OnHeaderCtrlClickColumn)
	ON_COMMAND(ID_DELETE_COLUMN,OnDeleteColumn)
	ON_COMMAND(ID_DELETE_ROW,OnDeleteRow)
	ON_COMMAND(ID_NAMELIST_TITLE,OnButtonTitle)
END_MESSAGE_MAP()


// CImportFlightGroupsDlg message handlers

BOOL CImportFlightGroupsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	GetClientRect(rect);
	SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	m_headerCtrlDrag.SubclassWindow( m_wndListData.GetDlgItem(0)->GetSafeHwnd());
	InitListCtrl();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CImportFlightGroupsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolbar.LoadToolBar(IDR_NAMELIST_TEXT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here

	return 0;
}

void CImportFlightGroupsDlg::InitListCtrl()
{
	DWORD dwStyle = m_wndListData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListData.SetExtendedStyle( dwStyle );
	m_wndListGroup.SetExtendedStyle(dwStyle);

	CRect rect;
	m_wndListData.GetClientRect(rect);
	m_wndListData.InsertColumn(0,"No.",LVCFMT_LEFT,rect.Width());

	CRect listrect;
	m_wndListGroup.GetClientRect(&listrect);
	m_wndListGroup.InsertColumn(0, "Airline", LVCFMT_LEFT, listrect.Width()/3);
	m_wndListGroup.InsertColumn(1, "ID", LVCFMT_LEFT, listrect.Width()/3);
	m_wndListGroup.InsertColumn(2, "Day",LVCFMT_LEFT, listrect.Width()/3);

	GetDlgItem(IDC_BUTTON_REPLACE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_APPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONVERT)->EnableWindow(FALSE);
}

void CImportFlightGroupsDlg::OnBtnOpenFile()
{
	CFileDialog filedlg( TRUE,"frp", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"FlightGroup File (*.frp)|*.frp;*.FRP|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	if( ReadFile( csFileName ) )
	{
		// clear
		m_wndListData.DeleteAllItems();			// clear result list_ctrl

		AddFlightToListCtrl();						// add flight schedule to edit

		
		GetDlgItem(IDC_BUTTON_APPEND)->EnableWindow( FALSE );
		GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText( csFileName );
		GetDlgItem(IDC_BUTTON_CONVERT)->EnableWindow( TRUE );

		if( m_bFlightGroupFile)					// if is ARCTerm schedule file formate. auto convert
			OnConvertCompile();
	}
}

void CImportFlightGroupsDlg::SetItemText(CStringArray &strArray)
{
	int nLineNum=m_wndListData.GetItemCount();
	CString strLineNum;
	strLineNum.Format("%d",nLineNum+1);
	int nLineIndex=m_wndListData.InsertItem(nLineNum,strLineNum);
	for(int k=0;k<strArray.GetSize();k++)
	{
		m_wndListData.SetItemText(nLineIndex,k+1,strArray[k]);
	}
}

void CImportFlightGroupsDlg::AddColumn(int nColNum)
{
	m_wndListData.InsertColumn(0,_T("No."),LVCFMT_LEFT,36);
	for(int j=0;j<nColNum;j++)
	{
		CString strColumn;
		if( m_bFlightGroupFile && j< 4 )
			strColumn = columnLabel[j] ;
		else
			strColumn.Format("Col %d",j+1);

		m_wndListData.InsertColumn(j+1,strColumn,LVCFMT_LEFT,50);
	}
}

int CImportFlightGroupsDlg::GetColNumFromStr(CString strText, CString strSep,CStringArray& strArray)
{
	int nColNum=1;
	int nPos=strText.Find(strSep,0);
	while(nPos!=-1)
	{
		nColNum++;
		strArray.Add(strText.Left(nPos));
		strText=strText.Right(strText.GetLength()-nPos-1);
		nPos=strText.Find(strSep,0);
	}
	strArray.Add(strText);
	return nColNum;
}
// read file data to memory
bool CImportFlightGroupsDlg::ReadFile(const CString &strFileName)
{
	m_bFlightGroupFile = FALSE;

	// clear string array 
	m_vFileData.RemoveAll();

	CStdioFile file;
	if(!file.Open( strFileName, CFile::modeRead ) )
	{
		CString strMsg;
		strMsg.Format("Can not open file:\r\n%s", strFileName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
		return false;
	}

	// read all line to file
	CString strLineText;
	while( file.ReadString( strLineText )  )
	{
		m_vFileData.Add( strLineText );
	}

	// if is arcterm file, remove the first three line and the last two line of the file
	if( m_vFileData.GetAt(1) == CString(fileheader) )	// is arcterm file with header
	{
		//get file version
		CString strFileNameAndVersion = m_vFileData.GetAt(0);
		int nPos = strFileNameAndVersion.ReverseFind(',');
		CString strFileVersion = strFileNameAndVersion.Right(strFileNameAndVersion.GetLength() - nPos - 1);
		m_dFileVersion = atof(strFileVersion);

		m_bFlightGroupFile = TRUE;

		// remove first three line
		m_vFileData.RemoveAt( 0,3 );
		// remove last two line
		m_vFileData.RemoveAt( m_vFileData.GetSize()-2,2 );
	}

	file.Close();
	return true;
}

void CImportFlightGroupsDlg::OnBtnAppend()
{
	CWaitCursor wc;
	for( int i=0; i<static_cast<int>(m_vFlightGroups.size()); i++ )
	{
		FlightGroup* pFlightGroup = new FlightGroup(*m_vFlightGroups[i]);
		m_pInterm->m_pAirportDB->getFlightMan()->AddFlightGroup(pFlightGroup);
	}

	CDialog::OnOK();
}

void CImportFlightGroupsDlg::OnBtnReplace()
{
	CWaitCursor wc;
	m_pInterm->m_pAirportDB->getFlightMan()->Clear();
	for( int i=0; i<static_cast<int>(m_vFlightGroups.size()); i++ )
	{
		FlightGroup* pFlightGroup = new FlightGroup(*m_vFlightGroups[i]);
		m_pInterm->m_pAirportDB->getFlightMan()->AddFlightGroup(pFlightGroup);
	}

	CDialog::OnOK();
}

void CImportFlightGroupsDlg::OnBtnConvert()
{
	OnConvertCompile();
	GetDlgItem(IDC_BUTTON_CONVERT)->EnableWindow(FALSE);
}

void CImportFlightGroupsDlg::DeleteAllColumn()
{
	int nColNum=m_wndListData.GetHeaderCtrl()->GetItemCount();
	while(nColNum!=0)
	{
		m_wndListData.DeleteColumn(0);
		nColNum=m_wndListData.GetHeaderCtrl()->GetItemCount();
	}

}

int CImportFlightGroupsDlg::AddFlightToListCtrl()
{
	CString strEditText;
	int	flightNum = m_vFileData.GetSize();
	CStringArray strArray;	

	if( flightNum == 0)
	{
		return flightNum;
	}

	strEditText = m_vFileData.GetAt(0);
	if(strEditText.IsEmpty())
		return 0;

	DeleteAllColumn();
	m_wndListData.DeleteAllItems();

	int nColNum = GetColNumFromStr(strEditText,",",strArray);

	AddColumn(nColNum);

	for (int i = 0; i < flightNum; i++)
	{
		strArray.RemoveAll();
		strEditText = m_vFileData.GetAt(i);
		if (strEditText.IsEmpty())
		{
			break;
		}
		GetColNumFromStr(strEditText,",",strArray);
		SetItemText(strArray);
	}

	return flightNum;
}

void CImportFlightGroupsDlg::PopulateGroupsList()
{
	if(m_vFlightGroups.size() == 0)
		return; //bail if no list

	m_wndListBox.ResetContent();
	int nCurItem = 0;
	for (FLIGHTGROUPLIST::const_iterator iter = m_vFlightGroups.begin();
		iter != m_vFlightGroups.end();
		iter++)
	{
		FlightGroup* pGroups= *iter;
		m_wndListBox.SetItemData(m_wndListBox.InsertString(nCurItem, pGroups->getGroupName()), (DWORD)pGroups);
		nCurItem ++;
	}
	if (m_wndListBox.GetCount()>0)
	{
		m_wndListBox.SetCurSel(0);
		PopulateFlightList(0);
	}
}

void CImportFlightGroupsDlg::PopulateFlightList(int _nItem)
{
	int nItem = _nItem;
	m_wndListBox.SetCurSel(_nItem);
	ASSERT( nItem == _nItem );
	m_wndListGroup.DeleteAllItems();

	if (m_wndListBox.GetCount() != 0)
	{
		FlightGroup* pGroup = reinterpret_cast<FlightGroup*>(m_wndListBox.GetItemData(nItem));
		ASSERT(pGroup != NULL);

		CString strDay = _T("");
		ElapsedTime etDayTime;
		int nIndex = 0;
	
		for (size_t i = 0; i < pGroup->GetFilterCount();i++)
		{
			FlightGroup::CFlightGroupFilter* pFliter = pGroup->GetFilterByIndex(i);

			CString strFilter(_T(""));
			CString strAirline(_T(""));
			CString strID(_T(""));
			CString strDay(_T(""));

			strFilter = pFliter->GetFilter();
			int nPos = strFilter.Find('-');
			strAirline = strFilter.Left(nPos);
			strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);

			nPos = strFilter.Find('-');
			strID = strFilter.Left(nPos);
			strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);;

			strDay = strFilter;

			m_wndListGroup.InsertItem(nIndex,strAirline);
			m_wndListGroup.SetItemText(nIndex,1,strID);
			m_wndListGroup.SetItemText(nIndex,2,strDay);

			nIndex++;	
		}
	}
}

void CImportFlightGroupsDlg::OnLbnSelchangeListGroups()
{
	// TODO: Add your control notification handler code here
	int nCurIndex = m_wndListBox.GetCurSel();
	m_wndListGroup.DeleteAllItems();
	FlightGroup* pGroups = reinterpret_cast<FlightGroup*>(m_wndListBox.GetItemData(nCurIndex));
	PopulateFlightList(nCurIndex);
}

void CImportFlightGroupsDlg::OnConvertCompile()
{
	std::vector<CString> vGroupName;
	m_vFlightGroups.clear();

	CString m_strResultInfo(_T(""));
	int iItemCount = m_wndListData.GetItemCount();
	CArray<int,int> arrayDeletedItem;
	m_strResultInfo+=CString("-------------------------------------Convert Result---------------------------------------\r\n");
	CString strTemp;
	strTemp.Format("Convert(%d Lines)...\r\n",iItemCount);
	m_strResultInfo+=strTemp;

	int iSuccess = 0;
	for (int i = 0; i < m_wndListData.GetItemCount(); i++)
	{
		FlightGroup* pFlightGroup = NULL;
		CString strGroupName(_T(""));
		strGroupName = m_wndListData.GetItemText(i,1);
		std::vector<CString>::iterator iter = std::find(vGroupName.begin(),vGroupName.end(),strGroupName);
		if (iter == vGroupName.end())
		{
			pFlightGroup = new FlightGroup;
			m_vFlightGroups.push_back(pFlightGroup);
			vGroupName.push_back(strGroupName);
		}
		else
		{
			pFlightGroup = m_vFlightGroups.at(iter - vGroupName.begin());
		}
		 
		pFlightGroup->setGroupName(strGroupName);
		
		CString strFilter(_T(""));
		CString strAirline(_T(""));
		CString strID(_T(""));
		CString strDay(_T(""));
		strAirline = m_wndListData.GetItemText(i,2);
		strFilter = strAirline;
		strFilter += "-";
		strID = m_wndListData.GetItemText(i,3);
		strFilter += strID;
		strFilter += "-";
		strDay = m_wndListData.GetItemText(i,4);
		strFilter += strDay;

		if (IfFlightExistInFlightSchedule(strAirline,strID,strDay))
		{
			FlightGroup::CFlightGroupFilter* pFilter = new FlightGroup::CFlightGroupFilter;
			pFilter->SetFilter(strFilter);
			pFlightGroup->AddFilter(pFilter);

			iSuccess++;
			strTemp.Format("Line %d: Succeed\r\n",i+1);
		}
		else
		{
			CString strAircraft(_T(""));
			strAircraft.Format(_T("Flight %s%s of day%s does not exist in flight schedule"),strAirline,strID,strDay);
			strTemp.Format("Line %d: Failed - %s\r\n",i+1,strAircraft);
		}
		m_strResultInfo+=strTemp;
	}

	strTemp.Format( "--------------------------------------Total Lines:%d    Succeed:%d    Failed:%d-----------------------------------------\r\n\r\n\r\n" ,iItemCount, iSuccess, iItemCount-iSuccess);
	m_strResultInfo+=strTemp;
	m_editResultInfo.SetWindowText(m_strResultInfo);
	int nLineCount=m_editResultInfo.GetLineCount();
	m_editResultInfo.LineScroll(nLineCount);

	PopulateGroupsList();
	GetDlgItem(IDC_BUTTON_REPLACE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_APPEND)->EnableWindow(TRUE);
}

bool CImportFlightGroupsDlg::IfFlightExistInFlightSchedule(const CString& strAirline,const CString& strID,const CString& strDay)
{
	ASSERT(m_pInterm);
	char buff[128];
	memset(buff,0,128*sizeof(char));

	CString sAirline(_T(""));
	CString sID(_T(""));
	CString sDay(_T(""));

	for (int i = 0; i < m_pInterm->flightSchedule->getCount(); i++)
	{
		Flight* pFlight = m_pInterm->flightSchedule->getItem(i);
		pFlight->getAirline(buff);
		sAirline.Format(_T("%s"),buff);

		if (sAirline == strAirline)
		{

			if (pFlight->isDeparting())
			{
				sID.Format("%s",pFlight->getDepID().GetValue());

				ElapsedTime etDayTime = pFlight->getDepTime();
				sDay.Format("%d", etDayTime.GetDay());

				if (sID == strID && sDay == strDay)
				{
					return true;
				}
			}

			if (pFlight->isArriving())
			{
				sID.Format("%s",pFlight->getArrID().GetValue());

				ElapsedTime etDayTime = pFlight->getArrTime();
				sDay.Format("%d", etDayTime.GetDay());

				if (sID == strID && sDay == strDay)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void CImportFlightGroupsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	
	if (m_wndListData.GetSafeHwnd())
	{
		CRect rectWnd;
		int nSplitterWidth=9;
		int nLeft=nSplitterWidth;

		CRect leftRect;
		GetDlgItem(IDC_STATIC_PATH)->GetWindowRect(&leftRect);
		ScreenToClient(&leftRect);
		CRect rectOpen;
		GetDlgItem(IDC_BUTTON_OPEN)->GetWindowRect(&rectOpen);
		ScreenToClient(&rectOpen);

		GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowPos(NULL,leftRect.right + 2,nSplitterWidth,cx - 2*nSplitterWidth - rectOpen.Width() - leftRect.Width() -2,rectOpen.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		GetDlgItem(IDC_EDIT_FILEPATH)->GetWindowRect(&leftRect);
		ScreenToClient(&leftRect);

		GetDlgItem(IDC_BUTTON_OPEN)->SetWindowPos(NULL,leftRect.right,leftRect.top,rectOpen.Width(),rectOpen.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		
		GetDlgItem(IDC_STATIC_SOURCE)->SetWindowPos(NULL,nSplitterWidth,leftRect.bottom + 10,cx/2 - nSplitterWidth,cy/2,SWP_NOACTIVATE|SWP_NOZORDER);
		GetDlgItem(IDC_STATIC_SOURCE)->GetWindowRect(&leftRect);
		ScreenToClient(&leftRect);

		m_wndListData.SetWindowPos(NULL,leftRect.left,leftRect.top + 30,cx/2 - nSplitterWidth,cy/2 - 30,SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndListData.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		CRect rectBar;
		rectBar = rectWnd;
		rectBar.top-=22;
		rectBar.bottom=rectBar.top+22;
		rectBar.left += 2;
		rectBar.right = 75;
		m_wndToolbar.MoveWindow(&rectBar);

		CRect rectFor;
		GetDlgItem(IDC_STATIC_FORMAT)->GetWindowRect(rectFor);
		GetDlgItem(IDC_STATIC_FORMAT)->SetWindowPos(NULL,rectBar.left + 70,rectBar.bottom - 18,rectFor.Width(),rectFor.Height(),SWP_NOACTIVATE|SWP_NOZORDER);

		//Splitter1
		if(!m_wndSplitter1.m_hWnd)//Create Splitter1
		{
			m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTERCONTROL1);
			m_wndSplitter1.SetStyle(SPS_VERTICAL);
		}

		m_wndSplitter1.SetWindowPos(NULL,rectWnd.right,leftRect.top,4,leftRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndSplitter1.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		m_wndSplitter1.SetRange(leftRect.left,cx - 2 * nSplitterWidth);
		
		GetDlgItem(IDC_STATIC_DEST)->SetWindowPos(NULL,rectWnd.right + 5,leftRect.top,cx/2 - nSplitterWidth - 5,cy/2,SWP_NOACTIVATE|SWP_NOZORDER);
		GetDlgItem(IDC_STATIC_DEST)->GetWindowRect(&rectWnd);
		ScreenToClient(&rectWnd);

		GetDlgItem(IDC_STATIC_RESULT)->GetWindowRect(rectFor);
		GetDlgItem(IDC_STATIC_RESULT)->SetWindowPos(NULL,rectWnd.left + 2,rectWnd.top + 12 ,rectWnd.Width(),rectFor.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		
		m_wndListBox.SetWindowPos(NULL,rectWnd.left,leftRect.top + 30,rectWnd.Width()/3,cy/2 - 30,SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndListBox.GetWindowRect(&leftRect);
		ScreenToClient(&leftRect);

		//Splitter1
		if(!m_wndSplitter2.m_hWnd)//Create Splitter1
		{
			m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTERCONTROL2);
			m_wndSplitter2.SetStyle(SPS_VERTICAL);
		}

		m_wndSplitter2.SetWindowPos(NULL,leftRect.right,leftRect.top,4,leftRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndSplitter2.SetRange(leftRect.left,leftRect.left + rectWnd.Width());

		m_wndListGroup.SetWindowPos(NULL,leftRect.right + 2,leftRect.top,leftRect.Width()*2 - 2,leftRect.Height(),SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndListGroup.GetWindowRect(&rectWnd);
		ScreenToClient(&rectWnd);
	
		GetDlgItem(IDC_STATIC_CONVERSION)->GetWindowRect(&leftRect);
		ScreenToClient(&leftRect);

		GetDlgItem(IDC_STATIC_CONVERSION)->SetWindowPos(NULL,nSplitterWidth,rectWnd.bottom + 5,leftRect.Width(),leftRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		GetDlgItem(IDC_EDIT_ERROR)->SetWindowPos(NULL,nSplitterWidth,rectWnd.bottom + 20,cx - 2*nSplitterWidth,cy/2 - 100,SWP_NOACTIVATE|SWP_NOZORDER);
		GetDlgItem(IDC_EDIT_ERROR)->GetWindowRect(&rectWnd);
		ScreenToClient(&rectWnd);

		CRect rcBtn;
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rectWnd.right - rcBtn.Width() - 2,rectWnd.bottom + 10,rcBtn.Width(),rcBtn.Height(),NULL);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		GetDlgItem(IDC_BUTTON_REPLACE)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() - 6,rectWnd.bottom + 10,rcBtn.Width(),rcBtn.Height(), NULL);
		GetDlgItem(IDC_BUTTON_REPLACE)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		CRect rcSave;
		GetDlgItem(IDC_BUTTON_APPEND)->GetWindowRect(&rcSave);
		GetDlgItem(IDC_BUTTON_APPEND)->SetWindowPos(NULL,rcBtn.left - rcSave.Width() - 10,rectWnd.bottom+10,rcSave.Width(),rcSave.Height(),NULL);

		GetDlgItem(IDC_BUTTON_CONVERT)->SetWindowPos(NULL,nSplitterWidth,rectWnd.bottom+10,rcSave.Width(),rcSave.Height(),NULL);
	}
}

LRESULT CImportFlightGroupsDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	
	if(message == WM_NOTIFY)
	{
		SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
		switch(wParam)
		{
		case ID_WND_SPLITTERCONTROL1:
		case ID_WND_SPLITTERCONTROL2:
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize(pHdr->delta,wParam);
			}
			break;
		}		
	}

	return CDialog::DefWindowProc(message,wParam,lParam);
}

void CImportFlightGroupsDlg::DoResize(int delta,UINT nIDSplitter)
{
	CRect rectWnd;
	if(nIDSplitter == ID_WND_SPLITTERCONTROL1)
	{
		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_SOURCE), delta,CW_LEFTALIGN);
		CSplitterControl::ChangeWidth(&m_wndListData,delta,CW_LEFTALIGN);

		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_DEST),-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_RESULT),-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangeWidth(&m_wndListBox,-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangeWidth(&m_wndListBox,delta/2,CW_LEFTALIGN);
		CRect leftRect;
		m_wndListBox.GetWindowRect(&leftRect);
		ScreenToClient(&leftRect);
		m_wndSplitter2.SetWindowPos(NULL,leftRect.right,leftRect.top,4,leftRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		CSplitterControl::ChangeWidth(&m_wndListGroup,-delta/2,CW_RIGHTALIGN);
	}
	else if(nIDSplitter == ID_WND_SPLITTERCONTROL2)
	{
		CSplitterControl::ChangeWidth(&m_wndListBox, delta,CW_LEFTALIGN);
		CSplitterControl::ChangeWidth(&m_wndListGroup,-delta,CW_RIGHTALIGN);
	}
	Invalidate(FALSE);
}

void CImportFlightGroupsDlg::OnPaint()
{
	CPaintDC dc(this);

	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))||
			pWnd->IsKindOf(RUNTIME_CLASS(CListBox)))
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
}

BOOL CImportFlightGroupsDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CImportFlightGroupsDlg::OnButtonTitle() 
{
	// set current select row as column title
	// and remove the row
	POSITION pos = m_wndListData.GetFirstSelectedItemPosition();
	if( pos!= NULL )
	{
		int iItemIdx = m_wndListData.GetNextSelectedItem( pos );
		// get item text and remove the row
		std::vector<CString> vItemText;
		for( int i=0; i<m_wndListData.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			CString itemText = m_wndListData.GetItemText( iItemIdx, i );
			if( itemText.IsEmpty() )
				itemText.Format("Col %d", i);

			vItemText.push_back( itemText );
		}

		m_wndListData.DeleteItem( iItemIdx );

		// set the colum title
		HDITEM hd;
		for( i=0; i<m_wndListData.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			m_wndListData.GetHeaderCtrl()->GetItem( i,&hd);
			hd.mask |= HDI_TEXT;
			hd.pszText = vItemText[i].GetBuffer(0);
			hd.cchTextMax = 128;
			m_wndListData.GetHeaderCtrl()->SetItem( i, &hd );
		}
		GetDlgItem(IDC_BUTTON_CONVERT)->EnableWindow(TRUE);
	}

}

void CImportFlightGroupsDlg::OnDeleteRow()
{

	POSITION pos=m_wndListData.GetFirstSelectedItemPosition();
	while (pos) 
	{
		int nIndex=m_wndListData.GetNextSelectedItem(pos);
		m_wndListData.DeleteItem(nIndex);
		break;
	}
}

void CImportFlightGroupsDlg::OnDeleteColumn()
{
	char arBuf[256];
	LVCOLUMN lvCol;
	lvCol.mask=LVCF_TEXT;
	lvCol.pszText=arBuf;
	lvCol.cchTextMax=256;
	m_wndListData.GetColumn(m_nColIndexClicked,&lvCol);
	CString strColumn=lvCol.pszText;

	m_wndListData.DeleteColumn(m_nColIndexClicked);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
}

LRESULT CImportFlightGroupsDlg::OnHeaderCtrlClickColumn(WPARAM wParam,LPARAM lParam)
{
	m_nColIndexClicked=wParam;
	if(wParam!=-1 && wParam!=0 )	// can not delete the first column
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,TRUE);
	else
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
	return TRUE;
}
