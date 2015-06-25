// DlgAirRouteWayPointDefine.cpp : implementation file
//

#include "stdafx.h"
#include "../AirsideGUI/Resource.h"
#include "DlgAirRoute.h"
#include "DlgAirRouteWayPointDefine.h"
#include ".\dlgairroutewaypointdefine.h"
#include "../InputAirside/AirRoute.h"
//#include <Common/ARCConsole.h>

// CDlgAirRouteWayPointDefine dialog

IMPLEMENT_DYNAMIC(CDlgAirRouteWayPointDefine, CDialog)
CDlgAirRouteWayPointDefine::CDlgAirRouteWayPointDefine(ARWaypoint* pARWayPoint,std::vector<AirWayPoint>& vWaypoint, bool bStar, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAirRouteWayPointDefine::IDD, pParent)
	, m_lMinSpeed(0.0)
	, m_lMaxSpeed(0.0)
	, m_lMinHeight(0.0)
	, m_lMaxHeight(0.0)
	, m_lDegrees(0)
	, m_lVisDistance(0.0)

{
	m_pARWayPoint = pARWayPoint;
	m_vWaypoint = vWaypoint;
	m_bSTAR = bStar;
}

CDlgAirRouteWayPointDefine::~CDlgAirRouteWayPointDefine()
{
}

void CDlgAirRouteWayPointDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WAYPOINT, m_cmbWayPoint);
	DDX_Control(pDX, IDC_EDIT_MINSPEED, m_edtMinSpeed);
	DDX_Control(pDX, IDC_EDIT_MAXSPEED, m_edtMaxSpeed);
	DDX_Control(pDX, IDC_EDIT_MINHEIGHT, m_edtMinHeight);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtMaxHeight);
	DDX_Control(pDX, IDC_EDITDEGREES,m_edtDegrees);
	DDX_Control(pDX, IDC_EDITVISDISTANCE, m_edtVisDistance);
	DDX_Text(pDX, IDC_EDIT_MINSPEED, m_lMinSpeed);
	DDX_Text(pDX, IDC_EDIT_MAXSPEED, m_lMaxSpeed);
	DDX_Text(pDX, IDC_EDIT_MINHEIGHT, m_lMinHeight);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_lMaxHeight);
	DDX_Text(pDX, IDC_EDITDEGREES, m_lDegrees);
	DDX_Text(pDX, IDC_EDITVISDISTANCE, m_lVisDistance);


	DDX_Control(pDX, IDC_COMBOHEADINGCHOICE, m_cmbHeadingChoice);
	DDX_Control(pDX, IDC_SPIN_DEGREES, m_spinDegrees);
}


BEGIN_MESSAGE_MAP(CDlgAirRouteWayPointDefine, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINSPEED, OnDeltaposSpinMinspeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXSPEED, OnDeltaposSpinMaxspeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINHEIGHT, OnDeltaposSpinMinheight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXHEIGHT, OnDeltaposSpinMaxheight)
	ON_BN_CLICKED(IDC_RADIO_MINSPEEDDEFINE, OnBnClickedRadioMinspeeddefine)
	ON_BN_CLICKED(IDC_RADIO_MINSPEEDNOLIMIT, OnBnClickedRadioMinspeednolimit)
	ON_BN_CLICKED(IDC_RADIO_MAXSPEEDNOLIMIT, OnBnClickedRadioMaxspeednolimit)
	ON_BN_CLICKED(IDC_RADIO_MAXSPEEDDEFINE, OnBnClickedRadioMaxspeeddefine)
	ON_BN_CLICKED(IDC_RADIO_MINHEIGHTDEFINE, OnBnClickedRadioMinheightdefine)
	ON_BN_CLICKED(IDC_RADIO_MAXHEIGHTDEFINE, OnBnClickedRadioMaxheightdefine)
	ON_BN_CLICKED(IDC_RADIO_MINHEIGHTNOLIMIT, OnBnClickedRadioMinheightnolimit)
	ON_BN_CLICKED(IDC_RADIO_MAXHEIGHTNOLIMIT, OnBnClickedRadioMaxheightnolimit)
	ON_CBN_SELCHANGE(IDC_COMBO_WAYPOINT, OnCbnSelchangeComboWaypoint)
	ON_BN_CLICKED(IDC_RADIOHEADING, OnBnClickedRadioheading)
	ON_BN_CLICKED(IDC_RADIONEXTWAYPOINT, OnBnClickedRadionextwaypoint)
