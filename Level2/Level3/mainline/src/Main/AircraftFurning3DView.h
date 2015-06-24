#pragma once


// CAircraftFurning3DView view

class CAircraftFurning3DView : public CView
{
	DECLARE_DYNCREATE(CAircraftFurning3DView)

protected:
	CAircraftFurning3DView();           // protected constructor used by dynamic creation
	virtual ~CAircraftFurning3DView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


