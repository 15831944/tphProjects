#pragma once


// CCmpReportGraphView view

class CCmpReportGraphView : public CView
{
	DECLARE_DYNCREATE(CCmpReportGraphView)

protected:
	CCmpReportGraphView();           // protected constructor used by dynamic creation
	virtual ~CCmpReportGraphView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


