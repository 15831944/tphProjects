// ImportFlightFromFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ImportFlightFromFileDlg.h"
#include "GateMapDlg.h"
#include "TermPlanDoc.h"
#include "inputs\stringconverttoflight.h"
#include "inputs\flight.h"
#include "inputs\schedule.h"
#include "inputs\UserName.h"
#include "inputs\fltdata.h"
#include "../InputAirside/InputAirside.h"
#include "../Common/ACTypesManager.h"
#include "OpenSqlDlg.h"
#include "DlgFlightActypeMatch.h"
#include "../Common/Aircraft.h"
#include "../Common/AirportDatabase.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char fileheader[] = "Flight Schedule file";
const CString  strMsg1 = "Never Convert User Name";
#define STANDGATEFLAG	DWORD(1)
#define ARRGATEFLAG		DWORD(2)
#define DEPGATEFLAG		DWORD(4)
#define	BAGDEVICELAG		DWORD(4)
static char* columnLabel2[] = {	
"Airline  **", 
"Arr ID  **", 
"Origin  **",
"Arr Stopover",
//"Arr Date  **",
"Arr Day  **",
"Arr Time  **", 
"Dep ID  **",
"Dep Stopover",
"Destination  **",
//"Dep Date  **",
"Dep Day  **",
"Dep Time  **",
"Ac Type  **",
"Arr Stand",
"Dep Stand",
"Intermediate Stand",
"Stay Time(Min)",
"Arr Gate",
"Dep Gate",
"BagDevice",
"Arr Load Factors(nn.nn)",
"Dep Load Factors(nn.nn)",
"Aircraft Seats",
"Transit Times",
"Busses Needed"
};

static char* columnLabel1[] = {	
"Airline", 
"Arr ID", 
"Origin",
"Arr Stopover",
//"Arr Date",
"Arr Day",
"Arr Time", 
"Dep ID",
"Dep Stopover",
"Destination",
//"Dep Date",
"Dep Day",
"Dep Time",
"Ac Type",
"Arr Stand",
"Dep Stand",
"Intermediate Stand",
"Stay Time(Min)",
"Arr Gate",
"Dep Gate",
"BagDevice",
"Arr Load Factors(nn.nn)",
"Dep Load Factors(nn.nn)",
"Aircraft Seats",
"Transit Times",
"Busses Needed"
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CImportFlightFromFileDlg dialog
CImportFlightFromFileDlg::CImportFlightFromFileDlg(InputTerminal* _pInTerm, const CString& _strPath, int nPrjID, CWnd* pParent /*=NULL*/)
	: CDialog(CImportFlightFromFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportFlightFromFileDlg)
	m_bFullCompatibleFormat = FALSE;
	//}}AFX_DATA_INIT
	m_pInTerm = _pInTerm;
	m_startDate = _pInTerm->flightSchedule->GetStartDate();
	m_strProjPath = _strPath;
	m_pDragWnd=NULL;
	m_pDragImage=NULL;
	for(int i=0;i<COLUMN_NUM;i++)
		m_bColumnDropped[i]=FALSE;
	m_bHaveAssignTitle = false;

	m_bIsFlightScheduleDlg = FALSE;
	m_nPrjID = nPrjID;
	m_dFileVersion = 0.0;
}

CImportFlightFromFileDlg::~CImportFlightFromFileDlg()
{
	if(m_pDragImage)
		delete m_pDragImage;
	m_pDragImage = NULL;
	
}


void CImportFlightFromFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportFlightFromFileDlg)
	DDX_Control(pDX, IDC_BUT_OPENFILE, m_btnOpen);
	DDX_Control(pDX, ID_APPEND, m_btnAppend);
	DDX_Control(pDX, IDC_COMBO_IMPORTED_FILE_FORMAT, m_cmbImportedFileFormat);
	DDX_Control(pDX, IDC_EDIT3, m_editResultInfo);
	DDX_Control(pDX, IDC_COMPILE, m_btnCompile);
	DDX_Control(pDX, IDC_BUTTON_MOVETO_LEFT, m_btnMoveToLeft);
	DDX_Control(pDX, IDC_BUTTON_MOVETO_RIGHT, m_btnMoveToRight);
	DDX_Control(pDX, IDC_LIST_MAPPING, m_listMapping);
	DDX_Control(pDX, IDC_LIST_SOURCE_COLUMN, m_listSourceColumn);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFileName);
	DDX_Control(pDX, IDC_LIST_SOURCE, m_listSource);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
	DDX_Control(pDX, IDOK, m_butOK);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_DATAFORMAT, m_DataFormatControl);
}


BEGIN_MESSAGE_MAP(CImportFlightFromFileDlg, CDialog)
	//{{AFX_MSG_MAP(CImportFlightFromFileDlg)
	//ON_BN_CLICKED(IDC_BUT_OPENFILE, OnButOpenfile)
	//ON_BN_CLICKED(IDC_BUT_COMPILER, OnButConvert)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SOURCE, OnItemchangedListSource)
	ON_COMMAND(ID_NAMELIST_TITLE,OnButtonTitle)
	ON_COMMAND(ID_DELETE_ROW,OnDeleteRow)
	ON_COMMAND(ID_DELETE_COLUMN,OnDeleteColumn)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RESULT, OnRclickListResult)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_SOURCE_COLUMN, OnSelchangeListSourceColumn)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MAPPING, OnClickListMapping)
	ON_BN_CLICKED(IDC_BUTTON_MOVETO_RIGHT, OnButtonMovetoRight)
	ON_BN_CLICKED(IDC_BUTTON_MOVETO_LEFT, OnButtonMovetoLeft)
	ON_BN_CLICKED(IDC_CHECK_FULL_COMPATIBLE_FORMAT, OnCheckFullCompatibleFormat)
	ON_BN_CLICKED(IDC_COMPILE, OnCompile)
	ON_CBN_SELCHANGE(IDC_COMBO_IMPORTED_FILE_FORMAT, OnSelchangeComboImportedFileFormat)
	ON_BN_CLICKED(ID_APPEND, OnAppend)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_MESSAGE(UM_HEADERCTRL_CLICK_COLUMN,OnHeaderCtrlClickColumn)
	ON_CBN_SELENDOK(ID_COMBOBOX, OnComboxSelectChanged)

	ON_COMMAND(ID_OPEN_FROM_FILE, OnOpenFromFile)
	ON_COMMAND(ID_OPEN_FROM_SQL, OnOpenFromSQL)
	ON_COMMAND(ID_OPEN_FROM_ARCCAPTURE, OnOpenFromArcCapture)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportFlightFromFileDlg message handlers

