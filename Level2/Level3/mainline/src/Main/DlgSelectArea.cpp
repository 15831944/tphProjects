// CpSelectArea.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectArea.h"
#include "../engine\terminal.h"
#include "..\inputs\AreasPortals.h"
// CDlgSelectArea dialog

IMPLEMENT_DYNAMIC( CDlgSelectArea, CDialog)
CDlgSelectArea::CDlgSelectArea(Terminal* pTerminal,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectArea::IDD, pParent)
{
	m_pTerminal = pTerminal;
	m_strArea = _T("");
}

CDlgSelectArea::~CDlgSelectArea()
{
}

void CDlgSelectArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AREA, m_lstArea);
}


BEGIN_MESSAGE_MAP(CDlgSelectArea, CDialog)
END_MESSAGE_MAP()


// CDlgSelectArea message handlers

BOOL CDlgSelectArea::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pTerminal == NULL)
		return FALSE;

	CAreas* pAreas = m_pTerminal->m_pAreas;
	int nCount = pAreas->m_vAreas.size();
	m_lstArea.ResetContent();
	for(int i=0; i<nCount; i++ )
	{		
		CString csStr = pAreas->m_vAreas[i]->name;
		int nIdx = m_lstArea.AddString( csStr );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectArea::OnOK()
{
	if (m_pTerminal != NULL)
	{
		int nSel = m_lstArea.GetCurSel();
		if (nSel != LB_ERR)
		{
			CAreas* pAreas = m_pTerminal->m_pAreas;
			m_strArea = pAreas->m_vAreas[nSel]->name;
		}
	}
	CDialog::OnOK();
}
