// Render3DFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "Render3DFrame.h"

#include "Render3DView.h"
#include "3DCamera.h"
#include "TermPlanDoc.h"
#include ".\render3dframe.h"
#include "Render3DEditor.h"
#include "DlgGetString.h"
#include <Common/UnitsManager.h>
#include "MainFrm.h"
#include "Common/CARCUnit.h"
#include "InputAirside/ARCUnitManager.h"
// CRender3DFrame

CReCalcSplitterWnd::CReCalcSplitterWnd()
	: m_bLoadingPaneInfo(FALSE)
{

}

void CReCalcSplitterWnd::RecalcLayout()
{
	CSplitterWnd2X2::RecalcLayout();
	if (!m_bLoadingPaneInfo)
	{
		CWnd* pWnd = GetParent();
		if (pWnd->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
		{
			((CRender3DFrame*)pWnd)->OnSplitterLayoutChange();
		}
		else
			ASSERT(FALSE);
	}
}

void CReCalcSplitterWnd::LoadPaneInfo( const C3DViewPaneInfo& paneInfo )
{
	m_bLoadingPaneInfo = TRUE;
	CRect rect;
	GetWindowRect(rect);
	if (paneInfo.GetRowCount() > 1)
	{
		int nTopHeight = rect.Height()*paneInfo.nTopHeight/(paneInfo.nTopHeight + paneInfo.nBottomHeight);
		if (GetRowCount()>1)
		{
			m_pRowInfo[0].nIdealSize = nTopHeight;
			m_pRowInfo[1].nIdealSize = rect.Height() - nTopHeight;
		}
		else
			CSplitterWnd::SplitRow(nTopHeight);
	}
	else if (GetRowCount()>1)
		DeleteRow(1);

	if (paneInfo.GetColCount() > 1)
	{
		int nLeftWidth = rect.Width()*paneInfo.nLeftWidth/(paneInfo.nLeftWidth + paneInfo.nRightWidth);
		if (GetColumnCount()>1)
		{
			m_pColInfo[0].nIdealSize = nLeftWidth;
			m_pColInfo[1].nIdealSize = rect.Width() - nLeftWidth;
		}
		else
			CSplitterWnd::SplitColumn(nLeftWidth);
	}
	else if (GetColumnCount()>1)
		DeleteColumn(1);

	m_bLoadingPaneInfo = FALSE;
	RecalcLayout();
}

IMPLEMENT_DYNCREATE(CRender3DFrame, IRenderFrame)


static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_3DMOUSEMODE,
	ID_INDICATOR_GRIDSPACING,
	ID_INDICATOR_3DMOUSEPOS,
	ID_INDICATOR_ANIMTIME,
	ID_INDICATOR_ANIMATIONSPEED,
	ID_INDICATOR_FPS
};


CRender3DFrame::CRender3DFrame()
{
}

CRender3DFrame::~CRender3DFrame()
{

}

BOOL CRender3DFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create(this, pContext);
}

BEGIN_MESSAGE_MAP(CRender3DFrame, IRenderFrame)
	ON_COMMAND(ID_WINDOW_SPLITHORIZONTAL, OnSplitHorizontal)
	ON_COMMAND(ID_WINDOW_SPLITVERTICAL, OnSplitVertical)
	ON_COMMAND(ID_WINDOW_UNSPLIT, OnUnSplit)
	ON_COMMAND(ID_VIEW_SAVENAMEDVIEW, OnViewSaveNamedView)
	ON_COMMAND(ID_VIEW_3DTERMINALVIEW, OnView3dterminalview)
	ON_COMMAND(ID_VIEW_2DTERMINALVIEW, OnView2dterminalview)
	ON_COMMAND(ID_VIEW_OPENDEFAULT3D, OnView3DDefaultView)
	ON_COMMAND(ID_VIEW_OPENDEFAULT2D, OnView2DDefaultView)
	ON_COMMAND_EX_RANGE(ID_VIEW_NAMEDVIEWS1, ID_VIEW_NAMEDVIEWS8, OnViewNamedViews3D)
	ON_COMMAND_EX_RANGE(ID_VIEW_NAMEDVIEWS9, ID_VIEW_NAMEDVIEWS16, OnViewNamedViews2D)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEMODE, OnUpdate3dMouseMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEPOS, OnUpdate3dMousePos)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CRender3DFrame message handlers

void CRender3DFrame::OnSplitHorizontal()
{
	SplitRow();
}

void CRender3DFrame::OnSplitVertical()
{
	SplitColumn();
}

void CRender3DFrame::OnUnSplit()
{
	UnSplitColumn();
	UnSplitRow();
}