BOOL CImportFlightFromFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_cmbImportedFileFormat.AddString("Semicolon");
	m_cmbImportedFileFormat.AddString("Comma");
	m_cmbImportedFileFormat.AddString("Space");
	m_cmbImportedFileFormat.AddString("Tab");
	m_cmbImportedFileFormat.AddString("ARCport Format");
	m_cmbImportedFileFormat.SetCurSel(1);

	//date format
	m_DataFormatControl.AddString(_T("MM/DD/YYYY"));
	m_DataFormatControl.AddString(_T("DD/MM/YYYY"));
	m_DataFormatControl.SetCurSel(0);

	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcNo = AfxGetApp()->LoadStandardCursor(IDC_NO);

	CRect rect;
	m_listSource.GetWindowRect(rect);
	ScreenToClient(&rect);
	rect.top-=26;
	rect.bottom=rect.top+26;
	m_wndToolBarLeft.MoveWindow(&rect);

	m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
	m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
	
	// TODO: Add extra initialization here
	InitListCtrl();

	m_butOK.EnableWindow( FALSE );
	m_btnAppend.EnableWindow( FALSE );
	m_headerCtrlDrag.SubclassWindow( m_listSource.GetDlgItem(0)->GetSafeHwnd());

	//	Init Cool button
	m_btnOpen.SetOperation(0);
	m_btnOpen.SetWindowText(_T("Open"));
	m_btnOpen.SetOpenButton(TRUE);

	EnableCompileBtn();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CImportFlightFromFileDlg::InitListCtrl()
{
	//Airline,Arr ID,Origin,Arr Time,Dep ID,Dest,Dep Time,AC Type,Gate,Service Time
	DWORD dwStyle = m_listResult.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listResult.SetExtendedStyle( dwStyle );
	m_listSource.SetExtendedStyle(dwStyle);
	m_listMapping.SetExtendedStyle(dwStyle);

	int DefaultColumnWidth[] = { 90, 75, 75, 100, 75, 75, 75, 100, 75, 75, 75, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80};
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
		LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, 
		LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER };
	// column
	for( int i=0; i<COLUMN_NUM; i++ )
	{
		m_listResult.InsertColumn( i, columnLabel1[i], nFormat[i], DefaultColumnWidth[i] );
	}

	CRect rect;
	m_listSource.GetClientRect(rect);
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT |LVCF_FMT;
	lvc.fmt=LVCFMT_NOEDIT;
	lvc.cx	=rect.Width();
	lvc.pszText="No.";
	CStringList strList;
	lvc.csList=&strList;
	
	m_listSource.InsertColumn(m_listSource.GetHeaderCtrl()->GetItemCount(),&lvc);
	m_listMapping.GetClientRect(rect);
	rect.right-=::GetSystemMetrics(SM_CXVSCROLL);
	m_listMapping.InsertColumn(0,"Source",LVCFMT_LEFT,rect.Width()/2);
	m_listMapping.InsertColumn(1,"Target ( ** : Required )",LVCFMT_LEFT,rect.Width()/2);
	for(int j=0;j<COLUMN_NUM;j++)
	{
		m_listMapping.InsertItem(j,"");
		m_listMapping.SetItemText(j,1,columnLabel2[j]);
		m_listMapping.SetItemData(j,-1);
	}
}
void CImportFlightFromFileDlg::Openfile() 
{
	CFileDialog filedlg( TRUE,"skd", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"FlightSchedule File (*.skd)|*.skd;*.SKD|All type (*.*)|*.*|", NULL, 0, FALSE  );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	
	if( ReadFile( csFileName ) )
	{
		// clear
		ClearFlightVect();						// clear flight vector
		m_listResult.DeleteAllItems();			// clear result list_ctrl
		for( int i = 0; i<COLUMN_NUM; i++ )		// zero drag flag
			m_bColumnDropped[i] = FALSE;
		
		if( m_bArctermFile )					// if is ARCTerm schedule file formate, make "ARCTerm formate" is selected
			m_cmbImportedFileFormat.SetCurSel(4);
	
		AddFlightToEdit();						// add flight schedule to edit
		
		m_bFullCompatibleFormat=FALSE;
		m_butOK.EnableWindow( FALSE );
		m_btnAppend.EnableWindow( FALSE );

		m_bHaveAssignTitle = false;

		m_editFileName.SetWindowText( csFileName );
		
		if( m_bArctermFile )					// if is ARCTerm schedule file formate. auto convert
			OnCompile();
	}
}


// read file data to memory
bool CImportFlightFromFileDlg::ReadFile(const CString &strFileName)
{
	m_bArctermFile = FALSE;
	
	// clear string array 
	flightSchedule.RemoveAll();

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
		flightSchedule.Add( strLineText );
	}

	// if is arcterm file, remove the first three line and the last two line of the file
	if( flightSchedule.GetAt(1) == CString(fileheader) )	// is arcterm file with header
	{
		//get file version
		CString strFileNameAndVersion = flightSchedule.GetAt(0);
		int nPos = strFileNameAndVersion.ReverseFind(',');
		CString strFileVersion = strFileNameAndVersion.Right(strFileNameAndVersion.GetLength() - nPos - 1);
		m_dFileVersion = atof(strFileVersion);

		m_bArctermFile = TRUE;

		// remove first three line
		flightSchedule.RemoveAt( 0,3 );
		// remove last two line
		flightSchedule.RemoveAt( flightSchedule.GetSize()-2,2 );
	}

	file.Close();
	return true;
}

int CImportFlightFromFileDlg::AddFlightToEdit()
{
	// clear edit
	CWaitCursor wc;

	CString strEditText;
	int	flightNum = flightSchedule.GetSize();
	
	if( flightNum == 0)
	{
		return flightNum;
	}
	
	strEditText = flightSchedule.GetAt(0);
	if(strEditText.IsEmpty())
		return 0;

	DeleteAllColumn();
	m_listSourceColumn.ResetContent();
	for(int k=0;k<m_listMapping.GetItemCount();k++)
	{
		m_listMapping.SetItemText(k,0, "");
	}
	m_listSource.DeleteAllItems();
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT |LVCF_FMT;
	lvc.fmt=LVCFMT_NOEDIT;
	lvc.cx	=36;
	lvc.pszText="No.";
	CStringList strList;
	lvc.csList=&strList;
	
	m_listSource.InsertColumn(m_listSource.GetHeaderCtrl()->GetItemCount(),&lvc);
	CStringArray strArray;		
	CString strSeparate;
	m_bFullCompatibleFormat=FALSE;
	switch(m_cmbImportedFileFormat.GetCurSel())
	{
	case 0:
		strSeparate=";";
		break;
	case 1:
	case 4://ARCTerm Format
		strSeparate=",";
		break;
	case 2:
		strSeparate=" ";
		break;
	case 3:
		strSeparate="\t";		
		break;
	}

	//moidfy by adam 2007-08-24
	//int nColNum=GetColNumFromStr(strEditText,strSeparate,strArray);

	int nColNum;

	//old version
	if (m_dFileVersion < 2.80000001)
	{
		nColNum = GetColNumFromStr(strEditText,strSeparate,strArray);
	}
	//new version
	else
	{
		nColNum = GetColNumFromStr2_9(strEditText,strSeparate,strArray);

	}
	//End moidfy by adam 2007-08-24

	AddColumn(nColNum);
	SetItemText(strArray);
	for( int i=1; i<flightNum; i++ )
	{
		strEditText += "\r\n";
		strEditText += flightSchedule.GetAt(i);
		CStringArray strArray;

		//modify by adam 2007-08-24
		int nColNum2;
		
		//old version
		if (m_dFileVersion < 2.80000001)
		{
			nColNum2 = GetColNumFromStr(flightSchedule.GetAt(i),strSeparate,strArray);
		}
		//new version
		else
		{
			nColNum2 = GetColNumFromStr2_9(flightSchedule.GetAt(i),strSeparate,strArray);
		}
		//End modify by adam 2007-08-24

		if(nColNum<nColNum2)
		{
			AddColumn(nColNum2-nColNum);
			nColNum = nColNum2;
		}
		SetItemText(strArray);
	}
	
	if(m_cmbImportedFileFormat.GetCurSel()==4)//ARCTerm Format
	{
		m_bFullCompatibleFormat=TRUE;
		OnCheckFullCompatibleFormat();

	}

	return flightNum;
}

