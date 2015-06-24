// DlgProcDispOverrides.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "Processor2.h"
#include "DlgProcDispOverrides.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProcDispOverrides dialog


CDlgProcDispOverrides::CDlgProcDispOverrides(BOOL* pVals, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcDispOverrides::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcDispOverrides)
	//}}AFX_DATA_INIT
	for(int i=PDP_MIN; i<=PDP_MAX; i++)
	{
		m_bDisplay[i] = pVals[i];
	}
}


void CDlgProcDispOverrides::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcDispOverrides)
	DDX_Check(pDX, IDC_CHK_SHAPE, m_bDisplay[PDP_DISP_SHAPE]);
	DDX_Check(pDX, IDC_CHK_SERVLOC, m_bDisplay[PDP_DISP_SERVLOC]);
	DDX_Check(pDX, IDC_CHK_QUEUE, m_bDisplay[PDP_DISP_QUEUE]);
	DDX_Check(pDX, IDC_CHK_PROCNAME, m_bDisplay[PDP_DISP_PROCNAME]);
	DDX_Check(pDX, IDC_CHK_OUTCONSTRAINT, m_bDisplay[PDP_DISP_OUTC]);
	DDX_Check(pDX, IDC_CHK_INCONSTRAINT, m_bDisplay[PDP_DISP_INC]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcDispOverrides, CDialog)
	//{{AFX_MSG_MAP(CDlgProcDispOverrides)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProcDispOverrides message handlers
