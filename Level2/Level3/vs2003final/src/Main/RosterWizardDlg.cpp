// RosterWizardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "RosterWizardDlg.h"
#include <assert.h>
#include "inputs\in_term.h"
#include "inputs\UserName.h"
#include "engine\proclist.h"
#include "inputs\assign.h"
#include "inputs\assigndb.h"
#include "TermPlanDoc.h"
#include "DlgMEDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRosterWizardDlg dialog


CRosterWizardDlg::CRosterWizardDlg(InputTerminal* _pTerm,CTermPlanDoc* pDoc, const CString& _strPath,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CRosterWizardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRosterWizardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pInTerm = _pTerm;
	m_strProjPath = _strPath;
	m_pDragWnd=NULL;
	m_pDragImage=NULL;
	m_bHaveAssignTitle = false;
	m_pTermDoc = pDoc;
	
	for( int i=0; i<ROSTER_COLUMN_NUM; i++ )
		m_bColumnDropped[i] = false;

}


void CRosterWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRosterWizardDlg)
	DDX_Control(pDX, IDC_BUTTON_TITLE, m_butTitle);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolbar);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_staticResult);
	DDX_Control(pDX, IDC_LIST_ROSTERTEXT, m_listText);
	DDX_Control(pDX, IDC_LIST_RESULT_SCHEDULE, m_listResult);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFileName);
	DDX_Control(pDX, IDC_BUTTON_OPENFILE, m_butFile);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_butClear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRosterWizardDlg, CXTResizeDialog)
	//{{AFX_MSG_MAP(CRosterWizardDlg)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, OnButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_CREATEME, OnCreateMeList)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT_SCHEDULE, OnItemchangedListResultSchedule)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULT_SCHEDULE, OnDblclkListResultSchedule)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_BN_CLICKED(IDC_BUTTON_TITLE, OnButtonTitle)
	ON_COMMAND(ID_NAMELIST_TITLE,OnButtonTitle)
	ON_COMMAND(ID_DELETE_ROW,OnDeleteRow)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROSTERTEXT, OnItemchangedListRostertext)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BEGIN_DRAG,OnBegindrag)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRosterWizardDlg message handlers

BOOL CRosterWizardDlg::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	
	m_hcArrow	= AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcNo		= AfxGetApp()->LoadStandardCursor(IDC_NO);

	InitListCtrl();

	// init tool bar
	CRect mRect;
	m_staticToolbar.GetWindowRect( &mRect );
	ScreenToClient( &mRect );
	m_ToolBar.MoveWindow( &mRect );
	m_ToolBar.ShowWindow( SW_SHOW );

	CRect rect;
	m_listText.GetWindowRect(rect);
	ScreenToClient(&rect);
	rect.top=mRect.top;
	rect.bottom=mRect.bottom;
	m_wndToolBarLeft.MoveWindow(&rect);
	m_wndToolBarLeft.GetToolBarCtrl().HideButton(ID_DELETE_COLUMN);
	m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
	m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
	
	m_staticToolbar.ShowWindow( SW_HIDE );

	GetDlgItem(IDC_STATIC_FORMAT)->SetWindowText(_T("Time: hh:mm:ss, Assignment relation: Inclusive/Exclusive/Not, Daily: Y/N"));


	SetResize(IDC_STATIC_TEXT,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_EDIT_FILENAME,SZ_TOP_LEFT,SZ_TOP_CENTER);
	SetResize(m_wndToolBarLeft.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_CENTER);
	SetResize(IDC_LIST_ROSTERTEXT,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_RESULT,SZ_TOP_CENTER,SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_FORMAT,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_RESULT_SCHEDULE,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_OPENFILE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_CREATEME,SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
	SetResize(IDC_BUTTON_CLEAR,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CRosterWizardDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),IDR_PEOPLEMOVEBAR) ||
		!m_ToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here
	if (!m_wndToolBarLeft.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),IDR_NAMELIST_TEXT) ||
		!m_wndToolBarLeft.LoadToolBar(IDR_NAMELIST_TEXT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here
	
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, FALSE );
	return 0;
}

