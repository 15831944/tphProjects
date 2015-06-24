// DlgProcessorTags.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "DlgProcessorTags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Strip out non-display characters from input and save and return output...
char *StripGarbage(const char *pszInput, char *pszOutput)
{
    // Variables...
    unsigned int    unInputIndex    = 0;
    unsigned int    unOutputIndex   = 0;

    // Scan through input and save graphic characters...
    for(unInputIndex = 0, unOutputIndex = 0; 
        unInputIndex < strlen(pszInput); 
        unInputIndex++)
    {
        // This is a display character or a space...
        if(isgraph(pszInput[unInputIndex]) || pszInput[unInputIndex] == ' ')
        {
            // Save...
            pszOutput[unOutputIndex] = pszInput[unInputIndex];

            // Seek to next output location...
            unOutputIndex++;
        }
    }

    // Terminate output...
    pszOutput[unOutputIndex] = '\x0';
    
    // Return output...
    return pszOutput;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgProcessorTags dialog


CDlgProcessorTags::CDlgProcessorTags(CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessorTags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcessorTags)
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
}


void CDlgProcessorTags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcessorTags)
	DDX_Control(pDX, IDC_TREE_PROCTAGS, m_treeProcTags);
	DDX_Control(pDX, IDC_STATIC_FRAME1, m_btnFrame1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcessorTags, CDialog)
	//{{AFX_MSG_MAP(CDlgProcessorTags)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProcessorTags message handlers

void CDlgProcessorTags::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TRACE("OnSize(%d, %d)\n", cx, cy);

	CRect btnRC;
	GetDlgItem(IDOK)->GetWindowRect(&btnRC);

	m_btnFrame1.MoveWindow(10, 10, cx-20, cy-btnRC.Height()-30);

	CRect r;
	m_btnFrame1.GetWindowRect(&r);
	ScreenToClient(&r);
	r.DeflateRect(4,15,5,5);
	m_treeProcTags.MoveWindow(&r);

	GetDlgItem(IDOK)->MoveWindow((cx/2)-btnRC.Width()-10, cy-btnRC.Height()-10, btnRC.Width(), btnRC.Height());
	GetDlgItem(IDCANCEL)->MoveWindow((cx/2)+10, cy-btnRC.Height()-10, btnRC.Width(), btnRC.Height());
}

BOOL CDlgProcessorTags::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitTree();
	LoadData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProcessorTags::InitTree()
{
	// Variables...
    char                szOutput[1024] = {0};
    COOLTREE_NODE_INFO  cni;

    CCoolTree::InitNodeInfo(this,cni);

	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;

	int i;
	int j=CProcessorTags::PROCTAGGEOGRAPHIC;
	int k=CProcessorTags::PROCTAGBEHAVIORAL;

    StripGarbage(CProcessorTags::PROCTAGNAMES[j], szOutput);
    m_htreeitems[j] = m_treeProcTags.InsertItem(szOutput, cni, FALSE);

	for(i=j+1; i<k; i++) {
        StripGarbage(CProcessorTags::PROCTAGNAMES[i], szOutput);
		m_htreeitems[i] = m_treeProcTags.InsertItem(szOutput, cni, FALSE, FALSE, m_htreeitems[j]);
	}

    StripGarbage(CProcessorTags::PROCTAGNAMES[k], szOutput);
	m_htreeitems[k] = m_treeProcTags.InsertItem(szOutput, cni, FALSE);

	for(i=k+1; i<PROCESSORTAGCOUNT; i++) {
        StripGarbage(CProcessorTags::PROCTAGNAMES[i], szOutput);
		m_htreeitems[i] = m_treeProcTags.InsertItem(szOutput, cni, FALSE, FALSE, m_htreeitems[k]);
	}

	m_treeProcTags.Expand(m_htreeitems[j],TVE_EXPAND);
	m_treeProcTags.Expand(m_htreeitems[k],TVE_EXPAND);
}

void CDlgProcessorTags::LoadData()
{
	const CProcessorTags& proctags = m_pDoc->m_procTags;

	for(int i=0; i<PROCESSORTAGCOUNT; i++) {
		m_treeProcTags.SetCheckStatus(m_htreeitems[i], proctags.Get(i));
	}
}

void CDlgProcessorTags::OnOK() 
{
	CProcessorTags& proctags = m_pDoc->m_procTags;

	for(int i=0; i<PROCESSORTAGCOUNT; i++) {
		proctags.Set(i, m_treeProcTags.IsCheckItem(m_htreeitems[i]) ? true : false);
	}

	proctags.saveDataSet(m_pDoc->m_ProjInfo.path, false);
	
	CDialog::OnOK();
}

void CDlgProcessorTags::OnCancel() 
{
	m_pDoc->m_procTags.loadDataSet(m_pDoc->m_ProjInfo.path);
	
	CDialog::OnCancel();
}

void CDlgProcessorTags::OnDestroy() 
{
	CDialog::OnDestroy();
	
	delete [] m_htreeitems;
	
}

int CDlgProcessorTags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_htreeitems = new HTREEITEM[PROCESSORTAGCOUNT];
	
	return 0;
}
