// ACComponentEdit3DView.cpp : implementation file
//

#include "stdafx.h"
#include "ACModelEdit3DView.h"
#include <Renderer/RenderEngine/3DObjModelEditContext.h>

#include <Common/OleDragDropManager.h>
#include ".\acmodeledit3dview.h"
// CACModelEdit3DView

IMPLEMENT_DYNCREATE(CACModelEdit3DView, C3DObjModelEdit3DView)

CACModelEdit3DView::CACModelEdit3DView()
{
}

CACModelEdit3DView::~CACModelEdit3DView()
{
}

BEGIN_MESSAGE_MAP(CACModelEdit3DView, C3DObjModelEdit3DView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CACModelEdit3DView drawing

void CACModelEdit3DView::OnDraw(CDC* pDC)
{
	C3DObjModelEdit3DView::OnDraw(pDC);
}


// CACModelEdit3DView diagnostics

#ifdef _DEBUG
void CACModelEdit3DView::AssertValid() const
{
	C3DObjModelEdit3DView::AssertValid();
}

void CACModelEdit3DView::Dump(CDumpContext& dc) const
{
	C3DObjModelEdit3DView::Dump(dc);
}
#endif //_DEBUG


// CACModelEdit3DView message handlers


C3DObjModelEditScene* CACModelEdit3DView::CreateScene() const
{
	return new C3DObjModelEditScene();
}

bool CACModelEdit3DView::IsDropAllowed( const DragDropDataItem& dropItem ) const
{
	return !GetEditContext()->IsViewLocked()
		&& dropItem.GetDataType() == DragDropDataItem::Type_AircraftComponentModel
		&& !GetParentFrame()->IsModelExternal();
}

int CACModelEdit3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (C3DObjModelEdit3DView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	DRAG_DROP_VIEW_REGIST(this);
	return 0;
}
