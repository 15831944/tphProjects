// ProcDataSheet.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcDataSheet.h"
#include "TermPlanDoc.h"
#include "LinkageDlg.h"
#include "ProcDataPageDepSink.h"
#include "BillboardProcDataPage.h"
#include "RetailProcDataPage.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcDataSheet

IMPLEMENT_DYNAMIC(CProcDataSheet, CResizableSheet)

CProcDataSheet::CProcDataSheet(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage) 
	:CResizableSheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pParent=pParentWnd;
}

CProcDataSheet::CProcDataSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(pszCaption, pParentWnd, iSelectPage)
{
	// by aivin memo.
	// AirSide include: 
	//	ArpProcessor, RunwayProcessor, TaxiwayProcessor, StandProcessor, NodeProcessor, DeiceBayProcessor, FixProcessor
	//	EnvMode_LandSide = 0, EnvMode_Terminal, EnvMode_AirSide	
	
	m_pParent=pParentWnd;

	int startmode, endmode;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	switch( pDoc->m_systemMode )
	{
	case EnvMode_LandSide:
		startmode = PointProc;
		endmode = FixProcessor;
		break;
	case EnvMode_Terminal:
		startmode = PointProc;
		endmode = RetailProc;//
		break;
	case EnvMode_AirSide:
		startmode = ArpProcessor;
		endmode = FixProcessor;
		break;
	default:
		{
			ASSERT(0);
			startmode = PointProc;
			endmode = RetailProc;
			break;
		}

	}

	for( int i=startmode; i<= endmode; i++ )
	{
		if (i == DepSinkProc) 
		{
			m_pProcDataPage[i] = new CProcDataPageDepSink( (enum PROCDATATYPE)i, GetInputTerminal(), GetProjPath() );
			AddPage( (CProcDataPageDepSink*)m_pProcDataPage[i] );			
		}else if (i == BillboardProcessor)
		{
			m_pProcDataPage[i] = new CBillboardProcDataPage( (enum PROCDATATYPE)i, GetInputTerminal(), GetProjPath() );
			AddPage( (CBillboardProcDataPage*)m_pProcDataPage[i] );		
		}
		else if( i == RetailProc)
		{
			m_pProcDataPage[i] = new CRetailProcDataPage( (enum PROCDATATYPE)i, GetInputTerminal(), GetProjPath() );
			AddPage( (CRetailProcDataPage*)m_pProcDataPage[i] );		

		}
		else if( i == BarrierProc )
			continue;
		else
		{
			int nIdx = i;
			if( i > BarrierProc )
				nIdx--;
			m_pProcDataPage[nIdx] = new CProcDataPage( (enum PROCDATATYPE)i, GetInputTerminal(), GetProjPath() );
			AddPage( (CPropertyPage*)m_pProcDataPage[nIdx] );
		}
	}
}

CProcDataSheet::~CProcDataSheet()
{
}


BEGIN_MESSAGE_MAP(CProcDataSheet, CResizableSheet)
	//{{AFX_MSG_MAP(CProcDataSheet)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_BTN_PROCDATASHEET_LEFT,OnBtnProcDataSheetLeft)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcDataSheet message handlers


InputTerminal* CProcDataSheet::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CProcDataSheet::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}



BOOL CProcDataSheet::OnInitDialog() 
{
	CResizableSheet::OnInitDialog();
	
	// TODO: Add extra initialization here
	// enable save/restore, with active page
	EnableSaveRestore(_T("Properties"), TRUE, TRUE); 

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_STRING,SC_MINIMIZE,"Minimize");
		pSysMenu->AppendMenu(MF_STRING,SC_MAXIMIZE,"Maximize");
		ModifyStyle(0,WS_MINIMIZEBOX|WS_MAXIMIZEBOX);
	}

	m_btnLeft.Create("Gate Linkage",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_BTN_PROCDATASHEET_LEFT);
	m_btnLeft.SetFont(GetFont());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcDataSheet::OnSize(UINT nType, int cx, int cy) 
{
	CResizableSheet::OnSize(nType, cx, cy);
	if(m_btnLeft.m_hWnd)
	{
		CWnd* pWnd=GetDlgItem(IDOK);
		CRect rectWnd;
		pWnd->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		int nWidthBtn=rectWnd.Width()+24;
		rectWnd.left=7;
		rectWnd.right=rectWnd.left+nWidthBtn;
		m_btnLeft.MoveWindow(rectWnd);
	}
	// TODO: Add your message handler code here
	
}

void CProcDataSheet::OnBtnProcDataSheetLeft()
{
	CLinkageDlg dlg(m_pProcDataPage[0]->GetInputTerm(),GetProjPath());
	if( dlg.DoModal() == IDOK )
	{
		CWaitCursor wc;
		int iActivePage = GetActiveIndex();
		for( int i=0; i<PROC_TYPES-2; i++ )
		{
			SetActivePage(i);
			((CProcDataPage*)GetActivePage())->LoadProcessorList();
		}
		SetActivePage( iActivePage );
	}
}
