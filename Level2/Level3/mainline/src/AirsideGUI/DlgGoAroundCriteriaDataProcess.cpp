// DlgGoAroundCriteriaDataProcess.cpp : implementation file
//

#include "stdafx.h"
#include "DlgGoAroundCriteriaDataProcess.h"
#include ".\dlggoaroundcriteriadataprocess.h"


// CDlgGoAroundCriteriaDataProcess dialog

IMPLEMENT_DYNAMIC(CDlgGoAroundCriteriaDataProcess, CDialogResize)
CDlgGoAroundCriteriaDataProcess::CDlgGoAroundCriteriaDataProcess(int nProjID,CUnitsManager* pGlobalUnits,CWnd* pParent /*=NULL*/)
	: CDialogResize(CDlgGoAroundCriteriaDataProcess::IDD, pParent),m_nProjID(nProjID),m_gacData(nProjID)
	,m_pGlobalUnits(pGlobalUnits)
{
}

CDlgGoAroundCriteriaDataProcess::~CDlgGoAroundCriteriaDataProcess()
{
}

void CDlgGoAroundCriteriaDataProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGoAroundCriteriaDataProcess, CDialogResize)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgGoAroundCriteriaDataProcess message handlers

void CDlgGoAroundCriteriaDataProcess::OnBnClickedOk()
{
	CString strTitle = _T("");

	m_gacData.SetProjID(m_nProjID);

	GetDlgItem(IDC_EDIT1)->GetWindowText(strTitle);
	m_gacData.SetLBLSRWDIS (_tstof(strTitle) );

	GetDlgItem(IDC_EDIT2)->GetWindowText(strTitle);
	m_gacData.SetLBLSRWADIS (_tstof(strTitle) );
 
	GetDlgItem(IDC_EDIT4)->GetWindowText(strTitle);
	m_gacData.SetLBLIRWFPTDIFF (_tstoi(strTitle) );

	GetDlgItem(IDC_EDIT5)->GetWindowText(strTitle);
	m_gacData.SetLBLIRWRWTDIFF (_tstoi(strTitle) );
 
	GetDlgItem(IDC_EDIT6)->GetWindowText(strTitle);
	m_gacData.SetLBLIRWFPDDIFF (_tstof(strTitle) );
 
	GetDlgItem(IDC_EDIT7)->GetWindowText(strTitle);
	m_gacData.SetLBLIRWRWDDIFF (_tstof(strTitle) );
 
	GetDlgItem(IDC_EDIT8)->GetWindowText(strTitle);
	m_gacData.SetLBTSRWDIS (_tstof(strTitle) );
 
	GetDlgItem(IDC_EDIT9)->GetWindowText(strTitle);
	m_gacData.SetLBTIRWTDIFF (_tstoi(strTitle) );

	if(m_pGlobalUnits)
	{
		m_gacData.SetLBLSRWDIS( m_pGlobalUnits->ConvertLength(m_pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,m_gacData.GetLBLSRWDIS()));
		m_gacData.SetLBLSRWADIS( m_pGlobalUnits->ConvertLength(m_pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS, m_gacData.GetLBLSRWADIS()));
		m_gacData.SetLBLIRWFPDDIFF(m_pGlobalUnits->ConvertLength(m_pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS, m_gacData.GetLBLIRWFPDDIFF()));
		m_gacData.SetLBLIRWRWDDIFF(m_pGlobalUnits->ConvertLength(m_pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS, m_gacData.GetLBLIRWRWDDIFF()));
		m_gacData.SetLBTSRWDIS( m_pGlobalUnits->ConvertLength(m_pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS, m_gacData.GetLBTSRWDIS()));
		m_gacData.SetLBLIRWFPTDIFF( 60*m_gacData.GetLBLIRWFPTDIFF());
		m_gacData.SetLBLIRWRWTDIFF( 60*m_gacData.GetLBLIRWRWTDIFF());
		m_gacData.SetLBTIRWTDIFF( 60*m_gacData.GetLBTIRWTDIFF());
	}

	m_gacData.SaveData();

	CDialogResize::OnOK();
}

