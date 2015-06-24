#pragma once


// CAircraftFurning2DView view

class CAircraftFurning2DView : public CView
{
	DECLARE_DYNCREATE(CAircraftFurning2DView)

protected:
	CAircraftFurning2DView();           // protected constructor used by dynamic creation
	virtual ~CAircraftFurning2DView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