int CImportFlightFromFileDlg::AddAFlightToList(Flight *_pFlight)
{
	char str[128];
	CString strProcID;
	//CStartDate sDate;
	//COleDateTime outDate;
	COleDateTime outTime;
	//bool bAbsDate;
	//int nOutDateIdx;
	//sDate = m_pInTerm->flightSchedule->GetStartDate();

	_pFlight->getAirline( str );
	int _nIndex = m_listResult.InsertItem(0, str );
	if( _pFlight->isArriving() )
	{
		_pFlight->getArrID( str );
		m_listResult.SetItemText( _nIndex, 1, str );
		_pFlight->getOrigin( str );
		m_listResult.SetItemText( _nIndex, 2, str );
		_pFlight->getArrStopover( str );
		m_listResult.SetItemText( _nIndex, 3, str );
		//m_listResult.SetItemText( _nIndex, 3, _pFlight->getArrDate().PrintDate());

		//Modify by adam 2007-05-10 
		//if (sDate.IsAbsoluteDate())
		//{
		//	sDate.GetDateTime( _pFlight->getArrTime(), bAbsDate, outDate, nOutDateIdx, outTime );
		//	m_listResult.SetItemText(_nIndex, 4, outDate.Format( _T("%x") ) );
		//	m_listResult.SetItemText(_nIndex, 5, outTime.Format( _T("%X") ) );
		//}
		//else
		//{
		//	(_pFlight->getArrTime()).printTime( str, 0 );
		//	m_listResult.SetItemText( _nIndex, 5, str );
		//}

		CString strDay;
		strDay.Format("%d", _pFlight->getArrTime().GetDay());
		m_listResult.SetItemText(_nIndex, 4, strDay);
		outTime.SetTime(_pFlight->getArrTime().GetHour(), _pFlight->getArrTime().GetMinute(), _pFlight->getArrTime().GetSecond());
        m_listResult.SetItemText(_nIndex, 5, outTime.Format( _T("%X") ) );
		//End Modify by adam 2007-05-10 
	}
	
	else
	{
		m_listResult.SetItemText( _nIndex, 1, " " );
		m_listResult.SetItemText( _nIndex, 2, " " );
		m_listResult.SetItemText( _nIndex, 3, " " );
		m_listResult.SetItemText( _nIndex, 4, " " );
		m_listResult.SetItemText( _nIndex, 5, " " );
	}
	
	if( _pFlight->isDeparting() )
	{
		_pFlight->getDepID( str );
		m_listResult.SetItemText( _nIndex, 6, str );
		_pFlight->getDepStopover( str );
		m_listResult.SetItemText( _nIndex, 7, str );
		_pFlight->getDestination( str );
		m_listResult.SetItemText( _nIndex, 8, str );

		//m_listResult.SetItemText( _nIndex, 7, _pFlight->getDepDate().PrintDate());

		//Modify by adam 2007-05-10
		//if (sDate.IsAbsoluteDate())
		//{
		//	sDate.GetDateTime( _pFlight->getDepTime(), bAbsDate, outDate, nOutDateIdx, outTime );
		//	m_listResult.SetItemText( _nIndex, 9, outDate.Format( _T("%x") ) );
		//	m_listResult.SetItemText( _nIndex, 10, outTime.Format( _T("%X")));
		//}
		//else
		//{
		//	(_pFlight->getDepTime()).printTime( str, 0 );
		//	m_listResult.SetItemText( _nIndex, 10, str );
		//}

		CString strDay;
		strDay.Format("%d", _pFlight->getDepTime().GetDay());
		m_listResult.SetItemText(_nIndex, 9, strDay);
		outTime.SetTime(_pFlight->getDepTime().GetHour(), _pFlight->getDepTime().GetMinute(), _pFlight->getDepTime().GetSecond());
		m_listResult.SetItemText(_nIndex, 10, outTime.Format( _T("%X") ) );
		//Modify by adam 2007-05-10
	}
	
	else
	{
		m_listResult.SetItemText( _nIndex, 6, " " );
		m_listResult.SetItemText( _nIndex, 7, " " );
		m_listResult.SetItemText( _nIndex, 8, " " );
		m_listResult.SetItemText( _nIndex, 9, " " );
		m_listResult.SetItemText( _nIndex, 10, " " );
	}
	
	_pFlight->getACType( str );
	m_listResult.SetItemText( _nIndex, 11, str );
		
	strProcID = _pFlight->getArrStand().GetIDString();//_pFlight->getGate().isBlank() ? "" : _pFlight->getGate().GetIDString();
	m_listResult.SetItemText( _nIndex, 12, strProcID );

	strProcID = _pFlight->getDepStand().GetIDString();//_pFlight->getGate().isBlank() ? "" : _pFlight->getGate().GetIDString();
	m_listResult.SetItemText( _nIndex, 13, strProcID );

	strProcID = _pFlight->getIntermediateStand().GetIDString();
	m_listResult.SetItemText( _nIndex, 14, strProcID );

	ElapsedTime time;
	if( _pFlight->isTurnaround())
		time = _pFlight->getDepTime() - _pFlight->getArrTime();
	else
		time = _pFlight->getServiceTime();
	time.printTime( str, 0 );
	m_listResult.SetItemText( _nIndex, 15, str );	
	
	strProcID = _pFlight->getArrGate().isBlank() ? "" : _pFlight->getArrGate().GetIDString();
	m_listResult.SetItemText( _nIndex, 16,strProcID );

	strProcID =	_pFlight->getDepGate().isBlank() ? "" : _pFlight->getDepGate().GetIDString();
	m_listResult.SetItemText( _nIndex, 17, strProcID );

	strProcID =	_pFlight->getBaggageDevice().isBlank() ? "" : _pFlight->getBaggageDevice().GetIDString();
	m_listResult.SetItemText( _nIndex, 18, strProcID );

	if(m_bColumnDropped[19])
	{	
		if (_pFlight->getArrLFInput() >=0)
		{
			CString str = "";
			str.Format("%.2f", _pFlight->getArrLFInput()*100);
			m_listResult.SetItemText( _nIndex, 19, str );
		}
	}
	if(m_bColumnDropped[20])
	{
		if (_pFlight->getDepLFInput() >=0)
		{
			CString str = "";
			str.Format("%.2f", _pFlight->getDepLFInput()*100);
			m_listResult.SetItemText( _nIndex, 20, str );
		}
	}
	if(m_bColumnDropped[21])
	{
		if (_pFlight->getCapacityInput() >=0)
		{
			CString str = "";
			str.Format("%d", _pFlight->getCapacityInput());
			m_listResult.SetItemText( _nIndex, 21, str );
		}
	}
	//if(m_bColumnDropped[22])
	//{
	//}

	//m_listResult.SetItemData( _nIndex,_nIndex );
	//now, can save 
	m_listResult.SetItemData(_nIndex,(DWORD)_pFlight);

	return _nIndex;
}

// convert a string to flight object
// if convert success, remove the string from edit
// else still place the string in the edit
//void CImportFlightFromFileDlg::OnButConvert() 
//{
/*
	CStringConvertToFlight convert( m_pInTerm );
	Flight* p_flight = NULL;
	ArctermFile file;
	
	int nRightNum = 0;

	int iFlightSize = flightSchedule.GetSize();
	if( iFlightSize > 0)
	{
		for( int i=iFlightSize-1; i>=0; i-- )
		{
			CString strLine = flightSchedule.GetAt(i);
			p_flight = convert.ConvertToFlight( strLine, file, *m_pInTerm->m_pGateMappingDB );
			if( p_flight != NULL )	// have no error in the string
			{
				m_vectFlight.push_back( p_flight );
				AddAFlightToList( p_flight, m_vectFlight.size()-1 );
				//remove the string from the string arry
				flightSchedule.RemoveAt(i);
				nRightNum ++;
			}
		}
		AddFlightToEdit();
		
		if( nRightNum > 0)
			for( i = 0; i<COLUMN_NUM; i++ )
				m_bColumnDropped[i] = TRUE;
	}

	/// out compiler infomation
	CString strMsg;
	strMsg.Format("   Total Lines:   %d!\r\n   Succeed:   %d!\r\n   Failed:   %d!" ,iFlightSize, nRightNum, iFlightSize-nRightNum );
	MessageBox( strMsg, "Compile Result", MB_OK|MB_ICONINFORMATION );
*/
//}

void CImportFlightFromFileDlg::ClearFlightVect()
{
	for( int i=0; i<static_cast<int>(m_vectFlight.size()); i++ )
	{
		if( m_vectFlight[i] )
			delete m_vectFlight[i];
	}
	m_vectFlight.clear();
}

