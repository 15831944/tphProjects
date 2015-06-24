#pragma once


// COnBoardLayoutView view

class COnBoardLayoutView : public CView
{
	DECLARE_DYNCREATE(COnBoardLayoutView)

protected:
	COnBoardLayoutView();           // protected constructor used by dynamic creation
	virtual ~COnBoardLayoutView();

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


