// TrackWnd.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TrackWnd.h"
#include "engine\terminal.h"
#include "engine\proclist.h"
#include <CommonData/procid.h>
#include "common\states.h"
#include "common\enginediagnose.h"
#include "results\paxdestdiagnoselog.h"
#include ".\trackwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrackWnd

CTrackWnd::CTrackWnd(Terminal* _pTerm,CString strProjPath,CWnd* pParent)
		  : m_DestDiagnoseInfo( _pTerm )
		  , m_pUsedParentWnd(pParent)
{
	m_pTerm				= _pTerm;
	m_strProjPath		= strProjPath;
	m_pDiagnoseEntry	= NULL;

	m_bClickSave		= FALSE;
	m_bWndClosing		= FALSE;

	m_nItem				= -1;
	m_nSubItem			= -1;


	LPCTSTR className	=AfxRegisterWndClass(CS_DBLCLKS, 
											 AfxGetApp()->LoadStandardCursor(IDC_ARROW), 
											 (HBRUSH)GetSysColorBrush(COLOR_3DFACE), 0);
	VERIFY(CreateEx(0, className, NULL, WS_POPUP | WS_BORDER, CRect(0,0,400,300), pParent, 0, NULL));
}

CTrackWnd::~CTrackWnd()
{
}


BEGIN_MESSAGE_MAP(CTrackWnd, CWnd)
	//{{AFX_MSG_MAP(CTrackWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(IDOK, OnOK)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTrackWnd message handlers

void CTrackWnd::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
// 	delete this;
	CWnd::PostNcDestroy();
}

int CTrackWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_staticPaxID.Create("PaxID:",WS_CHILD|WS_VISIBLE,CRect(0,0,400,16),this);

	// set list_ctrl	
	m_listTrack.Create(LVS_REPORT|LVS_SHOWSELALWAYS|WS_CHILD|WS_VISIBLE,CRect(0,16,400,250),this,2);
	DWORD dwStyle = m_listTrack.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listTrack.SetExtendedStyle( dwStyle );

	m_btnOK.Create("Close",WS_CHILD|WS_VISIBLE,CRect(CPoint(315,260),CSize(75,23)),this,IDOK);

	m_btnSave.Create("Save",WS_CHILD|WS_VISIBLE,CRect(CPoint(230,260),CSize(75,23)),this,ID_BUTTON_SAVE);

	CFont* pFont = CWnd::GetParent()->GetFont();
	m_staticPaxID.SetFont(pFont);
	m_listTrack.SetFont(pFont);
	m_btnOK.SetFont(pFont);
	m_btnSave.SetFont(pFont);

	return 0;
}


bool CTrackWnd::Init( Terminal* _pTerm, const CString& _strPath  )
{
	bool	_bResult = false;
	switch( m_pDiagnoseEntry->eType )
	{
	case MISS_FLIGHT:
		_bResult = m_PaxTrack.initByIndex(m_pDiagnoseEntry->lData, _pTerm, _strPath );
		if( _bResult )
			m_strTitle.Format("Passenger( %ld )'s track:", m_PaxTrack.getPaxID() );
		break;
		
	case PAXDEST_DIAGNOS:
		_bResult = m_pTerm->m_pDiagnosLog->getDiagnoseInfo( _pTerm, _strPath, m_pDiagnoseEntry->lData, &m_DestDiagnoseInfo );
		if( _bResult )
		{
			ElapsedTime time;
			time.setPrecisely( m_DestDiagnoseInfo.getDiagnoseTime() );
			m_strTitle.Format("Passenger(%ld) at processor(%s) when (%s)",
							   m_DestDiagnoseInfo.getDiagnosePaxID(), m_DestDiagnoseInfo.getSrcProcessor(), time.printTime() );
		}
		break;

	default:
		break;
	}
	
	return _bResult;
}



