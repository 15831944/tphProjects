#include "stdafx.h"
#include "../Resource.h"
#include "DlgApproachSeparationCriteria.h"
#include ".\dlgapproachseparationcriteria.h"
#include "../../Common/CARCUnit.h"
#include "./InputAirside/InputAirside.h"
#include "./InputAirside/ALTAirport.h"

// CDlgApproachSeparationCriteria dialog

IMPLEMENT_DYNAMIC(CDlgApproachSeparationCriteria, CXTResizeDialog)
CDlgApproachSeparationCriteria::CDlgApproachSeparationCriteria(int nProjID,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgApproachSeparationCriteria::IDD, pParent)
	,m_nProjID(nProjID)
	,m_nAirportID(-1)
	,m_wndTabLandingBehindLanded(0,pInputAirside,pSelectProbDistEntry)
	,m_wndTabLandingBehindTakeoff(1,pInputAirside,pSelectProbDistEntry)
	,m_wndRunwayImage(nProjID)
	,m_strNear(_T(""))
	,m_strFar(_T(""))
	,m_strConvergentDegree(_T(""))
	,m_strDivergentDegree(_T(""))
	,m_bInitialize(false)
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);

	if (!vAirportIds.empty())		//error
	{
		m_nAirportID = vAirportIds.at(0);
		ALTAirport::GetRunwayList(m_nAirportID,m_vRunways);		
	}

	m_pAppSepCriteria = new CApproachSeparationCriteria(nProjID,m_vRunways);
}

CDlgApproachSeparationCriteria::~CDlgApproachSeparationCriteria()
{
	delete m_pAppSepCriteria;
	m_pAppSepCriteria = NULL;
}

void CDlgApproachSeparationCriteria::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_wndTabCtrl);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_wndRunwayImage);
	DDX_Text(pDX, IDC_EDIT10, m_strNear);
	DDX_Text(pDX, IDC_EDIT11, m_strFar);
	DDX_Text(pDX, IDC_EDIT1, m_strConvergentDegree);
	DDX_Text(pDX, IDC_EDIT3, m_strDivergentDegree);
	DDX_Control(pDX, IDC_SPIN1, m_wndConvergentSpin);
	DDX_Control(pDX, IDC_SPIN10, m_wndDivergentSpin);
	DDX_Control(pDX, IDC_SPIN13, m_wndNearSpin);
	DDX_Control(pDX, IDC_SPIN14, m_wndFarSpin);
}


BEGIN_MESSAGE_MAP(CDlgApproachSeparationCriteria, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT10, OnEnChangeEdit10)
	ON_EN_CHANGE(IDC_EDIT11, OnEnChangeEdit11)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgApproachSeparationCriteria message handlers


void CDlgApproachSeparationCriteria::UpdateUIState()
{

}

