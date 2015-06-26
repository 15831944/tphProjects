// PopupDetailWnd.cpp : implementation file
//

#include "stdafx.h"
//#include "MFCExControl.h"
#include "PopupDetailWnd.h"
#include ".\popupdetailwnd.h"
#include "../common/exeption.h"

// CPopupDetailWnd
const UINT ID_BUTTON_SAVE = 100;
IMPLEMENT_DYNAMIC(CPopupDetailWnd, CWnd)
CPopupDetailWnd::CPopupDetailWnd(CWnd* pParent,int nItem,int nSubItem)
{

	m_bClickSave		=FALSE;

	m_nItem				=nItem;
	m_nSubItem			=nSubItem;

	LPCTSTR className	=AfxRegisterWndClass(CS_DBLCLKS, 
		AfxGetApp()->LoadStandardCursor(IDC_ARROW), 
		(HBRUSH)GetSysColorBrush(COLOR_3DFACE), 0);
	CreateEx ( 0,className, NULL,WS_POPUP | WS_BORDER  ,CRect(0,0,300,200), pParent, 0, NULL);

}

CPopupDetailWnd::~CPopupDetailWnd()
{
}


BEGIN_MESSAGE_MAP(CPopupDetailWnd, CWnd)
	//{{AFX_MSG_MAP(CTrackWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_SAVE, OnButtonSave)
	ON_WM_ACTIVATE()
	ON_COMMAND(IDOK, OnOK)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CPopupDetailWnd message handlers


void CPopupDetailWnd::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class

	CWnd::PostNcDestroy();
}
int CPopupDetailWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_staticPaxID.Create("Flight ID:",WS_CHILD|WS_VISIBLE,CRect(0,0,300,16),this);
	m_listTrack.Create(LVS_REPORT|LVS_SHOWSELALWAYS|WS_CHILD|WS_VISIBLE,CRect(0,16,300,200),this,2);
	m_btnOK.Create("Close",WS_CHILD|WS_VISIBLE,CRect(CPoint(215,160),CSize(75,23)),this,IDOK);

	m_btnSave.Create("Save",WS_CHILD|WS_VISIBLE,CRect(CPoint(130,160),CSize(75,23)),this,ID_BUTTON_SAVE);

	CFont* pFont=m_listTrack.GetFont();
	m_staticPaxID.SetFont(pFont);
	//m_listTrack.SetFont(pFont);
	//m_btnOK.SetFont(pFont);
	//m_btnSave.SetFont(pFont);
	m_btnSave.ShowWindow(SW_HIDE);
	m_btnOK.ShowWindow(SW_HIDE);

	try
	{
		InitCtrl();
//		Init(m_pTerm,m_strProjPath);
//		LoadDataToList();
//
		m_staticPaxID.SetWindowText(m_strTitle);
	}
	catch( FileOpenError* pe )
	{
		CString strError;
		pe->getMessage( strError.GetBuffer(32) );
		AfxMessageBox( strError, MB_OK|MB_ICONINFORMATION );
		strError.ReleaseBuffer();
		delete pe;
		return -1;
	}
	return 0;
}
void CPopupDetailWnd::InitCtrl( void )
{
	// set list_ctrl	
	DWORD dwStyle = m_listTrack.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listTrack.SetExtendedStyle( dwStyle );

	// insert column
	//switch( m_pDiagnoseEntry->eType )
	//{
	//case MISS_FLIGHT:
		//m_listTrack.InsertColumn( 0,"Time", LVCFMT_CENTER, 80 );
		//m_listTrack.InsertColumn( 1,"Object Name", LVCFMT_CENTER, 100 );
	//	m_listTrack.InsertColumn( 2,"Status", LVCFMT_CENTER, 120);
	//	break;

	//case PAXDEST_DIAGNOS:
	//	m_listTrack.InsertColumn( 0,"Destination Processor",LVCFMT_CENTER, 150);
	//	m_listTrack.InsertColumn( 1,"Reason",LVCFMT_CENTER, 250);
	//	m_btnSave.ShowWindow( SW_HIDE );
	//	break;

	//default:
	//	break;
	//}
}
void CPopupDetailWnd::LoadDataToList( void )
{
	m_listTrack.DeleteAllItems();


}
void CPopupDetailWnd::OnOK()
{
	SendMessage(WM_CLOSE);
}

void CPopupDetailWnd::OnButtonSave() 
{
	CFileDialog fileDlg( FALSE, ".txt","pax track", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text file (*.txt)|*.txt||", NULL, 0, FALSE  );
	m_bClickSave=TRUE;
	if( fileDlg.DoModal() == IDOK )
	{
		CString strFileName = fileDlg.GetPathName();
		SaveTrackToFile( strFileName );
	}
	m_bClickSave=FALSE;
}
void CPopupDetailWnd::SaveTrackToFile( const CString& _strName )
{

}

void CPopupDetailWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	if((!m_bClickSave)&&nState==WA_INACTIVE)
	{
		SendMessage(WM_CLOSE);
	}
	// TODO: Add your message handler code here
	
}
void CPopupDetailWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CWnd* pWnd=GetParent();
	pWnd->SendMessage(WM_TRACKWND_DESTROY,m_nItem,m_nSubItem);

	CWnd::OnClose();
}
