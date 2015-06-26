// DlgAirRouteWayPointDefine.cpp : implementation file
//

#include "stdafx.h"
#include "../AirsideGUI/Resource.h"
#include "DlgAirRoute.h"
#include "DlgAirRouteWayPointDefine.h"
#include ".\dlgairroutewaypointdefine.h"
#include "../InputAirside/AirRoute.h"
//#include <Common/ARCConsole.h>

const static short iAngleMax = 360;
const static short iAngleMin = 0;

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
    DDX_Control(pDX, IDC_EDIT_ALTITUDE, m_editAltitude);
    DDX_Control(pDX, IDC_EDIT_ONTRACK, m_editOnTrack);
    DDX_Control(pDX, IDC_EDIT_TRACKTONEXTWP, m_editTrackToNext);
    DDX_Control(pDX, IDC_EDIT_INTERCEPTANGLE, m_editInterceptAngle);
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
    ON_BN_CLICKED(IDC_RADIO_ALTITUDE, &CDlgAirRouteWayPointDefine::OnBnClickedRadioAltitude)
    ON_BN_CLICKED(IDC_RADIO_WAYPOINT, &CDlgAirRouteWayPointDefine::OnBnClickedRadioWaypoint)
    ON_BN_CLICKED(IDC_RADIO_TO_NEXT_ALTITUDE, &CDlgAirRouteWayPointDefine::OnBnClickedRadioToNextAltitude)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INTERCEPTANGLE, &CDlgAirRouteWayPointDefine::OnDeltaposSpinInterceptangle)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRACKTONEXTWP, &CDlgAirRouteWayPointDefine::OnDeltaposSpinTracktonextwp)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ALTITUDE, &CDlgAirRouteWayPointDefine::OnDeltaposSpinAltitude)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ONTRACK, &CDlgAirRouteWayPointDefine::OnDeltaposSpinOntrack)
    ON_BN_CLICKED(IDC_RADIO_TONEXTWAYPOINT, &CDlgAirRouteWayPointDefine::OnBnClickedRadioToNextWaypoint)
    ON_BN_CLICKED(IDC_RADIO_DIRECT, &CDlgAirRouteWayPointDefine::OnBnClickedRadioDirect)
    ON_BN_CLICKED(IDC_RADIO_INTERCEPTTRACK, &CDlgAirRouteWayPointDefine::OnBnClickedRadioInterceptTrack)
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


    m_editAltitude.SetPrecision(2);
    m_editOnTrack.SetPrecision(2);
    m_editTrackToNext.SetPrecision(2);
    m_editInterceptAngle.SetPrecision(2);
    SetAllSpinControlRange();

    if(m_pARWayPoint->GetWaypointType() == ARWaypoint::ARWayPoint_AirWayPoint)
    {
        ((CButton *)GetDlgItem(IDC_RADIO_WAYPOINT))->SetCheck(TRUE);
        OnBnClickedRadioWaypoint();
    }
    else // AirWayPoint::ARWayPoint_Altitude
    {
        ((CButton *)GetDlgItem(IDC_RADIO_ALTITUDE))->SetCheck(TRUE);
        OnBnClickedRadioAltitude();
    }

    // initialize EditBox values from m_pARWayPoint
    CString strTemp;
    strTemp.Format(_T("%.2f"), m_pARWayPoint->GetAltitude());
    GetDlgItem(IDC_EDIT_ALTITUDE)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), m_pARWayPoint->GetOnTrack());
    GetDlgItem(IDC_EDIT_ONTRACK)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), m_pARWayPoint->GetInboundTrackAngle());
    GetDlgItem(IDC_EDIT_TRACKTONEXTWP)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), m_pARWayPoint->GetInterceptAngle());
    GetDlgItem(IDC_EDIT_INTERCEPTANGLE)->SetWindowText(strTemp);
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

