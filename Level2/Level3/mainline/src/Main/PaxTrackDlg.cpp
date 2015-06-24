// PaxTrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxTrackDlg.h"
#include "engine\terminal.h"
#include "engine\proclist.h"
#include <CommonData/procid.h>
#include "common\states.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxTrackDlg dialog


CPaxTrackDlg::CPaxTrackDlg(Terminal* _pTerm, CWnd* pParent /*=NULL*/)
	: CDialog(CPaxTrackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxTrackDlg)
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
	m_pTerm = _pTerm;
}


void CPaxTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxTrackDlg)
	DDX_Control(pDX, IDC_LIST_TRACK, m_listTrack);
	DDX_Text(pDX, IDC_STATIC_PAXID, m_strTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxTrackDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxTrackDlg)
	ON_BN_CLICKED(ID_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxTrackDlg message handlers

BOOL CPaxTrackDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitListCtrl();
	LoadTrackToList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPaxTrackDlg::InitListCtrl( void )
{
	// set list_ctrl	
	DWORD dwStyle = m_listTrack.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listTrack.SetExtendedStyle( dwStyle );
	
	// insert column
	m_listTrack.InsertColumn( 0,"Time", LVCFMT_CENTER, 80 );
	m_listTrack.InsertColumn( 1,"Processor", LVCFMT_CENTER, 200 );
	m_listTrack.InsertColumn( 2,"Status", LVCFMT_CENTER, 120);
}


bool CPaxTrackDlg::Init( long _lLogIndex, Terminal* _pTerm, const CString& _strPath  )
{
	bool _bInit = m_PaxTrack.initByIndex( _lLogIndex, _pTerm, _strPath );
	if( _bInit )
		m_strTitle.Format("Passenger( %ld )'s track:", m_PaxTrack.getPaxID() );

	return _bInit;
}

void CPaxTrackDlg::LoadTrackToList( void )
{
	m_listTrack.DeleteAllItems();
	
	long _lCount = m_PaxTrack.getEventCount();
	for( long l=0; l<_lCount; l++ )
	{
		const MobEventStruct& _struct = m_PaxTrack.getEvent( l );
		
		ElapsedTime time;
		time.setPrecisely( _struct.time );
		int _iIdx = m_listTrack.InsertItem( m_listTrack.GetItemCount(), time.printTime() );

		CString strProceesorID;
		if( _struct.procNumber != -1 )
		{
			Processor* _pProc = m_pTerm->procList->getProcessor( _struct.procNumber );
			if( _pProc )
				strProceesorID = _pProc->getID()->GetIDString();
		}
		m_listTrack.SetItemText( _iIdx, 1, strProceesorID );

		CString strStatus;
		if( _struct.state>=0 && _struct.state < EVENT_ENUM_MAX )
			strStatus = STATE_NAMES[ _struct.state];
		m_listTrack.SetItemText( _iIdx, 2, strStatus );
	}
}

void CPaxTrackDlg::SaveTrackToFile( const CString& _strName )
{
	CString strMsg;

	fsstream file( _strName, stdios::out );
	if( file.bad() )
	{
		strMsg.Format(" Can not save track to file:%s", _strName );	
		MessageBox( strMsg );
		return;
	}
	file << m_strTitle.GetBuffer(0) << "\r\n";
	
	long _lCount = m_PaxTrack.getEventCount();
	for( long l=0; l<_lCount; l++ )
	{
		const MobEventStruct& _struct = m_PaxTrack.getEvent( l );
		
		ElapsedTime time;
		time.setPrecisely( _struct.time );
		file<<time.printTime().GetBuffer(0) << "\t"; 
		
		CString strProceesorID;
		if( _struct.procNumber != -1 )
		{
			Processor* _pProc = m_pTerm->procList->getProcessor( _struct.procNumber );
			if( _pProc )
				strProceesorID = _pProc->getID()->GetIDString();
		}
		file<< strProceesorID.GetBuffer(0) << "\t";
			
		CString strStatus;
		if( _struct.state>=0 && _struct.state < EVENT_ENUM_MAX )
			strStatus = STATE_NAMES[ _struct.state];
		file<< strStatus.GetBuffer(0) <<"\r\n";
	}

//	strMsg.Format("Have saved track to file:%s", _strName );
//	MessageBox( strMsg );
	return;
}

void CPaxTrackDlg::OnButtonSave() 
{
	CFileDialog fileDlg( FALSE, ".txt","pax track", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text file (*.txt)|*.txt||" , NULL, 0, FALSE );

	if( fileDlg.DoModal() == IDOK )
	{
		CString strFileName = fileDlg.GetPathName();
		SaveTrackToFile( strFileName );
	}
}