/////////////////////////////////////////////////////////////////////////
// if the contain of the edit has changed
// shall read string line by line from the edit ctrl
// the add the string to the string array
int CImportFlightFromFileDlg::GetStringArrayFromEdit()
{
	int iLineNum = 0;
/*
	char szText[1024];
	// clear old data
	flightSchedule.RemoveAll();
	
	UpdateData( );
	// get string line by line from the edit
	CString strLineText;
	 iLineNum= m_editFlight.GetLineCount();
	for( int i=0; i<iLineNum; i++ )
	{
		strLineText.Empty();
		if( m_editFlight.GetLine(i ,szText ) ==0 )
			continue;
		strLineText = szText;
		
		strLineText.TrimLeft();
		strLineText.TrimRight();
		if( strLineText.IsEmpty() )
			continue;
	
		//add the string to array
		flightSchedule.Add( strLineText );
	}
*/

	return iLineNum;
}
//BOOL CImportFlightFromFileDlg::CheckTheFlightActype()
//{
//	TCHAR actype[1024] = { 0 } ;
//	CACType* Pactype = NULL ;
//	std::map<CString,CDlgFlightActypeMatch::TY_DATA > m_NoMatchActype ;
//	std::map<CString,CDlgFlightActypeMatch::TY_DATA> m_confiltActype  ;
//	std::vector<CACType*> m_FindActypes;
//	Flight* Pflight = NULL ;
//	CACTypesManager* 	pManager = m_pInTerm->m_pAirportDB->getAcMan() ;
//	for (int i = 0 ; i <(int) m_vectFlight.size() ;i++)
//	{
//		Pflight =m_vectFlight[i] ;
//
//		Pflight->getACType(actype) ;
//		Pactype = pManager->getACTypeItem(CString(actype)) ;
//		if(Pactype == NULL)
//		{
//			if(m_NoMatchActype.find(actype) != m_NoMatchActype.end() )
//			{
//				m_NoMatchActype[CString(actype)].push_back(Pflight) ;
//				continue ;
//			}
//			if(m_confiltActype.find(actype) != m_confiltActype.end())
//			{
//				m_confiltActype[CString(actype)].push_back(Pflight) ;
//				continue ;
//			}
//			m_FindActypes.clear() ;
//			if(!pManager->FindActypeByIATACodeOrICAOCode(actype,m_FindActypes))
//				m_NoMatchActype[CString(actype)].push_back(Pflight) ;
//			else
//			{
//				if(m_FindActypes.size() == 1)
//				{
//					strcpy(actype,m_FindActypes[0]->GetIATACode()) ;
//					Pflight->setACType(actype) ;
//				}else
//					m_confiltActype[CString(actype)].push_back(Pflight) ;
//			}
//		}
//	}
//	if(!m_confiltActype.empty())
//	{
//		CString Errormsg ;
//		Errormsg.Format(_T("ARCTerm find some actype confilts in Flight schedule,please click \'Ok\'to edit confilts.")) ;
//		MessageBox(Errormsg,_T("Warning"),MB_OK) ;
//		CDlgFlightActypeMatch dlgflightActype(&m_confiltActype,pManager) ;
//		dlgflightActype.DoModal() ;
//	}
//	if(!m_NoMatchActype.empty())
//	{
//		std::map<CString,CDlgFlightActypeMatch::TY_DATA>::iterator iter = m_NoMatchActype.begin() ;
//		CString name ;
//		for (int i= 0 ; iter != m_NoMatchActype.end() ;iter++,i++)
//		{
//			if(i == 0)
//				name.Append("\'") ;
//			else
//				name.Append(",") ;
//			name.Append( (*iter).first ) ;
//		}
//		if(!m_NoMatchActype.empty())
//			name.Append("\'") ;
//		CString Errormsg ;
//		Errormsg.Format(_T("ARCTerm check that no actype can match to %s ,please define in airport DataBase firstly."),name) ;
//		MessageBox(Errormsg,_T("Warning"),MB_OK) ;
//		return FALSE ;
//	}
//	return TRUE ;
//}
void CImportFlightFromFileDlg::OnOK() 
{
	// TODO: Add extra validation here
	//if(!CheckTheFlightActype())
	//	return ;
	CWaitCursor wc;
	m_pInTerm->flightSchedule->clear();
	for( int i=0; i<static_cast<int>(m_vectFlight.size()); i++ )
	{
		m_pInTerm->flightSchedule->addItem( m_vectFlight[i] );
	}
	int nItemCount=m_listResult.GetItemCount();
	//for(int j=0;j<nItemCount;j++)
	//{
	//	Flight* pFlight=(Flight*)m_listResult.GetItemData(j);
	//	getMoreInfoFromList( pFlight,j);	
	//}
	//m_pInTerm->flightData->saveDataSet(  m_strProjPath, true );

	CDialog::OnOK();
}

void CImportFlightFromFileDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	ClearFlightVect();
	m_pInTerm->flightSchedule->SetStartDate(m_startDate);
	CDialog::OnCancel();
}

int CImportFlightFromFileDlg::GetColNumFromStr(CString strText, CString strSep,CStringArray& strArray)
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

int CImportFlightFromFileDlg::GetColNumFromStr2_9(CString strText,CString strSep,CStringArray& strArray)
{
	int nColNum=1;
	int nPos=strText.Find(strSep,0);

	while(nPos!=-1)
	{
		if (nColNum==5 || nColNum==10)
		{
			strArray.Add("");
		}
		else
		{
			strArray.Add(strText.Left(nPos));
			strText=strText.Right(strText.GetLength()-nPos-1);
			nPos=strText.Find(strSep,0);
		}
		nColNum++;
	}

	strArray.Add(strText);

	if (m_dFileVersion >=3.2f)
		nColNum -=3;		//neglect the stand parking time columns

	return nColNum;
}

void CImportFlightFromFileDlg::DeleteAllColumn()
{
	int nColNum=m_listSource.GetHeaderCtrl()->GetItemCount();
	while(nColNum!=0)
	{
		m_listSource.DeleteColumn(0);
		nColNum=m_listSource.GetHeaderCtrl()->GetItemCount();
	}
	
}

void CImportFlightFromFileDlg::SetItemText(CStringArray &strArray)
{
	int nLineNum=m_listSource.GetItemCount();
	CString strLineNum;
	strLineNum.Format("%d",nLineNum+1);
	int nLineIndex=m_listSource.InsertItem(nLineNum,strLineNum);
	for(int k=0;k<strArray.GetSize();k++)
	{
		if (m_dFileVersion >=3.2)
		{
			if (k ==16 || k ==17 || k ==18)
				continue;

			if (k > 18)
			{
				m_listSource.SetItemText(nLineIndex,k-2,strArray[k]);
				continue;
			}
		}
		m_listSource.SetItemText(nLineIndex,k+1,strArray[k]);
	}
}

void CImportFlightFromFileDlg::AddColumn(int nColNum,BOOL bAdd2ListBox)
{
	for(int j=0;j<nColNum;j++)
	{
		CString strColumn;
		if( m_bArctermFile && j< COLUMN_NUM )
			strColumn = columnLabel2[j] ;
		else
			strColumn.Format("Col %d",j+1);

		LV_COLUMNEX	lvc;
		lvc.mask = LVCF_WIDTH | LVCF_TEXT |LVCF_FMT;
		lvc.fmt=LVCFMT_EDIT;
		lvc.cx	=50;
		lvc.pszText=strColumn.GetBuffer(0);
		CStringList strList;
		lvc.csList=&strList;
		m_listSource.InsertColumn(m_listSource.GetHeaderCtrl()->GetItemCount(),&lvc);
		if(bAdd2ListBox)
		{
			int nIndex=m_listSourceColumn.AddString(strColumn);
			m_listSourceColumn.SetItemData(nIndex,j+1);
			
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CImportFlightFromFileDlg::OnItemchangedListSource(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	POSITION pos = m_listSource.GetFirstSelectedItemPosition();

	if( pos!= NULL)
	{
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,!m_bHaveAssignTitle );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,TRUE);
	}
	else
	{
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_NAMELIST_TITLE,FALSE);
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
	}

	*pResult = 0;
}

