#pragma once


// COnBoardLayoutPartView view
#include "OnBoardLayoutView.h"
class COnBoardLayoutPartView : public COnBoardLayoutView
{
	DECLARE_DYNCREATE(COnBoardLayoutPartView)

protected:
	COnBoardLayoutPartView();           // protected constructor used by dynamic creation
	virtual ~COnBoardLayoutPartView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


