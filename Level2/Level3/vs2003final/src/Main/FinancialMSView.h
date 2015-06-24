#pragma once


// CFinancialMSView view

class CFinancialMSView : public CView
{
	DECLARE_DYNCREATE(CFinancialMSView)

protected:
	CFinancialMSView();           // protected constructor used by dynamic creation
	virtual ~CFinancialMSView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


