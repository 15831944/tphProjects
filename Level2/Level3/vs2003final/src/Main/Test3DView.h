#pragma once

//
/////////////////////////////////////////////////////////////////////////////
// C3DView view
class CTest3DView : public CView
{
	//friend class C3DControlsWnd;
protected:
	CTest3DView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTest3DView)

// Attributes

protected:
	CString m_strTime;
	HGLRC m_hRC;	//opengl RC
	CDC* m_pDC;		//windows DC

//member functions
public:
//	CTermPlanDoc* GetDocument(); //get document associated with this view
	
protected:
	void Draw();
	void OnDraw(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

public:
	

};