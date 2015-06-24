// ProcessFlowView.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcessFlowView.h"
#include "MathematicView.h"
#include "TermPlanDoc.h"
#include "FlowChartPane.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessFlowView

IMPLEMENT_DYNCREATE(CProcessFlowView, CView)

CProcessFlowView::CProcessFlowView()
{
	m_pFlowVect = NULL;
}

CProcessFlowView::~CProcessFlowView()
{
	
}


BEGIN_MESSAGE_MAP(CProcessFlowView, CView)
	//{{AFX_MSG_MAP(CProcessFlowView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER+100, OnChildDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessFlowView drawing

LRESULT CProcessFlowView::OnChildDelete(WPARAM wParam, LPARAM lParam)
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return TRUE;

	return m_wndDotNetTabWad.OnChildDelete(wParam, lParam);
}

void CProcessFlowView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
//	CString str = pDoc->GetTitle();
//	GetParentFrame()->SetWindowText(str + "      Process Flow View");
}

/////////////////////////////////////////////////////////////////////////////
// CProcessFlowView diagnostics

#ifdef _DEBUG
void CProcessFlowView::AssertValid() const
{
	CView::AssertValid();
}

void CProcessFlowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProcessFlowView message handlers

int CProcessFlowView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	::GetWindowRect(((CMainFrame*)AfxGetMainWnd())->m_hWndMDIClient, &rc);
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf( RUNTIME_CLASS( CMathematicView ) ) )
		{
			pView->GetParentFrame()->MoveWindow(0, 0, rc.Width()/9*2, rc.Height()-2);
			break;
		}
	}

	GetParentFrame()->MoveWindow(rc.Width()/9*2+1, 0, rc.Width()-rc.Width()/9*2+1, rc.Height()-2);

	if (!m_wndDotNetTabWad.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, 111))
		return -1;

	return 0;
}

void CProcessFlowView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_wndDotNetTabWad.m_hWnd))
	{
		m_wndDotNetTabWad.MoveWindow(CRect(0, 0, cx, cy));
	}
}


void CProcessFlowView::OnDestroy() 
{
	
	CView::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CProcessFlowView message handlers

void CProcessFlowView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class

}

void CProcessFlowView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
}

void CProcessFlowView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
//	CString str = pDoc->GetTitle();
//	GetParentFrame()->SetWindowText(str + "      Process Flow View");
	// Do not call CView::OnPaint() for painting messages
}

void CProcessFlowView::SetMathFlowPtr(std::vector<CMathFlow>* pFlowVect)
{
	m_pFlowVect = pFlowVect;

}

BOOL CProcessFlowView::InitTabWnd()
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;

	if(m_pFlowVect->size() == 0)
		return FALSE;
	
	bool bFind = false;
	for( int i=0; i<static_cast<int>(m_pFlowVect->size()); i++)			
	{
		CString str = (*m_pFlowVect)[i].GetFlowName();		
		if(str != "")
		{
			AddFlowChartPane(str);
		}
	}

	return TRUE;
}

BOOL CProcessFlowView::AddFlowChartPane(const CString& strFlowName)
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;

	if(strFlowName == "")
		return FALSE;

	m_wndDotNetTabWad.AddChartPane(strFlowName);

	return TRUE;
}

BOOL CProcessFlowView::DeleteFlowChartPane(const CString& strFlowName)
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;
	if(strFlowName == "")
		return FALSE;

	return m_wndDotNetTabWad.DeleteChartPane(strFlowName);
}

BOOL CProcessFlowView::ModifyFlowChartPane(const CString& strOldName, const CString& strNewName)
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;
	if(strOldName == "" || strNewName == "" || strOldName == strNewName)
		return FALSE;

	return m_wndDotNetTabWad.ModifyChartPane(strOldName, strNewName);
}

BOOL CProcessFlowView::SaveFlowChartData()
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;
	
	m_wndDotNetTabWad.SaveFlowChartData();

	return TRUE;
}

BOOL CProcessFlowView::DeleteProcess(const CString &strName)
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;
	
	for(int i=0; i<static_cast<int>(m_pFlowVect->size()); i++)
	{
		CString strName = m_pFlowVect->at(i).GetFlowName();
		m_wndDotNetTabWad.AddChartPane(strName);
	}

	m_wndDotNetTabWad.DeleteProcess(strName);

	return TRUE;
}

BOOL CProcessFlowView::UpdateProcess(const CString &strOld, const CString &strNew)
{
	if(m_wndDotNetTabWad.m_hWnd == NULL)
		return FALSE;
	
	for(int i=0; i<static_cast<int>(m_pFlowVect->size()); i++)
	{
		CString strName = m_pFlowVect->at(i).GetFlowName();
		m_wndDotNetTabWad.AddChartPane(strName);
	}

	m_wndDotNetTabWad.UpdateProcess(strOld, strNew);

	return TRUE;
}

BOOL CProcessFlowView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CView::PreTranslateMessage(pMsg);
}