BOOL CDlgGoAroundCriteriaDataProcess::OnInitDialog()
{
	CDialogResize::OnInitDialog();

	// TODO:  Add extra initialization here
	CString strTitle = _T("");
	
	if(m_pGlobalUnits)
		strTitle.Format(_T("(%s)"),m_pGlobalUnits->GetLengthUnitString(m_pGlobalUnits->GetLengthUnits()));
	else
		strTitle = _T("(nm)");
	GetDlgItem(IDC_STATIC2)->SetWindowText(strTitle);
	GetDlgItem(IDC_STATIC3)->SetWindowText(strTitle);
	GetDlgItem(IDC_STATIC4)->SetWindowText(strTitle);

	if(m_pGlobalUnits)
		strTitle.Format(_T("(%s) of threshold ,"),m_pGlobalUnits->GetLengthUnitString(m_pGlobalUnits->GetLengthUnits()));
	else
		strTitle = _T("(nm) of threshold ,");
	GetDlgItem(IDC_STATIC1)->SetWindowText(strTitle);
	
	if(m_pGlobalUnits)
		strTitle.Format(_T("(%s) of landing threshold "),m_pGlobalUnits->GetLengthUnitString(m_pGlobalUnits->GetLengthUnits()));
	else
		strTitle = _T("(nm) of landing threshold ");
	GetDlgItem(IDC_STATIC5)->SetWindowText(strTitle);  

	if(m_nProjID != -1)
		m_gacData.ReadData(m_nProjID);

	if(m_pGlobalUnits)
	{
		m_gacData.SetLBLSRWDIS(m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,m_pGlobalUnits->GetLengthUnits(),m_gacData.GetLBLSRWDIS()));    
		m_gacData.SetLBLSRWADIS(m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,m_pGlobalUnits->GetLengthUnits(), m_gacData.GetLBLSRWADIS()));	
		m_gacData.SetLBLIRWFPTDIFF(m_gacData.GetLBLIRWFPTDIFF()/60);	
		m_gacData.SetLBLIRWFPDDIFF( m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,m_pGlobalUnits->GetLengthUnits(), m_gacData.GetLBLIRWFPDDIFF()));	
		m_gacData.SetLBLIRWRWTDIFF( m_gacData.GetLBLIRWRWTDIFF() / 60);
		m_gacData.SetLBLIRWRWDDIFF( m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,m_pGlobalUnits->GetLengthUnits(),m_gacData.GetLBLIRWRWDDIFF()));
		m_gacData.SetLBTSRWDIS(m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,m_pGlobalUnits->GetLengthUnits(),m_gacData.GetLBTSRWDIS()));
		m_gacData.SetLBTIRWTDIFF(m_gacData.GetLBTIRWTDIFF() /60 );
	}

	strTitle.Format(_T("%.2f"),m_gacData.GetLBLSRWDIS());
	GetDlgItem(IDC_EDIT1)->SetWindowText(strTitle);

	strTitle.Format(_T("%.2f"),m_gacData.GetLBLSRWADIS());
	GetDlgItem(IDC_EDIT2)->SetWindowText(strTitle);

	strTitle.Format(_T("%d"),m_gacData.GetLBLIRWFPTDIFF());
	GetDlgItem(IDC_EDIT4)->SetWindowText(strTitle);

	strTitle.Format(_T("%d"),m_gacData.GetLBLIRWRWTDIFF());
	GetDlgItem(IDC_EDIT5)->SetWindowText(strTitle);

	strTitle.Format(_T("%.2f"),m_gacData.GetLBLIRWFPDDIFF());
	GetDlgItem(IDC_EDIT6)->SetWindowText(strTitle);

	strTitle.Format(_T("%.2f"),m_gacData.GetLBLIRWRWDDIFF());
	GetDlgItem(IDC_EDIT7)->SetWindowText(strTitle);

	strTitle.Format(_T("%.2f"),m_gacData.GetLBTSRWDIS());
	GetDlgItem(IDC_EDIT8)->SetWindowText(strTitle);

	strTitle.Format(_T("%d"),m_gacData.GetLBTIRWTDIFF());
	GetDlgItem(IDC_EDIT9)->SetWindowText(strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
