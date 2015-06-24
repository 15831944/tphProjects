#pragma once


// CLayoutTreeView view

class CLayoutTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CLayoutTreeView)

protected:
	CLayoutTreeView();           // protected constructor used by dynamic creation
	virtual ~CLayoutTreeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


