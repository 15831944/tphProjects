// DlgAircraftDeckNameDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftDeckNameDefine.h"
#include "../InputOnBoard/Deck.h"
#include ".\dlgaircraftdecknamedefine.h"
#include "../Common/CARCUnit.h"
// CDlgAircraftDeckNameDefine dialog

IMPLEMENT_DYNAMIC(CDlgAircraftDeckNameDefine, CDialog)
CDlgAircraftDeckNameDefine::CDlgAircraftDeckNameDefine(CDeck* _deck ,CWnd* pParent /*=NULL*/)
	:m_deck(_deck), CDialog(CDlgAircraftDeckNameDefine::IDD, pParent)
{
}

CDlgAircraftDeckNameDefine::~CDlgAircraftDeckNameDefine()
{
}

void CDlgAircraftDeckNameDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_combo_level1);
	DDX_Control(pDX, IDC_COMBO_ACTYPE, m_combo_level2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_combo_level3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_combo_level4);
	DDX_Control(pDX, IDC_EDIT_GROUND, m_Ground);
	DDX_Control(pDX, IDC_EDIT_THICKNESS, m_thickness);
	DDX_Control(pDX, IDC_SPIN_GROUND, m_ground_spin);
	DDX_Control(pDX, IDC_SPIN_THICKNESS, m_thickness_spin);
}

BEGIN_MESSAGE_MAP(CDlgAircraftDeckNameDefine, CDialog)
	ON_NOTIFY(NM_THEMECHANGED, IDC_SPIN_GROUND, OnNMThemeChangedSpinGround)
	ON_NOTIFY(NM_THEMECHANGED, IDC_SPIN_THICKNESS, OnNMThemeChangedSpinThickness)
END_MESSAGE_MAP()

BOOL CDlgAircraftDeckNameDefine::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	OnInitCaption() ;
	OnInitComboxName() ;
	OnInitEdit() ;
	return TRUE ;
}
void CDlgAircraftDeckNameDefine::OnInitCaption()
{
	CString caption ;
	caption.Format(_T("Aircraft Deck Configuration - %s"),m_deck->GetName()) ;
	SetWindowText(caption) ;
}
void CDlgAircraftDeckNameDefine::OnInitComboxName()
{
   int ndx = 0 ;
   if(!m_deck->GetLevel1Name().IsEmpty())
   {
    ndx = m_combo_level1.AddString(m_deck->GetLevel1Name()) ;
    m_combo_level1.SetCurSel(ndx) ;
   }
   if(!m_deck->GetLevel2Name().IsEmpty())
   {
	   ndx = m_combo_level2.AddString(m_deck->GetLevel2Name());
	   m_combo_level2.SetCurSel(ndx) ;
   }
   if(!m_deck->GetLevel3Name().IsEmpty())
   {
	   ndx = m_combo_level3.AddString(m_deck->GetLevel3Name());
	   m_combo_level3.SetCurSel(ndx) ;
   }
   if(!m_deck->Getlevel4Name().IsEmpty())
   {
	   ndx = m_combo_level4.AddString(m_deck->Getlevel4Name()) ;
	   m_combo_level4.SetCurSel(ndx) ;
   }
}
void CDlgAircraftDeckNameDefine::OnInitEdit()
{
	CString val ;
  double ground = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,m_deck->RealAltitude()) ;
   val.Format("%0.2f",ground) ;
	m_Ground.SetWindowText(val) ;
	double thickness = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,m_deck->GetThickness()) ;

 val.Format("%0.2f",thickness) ;
	m_thickness.SetWindowText(val) ;
  //  m_ground_spin.SetBase()
}
void CDlgAircraftDeckNameDefine::GetLevel1Name()
{
	CString val ;
	m_combo_level1.GetWindowText(val) ;
   m_deck->SetLevel1Name(val) ;
}
void CDlgAircraftDeckNameDefine::GetLevel2Name()
{
	CString val ;
	m_combo_level2.GetWindowText(val) ;
	m_deck->SetLevel2Name(val) ;
}
void CDlgAircraftDeckNameDefine::GetLevel3Name()
{
	CString val ;
	m_combo_level3.GetWindowText(val) ;
	m_deck->SetLevel3Name(val) ;
}
void CDlgAircraftDeckNameDefine::GetLevel4Name()
{
	CString val ;
	m_combo_level4.GetWindowText(val) ;
	m_deck->SetLevel4Name(val) ;
}
void CDlgAircraftDeckNameDefine::GetGround()
{
	CString val ;
	m_Ground.GetWindowText(val) ;
	m_deck->RealAltitude(CARCLengthUnit::ConvertLength(AU_LENGTH_METER,DEFAULT_DATABASE_LENGTH_UNIT,atof(val)) );
}
void CDlgAircraftDeckNameDefine::GetThicknessVal()
{
	CString val ;
	m_thickness.GetWindowText(val) ;
	m_deck->SetThickness(CARCLengthUnit::ConvertLength(AU_LENGTH_METER,DEFAULT_DATABASE_LENGTH_UNIT,atof(val)) );
}
void CDlgAircraftDeckNameDefine::OnOK()
{
  GetLevel1Name();
  GetLevel2Name() ;
GetLevel3Name() ;
GetLevel4Name() ;
GetGround() ;
GetThicknessVal() ;
CDialog::OnOK() ;
}
void CDlgAircraftDeckNameDefine::OnCancel()
{
	CDialog::OnCancel() ;
}
// CDlgAircraftDeckNameDefine message handlers

void CDlgAircraftDeckNameDefine::OnNMThemeChangedSpinGround(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	CString val ;
	m_Ground.GetWindowText(val) ;
	double floatval = atof(val) ;
	val.Format("%0.2f",floatval) ;
	m_Ground.SetWindowText(val) ;
	*pResult = 0;
}

void CDlgAircraftDeckNameDefine::OnNMThemeChangedSpinThickness(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	CString val ;
	m_thickness.GetWindowText(val) ;
	double floatval = atof(val) ;
	val.Format("%0.2f",floatval) ;
	m_thickness.SetWindowText(val) ;
	*pResult = 0;
}
