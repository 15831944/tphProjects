#include "stdafx.h"
#include "termplan.h"
#include "PipeDropWnd.h"
#include "TermPlanDoc.h"
#include "Inputs\PipeDataSet.h"

CPipeDropWnd::CPipeDropWnd()
{
}

CPipeDropWnd::~CPipeDropWnd()
{
}

BEGIN_MESSAGE_MAP(CPipeDropWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_BUT_PIPESHOWALL, OnButPipeshowall)
	ON_COMMAND(ID_BUT_PIPESHOWNO, OnButPipeshowno)
	ON_LBN_SELCHANGE( IDC_PIPESHOWWND_LISTBOX,OnListSelChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPipeDropWnd message handlers

int CPipeDropWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		 | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_PIPESHOW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	InitFont(80,"MS Sans Serif");
	if(!m_listBox.Create( LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
						  CRect(0,0,0,0),this,IDC_PIPESHOWWND_LISTBOX) )
	{
		// TRACE("Failed to create checklistbox\n");
		
		return false;
	}
	m_listBox.SetFont(&m_Font,TRUE) ;
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CPipeDropWnd::ShowPipeWnd(CPoint point)
{
	InitListBox();
	m_toolBar.MoveWindow(0,-2,PIPEDROPWND_WIDTH,PIPEDROPWND_TOOLBAR_HEIGHT);
	m_listBox.MoveWindow(2,PIPEDROPWND_TOOLBAR_HEIGHT-4,PIPEDROPWND_WIDTH-6,130);
	SetWindowPos(NULL,point.x,point.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
}

void CPipeDropWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	if(pNewWnd&&pNewWnd->m_hWnd!=m_listBox.m_hWnd)
		ShowWindow(SW_HIDE);
	
}

void CPipeDropWnd::InitListBox()
{
	m_listBox.ResetContent();
	
	
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	if( pMDIActive== NULL ) return;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if( pDoc== NULL ) return;
	CPipeDataSet* pPipeDataSet = pDoc->GetTerminal().m_pPipeDataSet;

	int iPipeCount = pPipeDataSet->GetPipeCount();
	for( int i=0; i<iPipeCount; i++ )
	{
		int iIndex = m_listBox.InsertString(-1, pPipeDataSet->GetPipeAt(i)->GetPipeName());
		m_listBox.SetItemData( iIndex, i);
		m_listBox.SetCheck( iIndex, (int)pPipeDataSet->GetPipeAt(i)->GetDisplayFlag() );
	}


}

void CPipeDropWnd::InitFont(int nPointSize, LPCTSTR lpszFaceName)
{
    m_Font.DeleteObject(); 
    m_Font.CreatePointFont (nPointSize,lpszFaceName);
}

void CPipeDropWnd::OnButPipeshowall() 
{
	// TODO: Add your command handler code here
	
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );
	CPipeDataSet* pPipeDataSet = pDoc->GetTerminal().m_pPipeDataSet;

	for( int i=0; i<pPipeDataSet->GetPipeCount(); i++ )
	{
		pPipeDataSet->GetPipeAt( i )->SetDisplayFlag( TRUE );
	}

	pDoc->UpdateAllViews( NULL );
	InitListBox();
	
}

void CPipeDropWnd::OnButPipeshowno() 
{
	// TODO: Add your command handler code here
	
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );
	CPipeDataSet* pPipeDataSet = pDoc->GetTerminal().m_pPipeDataSet;

	for( int i=0; i<pPipeDataSet->GetPipeCount(); i++ )
	{
		pPipeDataSet->GetPipeAt( i )->SetDisplayFlag( FALSE );
	}

	pDoc->UpdateAllViews( NULL );
	InitListBox();
	
}

void CPipeDropWnd::OnListSelChanged()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );
	CPipeDataSet* pPipeDataSet = pDoc->GetTerminal().m_pPipeDataSet;

	for( int i=0; i<pPipeDataSet->GetPipeCount(); i++)
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck( i );
		if( bSel != pPipeDataSet->GetPipeAt(i)->GetDisplayFlag())
		{
			pPipeDataSet->GetPipeAt( i )->SetDisplayFlag( bSel );
		}
	}

	pDoc->UpdateAllViews( NULL );
}

