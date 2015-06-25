// DlgStretchOption.cpp : implementation file
//

#include "stdafx.h"

#include "Resource.h"
//#include "AirsideGUI.h"
#include "DlgStretchOption.h"
#include ".\dlgstretchoption.h"

//#include "Resource.h"
// CDlgStretchOption dialog

IMPLEMENT_DYNAMIC(CDlgStretchOption, CDialog)
CDlgStretchOption::CDlgStretchOption(int nProjID,CWnd* pParent /*=NULL*/)
	: CDialog( IDD_DIALOG_STRETCHOPTION, pParent)
	,m_nProjID(nProjID)
	, m_strDrive(0)
	, m_dLaneWidth(0.0)
	, m_iLaneNumber(0)
{

}

CDlgStretchOption::~CDlgStretchOption()
{
}

void CDlgStretchOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_DRIVE, m_strDrive);
	DDX_Text(pDX, IDC_LANEWIDTH, m_dLaneWidth);
	DDX_Text(pDX, IDC_LANENUMBER, m_iLaneNumber);
}


BEGIN_MESSAGE_MAP(CDlgStretchOption, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCLE,OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgStretchOption message handlers



BOOL CDlgStretchOption::OnInitDialog()
{
	CDialog::OnInitDialog();
	//CString strTitle = _T("");
	//if(m_nProjID != -1)
		m_stretchSetOption.ReadData(m_nProjID);

	// TODO:  Add extra initialization here
   
	m_dLaneWidth = m_stretchSetOption.GetLaneWidth();
	m_iLaneNumber = m_stretchSetOption.GetLaneNumber();
	m_strDrive = m_stretchSetOption.GetDrive();


	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_NUM))->SetRange(1,10000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_LANE))->SetRange(1,10000);


	UpdateData(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStretchOption::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strTitle = _T("");

	m_stretchSetOption.SetProjID(m_nProjID);
	m_stretchSetOption.SetDrive( m_strDrive);
	m_stretchSetOption.SetLaneWidth(m_dLaneWidth);
	m_stretchSetOption.SetLaneNumber(m_iLaneNumber);
	m_stretchSetOption.SaveDatas( );

	CDialog::OnOK();
}

void CDlgStretchOption::OnBnClickedCancel()
{
	CDialog::OnCancel();
}