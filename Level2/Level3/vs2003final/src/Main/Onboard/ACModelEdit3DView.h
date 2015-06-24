#pragma once

#include "3DObjModelEdit3DView.h"
// CACModelEdit3DView view

class CACModelEdit3DView : public C3DObjModelEdit3DView
{
	DECLARE_DYNCREATE(CACModelEdit3DView)

protected:
	CACModelEdit3DView();           // protected constructor used by dynamic creation
	virtual ~CACModelEdit3DView();

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