void CImportFlightFromFileDlg::OnButtonTitle() 
{
	// set current select row as column title
	// and remove the row
	POSITION pos = m_listSource.GetFirstSelectedItemPosition();
	if( pos!= NULL )
	{
		int iItemIdx = m_listSource.GetNextSelectedItem( pos );
		// get item text and remove the row
		std::vector<CString> vItemText;
		for( int i=0; i<m_listSource.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			CString itemText = m_listSource.GetItemText( iItemIdx, i );
			if( itemText.IsEmpty() )
				itemText.Format("Col %d", i);
			
			vItemText.push_back( itemText );
		}

		m_listSource.DeleteItem( iItemIdx );

		// set the colum title
		HDITEM hd;
		for(int i=0; i<m_listSource.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			m_listSource.GetHeaderCtrl()->GetItem( i,&hd);
			hd.mask |= HDI_TEXT;
			hd.pszText = vItemText[i].GetBuffer(0);
			hd.cchTextMax = 128;
			m_listSource.GetHeaderCtrl()->SetItem( i, &hd );
			//Update ListCtrl Item Label
			for(int j=0;j<m_listMapping.GetItemCount();j++)
			{
				if(m_bColumnDropped[j])
				{
					DWORD nItemData=m_listMapping.GetItemData(j);
					if(nItemData==i)
						m_listMapping.SetItemText(j,0,CString(hd.pszText));
				}
			}
		}
		m_listSourceColumn.ResetContent() ;
		for(int j=1;j<m_listSource.GetHeaderCtrl()->GetItemCount();j++)
		{
			m_listSource.GetHeaderCtrl()->GetItem( j,&hd);
			int nIndex=m_listSourceColumn.InsertString(j-1 ,CString(hd.pszText));
			m_listSourceColumn.SetItemData(nIndex,j);
		}
		
		m_bHaveAssignTitle = true;
	}
	
}

void CImportFlightFromFileDlg::OnRclickListResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint pt;
	::GetCursorPos( &pt );
	CPoint pt2menu = pt;
	m_listResult.ScreenToClient( &pt );

	// get the item index and the sub item 
	LVHITTESTINFO testinfo;
	testinfo.pt = pt;
	LRESULT result = m_listResult.SendMessage( LVM_SUBITEMHITTEST, 0,(LPARAM)(LVHITTESTINFO FAR *)&testinfo );
	if( result != -1 )
	{
		if( testinfo.iSubItem == 12 || testinfo.iSubItem == 13 || testinfo.iSubItem == 14 
			|| testinfo.iSubItem == 16|| testinfo.iSubItem == 17|| testinfo.iSubItem == 18)
		{
			if( testinfo.iItem>=m_listResult.GetItemCount())
				return;
			CString strItem = m_listResult.GetItemText( testinfo.iItem, testinfo.iSubItem );
			strItem.MakeUpper();
			strItem.TrimLeft();
			strItem.TrimRight();
			if( strItem.IsEmpty() )
				return;

			// get item data  then just the sub item if conver from user name
			DWORD dwItemData = m_listResult.GetItemData( testinfo.iItem );
			CString strUserName,strModelName;
			strUserName = strItem;
			strModelName = strMsg1;
			switch(testinfo.iSubItem) 
			{
			case 12:
				if( (dwItemData&STANDGATEFLAG) == STANDGATEFLAG )
				{
					strModelName = strItem;
					m_pInTerm->m_pUserNameListDB->getUserNameByProcessorName( strItem, strUserName );
				}
				break;
			case 13:
				if( (dwItemData&STANDGATEFLAG) == STANDGATEFLAG )
				{
					strModelName = strItem;
					m_pInTerm->m_pUserNameListDB->getUserNameByProcessorName( strItem, strUserName );
				}
				break;
			case 14:
				if( (dwItemData&STANDGATEFLAG) == STANDGATEFLAG )
				{
					strModelName = strItem;
					m_pInTerm->m_pUserNameListDB->getUserNameByProcessorName( strItem, strUserName );
				}
				break;
			case 16:
				if( (dwItemData&ARRGATEFLAG) == ARRGATEFLAG )
				{
					strModelName = strItem;
					m_pInTerm->m_pUserNameListDB->getUserNameByProcessorName( strItem, strUserName );
				}
				break;
			case 17:
				if( (dwItemData&DEPGATEFLAG) == DEPGATEFLAG )
				{
					strModelName = strItem;
					m_pInTerm->m_pUserNameListDB->getUserNameByProcessorName( strItem, strUserName );
				}
				break;
			case 18:
				if( (dwItemData&BAGDEVICELAG) == BAGDEVICELAG )
				{
					strModelName = strItem;
					m_pInTerm->m_pUserNameListDB->getUserNameByProcessorName( strItem, strUserName );
				}
				break;
			}

			// build a menu to show the information
			strModelName = "Model Name: " + strModelName;
			strUserName  = " User Name: " + strUserName;
			CMenu* pMenu = new CMenu;
			pMenu->CreatePopupMenu();
			pMenu->AppendMenu( MF_STRING|MF_DISABLED, 0, strUserName );
			pMenu->AppendMenu( MF_STRING|MF_DISABLED, 0, strModelName );
			pMenu->TrackPopupMenu(TPM_RIGHTBUTTON ,pt2menu.x, pt2menu.y,this );
			delete pMenu;
		}
	}

	*pResult = 0;
}


void CImportFlightFromFileDlg::getMoreInfoFromList( Flight* _pFlight,int _iItemIdx)
{
	assert( _pFlight );

	FlightConstraint arrFlightCon, depFlightCon;
	FlightConstraint* _pArrFlightCon = NULL;
	FlightConstraint* _pDepFlightCon = NULL;

	if( _pFlight->isTurnaround() )
	{
		arrFlightCon	= _pFlight->getType('A');
		_pArrFlightCon	= &arrFlightCon;
		depFlightCon	= _pFlight->getType('D');
		_pDepFlightCon	= &depFlightCon;	
	}
	else if( _pFlight->isArriving() )
	{
		arrFlightCon	= _pFlight->getType('A');
		_pArrFlightCon = &arrFlightCon;
		_pDepFlightCon = NULL;
	}
	else if( _pFlight->isDeparting() )
	{
		depFlightCon	= _pFlight->getType('D');
		_pDepFlightCon	= &depFlightCon;	
		_pArrFlightCon	= NULL;
	}
	else
	{
		_pArrFlightCon = NULL;
		_pDepFlightCon = NULL;
	}
	
	//////////////////////////////////////////////////////////////////////////
	CString strItem;
	int inum;
	// aircraft seats (Aircraft Capacities )
	//strItem = m_listResult.GetItemText( _iItemIdx, 12);
	strItem = m_listResult.GetItemText( _iItemIdx, 19);
	strItem.Remove(' ');
	if( !strItem.IsEmpty() &&  isNumString(strItem) )
	{
		inum = atoi( strItem );
		
		FlightConstraintDatabase* pAC = m_pInTerm->flightData->getCapacity();
		// first delete old entry
		int iCount = pAC->getCount();
		int _iDelCount = 0;
		for( int i =iCount-1; i>=0 && _iDelCount<2; i-- )
		{
			const FlightConstraint* pflight = pAC->getConstraint( i );
			if( _pArrFlightCon && _pArrFlightCon->isEqual( pflight ) )
			{
				pAC->deleteItem( i );
				_iDelCount++;
			}
			if( _pDepFlightCon && _pDepFlightCon->isEqual( pflight ) )
			{
				pAC->deleteItem( i );
				_iDelCount++;
			}
		}
		
		// add new entry
		if( _pArrFlightCon )
		{
			FlightConstraint* pFltCon = new FlightConstraint;
			*pFltCon = *_pArrFlightCon;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( inum );
			pNewEntry->initialize( pFltCon, pDist );
			pAC->addEntry( pNewEntry );
		}
		if( _pDepFlightCon )
		{
			FlightConstraint* pFltCon = new FlightConstraint;
			*pFltCon = *_pDepFlightCon;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( inum );
			pNewEntry->initialize( pFltCon, pDist );
			pAC->addEntry( pNewEntry );
		}

	}
	
	//Load factors
	//strItem = m_listResult.GetItemText( _iItemIdx,13);
	strItem = m_listResult.GetItemText( _iItemIdx,20);
	strItem.Remove(' ');
	if( !strItem.IsEmpty() &&  isNumString(strItem) )
	{
		inum = atoi( strItem );
		
		FlightConstraintDatabase* pLoad = m_pInTerm->flightData->getLoads();
		// first delet old entry
		int iCount = pLoad->getCount();
		int _iDelCount = 0;
		for( int i=iCount-1; i>=0 && _iDelCount<2; i-- )
		{
			const FlightConstraint* pFlight = pLoad->getConstraint( i );
		
			if( _pArrFlightCon && _pArrFlightCon->isEqual( pFlight ) )
			{
				pLoad->deleteItem( i );
				_iDelCount++;
			}
			if( _pDepFlightCon && _pDepFlightCon->isEqual( pFlight ) )
			{
				pLoad->deleteItem( i );
				_iDelCount++;
			}
		}
		
		// add new entry
		if( _pArrFlightCon )
		{
			FlightConstraint* pFltCon = new FlightConstraint;
			*pFltCon = *_pArrFlightCon;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( inum );
			pNewEntry->initialize( pFltCon, pDist );
			pLoad->addEntry( pNewEntry );
		}
		if( _pDepFlightCon )
		{
			FlightConstraint* pFltCon = new FlightConstraint;
			*pFltCon = *_pDepFlightCon;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( inum );
			pNewEntry->initialize( pFltCon, pDist );
			pLoad->addEntry( pNewEntry );
		}
	}
	//Stand to Terminal Times
	//the number of busses needed
	
	//m_pInTerm->flightData->saveDataSet(  m_strProjPath, true );
}