void CDlgAirRouteWayPointDefine::OnBnClickedRadioWaypoint()
{
    EnableLevel1(FALSE);
    GetDlgItem(IDC_COMBO_WAYPOINT)->EnableWindow(TRUE);
    ShowWaypointControls(TRUE);
    ShowAltitudeControls(FALSE);

    m_spinDegrees.SetRange(0,359);

    ARWaypoint::DepartType Departtype = m_pARWayPoint->getDepartType();
    if (Departtype == ARWaypoint::NextWaypoint)
    {
        OnBnClickedRadionextwaypoint();
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_RADIOHEADING))->EnableWindow(TRUE);
        ((CButton *)GetDlgItem(IDC_RADIOHEADING))->SetCheck(TRUE);
        m_lDegrees = m_pARWayPoint->getDegrees();
        m_lVisDistance = m_pARWayPoint->getVisDistance();
        AirsideGUI::CDlgAirRoute* pParentWnd = (AirsideGUI::CDlgAirRoute*)GetParent();
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
}


void CDlgAirRouteWayPointDefine::SpinChangeEditboxValue(CEdit* pEdit, LPNMUPDOWN pNMUpDown)
{
    CString strValue;
    pEdit->GetWindowText(strValue);
    float fInput = (float)atof(strValue);
    if(pNMUpDown->iDelta > 0 && fInput < iAngleMax - 1)
    {
        fInput += 1;
    }
    else if(pNMUpDown->iDelta < 0 && fInput >= iAngleMin+1)
    {
        fInput -= 1;
    }
    strValue.Format(_T("%.2f"), fInput);
    pEdit->SetWindowText(strValue);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioAltitude()
{
    EnableLevel1(FALSE);
    GetDlgItem(IDC_EDIT_ALTITUDE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_ALTITUDE)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_ONTRACK)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_ONTRACK)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_ONTRACK)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_ANGLE)->EnableWindow(TRUE);
    ShowWaypointControls(FALSE);
    ShowAltitudeControls(TRUE);
    if(m_pARWayPoint->GetNextPtType() == ARWaypoint::ToNextWaitPoint)
    {
        ((CButton *)GetDlgItem(IDC_RADIO_TONEXTWAYPOINT))->SetCheck(TRUE);
        ((CButton *)GetDlgItem(IDC_RADIO_TO_NEXT_ALTITUDE))->SetCheck(FALSE);
        OnBnClickedRadioToNextWaypoint();
    }
    else // ARWaypoint::ToNextAltitude
    {
        ((CButton *)GetDlgItem(IDC_RADIO_TONEXTWAYPOINT))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_RADIO_TO_NEXT_ALTITUDE))->SetCheck(TRUE);
        OnBnClickedRadioToNextAltitude();
    }
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioToNextWaypoint()
{
    EnableLevel2(FALSE);
    GetDlgItem(IDC_RADIO_DIRECT)->EnableWindow(TRUE);
    GetDlgItem(IDC_RADIO_INTERCEPTTRACK)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_TRACKTONEXTWP)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP2)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_INTERCEPTANGLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE2)->EnableWindow(TRUE);
    if(m_pARWayPoint->GetDirectTpye() == ARWaypoint::DirectType_Direct)
    {
        ((CButton *)GetDlgItem(IDC_RADIO_DIRECT))->SetCheck(TRUE);
        ((CButton *)GetDlgItem(IDC_RADIO_INTERCEPTTRACK))->SetCheck(FALSE);
        OnBnClickedRadioDirect();
    }
    else // ARWaypoint::InterceptTrack
    {
        ((CButton *)GetDlgItem(IDC_RADIO_DIRECT))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_RADIO_INTERCEPTTRACK))->SetCheck(TRUE);
        OnBnClickedRadioInterceptTrack();
    }
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioToNextAltitude()
{
    EnableLevel2(FALSE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioDirect()
{
    EnableLevel3(FALSE);
}

void CDlgAirRouteWayPointDefine::OnBnClickedRadioInterceptTrack()
{
    EnableLevel3(FALSE);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_TRACKTONEXTWP)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_TRACKTONEXTWP)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP2)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_INTERCEPTANGLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_INTERCEPTANGLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE2)->EnableWindow(TRUE);
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinAltitude(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    SpinChangeEditboxValue(&m_editAltitude, pNMUpDown);
    *pResult = 0;
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinOntrack(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    SpinChangeEditboxValue(&m_editOnTrack, pNMUpDown);
    *pResult = 0;
}

void CDlgAirRouteWayPointDefine::OnDeltaposSpinInterceptangle(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    SpinChangeEditboxValue(&m_editInterceptAngle, pNMUpDown);
    *pResult = 0;
}


void CDlgAirRouteWayPointDefine::OnDeltaposSpinTracktonextwp(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    SpinChangeEditboxValue(&m_editTrackToNext, pNMUpDown);
    *pResult = 0;
}

void CDlgAirRouteWayPointDefine::ShowWaypointControls(BOOL bEnable)
{
    GetDlgItem(IDC_STATIC_DEPARTFOR)->ShowWindow(bEnable);
    GetDlgItem(IDC_RADIONEXTWAYPOINT)->ShowWindow(bEnable);
    GetDlgItem(IDC_RADIOHEADING)->ShowWindow(bEnable);
    GetDlgItem(IDC_EDITDEGREES)->ShowWindow(bEnable);
    GetDlgItem(IDC_SPIN_DEGREES)->ShowWindow(bEnable);
    GetDlgItem(IDC_STATICDEGREE)->ShowWindow(bEnable);
    GetDlgItem(IDC_COMBOHEADINGCHOICE)->ShowWindow(bEnable);
    GetDlgItem(IDC_EDITVISDISTANCE)->ShowWindow(bEnable);
    GetDlgItem(IDC_STATICVISUAL)->ShowWindow(bEnable);
}

void CDlgAirRouteWayPointDefine::ShowAltitudeControls(BOOL bEnable)
{
    GetDlgItem(IDC_RADIO_TONEXTWAYPOINT)->ShowWindow(bEnable);
    GetDlgItem(IDC_RADIO_TO_NEXT_ALTITUDE)->ShowWindow(bEnable);
    GetDlgItem(IDC_RADIO_DIRECT)->ShowWindow(bEnable);
    GetDlgItem(IDC_RADIO_INTERCEPTTRACK)->ShowWindow(bEnable);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP)->ShowWindow(bEnable);
    GetDlgItem(IDC_EDIT_TRACKTONEXTWP)->ShowWindow(bEnable);
    GetDlgItem(IDC_SPIN_TRACKTONEXTWP)->ShowWindow(bEnable);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP2)->ShowWindow(bEnable);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE)->ShowWindow(bEnable);
    GetDlgItem(IDC_EDIT_INTERCEPTANGLE)->ShowWindow(bEnable);
    GetDlgItem(IDC_SPIN_INTERCEPTANGLE)->ShowWindow(bEnable);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE2)->ShowWindow(bEnable);
}

