// DlgAircraftConfigurationNameDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftConfigurationNameDefine.h"
#include "../Inputs/IN_TERM.H"
#include "../Common/Aircraft.h"
#include "../Common/ACTypesManager.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "DlgAircraftConfigurationSpecView.h"
// CDlgAircraftConfigurationNameDefine dialog

IMPLEMENT_DYNAMIC(CDlgAircraftConfigurationNameDefine, CDialog)
CDlgAircraftConfigurationNameDefine::CDlgAircraftConfigurationNameDefine(CAircaftLayOut* _layout ,InputTerminal* _terminal ,CWnd* pParent /*=NULL*/)
	:m_layout(_layout),m_Terminal(_terminal), CDialog(CDlgAircraftConfigurationNameDefine::IDD, pParent)
{
}

CDlgAircraftConfigurationNameDefine::~CDlgAircraftConfigurationNameDefine()
{
}

void CDlgAircraftConfigurationNameDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_Combo_level2);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_Combo_level1);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_Combo_level3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_Combo_level4);
	DDX_Control(pDX, IDC_EDIT_CREATOR, m_edit_Creator);
}


BEGIN_MESSAGE_MAP(CDlgAircraftConfigurationNameDefine, CDialog)
END_MESSAGE_MAP()
BOOL CDlgAircraftConfigurationNameDefine::OnInitDialog()
{
	CDialog::OnInitDialog() ;

	return TRUE ;
}

CString CDlgAircraftConfigurationNameDefine::GetCurSelLevel4Name()
{
	CString val ;
	m_Combo_level4.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftConfigurationNameDefine::GetCurSelLevel1Name()
{
	CString val ;
	m_Combo_level1.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftConfigurationNameDefine::GetCurSelLevel2Name()
{
	CString val ;
	m_Combo_level2.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftConfigurationNameDefine::GetCurSelLevel3Name()
{
	CString val ;
	m_Combo_level3.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftConfigurationNameDefine::GetCreator()
{
	CString val ;
	m_edit_Creator.GetWindowText(val) ;
	return val ;
}
void CDlgAircraftConfigurationNameDefine::SetTimeString()
{
	CTime time ;
	time = CTime::GetCurrentTime() ;
	CString createtime ; 
	createtime = time.Format("%m/%d/%y-%H:%M") ;
	m_layout->SetCreateTime(createtime);
	m_layout->SetModifyTime(createtime) ;
}
void CDlgAircraftConfigurationNameDefine::OnOK()
{
   if(m_layout != NULL)
   {
	   m_layout->SetLevel1Name(GetCurSelLevel1Name()) ;
	   m_layout->SetLevel2Name(GetCurSelLevel2Name()) ;
	   m_layout->SetLevel3Name(GetCurSelLevel3Name()) ;
	    m_layout->SetLevel4Name(GetCurSelLevel4Name()) ;
	   m_layout->SetCreator(GetCreator()) ;
	   SetTimeString() ;
   }
   CDialog::OnOK() ;
}
void CDlgAircraftConfigurationNameDefine::OnCancel()
{
	CDialog::OnCancel() ;
}

CDlgAircraftConfigurationNameEdit::CDlgAircraftConfigurationNameEdit(CAircaftLayOut* _layout ,InputTerminal* _terminal ,CWnd* pParent /* = NULL */)
:CDlgAircraftConfigurationNameDefine(_layout,_terminal,pParent)
{

}
CDlgAircraftConfigurationNameEdit::~CDlgAircraftConfigurationNameEdit()
{

}
void CDlgAircraftConfigurationNameEdit::SetTimeString()
{
	CTime time ;
    time = CTime::GetCurrentTime() ;
	CString createtime ; 
	createtime = time.Format("%m/%d/%y-%H:%M") ;
	m_layout->SetModifyTime(createtime) ;
}
BOOL CDlgAircraftConfigurationNameEdit::OnInitDialog()
{
	CDlgAircraftConfigurationNameDefine::OnInitDialog() ;
	m_Combo_level1.SetWindowText(m_layout->GetLevel1Name()) ;
	m_Combo_level2.SetWindowText(m_layout->GetLevel2Name()) ;
	m_Combo_level3.SetWindowText(m_layout->GetLevel3Name()) ;
	m_Combo_level4.SetWindowText(m_layout->GetLevel4Name()) ;
	m_edit_Creator.SetWindowText(m_layout->GetCreator()) ;
	return TRUE ;
}
// CDlgAircraftConfigurationNameDefine message handlers
