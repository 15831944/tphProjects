#pragma once
#include <afxole.h>

#include <renderer\renderengine\3dbaseview.h>
// C3DDragDropView view

// add ole drag drop support
class DragDropDataItem;
class CTermPlanDoc;

class C3DDragDropView : public C3DBaseView
{
	DECLARE_DYNCREATE(C3DDragDropView)

protected:
	C3DDragDropView();           // protected constructor used by dynamic creation
	virtual ~C3DDragDropView();

public:
	CTermPlanDoc* GetDocument() const;

protected:
	virtual bool IsDropAllowed(const DragDropDataItem& dropItem) const;
	virtual void DoDrop(const DragDropDataItem& dropItem, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

	DECLARE_MESSAGE_MAP()

protected:
	COleDropTarget m_oleDropTarget; // OLE drag & drop support

};

#define DRAG_DROP_VIEW_REGIST(pThisView)	m_oleDropTarget.Register(pThisView)



