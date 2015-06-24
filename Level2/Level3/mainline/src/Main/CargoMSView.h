#pragma once


// CCargoMSView view

class CCargoMSView : public CView
{
	DECLARE_DYNCREATE(CCargoMSView)

protected:
	CCargoMSView();           // protected constructor used by dynamic creation
	virtual ~CCargoMSView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