void CTrackWnd::InitCtrl( void )
{
	assert( m_pDiagnoseEntry );

	int nColCount = m_listTrack.GetHeaderCtrl()->GetItemCount();
	while (nColCount--)
	{
		m_listTrack.DeleteColumn(nColCount);
	}
	// insert column
	switch( m_pDiagnoseEntry->eType )
	{
	case MISS_FLIGHT:
		m_listTrack.InsertColumn( 0,"Time", LVCFMT_CENTER, 80 );
		m_listTrack.InsertColumn( 1,"Processor", LVCFMT_CENTER, 200 );
		m_listTrack.InsertColumn( 2,"Status", LVCFMT_CENTER, 120);
		m_btnSave.ShowWindow( SW_SHOW );
		break;
		
	case PAXDEST_DIAGNOS:
		m_listTrack.InsertColumn( 0,"Destination Processor",LVCFMT_CENTER, 150);
		m_listTrack.InsertColumn( 1,"Reason",LVCFMT_CENTER, 250);
		m_btnSave.ShowWindow( SW_HIDE );
		break;

	default:
		break;
	}
}


void CTrackWnd::LoadDataToList( void )
{
	m_listTrack.DeleteAllItems();
	
	switch( m_pDiagnoseEntry->eType )
	{
	case MISS_FLIGHT:
		{
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
			break;
		}

	case PAXDEST_DIAGNOS:
		{
			int _iCount = m_DestDiagnoseInfo.getDiagnoseListSize();
			for( int i =0; i<_iCount; i++ )
			{
				const DEST_REASON_PAIR& reason_pair = m_DestDiagnoseInfo.getDiagnoseInfoByIndex( i );
				int _iIdx = m_listTrack.InsertItem( m_listTrack.GetItemCount(), reason_pair.first );
				m_listTrack.SetItemText( _iIdx, 1 , reason_pair.second );
			}
			break;
		}

	default:
		break;
	}
}

void CTrackWnd::OnOK()
{
	SendMessage(WM_CLOSE);
}

void CTrackWnd::OnButtonSave() 
{
	CFileDialog fileDlg( FALSE, ".txt","pax track", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text file (*.txt)|*.txt||",NULL, 0, FALSE );
	m_bClickSave=TRUE;
	if( fileDlg.DoModal() == IDOK )
	{
		CString strFileName = fileDlg.GetPathName();
		SaveTrackToFile( strFileName );
	}
	m_bClickSave=FALSE;
}


void CTrackWnd::SaveTrackToFile( const CString& _strName )
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

void CTrackWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_bWndClosing = TRUE;
	CWnd* pWnd = GetParent();
	pWnd->SendMessage(WM_TRACKWND_END,m_nItem,m_nSubItem);

	ShowWindow(SW_HIDE);
// 	CWnd::OnClose(); // do not close it, just hide
	m_bWndClosing = FALSE;
}

CWnd* CTrackWnd::GetParent() const
{
	return m_pUsedParentWnd ? m_pUsedParentWnd : CWnd::GetParent();
}


void CTrackWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
	if(    WA_INACTIVE == nState
		&& FALSE == m_bClickSave
		&& FALSE == m_bWndClosing // 
		)
	{
		SendMessage(WM_CLOSE);
	}
}

void CTrackWnd::ShowInfo( DiagnoseEntry* _pEntry, int nItem, int nSubItem )
{
	m_pDiagnoseEntry = _pEntry;
	m_nItem = nItem;
	m_nSubItem = nSubItem;

	try
	{
		InitCtrl();
		Init(m_pTerm,m_strProjPath);
		LoadDataToList();

		m_staticPaxID.SetWindowText(m_strTitle);
	}
	catch( FileOpenError* pe )
	{
		CString strError;
		pe->getMessage( strError.GetBuffer(32) );
		AfxMessageBox( strError, MB_OK|MB_ICONINFORMATION );
		strError.ReleaseBuffer();
		delete pe;
		return;
	}

	ShowWindow(SW_SHOW);
	SetActiveWindow();
}