BOOL CRender3DFrame::SplitRow(int nTop/* = 150*/, int nBottom/* = 150*/)
{
	CRect r;
	GetClientRect(&r);
	return m_wndSplitter.SplitRow(nTop*r.Height()/(nTop+nBottom));
}

BOOL CRender3DFrame::SplitColumn(int nLeft/* = 150*/, int nRight/* = 150*/)
{
	CRect r;
	GetClientRect(&r);
	return m_wndSplitter.SplitColumn(nLeft*r.Width()/(nLeft+nRight));
}

BOOL CRender3DFrame::UnSplitRow()
{
	if(m_wndSplitter.GetRowCount() > 1)
	{
		m_wndSplitter.DeleteRow(1);
		return TRUE;
	}

	return FALSE;
}

BOOL CRender3DFrame::UnSplitColumn()
{
	if(m_wndSplitter.GetColumnCount() > 1)
	{
		m_wndSplitter.DeleteColumn(1);
		return TRUE;
	}

	return FALSE;
}

void CRender3DFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	if(bAddToTitle)
	{
		CTermPlanDoc* pDoc = GetDocument();
		CString strAdd = pDoc->GetCurModeRenderCameras().m_b3DMode ? _T("       3D View") : _T("       2D View");
		SetWindowText(pDoc->GetTitle() + strAdd);
	}
}

CRender3DView* CRender3DFrame::GetActivePane( int* _pRow/*=NULL*/, int* _pCol/*=NULL*/ )
{
	CWnd* pWnd = m_wndSplitter.GetActivePane(_pRow, _pCol);
	ASSERT(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CRender3DView)));
	return (CRender3DView*)pWnd;
}

CTermPlanDoc* CRender3DFrame::GetDocument() 
{
	CDocument* pDoc = GetActiveDocument();
// 	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*)pDoc;
}

void CRender3DFrame::LoadWorkingViews()
{
	const C3DViewCameraDesc& camDesc = GetDocument()->GetCurModeRenderCameras().GetUserCamera();
	m_wndSplitter.LoadPaneInfo(camDesc.GetPaneInfo());

	for(int i=0;i<m_wndSplitter.GetRowCount();i++)
	{
		for(int j=0;j<m_wndSplitter.GetColumnCount();j++)
		{
			CRender3DView* p3DView = (CRender3DView*)m_wndSplitter.GetPane(i,j);
			if (p3DView->IsKindOf(RUNTIME_CLASS(CRender3DView)))
			{
				p3DView->LoadCamera(true);
				p3DView->Invalidate(FALSE);
			}
			else
				ASSERT(FALSE);
		}
	}
	OnUpdateFrameTitle(TRUE);
}

void CRender3DFrame::OnDestroy()
{
	IRenderFrame::OnDestroy();

	// TODO: Add your message handler code here
// 	StorePanInfo();
// 
// 	CWnd* pMF=AfxGetMainWnd();
// 	ToolBarUpdateParameters updatePara;
// 	updatePara.m_operatType = ToolBarUpdateParameters::Close3DViewFrm;
// 	updatePara.m_nCurMode = GetDocument()->GetCurrentMode();
// 	pMF->SendMessage(UM_SHOW_CONTROL_BAR,2,(LPARAM)&updatePara);
}

int CRender3DFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (IRenderFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_ANIMTIME, SBPS_NOBORDERS, 110);
	m_wndStatusBar.SetPaneInfo( 0, ID_INDICATOR_3DMOUSEMODE, SBPS_STRETCH | SBPS_NOBORDERS, 0 );
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_ANIMATIONSPEED, SBPS_NOBORDERS, 110);

	CWnd* pView00 = m_wndSplitter.GetPane(0, 0);
	if (!pView00 || !pView00->IsKindOf(RUNTIME_CLASS(CRender3DView)))
	{
		ASSERT(FALSE);
		return -1;
	}
	CRender3DView* p3DView00 = (CRender3DView*)pView00;
	SetActiveView(p3DView00); // only after one view is set active, the method GetDocument will work

	// init data
	int aptID = InputAirside::GetAirportID(GetDocument()->GetProjectID());
	m_altAirport.ReadAirport(aptID);

	// prepare 3d
	OnUpdateFrameTitle(TRUE);
	return 0;
}

