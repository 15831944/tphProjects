// ProcAssignEventDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcAssignEventDlg.h"
#include "PassengerTypeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcAssignEventDlg dialog


CProcAssignEventDlg::CProcAssignEventDlg(CWnd* pParent)
	: CDialog(CProcAssignEventDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcAssignEventDlg)
	m_oTime = COleDateTime::GetCurrentTime();
	m_nStatus = -1;
	//}}AFX_DATA_INIT
	m_oTime.SetTime( 0, 0, 0 );
	m_nStatus = 0;
}


void CProcAssignEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcAssignEventDlg)
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDel);
	DDX_Control(pDX, IDC_LIST_PAXCONSTR, m_listBoxPaxConstr);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_EVENTTIME, m_oTime);
	DDX_Radio(pDX, IDC_RADIO_OPEN, m_nStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcAssignEventDlg, CDialog)
	//{{AFX_MSG_MAP(CProcAssignEventDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_PAXCONSTR, OnSelchangeListPaxconstr)
	ON_BN_CLICKED(IDC_RADIO_OPEN, OnRadioOpen)
	ON_BN_CLICKED(IDC_RADIO_CLOSE, OnRadioClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcAssignEventDlg message handlers


BOOL CProcAssignEventDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LoadList();

	UpdateData( false );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcAssignEventDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CPassengerTypeDialog dlg( m_pParentWnd );
	if (dlg.DoModal() == IDOK)
	{
		//PassengerConstraint paxConstr = dlg.GetPaxSelection();
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		int nCount = m_listBoxPaxConstr.GetCount();
		for( int i=0; i<nCount; i++ )
		{
			int nIdx = m_listBoxPaxConstr.GetItemData( i );
			const CMobileElemConstraint * pPaxConstr = m_multiPaxConstr.getConstraint( nIdx );
			if( mobileConstr.isEqual(pPaxConstr) )
			{
				// select the item
				m_listBoxPaxConstr.SetCurSel( i );
				OnSelchangeListPaxconstr();
				return;
			}
		}
		if( i == nCount )
		{
			// add new pax constraint.
			m_multiPaxConstr.addConstraint( mobileConstr );
			//char showStr[128];
			CString showStr;
			mobileConstr.screenPrint( showStr, 0, 54 );
			int nIdx = m_listBoxPaxConstr.AddString( showStr.GetBuffer(0) );
			m_listBoxPaxConstr.SetItemData( nIdx, nCount );
			m_listBoxPaxConstr.SetCurSel( nIdx );
		}
		OnSelchangeListPaxconstr();
	}	
}

void CProcAssignEventDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_listBoxPaxConstr.GetCurSel();
	int nIdx = m_listBoxPaxConstr.GetItemData( nCurSel );
	m_multiPaxConstr.deleteConst( nIdx );
	m_listBoxPaxConstr.DeleteString( nCurSel );
	m_listBoxPaxConstr.SetCurSel( -1 );
	OnSelchangeListPaxconstr();
}

void CProcAssignEventDlg::OnSelchangeListPaxconstr() 
{
	// TODO: Add your control notification handler code here
	int nIdx = m_listBoxPaxConstr.GetCurSel();
	if( nIdx >= 0 )
		m_btnDel.EnableWindow( true );
	else
		m_btnDel.EnableWindow( false );
	
}

void CProcAssignEventDlg::SetTime(const COleDateTime &_oTime)
{
	m_oTime = _oTime;
}

void CProcAssignEventDlg::SetOpen(bool _bOpen)
{
	m_nStatus = _bOpen ? 0 : 1;
}

void CProcAssignEventDlg::SetPaxConstr(const CMultiMobConstraint &_paxConstr)
{
	m_multiPaxConstr = _paxConstr;
}

COleDateTime& CProcAssignEventDlg::GetTime()
{
	return m_oTime;
}

bool CProcAssignEventDlg::IsOpen()
{
	return m_nStatus == 0;
}

CMultiMobConstraint& CProcAssignEventDlg::GetPaxConstr()
{
	return m_multiPaxConstr;
}

void CProcAssignEventDlg::LoadList()
{

	m_listBoxPaxConstr.ResetContent();
	int nCount = m_multiPaxConstr.getCount();
	for( int i=0; i<nCount; i++ )
	{
		const CMobileElemConstraint * pPaxConstr = m_multiPaxConstr.getConstraint( i );
		//char buf[256];
		CString buf;
		pPaxConstr->screenPrint( buf );
		int nIdx = m_listBoxPaxConstr.AddString( buf.GetBuffer(0) );
		m_listBoxPaxConstr.SetItemData( nIdx, i );
	}
}

void CProcAssignEventDlg::OnRadioOpen() 
{
	// TODO: Add your control notification handler code here
	LoadList();
	m_listBoxPaxConstr.EnableWindow();
	m_btnAdd.EnableWindow();
}

void CProcAssignEventDlg::OnRadioClose() 
{
	// TODO: Add your control notification handler code here
	m_listBoxPaxConstr.ResetContent();
	m_listBoxPaxConstr.EnableWindow( false );
	m_btnAdd.EnableWindow( false );
	m_btnDel.EnableWindow( false );	
}
