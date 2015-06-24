#include "StdAfx.h"
#include ".\3dobjmodeleditframe.h"
#include "../Resource.h"

#include "..\TermPlanDoc.h"
#include "3DObjModelEdit3DView.h"
#include "3DObjModelEditTreeView.h"

#include <Common/UnitsManager.h>
#include <Common/FileInDB.h>

#include <Renderer/RenderEngine/3DObjModelEditContext.h>
#include <Renderer/RenderEngine/3DObjModelEditScene.h>
#include <InputOnBoard/CInputOnboard.h>

#include "../ACComponentShapesManager.h"


IMPLEMENT_DYNCREATE(C3DObjModelEditFrame, CMDIChildWnd)

C3DObjModelEditFrame::C3DObjModelEditFrame()
	: m_pTreeView(NULL)
	, m_p3DView(NULL)
	, m_pModelEditContext(NULL)
	, m_dlg3DObjProperty(NULL)
{
}

C3DObjModelEditFrame::~C3DObjModelEditFrame()
{
	if (m_pModelEditContext)
	{
		delete m_pModelEditContext;
		m_pModelEditContext = NULL;
	}
}

static UINT indicators[] =
{
	ID_INDICATOR_DENSITYPARAMS,
	ID_INDICATOR_ANIMTIME,
	ID_INDICATOR_3DMOUSEPOS,
	ID_INDICATOR_FPS,
};

int C3DObjModelEditFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	// Create and display property dialog
	m_dlg3DObjProperty = new CDlg3DObjectProperty();
	m_dlg3DObjProperty->Create(IDD_DIALOG_3D_OBJECT_PROP/*CDlg3DObjectProperty::IDD*/);
	m_dlg3DObjProperty->SetListener(this);
	m_pModelEditContext->AddSelNodeObjListener(m_dlg3DObjProperty);
// 	On3DNodeObjectProperty();

	// Move property window to the bottom right of the 3d view
	CRect rect3DView;
	m_p3DView->GetWindowRect(rect3DView);
	CRect rectProp;
	m_dlg3DObjProperty->GetWindowRect(rectProp);
	rectProp.MoveToXY(rect3DView.right - rectProp.Width(), rect3DView.bottom - rectProp.Height());
	m_dlg3DObjProperty->MoveWindow(rectProp);
// 	m_dlg3DObjProperty->CenterWindow();

	return 0;
}

BOOL C3DObjModelEditFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	ASSERT(NULL == m_pModelEditContext);
	m_pModelEditContext = CreateEditContext();
	if (NULL == m_pModelEditContext)
		return FALSE;

	// Create the splitter window with two columns
	if (!m_wndSplitterLR.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	m_wndSplitterLR.SetColumnInfo(0, 150, 100);

	// tree view
	if (!m_wndSplitterLR.CreateView(0, 0, RUNTIME_CLASS(C3DObjModelEditTreeView),
		CSize(190, 500), pContext))
	{
		TRACE0("Failed to create C3DObjModelEditTreeView\n");
		return FALSE;
	}
	m_pTreeView = (C3DObjModelEditTreeView*)m_wndSplitterLR.GetPane(0, 0);

	//3d view
	if (!m_wndSplitterLR.CreateView(0, 1, Get3DViewClass(),
		CSize(600, 500), pContext))
	{
		TRACE0("Failed to create C3DObjModelEdit3DView\n");
		return FALSE;
	}
	m_p3DView = (C3DObjModelEdit3DView*)m_wndSplitterLR.GetPane(0, 1);
	CDocument* pDoc = m_p3DView->GetDocument();
	if(pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)) )
	{
		CTermPlanDoc* pTermDoc = DYNAMIC_DOWNCAST(CTermPlanDoc,pDoc);
		m_pModelEditContext->ChangeEditModel(pTermDoc->GetInputOnboard()->GetEditModel());
	}

	SetActiveView( (CView*) m_wndSplitterLR.GetPane(0, 1) );
	return TRUE;
}

BEGIN_MESSAGE_MAP(C3DObjModelEditFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEPOS, OnUpdate3dMousePos)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

void C3DObjModelEditFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy();

	// TODO: Add your message handler code here
	CWnd* pMF=AfxGetMainWnd();
	/*ToolBarUpdateParameters updatePara;
	updatePara.m_operatType = ToolBarUpdateParameters::CloseComponentEditView;
	updatePara.m_nCurMode = ((CTermPlanDoc*)GetActiveDocument())->GetCurrentMode();
	pMF->SendMessage(UM_SHOW_CONTROL_BAR,2,(LPARAM)&updatePara);*/

	delete m_dlg3DObjProperty;
	m_dlg3DObjProperty = NULL;
}



void C3DObjModelEditFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{

	CWnd* pWnd = m_wndSplitterLR.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;

	if( pWnd->IsKindOf(RUNTIME_CLASS(C3DObjModelEdit3DView) ) )
	{
		C3DObjModelEdit3DView* pView = (C3DObjModelEdit3DView*)pWnd;
		ARCVector3 mousePt;

		CString s;
		if (pView->GetViewPortMousePos(mousePt))
		{
			CUnitsManager* pUM = CUnitsManager::GetInstance();
			s.Format("\tX: %5.2f | Y: %5.2f | Z: %5.2f %s",
				pUM->ConvertLength( mousePt[VX] ),
				pUM->ConvertLength( mousePt[VY] ),
				pUM->ConvertLength( mousePt[VZ] ),
				pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		}

		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DObjModelEditFrame::UpdateViews( CView* pSrcView /*= NULL*/, UpdateViewType updateType /*= Type_ReloadData*/, DWORD dwData /*= 0*/ )
{
	if (m_pTreeView && m_pTreeView != pSrcView)
	{
		m_pTreeView->UpdateView(updateType, dwData);
	}
	if (m_p3DView && m_p3DView != pSrcView)
	{
		m_p3DView->UpdateView(updateType, dwData);
	}

}

C3DObjModelEditScene* C3DObjModelEditFrame::GetModelEditScene()
{
	return m_p3DView->GetModelEditScene();
}

C3DNodeObject C3DObjModelEditFrame::GetEditModel() const
{
	return m_pModelEditContext->GetCurModel();
}

CRuntimeClass* C3DObjModelEditFrame::Get3DViewClass() const
{
	ASSERT(FALSE);
	return NULL;
}

C3DObjModelEditContext* C3DObjModelEditFrame::CreateEditContext() const
{
	ASSERT(FALSE);
	return NULL;
}

void C3DObjModelEditFrame::On3DObjectPropertyUpdate()
{
	if (m_p3DView)
	{
		m_p3DView->Invalidate();
	}
}

void C3DObjModelEditFrame::On3DNodeObjectProperty()
{
	m_dlg3DObjProperty->Load3DObject(m_pModelEditContext->GetSelNodeObj());
	m_dlg3DObjProperty->ShowWindow(SW_SHOW);
	m_dlg3DObjProperty->SetFocus();
}

BOOL C3DObjModelEditFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_MAXIMIZE;

	return CMDIChildWnd::PreCreateWindow(cs);
}

void C3DObjModelEditFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (SC_CLOSE == nID)
	{
		// close toolbar
		ACCOMPONENTSHAPESMANAGER.ShowShapesBar(FALSE);
	}

	CMDIChildWnd::OnSysCommand(nID, lParam);
}

bool C3DObjModelEditFrame::IsModelExternal() const
{
	return m_pModelEditContext->GetEditModel()->mbExternalMesh;
}