void CRosterWizardDlg::InitListCtrl()
{
	m_listText.SubClassHeaderCtrl();
	m_listResult.SubClassHeaderCtrl();
	
	DWORD dwStyle = m_listText.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listText.SetExtendedStyle( dwStyle );
	m_listText.InsertColumn( 0, "NO.", LVCFMT_CENTER, 100 );
	
	// result list_ctrl
	m_listResult.SetExtendedStyle( dwStyle );
	char* columnLabel[] = 
	{
		"Processor",
			"Open Time",
			"Close Time",
			"Duration Time",
		"Airline",
		"Flight ID",
		"Assignment Relation",
		"IsDaily",
		"Pax Type"
	};
	int DefaultColumnWidth[] = {100, 70,70,85,50,60,120,50,80 };
	// column
	for( int i=0; i<9; i++ )
	{
		m_listResult.InsertColumn( i, columnLabel[i], LVCFMT_CENTER, DefaultColumnWidth[i] );
	}
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CRosterWizardDlg::ReadTextFromFile(const CString &_szFileName)
{
	CStdioFile file;
	if( !file.Open( _szFileName, CFile::modeRead) )
	{
		CString strMsg;
		strMsg.Format("Can not open the file!\r\n%s", _szFileName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
		return false;
	}
	
	// clear old data from text list
	deleteAllColumn( &m_listText );
	m_listText.DeleteAllItems();
	m_listText.InsertColumn(m_listText.GetHeaderCtrl()->GetItemCount(),"NO.",LVCFMT_LEFT,36);

	CString strLine;
	CStringArray strItemText;
	while( file.ReadString(strLine) )
	{
		if( strLine.IsEmpty() )
			continue;
		int iColumnNum = getColNumFromStr( strLine, ",", strItemText );
		addColumn( &m_listText, iColumnNum - m_listText.GetHeaderCtrl()->GetItemCount() + 1);
		setItemText( strItemText );
	}
	
	return true;
}

void CRosterWizardDlg::deleteAllColumn(CListCtrl *_pListCtrl)
{
	assert( _pListCtrl );
	
	int nColNum=_pListCtrl->GetHeaderCtrl()->GetItemCount();
	while(nColNum!=0)
	{
		_pListCtrl->DeleteColumn(0);
		nColNum=_pListCtrl->GetHeaderCtrl()->GetItemCount();
	}
}

int CRosterWizardDlg::getColNumFromStr(CString strText, CString strSep, CStringArray &strArray)
{	
	strArray.RemoveAll();
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

void CRosterWizardDlg::addColumn(CListCtrl* _pListCtrl, int nColNum)
{
	assert( _pListCtrl );

	CString strColumn;
	int iColumnCount = _pListCtrl->GetHeaderCtrl()->GetItemCount();
	for(int j=0;j<nColNum;j++)
	{
		strColumn.Format("Col %d",iColumnCount + j);
		 _pListCtrl->InsertColumn(_pListCtrl->GetHeaderCtrl()->GetItemCount(),strColumn,LVCFMT_CENTER,50);
	}
}

void CRosterWizardDlg::setItemText(const CStringArray &_strArray)
{
	CString strLineNum;
	int nLineNum = m_listText.GetItemCount();
	strLineNum.Format("%d",nLineNum+1);
	int nLineIndex = m_listText.InsertItem(nLineNum,strLineNum);
	for(int k=0;k<_strArray.GetSize();k++)
	{
		m_listText.SetItemText(nLineIndex,k+1,_strArray[k]);
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void CRosterWizardDlg::OnButtonOpenfile() 
{
	CFileDialog filedlg( TRUE,"csv", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"CSV File (*.csv)|*.csv;*.CSV|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	if( ReadTextFromFile( csFileName ) )
	{
		OnButtonClear();
		m_editFileName.SetWindowText( csFileName );
		m_butOK.EnableWindow( FALSE );
		
		m_butTitle.EnableWindow( FALSE );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
		m_bHaveAssignTitle = false;
	}
}


void CRosterWizardDlg::OnButtonClear() 
{
	m_listResult.DeleteAllItems();	
	for( int i=0; i<ROSTER_COLUMN_NUM; i++ )
		m_bColumnDropped[i] = false;
}

void CRosterWizardDlg::OnOK() 
{
	// first remove old data
	for( int i=0; i<m_listResult.GetItemCount(); i++ )
	{
		CString strProcName = m_listResult.GetItemText( i, 0 );
		strProcName.MakeUpper();
		if( strProcName.IsEmpty() )
			continue;
		
		ProcessorID procID;
		procID.SetStrDict( m_pInTerm->inStrDict );
		procID.setID( strProcName );
		GroupIndex group = m_pInTerm->procList->getGroupIndex( procID );
		if( group.start == -1 && group.end == -1 )
			continue;

		int nIdx = m_pInTerm->procAssignDB->findEntry( procID );
		if( nIdx!= INT_MAX )
			m_pInTerm->procAssignDB->deleteItem( nIdx );
	}

	// then add new data
	for( int k=0; k<m_listResult.GetItemCount(); k++ )
	{
		if( !createAssignEvent( k ) )
			return;
	}
	
	CXTResizeDialog::OnOK();
}


bool CRosterWizardDlg::createAssignEvent( int _indx )
{
	CString strMsg;
	//0. proceName 
	CString strProcName = m_listResult.GetItemText( _indx, 0 );
	strProcName.MakeUpper();
	if( strProcName.IsEmpty() )
		return true;
	
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	procID.setID( strProcName );
	GroupIndex group = m_pInTerm->procList->getGroupIndex( procID );
	if( group.start == -1 && group.end == -1 )
	{
		strMsg.Format("Row %d\r\nThe processor name:%s is invalid", _indx+1, strProcName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
		return false;
	}

	//1. open time
	CString strTime = m_listResult.GetItemText( _indx, 1 );
	ElapsedTime openTime;
	if( !getTimeFromString( strTime, openTime ) )
	{
		if (strTime.IsEmpty())
		{
			strMsg.Format(_T("Row %d\r\n Need open time"),_indx+1);
		}
		else
		{
			strMsg.Format("Row %d\r\nThe string:%s is invalid time,time format should like this: hh:mm:ss",_indx+1, strTime );
		}
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
		return false;
	}

	//2.close time
	//3.duration time
	ElapsedTime closeTime, durationTime;
	CString strClose;
	if( m_bColumnDropped[2] )
	{
		strClose = m_listResult.GetItemText( _indx,2 );
		if( !strClose.IsEmpty() && !getTimeFromString( strClose, closeTime))
		{
			strMsg.Format("Row %d\r\nThe string:%s is invalid time, time format should like this: hh:mm:ss", _indx+1, strClose );
			MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
			return false;
		}
	}

	CString strDuration;
   if( m_bColumnDropped[3] && strClose.IsEmpty())
	{
		strDuration = m_listResult.GetItemText( _indx,3 );
		if( !strDuration.IsEmpty() && !getTimeFromString( strDuration, durationTime ))
		{
			strMsg.Format("Row %d\r\nThe string:%s is invalid time,time format should like this: hh:mm:ss", _indx+1, strDuration );
			MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
			return false;
		}
		closeTime = openTime + durationTime;
	}
	
	if (strClose.IsEmpty() && strDuration.IsEmpty())
	{
		strMsg.Format("Row %d\r\nNeed duration or close time,time format should like this: hh:mm:ss", _indx+1 );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
		return false;
	}

	if( closeTime<= openTime )
	{
		strMsg.Format("Row %d:\r\nThe close time must more than open time.\r\n", _indx+1 );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
		return false;
	}
	
	CMobileElemConstraint mob(m_pInTerm);
	//mob.SetInputTerminal( m_pInTerm );

	int nIdxCon = (int)m_listResult.GetItemData(_indx);
	if (nIdxCon != -1)
	{
		mob =  *(m_pTermDoc->GetCreateMeList().getConstraint(nIdxCon));
	}
	else
	{
		//4. airline
		CString strAirLine = m_listResult.GetItemText( _indx, 4);
		strAirLine.MakeUpper(); strAirLine.TrimLeft(), strAirLine.TrimRight();
		if( !strAirLine.IsEmpty() && strAirLine.CompareNoCase("default") != 0 )
		{
			if( strAirLine.GetLength() >= AIRLINE_LEN )
			{
				strMsg.Format("Airline: %s\r\n is too length( more than %d )",strAirLine, AIRLINE_LEN );
				MessageBox( strMsg, "ERROR", MB_OK | MB_ICONINFORMATION );
				return false;
			}
			mob.setAirline( strAirLine.Left( AIRLINE_LEN) );
		}
		//5. flight id
		CString strFlightID = m_listResult.GetItemText(_indx,5);
		strFlightID.TrimLeft();
		strFlightID.TrimRight();
		if (!strFlightID.IsEmpty())
		{
			if (strFlightID.GetLength() >= FLIGHT_LEN)
			{
				strMsg.Format("Flight ID: %s\r\n is too length( more than %d )",strFlightID, FLIGHT_LEN );
				MessageBox( strMsg, "ERROR", MB_OK | MB_ICONINFORMATION );
				return false;
			}
			mob.setFlightID(strFlightID.Left(FLIGHT_LEN));
		}
	}
	

	CString strAssign = m_listResult.GetItemText( _indx, 6);
	strAssign.TrimLeft();
	strAssign.TrimRight();
	strAssign.MakeUpper();
	ProcAssRelation emRelate = Inclusive;
	if (!strAssign.IsEmpty())
	{
		if (strAssign.Compare("INCLUSIVE") == 0)
		{
			emRelate = Inclusive;
		}
		else if (strAssign.Compare("EXCLUSIVE") == 0)
		{
			emRelate = Exclusive;
		}
		else if (strAssign.Compare("NOT") == 0)
		{
			emRelate = Not;
		}
		else 
		{
			strMsg.Format("Assignment Relation: %s\r\n format should like: Inclusive, Exclusive or Not(ignoring case)",strAssign );
			MessageBox( strMsg, "ERROR", MB_OK | MB_ICONINFORMATION );
			return false;
		}
	}

	CString strDaily = m_listResult.GetItemText( _indx, 7);
	strDaily.TrimLeft();
	strDaily.TrimRight();
	strDaily.MakeUpper();
	BOOL bDaily = TRUE;
	if (!strDaily.IsEmpty())
	{
		if (strDaily.Compare("N") == 0)
		{
			bDaily = FALSE;
		}
		else if (strDaily.Compare("Y") == 0)
		{
			bDaily = TRUE;
		}
		else 
		{
			strMsg.Format("IsDaily: %s\r\n format should like: Y or N(ignoring case)",strDaily );
			MessageBox( strMsg, "ERROR", MB_OK | MB_ICONINFORMATION );
			return false;
		}
	}

	if(bDaily)//daily item, check the open and close time, if exceed 24 hour, there would be an  error
	{
		if (openTime.GetDay() > 1)
		{
			strMsg.Format("Row %d\r\nIt is a DAILY roster, Open Time should be less than 24 hours.", _indx+1);
			MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
			return false;
		}
		else if (closeTime.GetDay() > 1)
		{
			strMsg.Format("Row %d\r\nIt is a DAILY roster, Close Time should be less than 24 hours.", _indx+1);
			MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
			return false;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	// create assign event
	ProcessorRoster* pProcAssn = new ProcessorRoster( m_pInTerm );
	pProcAssn->setFlag(1 );
	pProcAssn->setOpenTime( openTime );
	pProcAssn->setCloseTime( closeTime );
	CMultiMobConstraint multiMob;
	multiMob.addConstraint( mob );
	pProcAssn->setAssignment(multiMob );
	pProcAssn->setAssRelation(emRelate);
	pProcAssn->IsDaily(bDaily);

	// save to assignment dataset
	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict );
	id.setID( strProcName );
	
	int nIdx = m_pInTerm->procAssignDB->findEntry( id );
	if( nIdx == INT_MAX )
	{
		m_pInTerm->procAssignDB->addEntry( id );
		nIdx = m_pInTerm->procAssignDB->findEntry( id );
		assert( nIdx != INT_MAX  );
	}
	ProcessorRosterSchedule* pSchd = m_pInTerm->procAssignDB->getDatabase( nIdx );
	UnmergedAssignments m_unMergedAssgn;
	m_unMergedAssgn.split( pSchd );
	m_unMergedAssgn.addItem( pProcAssn );
	m_unMergedAssgn.merge( pSchd );
	
	return true;
}

bool CRosterWizardDlg::getTimeFromString( const CString& _strItem, ElapsedTime& _time )
{
	CString strTime = _strItem;
	strTime.Remove(' ');
	
	// get hours
	int pos = strTime.Find(':');
	if( pos == -1 )
		return false;
	CString strHour = strTime.Left(pos);
	for( int i=0; i<strHour.GetLength(); i++ )
		if( !isNumeric(strHour[i]) )
			return false;
	int ihours = atoi(strHour);
	if( ihours<0 /*|| ihours>24*/ )
		return false;

	// get minutes
	CString strMin = strTime.Right( strTime.GetLength() - pos - 1 );
	pos = strMin.Find(':');
	if( pos == -1 )
		return false;
	strMin = strMin.Left(pos);
	CString strSec = strMin.Right(strMin.GetLength() - pos - 1);
	for( i=0; i<strMin.GetLength(); i++ )
		if( !isNumeric( strMin[i] ) )
			return false;
	int imin = atoi(strMin);
	if( imin<0 || imin>59 )
		return false;
	
	for( i=0; i<strSec.GetLength(); i++ )
		if( !isNumeric( strSec[i] ) )
			return false;
	int isec = atoi(strSec);
	if (isec < 0 || isec >59)
		return false;
	
	_time.set( atoi( strHour), atoi( strMin), atoi(strSec) );
	return true;
}


void CRosterWizardDlg::OnCancel() 
{
	//m_pInTerm->procAssignDB->loadDataSet( m_strProjPath );
	
	CXTResizeDialog::OnCancel();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// drag & drop
void CRosterWizardDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_pDragImage && m_pDragWnd) // In Drag&Drop mode ?
	{
		CPoint ptDropPoint(point);
		ClientToScreen(&ptDropPoint);
		m_pDragImage->DragMove(ptDropPoint);
		CWnd* pDropWnd = CWnd::WindowFromPoint(ptDropPoint);
		HCURSOR hCursor;
		if( pDropWnd == m_listResult.GetHeaderCtrl()||pDropWnd == &m_listResult)
		{
			int nColNum=m_listResult.GetHeaderCtrl()->GetItemCount();
			m_listResult.GetHeaderCtrl()->ScreenToClient(&ptDropPoint);
			int nColIndexDrop=m_listResult.GetColIndexUnderPoint(ptDropPoint,nColNum);
			if(nColIndexDrop<m_listResult.GetHeaderCtrl()->GetItemCount()&&m_bColumnDropped[nColIndexDrop]==FALSE)
				hCursor=m_hcArrow;
			else
				hCursor=m_hcNo;
		}
		else
			hCursor=m_hcNo;
		SetCursor(hCursor);
	}
	CXTResizeDialog::OnMouseMove(nFlags, point);
}

LRESULT CRosterWizardDlg::OnBegindrag(WPARAM wParam,LPARAM lParam)
{
	int nColIndexDragged=wParam;
	CListCtrl* pList=(CListCtrl*)lParam;
	if((pList == &m_listText) && nColIndexDragged==0 )
	{
		m_listText.EndDrag();
		return TRUE;
	}
	else if(pList==&m_listResult)
	{
		if(m_bColumnDropped[nColIndexDragged]==FALSE)	
		{
			m_listResult.EndDrag();
			return TRUE;
		}
	}
	
	POINT pt;
	m_pDragImage = CreateDragImageEx(nColIndexDragged,pList, &pt);
	
	if (m_pDragImage == NULL)
		return TRUE;
	
	m_pDragWnd = pList;
	m_pDragImage->BeginDrag(0, CPoint(0,0));
	CPoint ptPoint;
	GetCursorPos(&ptPoint);
	m_pDragImage->DragEnter(GetDesktopWindow(), ptPoint);
	SetCapture();
	return TRUE;
}

void CRosterWizardDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CString procName, strTemp;
	if (m_pDragImage && m_pDragWnd) // In Drag&Drop mode ?
	{
		CWaitCursor wc;
		::ReleaseCapture();
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();
		
		CPoint pt(point); 
		ClientToScreen(&pt);
		CWnd* m_pDropWnd = WindowFromPoint(pt);
		
		m_pDragImage->DeleteImageList();
		delete m_pDragImage;
		m_pDragImage = NULL;
		if(m_pDragWnd!=m_pDropWnd&&m_pDropWnd->GetParent()!=m_pDragWnd)
		{
			//Move Dragged Column Data To Target Column
			if((m_pDropWnd==&m_listResult||m_pDropWnd==m_listResult.GetHeaderCtrl()))
			{
				
				int nColNum=m_listResult.GetHeaderCtrl()->GetItemCount();
				int nWidthColTotal=0;
				CPoint ptCol=pt;
				m_listResult.GetHeaderCtrl()->ScreenToClient(&ptCol);
				int nColIndexDrop=m_listResult.GetColIndexUnderPoint(ptCol,nColNum);

				if(m_bColumnDropped[nColIndexDrop]==FALSE&&nColIndexDrop<m_listResult.GetHeaderCtrl()->GetItemCount())
				{
					if( nColIndexDrop ==0 )	// is drop to processor
					{
						// get column title
						HDITEM hd;
						char szBuf[256];
						hd.mask		   = HDI_TEXT;
						hd.cchTextMax  = 256;
						hd.pszText = szBuf;
						m_listText.GetHeaderCtrl()->GetItem( m_listText.m_nColIndexDragged, &hd );
						CString strTitle = CString(szBuf );
						strTitle.TrimLeft();
						strTitle.TrimRight();
						strTitle.MakeUpper();
						
						// switch user name to our model names
						for(int j=0;j<m_listText.GetItemCount();j++)
						{
							if( m_listResult.GetItemCount()!=m_listText.GetItemCount() )
							{
								m_listResult.InsertItem(j,"");
								m_listResult.SetItemData(j,(DWORD)-1);
							}


							CString strItem = m_listText.GetItemText(j,m_listText.m_nColIndexDragged);
							strItem.TrimLeft();
							strItem.TrimRight();
							strItem.MakeUpper();
							if( strItem.IsEmpty() )
								continue;
							
							strTemp = strItem;
							if( isNumString(strItem ))
								strTemp = strTitle + "-" + strItem ;
							
							if( !isProcessor( strItem) && m_pInTerm->m_pUserNameListDB->findProcNameByUserName( strTemp, procName ) )
							{
								m_listResult.SetItemText( j, nColIndexDrop, procName );
							}
							else
							{
								m_listResult.SetItemText( j, nColIndexDrop, strItem );
							}
						}	
					}
					else
					{
						for(int j=0;j<m_listText.GetItemCount();j++)
						{
							if(m_listResult.GetItemCount()!=m_listText.GetItemCount())
							{
								m_listResult.InsertItem(j,"");
								m_listResult.SetItemData(j,(DWORD)-1);
							}
							m_listResult.SetItemText(j,nColIndexDrop,m_listText.GetItemText(j,m_listText.m_nColIndexDragged));
						}
					}

					m_listText.DeleteColumn(m_listText.m_nColIndexDragged);
					m_bColumnDropped[nColIndexDrop]=true;
				}
			}
			else if((m_pDropWnd==&m_listText||m_pDropWnd==m_listText.GetHeaderCtrl()))
			{		
				int nColNumSourceList=m_listText.GetHeaderCtrl()->GetItemCount();
				CString strColumn;
				LVCOLUMN lvcol;
				lvcol.mask=LVCF_TEXT ;
				char strBuf[256];
				lvcol.pszText=strBuf;
				lvcol.cchTextMax=sizeof(strBuf);
				m_listResult.GetColumn(m_listResult.m_nColIndexDragged,&lvcol);
				strColumn=lvcol.pszText;
				m_listText.InsertColumn(nColNumSourceList,strColumn,LVCFMT_CENTER,50);
				for(int j=0;j<m_listResult.GetItemCount();j++)
				{
					CString strItem=m_listResult.GetItemText(j,m_listResult.m_nColIndexDragged);
					m_listText.SetItemText(j,nColNumSourceList,strItem);
					m_listResult.SetItemText(j,m_listResult.m_nColIndexDragged,"");
				}
				m_bColumnDropped[m_listResult.m_nColIndexDragged]=FALSE;
				
			}		
			
		}

		m_listText.EndDrag();
		m_listResult.EndDrag();
		m_pDragWnd = NULL;
		
		// set ok button enable/ disable
		// 0 && 1 && (2 || 3)
		if( m_bColumnDropped[0] && m_bColumnDropped[1] &&
			( m_bColumnDropped[2] || m_bColumnDropped[3] ) )
			m_butOK.EnableWindow( TRUE );
		else
			m_butOK.EnableWindow( FALSE );
	}
	
	CXTResizeDialog::OnLButtonUp(nFlags, point);
}

CImageList* CRosterWizardDlg::CreateDragImageEx(int nColIndexDragged, CListCtrl *pList, LPPOINT lpPoint)
{
	
	CRect rectComplete(0,0,0,0);
	
	// Determine List Control Client width size
	int nWidthCol=pList->GetColumnWidth(nColIndexDragged);
	rectComplete.right=nWidthCol;
	CListCtrlDrag* pLCD=(CListCtrlDrag*)pList;
	int nItemHeight=pLCD->GetItemHeight();
	int nCPP=pList->GetCountPerPage();
	CRect rcHeaderCtrl;
	pList->GetHeaderCtrl()->GetClientRect(rcHeaderCtrl);
	rectComplete.bottom=nCPP*nItemHeight+rcHeaderCtrl.Height();
	
	
	// Determine the size of the drag image (limite for rows visibled and Client width)
	CClientDC dcClient(this);
	CDC dcMem;
	CBitmap Bitmap;
	
	if (!dcMem.CreateCompatibleDC(&dcClient))
		return NULL;
	
	if (!Bitmap.CreateCompatibleBitmap(&dcClient, rectComplete.Width(), rectComplete.Height()))
		return NULL;
	
	CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);
	// Use green as mask color
	dcMem.FillSolidRect(0, 0, rectComplete.Width(), rectComplete.Height(), RGB(0,255,0));
	
	// Paint each DragImage in the DC
	int nCountRow=pList->GetItemCount();
	
	int nVisibleRowNum=0;
	CFont* pOleFont=dcMem.SelectObject(pList->GetFont());
	CImageList* pColIL=pList->GetHeaderCtrl()->CreateDragImage(nColIndexDragged);
	pColIL->Draw(&dcMem,0,CPoint(0,0),ILD_NORMAL);
	
	CRect rcItem;
	for(int i=pList->GetTopIndex();i<nCountRow;i++)
	{
		rcItem=CRect(CPoint(0,rcHeaderCtrl.Height()+nVisibleRowNum*nItemHeight),CSize(nWidthCol,nItemHeight));
		dcMem.DrawText(pList->GetItemText(i,nColIndexDragged),rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		nVisibleRowNum++;
		if(nVisibleRowNum>nCPP)
			break;
	}
	dcMem.SelectObject(pOleFont);
	dcMem.SelectObject(pOldMemDCBitmap);
	CImageList* pCompleteImageList = new CImageList;
	pCompleteImageList->Create(rectComplete.Width(), rectComplete.Height(), ILC_COLOR | ILC_MASK, 0, 1);
	pCompleteImageList->Add(&Bitmap, RGB(0, 255, 0)); // Green is used as mask color
	Bitmap.DeleteObject();
	
	if (lpPoint)
	{
		lpPoint->x = rectComplete.left;
		lpPoint->y = rectComplete.top;
	}
	
	return pCompleteImageList;
}

void CRosterWizardDlg::OnItemchangedListResultSchedule(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	POSITION pos = m_listResult.GetFirstSelectedItemPosition();
	if( pos!= NULL )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, TRUE );
	else
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, FALSE );

	*pResult = 0;
}

void CRosterWizardDlg::OnItemchangedListRostertext(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	if( pos!= NULL && !m_bHaveAssignTitle )
	{
		m_butTitle.EnableWindow( TRUE );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,TRUE);
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,TRUE);
	}
	else if( pos!= NULL )
	{
		m_butTitle.EnableWindow( FALSE );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,TRUE);
	}
	else
	{
		m_butTitle.EnableWindow( FALSE );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
	}
		
	*pResult = 0;
}


void CRosterWizardDlg::OnPeoplemoverDelete() 
{
	POSITION pos = m_listResult.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int iItemIdx = m_listResult.GetNextSelectedItem( pos );
		m_listResult.DeleteItem( iItemIdx );
	}
	
}

