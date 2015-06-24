// OnBoardLayoutFrame.cpp : implementation file
//

#include "stdafx.h"
#include "MeshEditFrame.h"

#include <inputonboard\ComponentSection.h>
#include <InputOnBoard\ComponentModel.h>

#include <Common/UnitsManager.h>
#include "MeshEdit3DView.h"
#include "MeshSectionEdit3DView.h"
#include "../Resource.h"
#include ".\mesheditframe.h"
#include "MeshEditTreeView.h"
#include "MeshSegEdit3DView.h"
// COnBoardLayoutFrame

static UINT indicators[] =
{
	ID_INDICATOR_DENSITYPARAMS,
	ID_INDICATOR_ANIMTIME,
	ID_INDICATOR_3DMOUSEPOS
};

IMPLEMENT_DYNCREATE(COnBoardMeshEditFrame, CMDIChildWnd)


COnBoardMeshEditFrame::COnBoardMeshEditFrame()
{
}

COnBoardMeshEditFrame::~COnBoardMeshEditFrame()
{
}

int COnBoardMeshEditFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_ANIMTIME, SBPS_NOBORDERS, 200);
	m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_3DMOUSEMODE, SBPS_STRETCH | SBPS_NOBORDERS, 50 );
	ShowWindow(SW_SHOWMAXIMIZED);
	ShowWindow(SW_SHOW);
	return 0;
}

BOOL COnBoardMeshEditFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	//return m_wndSplitter1.Create(this, 2,2, CSize(10,10), pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);
	
	// Create the splitter window with two columns
	//ShowWindow(SW_SHOWMAXIMIZED);
	CRect rc(0, 0, 0, 0);
	GetWindowRect(&rc);

	if (!m_wndSplitterLR.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}


	// Create the first nested splitter window to contain the ...

	if (!m_wndSplitterRTB.CreateStatic(&m_wndSplitterLR, 2, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitterLR.IdFromRowCol(0, 1)))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	if(!m_wndSplitterRBLR.CreateStatic(&m_wndSplitterRTB,1,2,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitterRTB.IdFromRowCol(1, 0)))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	//tree view
	if(!m_wndSplitterLR.CreateView(0,0,RUNTIME_CLASS(CMeshEditTreeView),
		CSize(250, 200), pContext))
	{
		TRACE0("Failed to create CTreeView\n");
		return FALSE;
	}

	int rWidth = rc.Width() - 250;
	//perspect view
	if (!m_wndSplitterRTB.CreateView(0, 0, RUNTIME_CLASS(CSimpleMeshEditView),
		CSize(rWidth, 500), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}

	//xz view
	if (!m_wndSplitterRBLR.CreateView(0, 0, RUNTIME_CLASS(CMeshSegEdit3DView),
		CSize(rWidth/2, 200), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}

	//yz view
	if (!m_wndSplitterRBLR.CreateView(0, 1, RUNTIME_CLASS(CMeshSectionEditView),
		CSize(rWidth/2, 200), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}
	m_wndSplitterLR.SetColumnInfo(0,250,150);
	m_wndSplitterRBLR.SetColumnInfo(0,rc.Width()/2, 10);
	CView* pView = (CView*) m_wndSplitterRBLR.GetPane(0,0);
	SetActiveView( pView);

	return TRUE;
}

BEGIN_MESSAGE_MAP(COnBoardMeshEditFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEPOS, OnUpdate3dMousePos)
END_MESSAGE_MAP()




void COnBoardMeshEditFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy();

	// TODO: Add your message handler code here
	CWnd* pMF=AfxGetMainWnd();
	/*ToolBarUpdateParameters updatePara;
	updatePara.m_operatType = ToolBarUpdateParameters::CloseComponentEditView;
	updatePara.m_nCurMode = ((CTermPlanDoc*)GetActiveDocument())->GetCurrentMode();
	pMF->SendMessage(UM_SHOW_CONTROL_BAR,2,(LPARAM)&updatePara);*/
}



void COnBoardMeshEditFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{

	CWnd* pWnd = m_wndSplitterLR.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;

	ARCVector3 mousePt(0,0,0);
	if( pWnd->IsKindOf(RUNTIME_CLASS(CSimpleMeshEditView) ) )
	{
		mousePt = mMeshEditScene.GetMousePosXY();
	}
	else if( pWnd->IsKindOf(RUNTIME_CLASS(CMeshSegEdit3DView)) )
	{
		 mousePt = mMeshEditScene.GetMousePosXZ();
	}	
	else if( pWnd->IsKindOf(RUNTIME_CLASS(CMeshSectionEditView)) )
	{
		mousePt = mMeshEditScene.GetMousePosYZ();
	}
	else
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	
	{ // if its valid
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();

		s.Format("\tX: %5.2f | Y: %5.2f | Z: %5.2f %s",
			pUM->ConvertLength( mousePt[VX] ),
			pUM->ConvertLength( mousePt[VY] ),
			pUM->ConvertLength( mousePt[VZ] ),
			pUM->GetLengthUnitString(pUM->GetLengthUnits()));

		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}	
}



//Edit Command
//edit history
