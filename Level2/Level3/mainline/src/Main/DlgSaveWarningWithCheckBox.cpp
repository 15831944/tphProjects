// DlgSaveWarningWithCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSaveWarningWithCheckBox.h"


// CDlgSaveWarningWithCheckBox dialog

IMPLEMENT_DYNAMIC(CDlgSaveWarningWithCheckBox, CDialog)
CDlgSaveWarningWithCheckBox::CDlgSaveWarningWithCheckBox(CWnd* pParent /*=NULL*/)
	:m_IsCheck(FALSE), CDialog(CDlgSaveWarningWithCheckBox::IDD, pParent)
{
}

CDlgSaveWarningWithCheckBox::~CDlgSaveWarningWithCheckBox()
{
}

void CDlgSaveWarningWithCheckBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SAVE, m_CheckBox);
}


BEGIN_MESSAGE_MAP(CDlgSaveWarningWithCheckBox, CDialog)
END_MESSAGE_MAP()

void CDlgSaveWarningWithCheckBox::OnOK()
{
	if(m_CheckBox.GetCheck() == BST_CHECKED)
		m_IsCheck =TRUE ;
	else
		m_IsCheck  = FALSE ;
	CDialog::OnOK() ;
}
BOOL CDlgSaveWarningWithCheckBox::IsCheck()
{
	return m_IsCheck ;
}
// CDlgSaveWarningWithCheckBox message handlers
