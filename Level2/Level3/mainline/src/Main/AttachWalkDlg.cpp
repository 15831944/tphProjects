// AttachWalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "AttachWalkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttachWalkDlg dialog


CAttachWalkDlg::CAttachWalkDlg(CPipe* _pPipe,CTermPlanDoc* _pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CAttachWalkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttachWalkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pPipe	= _pPipe;
	m_pDoc = _pDoc;
}


void CAttachWalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttachWalkDlg)
	DDX_Control(pDX, IDC_LIST_WALK, m_listWalk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttachWalkDlg, CDialog)
	//{{AFX_MSG_MAP(CAttachWalkDlg)
	ON_LBN_SELCHANGE(IDC_LIST_WALK, OnSelchangeListWalk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttachWalkDlg message handlers

BOOL CAttachWalkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	
	for( int i=0;i<m_pDoc->GetTerminal().procList->getProcessorCount(); i++ )
	{
		 Processor* pProcessor	= m_pDoc->GetTerminal().procList->getProcessor( i );
		if(pProcessor->getProcessorType() == MovingSideWalkProc)
		{
			if( pPipe->CanAttached( *(pProcessor->getID()) ) )
			{
				if( !pPipe->IsSideWalkHasAttach( *(pProcessor->getID())))
				{
					m_listWalk.InsertString( -1, pProcessor->getID()->GetIDString() ); 
					m_listWalk.SetItemData( m_listWalk.GetCount()-1, i );
				}
			}
		}

	}

	GetDlgItem( IDOK )->EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttachWalkDlg::OnOK() 
{
	// TODO: Add extra validation here
	for( int i=0; i<m_listWalk.GetCount(); i++ )
	{
		if( m_listWalk.GetSel(i) > 0 )
		{
			int iIndex	= m_listWalk.GetItemData( i );
			pPipe->AttachSideWalk( *(m_pDoc->GetTerminal().procList->getProcessor(iIndex)->getID()) );
		}
	}

	CDialog::OnOK();
}


void CAttachWalkDlg::OnSelchangeListWalk() 
{
	// TODO: Add your control notification handler code here
	if( m_listWalk.GetSelCount() > 0 )
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}