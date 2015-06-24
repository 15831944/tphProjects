// 3DDragDropView.cpp : implementation file
//

#include "stdafx.h"
#include "3DDragDropView.h"
#include ".\3ddragdropview.h"

#include <Common/OleDragDropManager.h>
#include "TermPlanDoc.h"
// C3DDragDropView

IMPLEMENT_DYNCREATE(C3DDragDropView, C3DBaseView)

C3DDragDropView::C3DDragDropView()
{
}

C3DDragDropView::~C3DDragDropView()
{
}

BEGIN_MESSAGE_MAP(C3DDragDropView, C3DBaseView)
END_MESSAGE_MAP()

DROPEFFECT C3DDragDropView::OnDragEnter( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DragDropDataItem dataItem;
	return (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject) && IsDropAllowed(dataItem))
		? DROPEFFECT_COPY : DROPEFFECT_NONE;
}

DROPEFFECT C3DDragDropView::OnDragOver( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DragDropDataItem dataItem;
	return (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject) && IsDropAllowed(dataItem))
		? DROPEFFECT_COPY : DROPEFFECT_NONE;
}

BOOL C3DDragDropView::OnDrop( COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	// TODO: Add your specialized code here and/or call the base class
	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject) && IsDropAllowed(dataItem))
	{
		DoDrop(dataItem, point);
	}
	return C3DBaseView::OnDrop(pDataObject, dropEffect, point);
}

// C3DDragDropView message handlers

bool C3DDragDropView::IsDropAllowed( const DragDropDataItem& dropItem ) const
{
	ASSERT(FALSE);
	return false;
}

void C3DDragDropView::DoDrop( const DragDropDataItem& dropItem, CPoint point )
{
	ASSERT(FALSE);
}

CTermPlanDoc* C3DDragDropView::GetDocument() const
{
	CDocument* pDoc = C3DBaseView::GetDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*)pDoc;
}