BOOL CDlgApproachSeparationCriteria::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_pAppSepCriteria->ReadData();
	m_wndTabLandingBehindLanded.SetApproachSeparationCriteria(m_pAppSepCriteria);
	m_wndTabLandingBehindTakeoff.SetApproachSeparationCriteria(m_pAppSepCriteria);

	int nPageID = 0;
	m_wndTabLandingBehindLanded.Create (IDD_DIALOG_APPROACHSEPARATIONITEM, this);
	m_wndTabCtrl.AddSSLPage (_T("Landing behind landing"), nPageID++, &m_wndTabLandingBehindLanded);
	m_wndTabLandingBehindTakeoff.Create (IDD_DIALOG_APPROACHSEPARATIONITEM, this);
	m_wndTabCtrl.AddSSLPage (_T("Landing behind take off"), nPageID++, &m_wndTabLandingBehindTakeoff);

	m_strConvergentDegree.Format(_T("%.2f"),m_pAppSepCriteria->m_runwaySeparationRelation.m_dConvergentAngle);
	m_strDivergentDegree.Format(_T("%.2f"),m_pAppSepCriteria->m_runwaySeparationRelation.m_dDivergentAngle);
	m_strNear.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_FEET,m_pAppSepCriteria->m_runwaySeparationRelation.m_dCloseDist));
	m_strFar.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_FEET,m_pAppSepCriteria->m_runwaySeparationRelation.m_dFarDist));

	m_wndConvergentSpin.SetRange(0,180);
	m_wndDivergentSpin.SetRange(0,180);
	m_wndNearSpin.SetRange(0,5000);
	m_wndFarSpin.SetRange(0,5000);

	SetResize(IDC_STATIC_IMAGE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_LEFT_INCLUDE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_LEFT_SETTING, SZ_BOTTOM_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_CONVERGENT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT1, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_DEG1, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_DIVERGENT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT3, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_DEG2, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_LIMIT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT10, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_FT1, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT11, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_FT2, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SPIN1, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SPIN10, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SPIN13, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SPIN14, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_STATIC_RIGHT_INCLUDE, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TAB, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	
	SetResize(IDC_BUTTON1, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	m_wndRunwayImage.GetRunway(m_nAirportID,m_vRunways);

	UpdateData(FALSE);
	m_bInitialize = true;
	return TRUE;
}

void CDlgApproachSeparationCriteria::OnOK()
{
	OnBnClickedButton1();
	CXTResizeDialog::OnOK();
}

void CDlgApproachSeparationCriteria::OnBnClickedButton1()
{
	UpdateData();
	double dTemp = -1.0;

	dTemp = _tstof(m_strConvergentDegree);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (dTemp > 90.0) 
		dTemp = 90.0;
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dConvergentAngle = dTemp;

	dTemp = _tstof(m_strDivergentDegree);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (dTemp > 90.0) 
		dTemp = 90.0;
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dDivergentAngle = dTemp;

	dTemp = _tstof(m_strNear);
	dTemp = CARCLengthUnit::ConvertLength(AU_LENGTH_FEET,AU_LENGTH_CENTIMETER,dTemp);
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dCloseDist = dTemp;

	dTemp = _tstof(m_strFar);
	dTemp = CARCLengthUnit::ConvertLength(AU_LENGTH_FEET,AU_LENGTH_CENTIMETER,dTemp);
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dFarDist = dTemp;

	m_pAppSepCriteria->SaveData();
}

void CDlgApproachSeparationCriteria::OnEnChangeEdit1()
{
	if(!m_bInitialize)return;
	double dTemp = -1.0;

	CString strConvergentDegree = _T("");
	GetDlgItem(IDC_EDIT1)->GetWindowText(strConvergentDegree);
	dTemp = _tstof(strConvergentDegree);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (dTemp > 90.0) 
		dTemp = 90.0;
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dConvergentAngle = dTemp;
}

void CDlgApproachSeparationCriteria::OnEnChangeEdit10()
{
	if(!m_bInitialize)return;
	double dTemp = -1.0;

	CString strNear = _T("");
	GetDlgItem(IDC_EDIT10)->GetWindowText(strNear);
	dTemp = _tstof(strNear);
	if (dTemp < 0.0)
		dTemp = 0.0;
	dTemp = CARCLengthUnit::ConvertLength(AU_LENGTH_FEET,AU_LENGTH_CENTIMETER,dTemp);
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dCloseDist = dTemp;
}

void CDlgApproachSeparationCriteria::OnEnChangeEdit11()
{
	if(!m_bInitialize)return;
	double dTemp = -1.0;

	CString strFar = _T("");
	GetDlgItem(IDC_EDIT10)->GetWindowText(strFar);
	dTemp = _tstof(strFar);
	if (dTemp < 0.0)
		dTemp = 0.0;
	dTemp = CARCLengthUnit::ConvertLength(AU_LENGTH_FEET,AU_LENGTH_CENTIMETER,dTemp);
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dFarDist = dTemp;
}

void CDlgApproachSeparationCriteria::OnEnChangeEdit3()
{
	if(!m_bInitialize)return;
	double dTemp = -1.0;

	CString strDivergentDegree = _T("");
	GetDlgItem(IDC_EDIT1)->GetWindowText(strDivergentDegree);
	dTemp = _tstof(strDivergentDegree);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (dTemp > 90.0) 
		dTemp = 90.0;
	m_pAppSepCriteria->m_runwaySeparationRelation.m_dDivergentAngle = dTemp;
}


void CDlgApproachSeparationCriteria::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType, cx, cy);

	if(m_wndTabCtrl.m_hWnd){
		int nActPage = m_wndTabCtrl.GetSSLActivePage();
		int nPageCount = m_wndTabCtrl.GetSSLPageCount();
		for (int i = 0;i < nPageCount;++i) {
			if (i != nActPage) {
				m_wndTabCtrl.ActivateSSLPage (i);
				break;
			}
		}
		m_wndTabCtrl.ActivateSSLPage (nActPage);
	}
}