END_MESSAGE_MAP()


// CDlgAirRouteWayPointDefine message handlers

BOOL CDlgAirRouteWayPointDefine::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nCursel =m_pARWayPoint->getWaypoint().getID();
	//init way point
	for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++i)
	{
		CString strName = m_vWaypoint[i].GetObjNameString();
		int nWaypointID = m_vWaypoint[i].getID();
		int nItem = m_cmbWayPoint.AddString(strName);
		m_cmbWayPoint.SetItemData(nItem,nWaypointID);

		if (nWaypointID == nCursel )
		{
			m_cmbWayPoint.SetCurSel(nItem);
		}
	}


	CString strType = _T("Sequencing to next waypoint direct");
	m_cmbHeadingChoice.AddString(strType);
	m_cmbHeadingChoice.SetItemData(0,0);

	strType = _T("Sequencing to next waypoint aligned");
	m_cmbHeadingChoice.AddString(strType);
	m_cmbHeadingChoice.SetItemData(1,1);


	AirsideGUI::CDlgAirRoute * pParentWnd = 0;
	pParentWnd = (AirsideGUI::CDlgAirRoute *)GetParent();
	if(pParentWnd)
	{
		GetDlgItem(IDC_STATIC_MinSpeedUnit)->SetWindowText(_T("(") + CARCVelocityUnit::GetVelocityUnitString(pParentWnd->GetCurSelVelocityUnit()) + _T(")"));
		GetDlgItem(IDC_STATIC_MaxSpeedUnit)->SetWindowText(_T("(") + CARCVelocityUnit::GetVelocityUnitString(pParentWnd->GetCurSelVelocityUnit()) + _T(")"));
		GetDlgItem(IDC_STATIC_MinHeightUnit)->SetWindowText(_T("(") + CARCLengthUnit::GetLengthUnitString(pParentWnd->GetCurSelLengthUnit()) + _T(")"));
		GetDlgItem(IDC_STATIC_MaxHeightUnit)->SetWindowText(_T("(") + CARCLengthUnit::GetLengthUnitString(pParentWnd->GetCurSelLengthUnit()) + _T(")"));
		GetDlgItem(IDC_STATICVISUAL)->SetWindowText(_T("(") + CARCLengthUnit::GetLengthUnitString(pParentWnd->GetCurSelLengthUnit()) + _T(") for visualization"));
	}


	//min speed
	double nMinSpeed = m_pARWayPoint->getMinSpeed();
	if (nMinSpeed == -1)
	{
		((CButton *)GetDlgItem(IDC_RADIO_MINSPEEDNOLIMIT))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_MINSPEEDDEFINE))->SetCheck(FALSE);
		m_edtMinSpeed.EnableWindow(FALSE);
	}
	else
	{	
		((CButton *)GetDlgItem(IDC_RADIO_MINSPEEDNOLIMIT))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_MINSPEEDDEFINE))->SetCheck(TRUE);
		//CString strText = _T("");
		//strText.Format(_T("%d"),nMinSpeed);
		//m_edtMinSpeed.EnableWindow();
		//m_edtMinSpeed.SetWindowText(strText);
		m_lMinSpeed = nMinSpeed;
		if(pParentWnd)
			m_lMinSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,pParentWnd->CUnitPiece::GetCurSelVelocityUnit(),(double)m_lMinSpeed);
		CString strTitle = _T("");
		strTitle.Format(_T("%.2f"),m_lMinSpeed);
		GetDlgItem(IDC_EDIT_MINSPEED)->SetWindowText(strTitle);
	}
	
	//Max speed
	double nMaxSpeed = m_pARWayPoint->getMaxSpeed();
	if (nMaxSpeed == -1)
	{
		((CButton *)GetDlgItem(IDC_RADIO_MAXSPEEDNOLIMIT))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_MAXSPEEDDEFINE))->SetCheck(FALSE);
		m_edtMaxSpeed.EnableWindow(FALSE);
	}
	else
	{	
		((CButton *)GetDlgItem(IDC_RADIO_MAXSPEEDNOLIMIT))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_MAXSPEEDDEFINE))->SetCheck(TRUE);
		//CString strText = _T("");
		//strText.Format(_T("%d"),nMaxSpeed);
		//m_edtMaxSpeed.EnableWindow();
		//m_edtMaxSpeed.SetWindowText(strText);

		m_lMaxSpeed = nMaxSpeed;
		if(pParentWnd)
			m_lMaxSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,pParentWnd->CUnitPiece::GetCurSelVelocityUnit(),(double)m_lMaxSpeed);
		CString strTitle = _T("");
		strTitle.Format(_T("%.2f"),m_lMaxSpeed);
		GetDlgItem(IDC_EDIT_MAXSPEED)->SetWindowText(strTitle);
	}
	
	//min Height
	double nMinHeight = m_pARWayPoint->getMinHeight();
	if (std::abs(nMinHeight + 1) < ARCMath::EPSILON)
	{
		((CButton *)GetDlgItem(IDC_RADIO_MINHEIGHTNOLIMIT))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_MINHEIGHTDEFINE))->SetCheck(FALSE);
		m_edtMinHeight.EnableWindow(FALSE);
	}
	else
	{	
		((CButton *)GetDlgItem(IDC_RADIO_MINHEIGHTNOLIMIT))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_MINHEIGHTDEFINE))->SetCheck(TRUE);
		//CString strText = _T("");
		//strText.Format(_T("%d"),nMinHeight);
		//m_edtMinHeight.EnableWindow();
		//m_edtMinHeight.SetWindowText(strText);

		m_lMinHeight =nMinHeight;
		if(pParentWnd)
			m_lMinHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pParentWnd->CUnitPiece::GetCurSelLengthUnit(),(double)m_lMinHeight);
		CString strTitle = _T("");
		strTitle.Format(_T("%.2f"),m_lMinHeight);
		GetDlgItem(IDC_EDIT_MINHEIGHT)->SetWindowText(strTitle);
	}	


	//max Height
	double nMaxHeight = m_pARWayPoint->getMaxHeight();
	if (std::abs(nMaxHeight + 1) < ARCMath::EPSILON)
	{
		((CButton *)GetDlgItem(IDC_RADIO_MAXHEIGHTNOLIMIT))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_MAXHEIGHTDEFINE))->SetCheck(FALSE);
		m_edtMaxHeight.EnableWindow(FALSE);
	}
	else
	{	
		((CButton *)GetDlgItem(IDC_RADIO_MAXHEIGHTNOLIMIT))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_MAXHEIGHTDEFINE))->SetCheck(TRUE);
		//CString strText = _T("");
		//strText.Format(_T("%d"),nMaxHeight);
		//m_edtMaxHeight.EnableWindow();
		//m_edtMaxHeight.SetWindowText(strText);
		m_lMaxHeight =nMaxHeight;
		if(pParentWnd)
			m_lMaxHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pParentWnd->CUnitPiece::GetCurSelLengthUnit(),(double)m_lMaxHeight);
		CString strTitle = _T("");
		strTitle.Format(_T("%.2f"),m_lMaxHeight);
		GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(strTitle);
	}	

	m_spinDegrees.SetRange(0,359);

	ARWaypoint::DepartType Departtype = m_pARWayPoint->getDepartType();
	if (Departtype == ARWaypoint::NextWaypoint)
	{
		((CButton *)GetDlgItem(IDC_RADIONEXTWAYPOINT))->SetCheck(TRUE);
		OnBnClickedRadionextwaypoint();
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIOHEADING))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_RADIOHEADING))->SetCheck(TRUE);
		m_lDegrees = m_pARWayPoint->getDegrees();
		m_lVisDistance = m_pARWayPoint->getVisDistance();
		if(pParentWnd)
			m_lVisDistance = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pParentWnd->CUnitPiece::GetCurSelLengthUnit(),(double)m_lVisDistance);
		CString strTitle = _T("");
		strTitle.Format(_T("%.2f"),m_lVisDistance);
		GetDlgItem(IDC_EDITVISDISTANCE)->SetWindowText(strTitle);

		m_spinDegrees.SetRange(0,359);
		m_spinDegrees.SetPos(m_lDegrees);

		if (m_pARWayPoint->getHeadingType() == ARWaypoint::Direct )
		{
			m_cmbHeadingChoice.SetCurSel(0);
		}
		else
		{
			m_cmbHeadingChoice.SetCurSel(1);
		}
		OnBnClickedRadioheading();
	}

	UpdateData(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAirRouteWayPointDefine::OnOK()
{	
	UpdateData();

	if (m_lMinSpeed > m_lMaxSpeed && ((CButton *)GetDlgItem(IDC_RADIO_MAXSPEEDNOLIMIT))->GetCheck() == 0)
	{
		MessageBox(" The min speed is larger than max speed.");
		return;
	}

	if (m_lMinHeight > m_lMaxHeight&& ((CButton *)GetDlgItem(IDC_RADIO_MAXHEIGHTNOLIMIT))->GetCheck() == 0)
	{
		MessageBox(" The min height is larger than max height.");
		return;
	}
	//way point
	int nCurSel = m_cmbWayPoint.GetCurSel();
	if (nCurSel < 0)
	{
		CDialog::OnOK();
		return;
	}
	if (nCurSel != LB_ERR)
	{
		int nWaypointID = m_cmbWayPoint.GetItemData(nCurSel);
	//	m_pARWayPoint->setWaypoint(m_vWaypoint[nCurSel]);
		for (size_t i = 0; i < m_vWaypoint.size(); i++)
		{
			AirWayPoint& wayPoint = m_vWaypoint[i];
			if (wayPoint.getID() == nWaypointID)
			{
				m_pARWayPoint->setWaypoint(wayPoint);
				break;
			}
		}
	}

	AirsideGUI::CDlgAirRoute * pParentWnd = 0;
	pParentWnd = (AirsideGUI::CDlgAirRoute *)GetParent();
	if(pParentWnd)
	{
		m_lMinSpeed = CARCVelocityUnit::ConvertVelocity(pParentWnd->CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,(double)m_lMinSpeed);
		m_lMaxSpeed = CARCVelocityUnit::ConvertVelocity(pParentWnd->CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,(double)m_lMaxSpeed);
		m_lMinHeight = CARCLengthUnit::ConvertLength(pParentWnd->CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,(double)m_lMinHeight);
		m_lMaxHeight = CARCLengthUnit::ConvertLength(pParentWnd->CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,(double)m_lMaxHeight);
		m_lVisDistance   = CARCLengthUnit::ConvertLength(pParentWnd->CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,(double)m_lVisDistance);
	}
	//min speed
	if(((CButton *)GetDlgItem(IDC_RADIO_MINSPEEDNOLIMIT))->GetCheck())
	{
		m_pARWayPoint->setMinSpeed(-1);
	}
	else
	{
		//CString strText = _T("");
		//m_edtMinSpeed.GetWindowText(strText);
		//m_pARWayPoint->setMinSpeed(::atol(strText));
		m_pARWayPoint->setMinSpeed(m_lMinSpeed);

	}
	//max speed
	if(((CButton *)GetDlgItem(IDC_RADIO_MAXSPEEDNOLIMIT))->GetCheck())
	{
		m_pARWayPoint->setMaxSpeed(-1);
	}
	else
	{
		//CString strText = _T("");
		//m_edtMaxSpeed.GetWindowText(strText);
		//m_pARWayPoint->setMaxSpeed(::atol(strText));
		m_pARWayPoint->setMaxSpeed(m_lMaxSpeed);
	}

	//min height
	if(((CButton *)GetDlgItem(IDC_RADIO_MINHEIGHTNOLIMIT))->GetCheck())
	{
		m_pARWayPoint->setMinHeight(-1);
	}
	else
	{
		//CString strText = _T("");
		//m_edtMinHeight.GetWindowText(strText);
		//m_pARWayPoint->setMinHeight(::atol(strText));
		m_pARWayPoint->setMinHeight(m_lMinHeight);

	}

	//max height
	if(((CButton *)GetDlgItem(IDC_RADIO_MAXHEIGHTNOLIMIT))->GetCheck())
	{
		m_pARWayPoint->setMaxHeight(-1);
	}
	else
	{
		//CString strText = _T("");
		//m_edtMaxHeight.GetWindowText(strText);
		//m_pARWayPoint->setMaxHeight(::atol(strText));
		m_pARWayPoint->setMaxHeight(m_lMaxHeight);
	}

	if (((CButton *)GetDlgItem(IDC_RADIOHEADING))->GetCheck())
	{
		m_pARWayPoint->setDepartType(ARWaypoint::Heading);
		m_pARWayPoint->setHeadingType(ARWaypoint::HeadingType(m_cmbHeadingChoice.GetCurSel()));
		m_pARWayPoint->setDegrees((long)m_lDegrees);
		m_pARWayPoint->setVisDistance(m_lVisDistance);
	}
	else
	{
		m_pARWayPoint->setDepartType(ARWaypoint::NextWaypoint);
	}

	CDialog::OnOK();
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinMinspeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	UpdateData(TRUE);
	m_lMinSpeed = pNMUpDown->iPos + pNMUpDown->iDelta;
	if( m_lMinSpeed < 0 )
		m_lMinSpeed = 0;

	UpdateData(FALSE);

	*pResult = 0;
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinMaxspeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);
	m_lMaxSpeed = pNMUpDown->iPos + pNMUpDown->iDelta;

	if( m_lMaxSpeed < 0 )
		m_lMaxSpeed = 0;
	UpdateData(FALSE);	

	*pResult = 0;
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinMinheight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);
	m_lMinHeight = pNMUpDown->iPos + pNMUpDown->iDelta;
	if( m_lMinHeight < 0 )
		m_lMinHeight = 0;
	UpdateData(FALSE);
	*pResult = 0;
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinMaxheight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);
	m_lMaxHeight = pNMUpDown->iPos + pNMUpDown->iDelta;
	if( m_lMaxHeight < 0 )
		m_lMaxHeight = 0;
	UpdateData(FALSE);

	*pResult = 0;
}
void CDlgAirRouteWayPointDefine::OnBnClickedRadioMinspeeddefine()
{
	m_edtMinSpeed.EnableWindow(TRUE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMinspeednolimit()
{
	m_edtMinSpeed.EnableWindow(FALSE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMaxspeednolimit()
{
	m_edtMaxSpeed.EnableWindow(FALSE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMaxspeeddefine()
{
	m_edtMaxSpeed.EnableWindow(TRUE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMinheightdefine()
{
	m_edtMinHeight.EnableWindow(TRUE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMaxheightdefine()
{
	m_edtMaxHeight.EnableWindow(TRUE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMinheightnolimit()
{
	m_edtMinHeight.EnableWindow(FALSE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioMaxheightnolimit()
{
	m_edtMaxHeight.EnableWindow(FALSE);
}

void CDlgAirRouteWayPointDefine::OnCbnSelchangeComboWaypoint()
{
	// TODO: Add your control notification handler code here
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioheading()
{
	m_cmbHeadingChoice.EnableWindow(TRUE);
	m_edtDegrees.EnableWindow(TRUE);
	m_edtVisDistance.EnableWindow(TRUE);
	m_spinDegrees.EnableWindow(TRUE);
	GetDlgItem(IDC_STATICDEGREE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATICVISUAL)->EnableWindow(TRUE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadionextwaypoint()
{
	m_cmbHeadingChoice.EnableWindow(FALSE);
	m_edtDegrees.EnableWindow(FALSE);
	m_edtVisDistance.EnableWindow(FALSE);
	m_spinDegrees.EnableWindow(FALSE);
	GetDlgItem(IDC_STATICDEGREE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATICVISUAL)->EnableWindow(FALSE);
}

