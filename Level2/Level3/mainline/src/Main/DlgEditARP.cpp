// DlgEditARP.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgEditARP.h"
#include "dlgeditarp.h"

#include "TermPlanDoc.h"
#include "../Inputs/AirsideInput.h"
#include "TVNode.h"
#include "ModelingSequenceManager.h"
#include "AirSideMSImpl.h"
#include "../Inputs/AirportInfo.h"

#include "3DView.h"
#include "MainFrm.h"
#include <CommonData/Fallback.h>
#include "../common/UnitsManager.h"
#include "../Common/WinMsg.h"
#include ".\dlgeditarp.h"
#include "../inputAirside/ALTAirport.h"

// CDlgEditARP dialog
static bool bUnitChange = false;
IMPLEMENT_DYNAMIC(CDlgEditARP, CDialog)
CDlgEditARP::CDlgEditARP(int nProjID,int nAIrportID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditARP::IDD, pParent)
	, m_fMagVariation(0)
{
	m_dx			= 0L;
	m_dy			= 0L;
	m_dElevation	= 0L;
	m_nAirportID = -1;
	m_pAirport = new ALTAirport;
	m_bxyModified = false;
	m_bModified = false;
	m_nAirportID = nAIrportID;
	m_nProjID = nProjID;
}

CDlgEditARP::~CDlgEditARP()
{
	delete m_pAirport;
}

void CDlgEditARP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_X, m_editdx);
	DDX_Control(pDX, IDC_EDIT_Y, m_editdy);
	DDX_Control(pDX, IDC_EDIT_ELEVATION, m_editelevation);
	DDX_Control(pDX, IDC_COMBO1, m_MagVariationEastWestCombo);
	DDX_Control(pDX, IDC_EDIT_MAGVARIATION, m_editMagVar);
}


BEGIN_MESSAGE_MAP(CDlgEditARP, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PICK, OnBnClickedButtonPick)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_KILLFOCUS(IDC_EDIT_ELEVATION, OnEnKillfocusEditElevation)
	ON_EN_CHANGE(IDC_EDIT_ELEVATION, OnEnChangeEditElevation)
END_MESSAGE_MAP()


// CDlgEditARP message handlers
BOOL CDlgEditARP::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	m_pAirport->ReadAirport(m_nAirportID);
	m_strLatitude = m_pAirport->getLatitude();
	m_strLongitude = m_pAirport->getLongtitude();

//	CTermPlanDoc *pDoc=(CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
//	pDoc->GetTerminal().m_AirsideInput->GetLL(pDoc->m_ProjInfo.path,m_strLongitude,m_strLatitude);
	//latitude longitude
	CRect rectLongitude, rectLatitude;
	GetDlgItem(IDC_LONGITUDE)->GetWindowRect(&rectLongitude);
	GetDlgItem(IDC_LATITUDE)->GetWindowRect(&rectLatitude);
	ScreenToClient(rectLongitude);
	ScreenToClient(rectLatitude);

	//	GetDlgItem(IDC_LONGITUDE)->MoveWindow(CRect(0,0,0,0));
	//	GetDlgItem(IDC_LATITUDE)->MoveWindow(CRect(0,0,0,0));
	GetDlgItem(IDC_LONGITUDE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LATITUDE)->ShowWindow(SW_HIDE);

	m_pLongitude.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 10);
	m_pLatitude.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 11);
	m_pLongitude.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pLatitude.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pLongitude.SetWindowPos(&CWnd::wndTop, rectLongitude.left, rectLongitude.top,
		rectLongitude.Width(), rectLongitude.Height(), SWP_SHOWWINDOW);
	m_pLatitude.SetWindowPos(&CWnd::wndTop, rectLatitude.left, rectLatitude.top, 
		rectLatitude.Width(), rectLatitude.Height(), SWP_SHOWWINDOW);

	m_pLongitude.SetComboBoxItem("E");
	m_pLongitude.SetComboBoxItem("W");
	m_pLongitude.SetItemRange(180);
	m_pLongitude.SetCtrlText(m_strLongitude);

	m_pLatitude.SetComboBoxItem("N");
	m_pLatitude.SetComboBoxItem("S");
	m_pLatitude.SetItemRange(90);
	m_pLatitude.SetCtrlText(m_strLatitude);
	m_pLatitude.m_Addr[1].SetLimitText(2);

	// x,y position

