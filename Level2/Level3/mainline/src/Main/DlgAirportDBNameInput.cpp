// DlgAirportDBNameInput.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgAirportDBNameInput.h"
#include "resource.h"
#include "common\\airportdatabaselist.h"
#include "common\\airportdatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgAirportDBNameInput dialog


CDlgAirportDBNameInput::CDlgAirportDBNameInput( const CString& szTile,bool bShowDropDown/*=true*/, CWnd* pParent /*=NULL*/ )
	: CDialog(CDlgAirportDBNameInput::IDD, pParent)
{
	m_bShowDropDown = bShowDropDown;
	//{{AFX_DATA_INIT(CDlgAirportDBNameInput)
	m_sAirportDBName = _T("");
	m_sStaticTitle = szTile;
	m_sAirportDBName1 = _T("");
	//}}AFX_DATA_INIT
}

void CDlgAirportDBNameInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAirportDBNameInput)
	DDX_CBString(pDX, IDC_COMBO_AIRPORTDB_NAME, m_sAirportDBName);
	DDX_Text(pDX, IDC_STATIC_AIRPORT_NAME, m_sStaticTitle);
	DDX_Text(pDX, IDC_EDIT_AIRPORTDB_NAME, m_sAirportDBName1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAirportDBNameInput, CDialog)
	//{{AFX_MSG_MAP(CDlgAirportDBNameInput)
	ON_CBN_EDITCHANGE(IDC_COMBO_AIRPORTDB_NAME, OnEditchangeComboAirportdbName)
	ON_CBN_SELCHANGE(IDC_COMBO_AIRPORTDB_NAME, OnSelchangeComboAirportdbName)
	ON_EN_CHANGE(IDC_EDIT_AIRPORTDB_NAME, OnChangeEditAirportdbName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAirportDBNameInput message handlers

BOOL CDlgAirportDBNameInput::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComboBox* pCbx=(CComboBox*)GetDlgItem(IDC_COMBO_AIRPORTDB_NAME);
	if( m_bShowDropDown )
	{
		int nDBCount=AIRPORTDBLIST->getDBCount();
		CString sName("");
		for(int i=0;i<nDBCount;i++)
		{
			sName=AIRPORTDBLIST->getAirportDBByIndex(i)->getName();
			if(sName.CompareNoCase("DEFAULT DB")!=0)
			{
				pCbx->AddString( sName );
			}
		}
		switch(pCbx->GetCount()) 
		{
		case 1:
			pCbx->SetCurSel(0);
			break;
		case 0:
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			break;
		default:
			m_sAirportDBName="";
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}
		pCbx->SetFocus();
	}
	else
	{
		pCbx->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_AIRPORTDB_NAME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_AIRPORTDB_NAME)->SetFocus();
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAirportDBNameInput::OnEditchangeComboAirportdbName() 
{
	EnableChildControl();
}

void CDlgAirportDBNameInput::OnSelchangeComboAirportdbName() 
{
	EnableChildControl();
}

void CDlgAirportDBNameInput::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_bShowDropDown==false)
	{
		m_sAirportDBName=m_sAirportDBName1;
	}
	CDialog::OnOK();
}
void CDlgAirportDBNameInput::EnableChildControl()
{
	UpdateData(TRUE);
	if(m_bShowDropDown==false)
	{
		m_sAirportDBName1.TrimLeft();
		m_sAirportDBName1.TrimRight();
		if(m_sAirportDBName1!="")
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}
	}
	else
	{
		CComboBox* pCbx=(CComboBox*)GetDlgItem( IDC_COMBO_AIRPORTDB_NAME);
		int nSel=pCbx->GetCurSel();
		if(nSel!=-1)
		{
			pCbx->GetLBText(nSel,m_sAirportDBName);
		}
		m_sAirportDBName.TrimLeft();
		m_sAirportDBName.TrimRight();
		if(m_sAirportDBName!="")
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}
	}
}

void CDlgAirportDBNameInput::OnChangeEditAirportdbName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	EnableChildControl();
	// TODO: Add your control notification handler code here
	
}
