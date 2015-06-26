// CpSelectArea.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectPortal.h"
#include "Engine\TERMINAL.H"
#include "..\inputs\AreasPortals.h"
// CDlgSelectPortal dialog

IMPLEMENT_DYNAMIC( CDlgSelectPortal, CDialog)
	CDlgSelectPortal::CDlgSelectPortal(Terminal* pTermina,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectPortal::IDD, pParent)
{
	m_pTerminal = pTermina;
}

CDlgSelectPortal::~CDlgSelectPortal()
{
}

void CDlgSelectPortal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AREA, m_lstPortal);
}


BEGIN_MESSAGE_MAP(CDlgSelectPortal, CDialog)
END_MESSAGE_MAP()


// CDlgSelectPortal message handlers

BOOL CDlgSelectPortal::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pTerminal == NULL)
		return FALSE;

	int nCount = m_pTerminal->m_pPortals->m_vPortals.size();
	m_lstPortal.ResetContent();
	for(int i=0; i<nCount; i++ )
	{		
		CString csStr = m_pTerminal->m_pPortals->m_vPortals[i]->name;
		int nIdx = m_lstPortal.AddString( csStr );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectPortal::OnOK()
{
	if (m_pTerminal != NULL)
	{
		int nSel = m_lstPortal.GetCurSel();
		if (nSel != LB_ERR)
		{
			m_strPortal = m_pTerminal->m_pPortals->m_vPortals[nSel]->name;
		}
	}
	CDialog::OnOK();
}