//	pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->GetPosition(m_dx,m_dy);
	m_dx = m_pAirport->getRefPoint().getX();
	m_dy = m_pAirport->getRefPoint().getY();
	CString strdx ,strdy;
	strdx.Format("%.2f",m_dx);
	strdy.Format("%.2f",m_dy);
	m_editdx.SetWindowText(strdx);
	m_editdy.SetWindowText(strdy);
	
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_24,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	GetDlgItem(IDC_STATIC_ELEVATION)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);

	CString strElevationUnit(_T(""));
	strElevationUnit.Format(_T("Elevation(%s):"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	GetDlgItem(IDC_STATIC_ELEVATION)->SetWindowText(strElevationUnit);
	m_dElevation = m_pAirport->getElevation();
	double dElevation = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dElevation);
	CString strElevation ;
	strElevation.Format("%.2f",dElevation);
	bUnitChange = true;
	m_editelevation.SetWindowText(strElevation);
	bUnitChange = false;
	//
	m_fMagVariation = m_pAirport->GetMagnectVariation().m_dval;
	m_nMagEastWestSelected = m_pAirport->GetMagnectVariation().m_iEast_West;
	CString strMagVar ;
	strMagVar.Format("%.0f",m_fMagVariation);
	m_editMagVar.SetWindowText(strMagVar);
	m_MagVariationEastWestCombo.AddString( "E" );
	m_MagVariationEastWestCombo.AddString( "W" );
	
	m_MagVariationEastWestCombo.SetCurSel(m_nMagEastWestSelected);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// on pick from map
void CDlgEditARP::OnBnClickedButtonPick()
{
	// check the type
//	int nSelIdx = m_comboType.GetCurSel();
//	if( nSelIdx < 0 )
//		return;
//	int nProcType = m_comboType.GetItemData( nSelIdx );

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;

	enumReason= PICK_ONEPOINT;


	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}
// fall back finished
LRESULT CDlgEditARP::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{

	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	if(pData->size()>0)
	{
//		double dx,dy;
		Point ptList[MAX_POINTS];
		ARCVector3 v3D = pData->at(0);
		CString strTemp;
		m_dx= UNITSMANAGER->ConvertLength(v3D[VX]);
		m_dy=UNITSMANAGER->ConvertLength(v3D[VY]);
		
		//show the x , y position on the dialog
		CString strdx ,strdy;
		strdx.Format("%.2f",m_dx);
		strdy.Format("%.2f",m_dy);

		m_editdx.SetWindowText(strdx);
		m_editdy.SetWindowText(strdy);
	}
	//pDoc->UpdateAllViews(NULL);

	m_bxyModified = true;
	return TRUE;
}

void CDlgEditARP::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgEditARP::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}
void CDlgEditARP::OnBnClickedOk()
{	
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	 bool bModified = false;
	// x,y position
	CString strdx,strdy;
	m_editdx.GetWindowText(strdx);
	m_editdy.GetWindowText(strdy);
	m_dx = atof(strdx);
	m_dy = atof(strdy);


//	pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->SetElevation(m_dElevation);

	//longitude latitude 
	m_pLongitude.GetCtrlText(m_strLongitude);
	m_pLatitude.GetCtrlText(m_strLatitude);
//	pDoc->GetTerminal().m_AirsideInput->SetLL(pDoc->m_ProjInfo.path,m_strLongitude,m_strLatitude,FALSE);
	
	//if (m_bxyModified)
	{
		m_pAirport->setRefPoint(CPoint2008(m_dx,m_dy,0));
		m_bModified = true;
	}

	if (m_pAirport->getLatitude().CompareNoCase(m_strLatitude) != 0)
	{
		m_pAirport->setlatitude(m_pLatitude.GetCtrlValue());
		m_bModified = true;
	}

	if (m_pAirport->getLongtitude().CompareNoCase(m_strLongitude) != 0)
	{
		m_pAirport->setLongitude(m_pLongitude.GetCtrlValue());
		m_bModified = true;
	}

	
	if (m_pAirport->getElevation() != m_dElevation)
	{		
		m_pAirport->setElevation(m_dElevation);
		m_bModified = true;
	}

	MagneticVariation magVar;
	CString magVarStr;
	m_editMagVar.GetWindowText(magVarStr);	
	magVar.m_dval = atof(magVarStr);
	m_fMagVariation = magVar.m_dval;
	magVar.m_iEast_West = m_MagVariationEastWestCombo.GetCurSel();
	m_nMagEastWestSelected = m_MagVariationEastWestCombo.GetCurSel();
	if( magVar.m_dval != m_pAirport->GetMagnectVariation().m_dval || magVar.m_iEast_West!= m_pAirport->GetMagnectVariation().m_iEast_West)
	{ 
		m_pAirport->SetMagneticVariation(magVar);
		m_bModified = true;
	}

	if (m_bModified)
	{
		m_pAirport->UpdateAirport(m_nAirportID);
	}





	OnOK();
}

void CDlgEditARP::OnEnKillfocusEditElevation()
{
	// TODO: Add your control notification handler code here
	
}

void CDlgEditARP::OnEnChangeEditElevation()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!bUnitChange)
	{
		CString strElevation;
		m_editelevation.GetWindowText(strElevation);
		m_dElevation = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strElevation));
	}
}

bool CDlgEditARP::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CDlgEditARP::RefreshGUI(void)
{
	CString strElevationUnit(_T(""));
	strElevationUnit.Format(_T("Elevation(%s):"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	GetDlgItem(IDC_STATIC_ELEVATION)->SetWindowText(strElevationUnit);

	bUnitChange = true;
	double dElevation = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dElevation);
	CString strElevation ;
	strElevation.Format("%.2f",dElevation);
	m_editelevation.SetWindowText(strElevation);
	bUnitChange = false;
	return (true);
}

bool CDlgEditARP::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}