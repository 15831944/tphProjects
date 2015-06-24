// ACFurnishingEdit3DView.cpp : implementation file
//

#include "stdafx.h"
#include "ACFurnishingEdit3DView.h"
#include <Renderer/RenderEngine/3DObjModelEditContext.h>

#include <Common/OleDragDropManager.h>
#include ".\acfurnishingedit3dview.h"
// CACFurnishingEdit3DView

IMPLEMENT_DYNCREATE(CACFurnishingEdit3DView, C3DObjModelEdit3DView)

CACFurnishingEdit3DView::CACFurnishingEdit3DView()
{
}

CACFurnishingEdit3DView::~CACFurnishingEdit3DView()
{
}

BEGIN_MESSAGE_MAP(CACFurnishingEdit3DView, C3DObjModelEdit3DView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CACFurnishingEdit3DView drawing

void CACFurnishingEdit3DView::OnDraw(CDC* pDC)
{
	C3DObjModelEdit3DView::OnDraw(pDC);
}


// CACFurnishingEdit3DView diagnostics

#ifdef _DEBUG
void CACFurnishingEdit3DView::AssertValid() const
{
	C3DObjModelEdit3DView::AssertValid();
}

void CACFurnishingEdit3DView::Dump(CDumpContext& dc) const
{
	C3DObjModelEdit3DView::Dump(dc);
}
#endif //_DEBUG


// CACFurnishingEdit3DView message handlers


C3DObjModelEditScene* CACFurnishingEdit3DView::CreateScene() const
{
	return new C3DObjModelEditScene();
}

bool CACFurnishingEdit3DView::IsDropAllowed( const DragDropDataItem& dropItem ) const
{
	return !GetEditContext()->IsViewLocked()
		&& dropItem.GetDataType() == DragDropDataItem::Type_AircraftComponentModel
		&& !GetParentFrame()->IsModelExternal();
}
int CACFurnishingEdit3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (C3DObjModelEdit3DView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	DRAG_DROP_VIEW_REGIST(this);

	return 0;
}