void CDlgAirRouteWayPointDefine::EnableLevel1(BOOL bEnable)
{
    GetDlgItem(IDC_COMBO_WAYPOINT)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_ALTITUDE)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_ALTITUDE)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_ONTRACK)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_ONTRACK)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_ONTRACK)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_ANGLE)->EnableWindow(FALSE);
}

void CDlgAirRouteWayPointDefine::EnableLevel2(BOOL bEnable)
{
    GetDlgItem(IDC_RADIO_DIRECT)->EnableWindow(bEnable);
    GetDlgItem(IDC_RADIO_INTERCEPTTRACK)->EnableWindow(bEnable);
    EnableLevel3(bEnable);
}

void CDlgAirRouteWayPointDefine::EnableLevel3(BOOL bEnable)
{
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP)->EnableWindow(bEnable);
    GetDlgItem(IDC_EDIT_TRACKTONEXTWP)->EnableWindow(bEnable);
    GetDlgItem(IDC_SPIN_TRACKTONEXTWP)->EnableWindow(bEnable);
    GetDlgItem(IDC_STATIC_TRACKTONEXTWP2)->EnableWindow(bEnable);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE)->EnableWindow(bEnable);
    GetDlgItem(IDC_EDIT_INTERCEPTANGLE)->EnableWindow(bEnable);
    GetDlgItem(IDC_SPIN_INTERCEPTANGLE)->EnableWindow(bEnable);
    GetDlgItem(IDC_STATIC_INTERCEPTANGLE2)->EnableWindow(bEnable);
}

void CDlgAirRouteWayPointDefine::SetAllSpinControlRange()
{
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_INTERCEPTANGLE))->SetRange(iAngleMin, iAngleMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TRACKTONEXTWP))->SetRange(iAngleMin, iAngleMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_ALTITUDE))->SetRange(iAngleMin, iAngleMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_ONTRACK))->SetRange(iAngleMin, iAngleMax);
}



