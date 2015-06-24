// DlgFloorThickness.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgFloorThickness.h"

#include "../common/UnitsManager.h"
#include "Common/CARCUnit.h"
#include "InputAirside/ARCUnitManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFloorThickness dialog


CDlgFloorThickness::CDlgFloorThickness(double _thickness,CTermPlanDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFloorThickness::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFloorThickness)
	m_pDoc = pDoc;	
	if (m_pDoc->m_systemMode == EnvMode_LandSide || m_pDoc->m_systemMode == EnvMode_AirSide)
	{
		m_sUnits = CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
	}
	else
	{
		m_sUnits = CUnitsManager::GetInstance()->GetLengthUnitString();
	}	
	//}}AFX_DATA_INIT
	m_dThickness = _thickness;
}


void CDlgFloorThickness::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFloorThickness)
	DDX_Control(pDX, IDC_SPIN_THICKNESS, m_spinThickness);
	DDX_Text(pDX, IDC_STATIC_UNITS, m_sUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFloorThickness, CDialog)
	//{{AFX_MSG_MAP(CDlgFloorThickness)
	ON_EN_KILLFOCUS(IDC_EDIT_THICKNESS, OnKillfocusEditThickness)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THICKNESS, OnDeltaposSpinThickness)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFloorThickness message handlers

void CDlgFloorThickness::OnKillfocusEditThickness() 
{
	CString s;
	CWnd* pEditWnd = GetDlgItem(IDC_EDIT_THICKNESS);
	pEditWnd->GetWindowText(s);

	if (m_pDoc->m_systemMode == EnvMode_LandSide || m_pDoc->m_systemMode == EnvMode_AirSide)
	{
		m_dThickness = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),atof(s));
	}
	else
	{
		m_dThickness = CUnitsManager::GetInstance()->UnConvertLength(atof(s));
	}
		m_spinThickness.SetPos(static_cast<int>(m_dThickness));
	UpdateTextField();
}

void CDlgFloorThickness::OnDeltaposSpinThickness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// TRACE("Pos %d, Delta %d\n", pNMUpDown->iPos, pNMUpDown->iDelta);
	m_dThickness = pNMUpDown->iPos + pNMUpDown->iDelta;
	UpdateData(FALSE);
	UpdateTextField();
	
	*pResult = 0;
}

BOOL CDlgFloorThickness::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//CWnd* pEditWnd = GetDlgItem(IDC_EDIT_THICKNESS);
	//m_spinThickness.SetBuddy(pEditWnd);
	m_spinThickness.SetRange(0, 10000); //in cm
	m_spinThickness.SetPos(static_cast<int>(m_dThickness));

	UpdateTextField();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFloorThickness::UpdateTextField()
{
	CString s;
	if (m_pDoc->m_systemMode == EnvMode_LandSide || m_pDoc->m_systemMode == EnvMode_AirSide)
	{
		s.Format("%.2f", CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),m_dThickness));
	}
	else
	{
		s.Format("%.2f", CUnitsManager::GetInstance()->ConvertLength(m_dThickness));
	}	
	CWnd* pEditWnd = GetDlgItem(IDC_EDIT_THICKNESS);
	pEditWnd->SetWindowText(s);
}