bool CImportFlightFromFileDlg::isNumString( const CString& _strNum )
{
	for( int i=0; i<_strNum.GetLength(); i++ )
		if( !isNumeric(_strNum[i]) )
			return false;

	return true;
}


bool CImportFlightFromFileDlg::isProcessor( const CString& _strProcName )
{
	if( m_pInTerm->procList->getProcessor( _strProcName )!= NULL )
		return true;
	return false;
}
int CImportFlightFromFileDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_wndToolBarLeft.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarLeft.LoadToolBar(IDR_NAMELIST_TEXT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here
	
	return 0;
}

void CImportFlightFromFileDlg::OnDeleteRow()
{

	POSITION pos=m_listSource.GetFirstSelectedItemPosition();
	while (pos) 
	{
		int nIndex=m_listSource.GetNextSelectedItem(pos);
		m_listSource.DeleteItem(nIndex);
		break;
	}
}

void CImportFlightFromFileDlg::OnDeleteColumn()
{
	char arBuf[256];
	LVCOLUMN lvCol;
	lvCol.mask=LVCF_TEXT;
	lvCol.pszText=arBuf;
	lvCol.cchTextMax=256;
	m_listSource.GetColumn(m_nColIndexClicked,&lvCol);
	CString strColumn=lvCol.pszText;
	
	m_listSource.DeleteColumn(m_nColIndexClicked);
	m_nColIndexClicked=-1;
	m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
	//Remove Item From ListBox or ListCtrl
	int nCount=m_listSourceColumn.GetCount();
	for(int i=0;i<nCount;i++)
	{
		CString strTemp;
		m_listSourceColumn.GetText(i,strTemp);
		if(strTemp==strColumn)
		{
			m_listSourceColumn.DeleteString(i);
			OnSelchangeListSourceColumn();
			return;
		}
	}

	nCount=m_listMapping.GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		CString strTemp=m_listMapping.GetItemText(i,0);
		if(strTemp==strColumn)
		{
			m_listMapping.SetItemText(i,0,"");
			OnClickListMapping(NULL,NULL);
			return;
		}
	}
	
}

BOOL CImportFlightFromFileDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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

void CImportFlightFromFileDlg::OnSelchangeListSourceColumn() 
{
	// TODO: Add your control notification handler code here
	if(m_bFullCompatibleFormat) return;
	if(m_listSourceColumn.GetCurSel()==-1)
	{
		m_btnMoveToRight.EnableWindow(FALSE);
		return;
	}
	POSITION pos=m_listMapping.GetFirstSelectedItemPosition();
	CString strItem;
	while(pos)
	{
		int nItem=m_listMapping.GetNextSelectedItem(pos);
		strItem=m_listMapping.GetItemText(nItem,0);
		m_btnMoveToRight.EnableWindow(strItem.IsEmpty());
		return;
	}
	m_btnMoveToRight.EnableWindow(FALSE);
}

void CImportFlightFromFileDlg::OnClickListMapping(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(m_bFullCompatibleFormat) return;

	POINT pt;
	GetCursorPos(&pt);
	CPoint point(pt.x,pt.y);
	m_listMapping.ScreenToClient(&point);
	int nResult=m_listMapping.HitTest(point);
	if(nResult==-1)
	{
		m_btnMoveToLeft.EnableWindow(FALSE);
		m_btnMoveToRight.EnableWindow(FALSE);
	}
	else
	{
		CString strItem=m_listMapping.GetItemText(nResult,0);
		m_btnMoveToLeft.EnableWindow(!strItem.IsEmpty());
		m_btnMoveToRight.EnableWindow(strItem.IsEmpty()&&m_listSourceColumn.GetCurSel()!=-1);
	}
	if(pResult)
		*pResult = 0;
}

void CImportFlightFromFileDlg::OnButtonMovetoRight() 
{
	POSITION pos = m_listMapping.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_listMapping.GetNextSelectedItem(pos);
		CString strItem;
		int nCurSel=m_listSourceColumn.GetCurSel();
		m_listSourceColumn.GetText(nCurSel,strItem);
		m_listMapping.SetItemText(nItem,0,strItem);
		m_listMapping.SetItemData(nItem,m_listSourceColumn.GetItemData(nCurSel));
		m_listSourceColumn.DeleteString(nCurSel);
		m_btnMoveToLeft.EnableWindow(TRUE);
		m_btnMoveToRight.EnableWindow(FALSE);
		m_bColumnDropped[nItem]=TRUE;
		break;
	}
	EnableCompileBtn();
}



void CImportFlightFromFileDlg::OnButtonMovetoLeft() 
{
	POSITION pos = m_listMapping.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_listMapping.GetNextSelectedItem(pos);
		CString strItem=m_listMapping.GetItemText(nItem,0);
		int nIndex=m_listSourceColumn.AddString(strItem);
		m_listSourceColumn.SetItemData(nIndex,m_listMapping.GetItemData(nItem));
		m_listMapping.SetItemText(nItem,0,"");
		m_btnMoveToRight.EnableWindow(FALSE);
		m_btnMoveToLeft.EnableWindow(FALSE);
		m_bColumnDropped[nItem]=FALSE;
		break;
	}
	EnableCompileBtn();
}

void CImportFlightFromFileDlg::OnCheckFullCompatibleFormat() 
{
	// TODO: Add your control notification handler code here
	
	if(m_bFullCompatibleFormat)
	{
		m_btnMoveToRight.EnableWindow(FALSE);
		m_btnMoveToLeft.EnableWindow(FALSE);
		m_listSourceColumn.ResetContent();
		int nCountMapping=m_listMapping.GetItemCount();
		int nCountColumn=m_listSource.GetHeaderCtrl()->GetItemCount()-1;
		int nMinCount=min(nCountMapping,nCountColumn);
		for(int i=0;i<nCountMapping;i++)
		{
			m_listMapping.SetItemText(i,0,"");
			if(i<nMinCount)
			{
				char arBuf[256];
				LVCOLUMN lvCol;
				lvCol.mask=LVCF_TEXT;
				lvCol.pszText=arBuf;
				lvCol.cchTextMax=256;
				m_listSource.GetColumn(i+1,&lvCol);
				CString strItem=lvCol.pszText;
				m_listMapping.SetItemText(i,0,strItem);
				m_listMapping.SetItemData(i,i+1);
				m_bColumnDropped[i]=TRUE;
			}
		}
	}
	else
	{

	}
	EnableCompileBtn();
}

