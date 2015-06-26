// DlgProjectName.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgProjectName.h"
#include "common\airportdatabase.h"
#include "common\airportdatabaselist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProjectName dialog


CDlgProjectName::CDlgProjectName(const CString _csInitName,bool _bCanSelectDB, long _lInitDB, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProjectName::IDD, pParent), m_lSelectDB( _lInitDB ), m_bCanSelectDB( _bCanSelectDB )
{
	//{{AFX_DATA_INIT(CDlgProjectName)
	//}}AFX_DATA_INIT
	m_strProjectName = _csInitName;
	m_sSelectedDBName="";
}


void CDlgProjectName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProjectName)
	DDX_Control(pDX, IDC_COMBO_DB, m_comboDB);
	DDX_Control(pDX, IDC_PROJECTNAME, m_editName);
	DDX_Text(pDX, IDC_PROJECTNAME, m_strProjectName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProjectName, CDialog)
	//{{AFX_MSG_MAP(CDlgProjectName)
	ON_CBN_EDITCHANGE(IDC_COMBO_DB, OnEditchangeComboDb)
	ON_EN_CHANGE(IDC_PROJECTNAME, OnChangeProjectname)
	ON_CBN_SELCHANGE(IDC_COMBO_DB, OnSelchangeComboDb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProjectName message handlers

BOOL CDlgProjectName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_editName.SetFocus();
	m_editName.SetSel( 0, -1 );
	
	InitDBList();
	setInitSelectItem();

	UpdateData( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgProjectName::InitDBList( void )
{
	for( int i=0; i< AIRPORTDBLIST->getDBCount(); i++ )
	{
		CAirportDatabase* _pDB = AIRPORTDBLIST->getAirportDBByIndex( i );
		if(_pDB->getName().CompareNoCase("DEFAULTDB")!=0)
		{
			int _iIdx = m_comboDB.InsertString( -1, _pDB->getName() );
			m_comboDB.SetItemData( _iIdx, _pDB->getIndex() );
		}
	}
}


// set cursor select item by db index
void CDlgProjectName::setInitSelectItem( void )
{
	for( int i=0; i< m_comboDB.GetCount(); i++ )
	{
		if( m_comboDB.GetItemData( i ) == m_lSelectDB )
		{
			m_comboDB.SetCurSel( i );
			m_comboDB.EnableWindow( m_bCanSelectDB );
			return;
		}
	}

	m_lSelectDB = -1L;
//	m_comboDB.SetCurSel( 0 );
	m_comboDB.EnableWindow( m_bCanSelectDB );
	return;
}


void CDlgProjectName::OnOK()
{
	m_strProjectName.TrimLeft();
	m_strProjectName.TrimRight();
    CString strTemp;
    strTemp.LoadString(IDS_INVALID_PROJNAME_CHARACTER);
    int nPos = m_strProjectName.FindOneOf(strTemp);
    if(nPos > -1)
    {
        CString strMsg;
        strMsg.Format(_T("The project name should not contain the following characters:\n\n%s"), strTemp);
        MessageBox(strMsg);
        m_editName.SetFocus();
        m_editName.SetSel(nPos, nPos+1);
        return;
    }
	UpdateData( FALSE );

	UpdateData( TRUE );
	
	int _iCurSel = m_comboDB.GetCurSel();
	m_lSelectDB = m_comboDB.GetItemData( _iCurSel );
	m_comboDB.GetWindowText(m_sSelectedDBName);
	if(m_sSelectedDBName=="")
	{
		AfxMessageBox("Invalid Database!");
		return;
	}

	CDialog::OnOK();
}

void CDlgProjectName::OnEditchangeComboDb() 
{
	EnableChildControl();
}

void CDlgProjectName::OnChangeProjectname() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	EnableChildControl();
	
}

void CDlgProjectName::OnSelchangeComboDb() 
{
	// TODO: Add your control notification handler code here
	EnableChildControl();
}
void CDlgProjectName::EnableChildControl()
{
	UpdateData(TRUE);
	CString sDBName("");
	m_comboDB.GetWindowText(sDBName);
	sDBName.TrimLeft();
	sDBName.TrimRight();

	int nSel=m_comboDB.GetCurSel();
	if(nSel!=-1)
	{
		m_comboDB.GetLBText(nSel,sDBName);
	}
	
	if(sDBName!="" && m_strProjectName!="")
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

}
