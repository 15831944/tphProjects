#pragma once


// CCmpReportListView view

class CCmpReportListView : public CView
{
	DECLARE_DYNCREATE(CCmpReportListView)

protected:
	CCmpReportListView();           // protected constructor used by dynamic creation
	virtual ~CCmpReportListView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