LRESULT CImportFlightFromFileDlg::OnHeaderCtrlClickColumn(WPARAM wParam,LPARAM lParam)
{
	m_nColIndexClicked=wParam;
	if(wParam!=-1 && wParam!=0 )	// can not delete the first column
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,TRUE);
	else
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
	return TRUE;
}


void CImportFlightFromFileDlg::OnComboxSelectChanged()
{
	CString strFileName;
	m_editFileName.GetWindowText(strFileName);
	if( strFileName.IsEmpty() )
		return;
	
	if( ReadFile( strFileName ) )
	{
		// add flight schedule to edit
		for( int i = 0; i<COLUMN_NUM; i++ )
			m_bColumnDropped[i] = FALSE;
		AddFlightToEdit();
		
		m_listResult.DeleteAllItems();
		ClearFlightVect();
		m_butOK.EnableWindow( FALSE );
		m_btnAppend.EnableWindow( FALSE );
		
		m_bHaveAssignTitle = false;
		
	}
}

//add by adam 2007-05-25
void CImportFlightFromFileDlg::GetMinStartDate(CStartDate& minStartDate)
{
	CString strArrDate;
	CString strDepDate;

	bool ArrTimeSecond = false; 
	bool DepTimeSecond =false;

	int iItemCount = m_listSource.GetItemCount();

	for (int i=0; i<iItemCount; i++)
	{
		//no arrival day and depature day
		if (!m_bColumnDropped[4] && !m_bColumnDropped[9])
		{
			minStartDate.SetAbsoluteDate(false);
			break;
		}
		else
		{
			if ((int)m_listMapping.GetItemData(4) >= 0)
			{
				strArrDate = m_listSource.GetItemText( i, m_listMapping.GetItemData(4));
			}
			if((int)m_listMapping.GetItemData(9)>=0)
			{
				strDepDate = m_listSource.GetItemText( i, m_listMapping.GetItemData(9));
			}

		

			
			COleDateTime dateTimeArr;
			COleDateTime dateTimeDep;

			//arr date
			char chSplit = 0;
			if (!strArrDate.IsEmpty())
			{
				chSplit = 0;
				if (strArrDate.Find('-') != -1)
					chSplit = '-';
				else if (strArrDate.Find('/') != -1)
					chSplit = '/';
				else
				{
					minStartDate.SetAbsoluteDate(false);
					return;
				}

				if (chSplit != 0)
				{
					int nNum[3];
					int nPrevPos = 0;
					int nPos = 0;
					int i = 0;
					while (((nPos = strArrDate.Find(chSplit, nPos + 1)) != -1) && (i < 3))
					{
						nNum[i++] = atoi(strArrDate.Mid(nPrevPos, nPos - nPrevPos));
						nPrevPos = nPos + 1;
					}

					nNum[i] = atoi(strArrDate.Mid(nPrevPos, strArrDate.GetLength() - nPrevPos));

					if (nNum[0] > 100)
					{
						dateTimeArr.SetDate(nNum[0], nNum[1], nNum[2]);
					}
					else
					{
						//usa date format
						if (m_DataFormatControl.GetCurSel()==0)
						{
							dateTimeArr.SetDate(nNum[2], nNum[0], nNum[1]);
						}
						//european date format
						else if (m_DataFormatControl.GetCurSel()==1)
						{
							dateTimeArr.SetDate(nNum[2], nNum[1], nNum[0]);
						}
						else
						{
							ASSERT(FALSE);
						}
					}
				}
			}

			//depDate
			if (!strDepDate.IsEmpty())
			{
				chSplit = 0;
				if (strDepDate.Find('-') != -1)
					chSplit = '-';
				else if (strDepDate.Find('/') != -1)
					chSplit = '/';
				else
				{
					minStartDate.SetAbsoluteDate(false);
					return;
				}

				if (chSplit != 0)
				{
					int nNum[3];
					int nPrevPos = 0;
					int nPos = 0;
					int i = 0;
					while (((nPos = strDepDate.Find(chSplit, nPos + 1)) != -1) && (i < 3))
					{
						nNum[i++] = atoi(strDepDate.Mid(nPrevPos, nPos - nPrevPos));
						nPrevPos = nPos + 1;
					}

					nNum[i] = atoi(strDepDate.Mid(nPrevPos, strDepDate.GetLength() - nPrevPos));

					if (nNum[0] > 100)
					{
						dateTimeDep.SetDate(nNum[0], nNum[1], nNum[2]);
					}
					else
					{
						//usa date format
						if (m_DataFormatControl.GetCurSel()==0)
						{
							dateTimeDep.SetDate(nNum[2], nNum[0], nNum[1]);
						}
						//european date format
						else if (m_DataFormatControl.GetCurSel()==1)
						{
							dateTimeDep.SetDate(nNum[2], nNum[1], nNum[0]);
						}
						else
						{
							ASSERT(FALSE);
						}
					}
				}
			}				

			COleDateTime dateTimeMin;
			if (!strArrDate.IsEmpty() && !strDepDate.IsEmpty())
			{
				dateTimeMin = (dateTimeArr<dateTimeDep) ? dateTimeArr : dateTimeDep;
			}
			else if (!strArrDate.IsEmpty())
			{
				dateTimeMin = dateTimeArr;
			}
			else if (!strDepDate.IsEmpty())
			{
				dateTimeMin = dateTimeDep;
			}
			else
			{
				ASSERT(FALSE);
				continue;
			}

			//set start date
			if (minStartDate.IsAbsoluteDate())
			{
				COleDateTime dateTimeStartDate = minStartDate.GetDate();

				if (dateTimeMin < dateTimeStartDate)
				{
					minStartDate.SetDate(dateTimeMin);
				}
			}
			else
			{
				minStartDate.SetAbsoluteDate(true);
				minStartDate.SetDate(dateTimeMin);
			}
		}
	}	
}
//End add by adam 2007-05-25