void CRender3DFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{

	CWnd* pWnd = m_wndSplitter.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;

	if( pWnd->IsKindOf(RUNTIME_CLASS(CRender3DView) ) )
	{
		CRender3DView* pView = (CRender3DView*)pWnd;
		ARCVector3 mousePt;

		CString s;
		CTermPlanDoc* pDoc = GetDocument();
		if (pView->GetViewPortMousePos(mousePt))
		{
			s.Format("\tX: %5.2f %s | Y:%5.2f %s",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),mousePt[VX]),\
			CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),mousePt[VY]),\
			CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		}

		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void CRender3DFrame::OnUpdate3dMouseMode(CCmdUI* pCmdUI)
{
	CRender3DView* pRender3DView = GetActivePane();
	if(pRender3DView) { // if its valid
		CString s(_T(""));
		pCmdUI->Enable(TRUE);
		s =  pRender3DView->GetModelEditScene().GetMouseHintText();	
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

CARCportLayoutEditor* CRender3DFrame::GetEditContext()
{
	return GetDocument()->GetLayoutEditor();
}

void CRender3DFrame::OnUpdateFrameMenu( BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt )
{
	IRenderFrame::OnUpdateFrameMenu (bActivate, pActivateWnd, hMenuAlt);

	GetDocument()->GetMainFrame()->AdjustMenu();
	if(bActivate)
		GetDocument()->UpdateTrackersMenu();
}

void CRender3DFrame::OnSplitterLayoutChange()
{
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		C3DViewPaneInfo& paneInfo = pDoc->GetCurModeRenderCameras().GetUserCamera().GetPaneInfo();
		CRect rcTopLeft;
		m_wndSplitter.GetPane(0,0)->GetClientRect(rcTopLeft);
		paneInfo.nLeftWidth = rcTopLeft.Width();
		paneInfo.nTopHeight = rcTopLeft.Height();
		if (m_wndSplitter.GetColumnCount()>1)
		{
			CRect rc;
			m_wndSplitter.GetPane(0,1)->GetClientRect(rc);
			paneInfo.nRightWidth = rc.Width();
		}
		else
			paneInfo.nRightWidth = 0;

		if (m_wndSplitter.GetRowCount()>1)
		{
			CRect rc;
			m_wndSplitter.GetPane(1,0)->GetClientRect(rc);
			paneInfo.nBottomHeight = rc.Height();
		}
		else
			paneInfo.nBottomHeight = 0;
	}
}

void CRender3DFrame::OnViewSaveNamedView()
{
	CDlgGetString dlg(IDS_DIALOG_VIEWNAME);
	if(dlg.DoModal() == IDOK) {
		GetDocument()->SaveNamedRenderView(dlg.m_sValue);	
	}
}

void CRender3DFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (SC_CLOSE == nID)
	{
		GetDocument()->SaveCurModeRenderCameras();
	}

	IRenderFrame::OnSysCommand(nID, lParam);
}

BOOL CRender3DFrame::OnViewNamedViews2D(UINT nID) 
{
	C3DViewCameras& cams = GetDocument()->GetCurModeRenderCameras();
	cams.m_b3DMode = false;
	cams.m_userCamera2D = CTermPlanDoc::GetRenderCameraDescByIndex(cams, nID - ID_VIEW_NAMEDVIEWS9, CCameraData::parallel);
	LoadWorkingViews();
	return TRUE;
}

BOOL CRender3DFrame::OnViewNamedViews3D(UINT nID) 
{
	C3DViewCameras& cams = GetDocument()->GetCurModeRenderCameras();
	cams.m_b3DMode = true;
	cams.m_userCamera3D = CTermPlanDoc::GetRenderCameraDescByIndex(cams, nID - ID_VIEW_NAMEDVIEWS1, CCameraData::perspective);
	LoadWorkingViews();
	return TRUE;
}



void CRender3DFrame::OnView2DDefaultView() //opens or activates default 3d view (single pane)
{
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		C3DViewCameras& cams = pDoc->GetCurModeRenderCameras();
		cams.m_b3DMode = false;
		cams.m_userCamera2D.InitCameraDataByMode(CCameraData::parallel, pDoc->GetCurrentMode());
		LoadWorkingViews();
	}
}

void CRender3DFrame::OnView3DDefaultView() //opens or activates default 3d view (single pane)
{
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		C3DViewCameras& cams = pDoc->GetCurModeRenderCameras();
		cams.m_b3DMode = true;
		cams.m_userCamera3D.InitCameraDataByMode(CCameraData::perspective, pDoc->GetCurrentMode());
		LoadWorkingViews();
	}
}

void CRender3DFrame::OnView2dterminalview() //opens 2D user view (working view)
{
	C3DViewCameras& cams = GetDocument()->GetCurModeRenderCameras();
	cams.m_b3DMode = false;
	LoadWorkingViews();
}

void CRender3DFrame::OnView3dterminalview() //opens or activates working 3d view
{
	C3DViewCameras& cams = GetDocument()->GetCurModeRenderCameras();
	cams.m_b3DMode = true;
	LoadWorkingViews();
}