void CRosterWizardDlg::OnButtonTitle() 
{
	// set current select row as column title
	// and remove the row
	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	if( pos!= NULL )
	{
		int iItemIdx = m_listText.GetNextSelectedItem( pos );
		// get item text and remove the row
		std::vector<CString> vItemText;
		for( int i=0; i<m_listText.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			CString itemText = m_listText.GetItemText( iItemIdx, i );
			vItemText.push_back( itemText );
		}
		
		m_listText.DeleteItem( iItemIdx );
		
		// set the colum title
		HDITEM hd;
		for( i=0; i<m_listText.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			m_listText.GetHeaderCtrl()->GetItem( i,&hd);
			hd.mask |= HDI_TEXT;
			hd.pszText = vItemText[i].GetBuffer(0);
			hd.cchTextMax = 128;
			m_listText.GetHeaderCtrl()->SetItem( i, &hd );
		}
		
		m_butTitle.EnableWindow( FALSE );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
		m_bHaveAssignTitle = true;
	}
	
}

void CRosterWizardDlg::setResourceNum( int _iItemIdx, int _iSubItem, const CString& _strProcName )
{
	CString strItem;

	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict );
	id.setID( _strProcName );

	GroupIndex group  = m_pInTerm->procList->getGroupIndex( id );
	if( group.end == -1 && group.start == -1 )
	{
		strItem.Format("%d",-1);
	}
	else
	{
		strItem.Format("%d", group.end-group.start+1 );
	}

	m_listResult.SetItemText( _iItemIdx, _iSubItem, strItem );
}


