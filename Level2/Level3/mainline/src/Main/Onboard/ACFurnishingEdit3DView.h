#pragma once

#include "3DObjModelEdit3DView.h"
// CACFurnishingEdit3DView view

class CACFurnishingEdit3DView : public C3DObjModelEdit3DView
{
	DECLARE_DYNCREATE(CACFurnishingEdit3DView)

protected:
	CACFurnishingEdit3DView();           // protected constructor used by dynamic creation
	virtual ~CACFurnishingEdit3DView();

	virtual C3DObjModelEditScene* CreateScene() const;
	virtual bool IsDropAllowed(const DragDropDataItem& dropItem) const;

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