// Composite the flight string from the GUI, and Generate the Flight.
void CImportFlightFromFileDlg::OnCompile() 
{
	//////////////////////////////////////////////////////////////////////////
	// user drag content to right list

	//add by adam 2007-05-11
	UpdateData(TRUE);
	CStartDate convertStartDate;

	//read from *.csv file
	if (!m_bArctermFile)
	{
		GetMinStartDate(convertStartDate);
	}
	
	//COleDateTime startDate;
	//startDate.SetDate(m_StartData.GetYear(), m_StartData.GetMonth(), m_StartData.GetDay());
	//convertStartDate.SetAbsoluteDate(true);
	//convertStartDate.SetDate(startDate);
	//End add by adam 2007-05-11

	CWaitCursor wc;
	int iSuccess = 0;
	
	std::vector<int> vAirports;
	InputAirside::GetAirportList(m_nPrjID,vAirports);
	CStringConvertToFlight convert( m_pInTerm, vAirports.at(0) );
	
	Flight* p_flight = NULL;
	ArctermFile file;
	// create flight object from list_ctrl
	int iItemCount = m_listSource.GetItemCount();
	CArray<int,int> arrayDeletedItem;
	m_strResultInfo+=CString("--------------------------------------------------------Convert Result--------------------------------------------------------\r\n");
	CString strTemp;
	strTemp.Format("Convert(%d Lines)...\r\n",iItemCount);
	m_strResultInfo+=strTemp;

	bool ArrTimeSecond = false; 
	bool DepTimeSecond =false;
	for( int i=0; i<iItemCount; i++ )
	{
		CString strFlight;
		CString strItem;
		ArrTimeSecond = false; 
		DepTimeSecond =false;

		for( int j =0; j<22; j++ )
		{
			if( !m_bColumnDropped[j] )
			{
				if( j== 12 || j== 13 || j==14|| j == 16 || j == 17||j == 18 )
				{
					strFlight = strFlight + "All Processors,";
				}
				else if( j==15 )
				{
					strFlight = strFlight + "0:30,";

					strFlight = strFlight += "0:00,";		//arr parking time
					strFlight = strFlight += "0:00,";		//dep parking time
					strFlight = strFlight += "0:00,";		//int parking time
				}
				else
				{
					strFlight += ",";
				}
			}
			else
			{
				
				strItem = m_listSource.GetItemText( i, GetCurrentListColumnIndex(m_listMapping.GetItemText(j,0)) );
				if( strItem.IsEmpty() )
				{
					if(  j== 12 || j== 13 || j==14|| j == 16 || j == 17|| j == 18  )
					{
						strFlight = strFlight + "All Processors,";
					}
					else if( j==15 )
					{
						strFlight = strFlight + "0:30,";

						strFlight = strFlight += "0:00,";		//arr parking time
						strFlight = strFlight += "0:00,";		//dep parking time
						strFlight = strFlight += "0:00,";		//int parking time
					}
					else
					{
						strFlight += ",";
					}
				}
				else 
				{
					strFlight = strFlight + strItem + ",";

					if(j==10)
						if(strItem.GetLength()>5)  DepTimeSecond=true;
					if(j==5)
						if(strItem.GetLength()>5)  ArrTimeSecond =true;

					if( j==15 )
					{
						strFlight = strFlight += "0:00,";		//arr parking time
						strFlight = strFlight += "0:00,";		//dep parking time
						strFlight = strFlight += "0:00,";		//int parking time
					}
				}
			}
		}
		strFlight = strFlight.Left( strFlight.GetLength()-1 );
		
		//m_StartData = COleDateTime::GetCurrentTime();
		CString strErrorMsg ;
		p_flight = convert.ConvertToFlight( strFlight, file, *m_pInTerm->m_pGateMappingDB,ArrTimeSecond,DepTimeSecond, convertStartDate ,strErrorMsg, (DATAFORMAT)m_DataFormatControl.GetCurSel());
		if( p_flight != NULL 
			&& p_flight->getArrTime() >= 0l
			&& p_flight->getDepTime() >= 0l
			&& p_flight->getServiceTime() >= 0l)	// have no error in the string
		{
			iSuccess++;
			m_vectFlight.push_back( p_flight );
			//////////////////////////////////////////////////////////////////////////
			//Fill Item Text To RightList From LeftList
			int _index = AddAFlightToList(p_flight);
			AddExtendInfoToList( i, _index );
			arrayDeletedItem.Add(i);
			strTemp.Format("Line %d: Succeed\r\n",i+1);
			
		}
		else
			strTemp.Format("Line %d: Failed - %s\r\n",i+1,strErrorMsg);
		m_strResultInfo+=strTemp;
		m_editResultInfo.SetWindowText(m_strResultInfo);
	}
	for(int i=arrayDeletedItem.GetSize();i>0;i--)
		m_listSource.DeleteItem(arrayDeletedItem[i-1]);
	strTemp.Format( "---------------------------------------------Total Lines:%d    Succeed:%d    Failed:%d---------------------------------------------\r\n\r\n\r\n" ,iItemCount, iSuccess, iItemCount-iSuccess);
	m_strResultInfo+=strTemp;
	m_editResultInfo.SetWindowText(m_strResultInfo);
	int nLineCount=m_editResultInfo.GetLineCount();
	m_editResultInfo.LineScroll(nLineCount);

	wc.Restore();
	m_butOK.EnableWindow(m_vectFlight.size()>0);
	m_btnAppend.EnableWindow(m_vectFlight.size()>0);
}

void CImportFlightFromFileDlg::EnableCompileBtn()
{
	BOOL bEnable = TRUE;

	//	for arr id
	for (int i = 0; i < 6; i++)
	{
		if( i == 3)
			continue;

		if (m_bColumnDropped[i] == FALSE)
		{
			bEnable = FALSE;
			break;
		}
	}

	// for dep
	if (!bEnable)
	{
		bEnable = TRUE;
		for(int j = 6; j < 11; j++)
		{
			if (j == 7 || j == 9)
				continue;

			if(m_bColumnDropped[j]==FALSE)
			{
				bEnable=FALSE;
				break;
			}
		}
	}
	
	m_btnCompile.EnableWindow(bEnable);
	
}

void CImportFlightFromFileDlg::OnSelchangeComboImportedFileFormat() 
{
	// TODO: Add your control notification handler code here
	OnComboxSelectChanged();	
}

void CImportFlightFromFileDlg::OnAppend() 
{
	// TODO: Add extra validation here
	//if(!CheckTheFlightActype())
	//	return ;
	CWaitCursor wc;
	for( int i=0; i<static_cast<int>(m_vectFlight.size()); i++ )
	{
		m_pInTerm->flightSchedule->addItem( m_vectFlight[i] );
	}
	int nItemCount=m_listResult.GetItemCount();
	for(int j=0;j<nItemCount;j++)
	{
		Flight* pFlight=(Flight*)m_listResult.GetItemData(j);
		getMoreInfoFromList( pFlight,j);	
	}
	m_pInTerm->flightData->saveDataSet(  m_strProjPath, true );

	CDialog::OnOK();
	
}

void CImportFlightFromFileDlg::AddExtendInfoToList( int _sourceIdx, int _destIdx )
{
	//seats info
	//load info
	//Stand to Terminal Times
	//the number of busses needed
	for( int i=22; i<COLUMN_NUM; i++ )
	{
		int _iColumn = m_listMapping.GetItemData( i );
		if( _iColumn > 0 )	// have be set
		{
			CString strItemText = m_listSource.GetItemText( _sourceIdx, _iColumn );
			m_listResult.SetItemText( _destIdx, i, strItemText );
		}
	}
}

void CImportFlightFromFileDlg::OnOpenFromFile()
{
	m_btnOpen.SetOperation(0);
	m_btnOpen.SetWindowText(_T("From File"));
	OnOpenClick();
}

void CImportFlightFromFileDlg::OnOpenFromSQL()
{
	m_btnOpen.SetOperation(1);
	m_btnOpen.SetWindowText(_T("From SQL"));
	OnOpenClick();
}

void CImportFlightFromFileDlg::OnOpenFromArcCapture()
{
	m_btnOpen.SetOperation(2);
	m_btnOpen.SetWindowText(_T("From ArcCapture"));
	OnOpenClick();
}

void CImportFlightFromFileDlg::OnOpenClick()
{
	COpenSqlDlg dlg;
	int iOperation = m_btnOpen.GetOperation();
	switch( iOperation )
	{
	case 0:	//Open from file
		Openfile();
		break;
	case 1:	//open from sql
		if (dlg.DoModal() == IDOK)
		{
		}
		break;
	case 2:	//open from arccapture
		dlg.SetCommonSql(FALSE);
		if (dlg.DoModal() == IDOK)
		{
		}
		break;
	default:
		assert(0);
	}
}

void CImportFlightFromFileDlg::SetFlightScheduleDlg(BOOL bValue)
{
	m_bIsFlightScheduleDlg = bValue;
}

BOOL CImportFlightFromFileDlg::IsFlightScheduleDlg()
{
	return m_bIsFlightScheduleDlg;
}
int CImportFlightFromFileDlg::GetCurrentListColumnIndex(CString& colum_name)
{

	TCHAR name[1024] = {0} ;
	   LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT|LVCF_SUBITEM;
		lvc.pszText = name;
		lvc.cchTextMax = 1024;

	int size =  m_listSource.GetHeaderCtrl()->GetItemCount() ;
    int *coloumnArrary = new int[sizeof(int)*size] ; 
	m_listSource.GetColumnOrderArray(coloumnArrary,size) ;
	 for(int i = 0 ; i < size ;i++)
	   {
		   m_listSource.GetColumn(coloumnArrary[i],&lvc) ;
		   if(_tcscmp(lvc.pszText,colum_name) == 0)
		   {
			   i = coloumnArrary[i] ;
			   delete []coloumnArrary ;
			   return i ;
		   }
	   }
	   if(coloumnArrary != NULL)
		   delete []coloumnArrary ;
	   return 0 ;
}