bool CRosterWizardDlg::isProcessor( const CString& _strProcName )
{
	if( m_pInTerm->procList->getProcessor( _strProcName )!= NULL )
		return true;
	return false;
}

bool CRosterWizardDlg::isNumString( const CString& _strNum )
{
	for( int i=0; i<_strNum.GetLength(); i++ )
		if( !isNumeric(_strNum[i]) )
			return false;
		
		return true;
}

void CRosterWizardDlg::OnDeleteRow()
{
	POSITION pos=m_listText.GetFirstSelectedItemPosition();
	while (pos) 
	{
		int nIndex=m_listText.GetNextSelectedItem(pos);
		m_listText.DeleteItem(nIndex);
		break;
	}
}

BOOL CRosterWizardDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
    // if there is a top level routing frame then let it handle the message
    if (GetRoutingFrame() != NULL) return FALSE;
	
    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;
	
    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool 
        nID = ::GetDlgCtrlID((HWND)nID);
    }
	
    if (nID != 0) // will be zero on a separator
    {
        AfxLoadString(nID, szFullText);
        strTipText=szFullText;
		
#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif
		
        *pResult = 0;
		
        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
        
        return TRUE;
    }
	
    return FALSE;
}

void CRosterWizardDlg::OnDblclkListResultSchedule( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if(pNMLV->iItem < 0)
		return;

	if (pNMLV->iSubItem == 8)
	{
		CDlgMEDefine dlg(m_pTermDoc,false);
		if(dlg.DoModal() == IDOK)
		{
			CString strMobCon;
			int nIdxCon = dlg.GetSelectConstraint();
			const CMobileElemConstraint* pMobCon = m_pTermDoc->GetCreateMeList().getConstraint(nIdxCon);
			pMobCon->screenPrint(strMobCon);
			m_listResult.SetItemText(pNMLV->iItem,pNMLV->iSubItem,strMobCon);
			m_listResult.SetItemData(pNMLV->iItem,(DWORD)nIdxCon);
		}
	}
}

void CRosterWizardDlg::OnCreateMeList()
{
	CDlgMEDefine dlg(m_pTermDoc,true);
	dlg.DoModal();
}
