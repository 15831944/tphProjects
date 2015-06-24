// DynamicCreatedProcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DynamicCreatedProcDlg.h"
#include "engine\proclist.h"
#include <Inputs/IN_TERM.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynamicCreatedProcDlg dialog


CDynamicCreatedProcDlg::CDynamicCreatedProcDlg(InputTerminal* _pTerm,CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicCreatedProcDlg::IDD, pParent)
{
	m_pTerm = _pTerm;	
}


void CDynamicCreatedProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDynamicCreatedProcDlg)
	DDX_Control(pDX, IDC_LIST_PROC, m_listProcs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDynamicCreatedProcDlg, CDialog)
	//{{AFX_MSG_MAP(CDynamicCreatedProcDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicCreatedProcDlg message handlers

BOOL CDynamicCreatedProcDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	ProcessorList* pList = m_pTerm->procList->GetDynamicCreatedProcs();
	if( pList )
	{
		int iCount = pList->getProcessorCount();
		for( int i=0; i<iCount; ++i )
		{
			m_listProcs.AddString( pList->getProcessor( i )->getID()->GetIDString() );
		}
	}
	//m_listProc.Expand()
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
