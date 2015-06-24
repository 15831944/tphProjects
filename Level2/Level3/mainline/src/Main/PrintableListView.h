#if !defined(AFX_PRINTABLELISTVIEW_H__27B0F285_1672_4572_A52E_7CEFCDB39721__INCLUDED_)
#define AFX_PRINTABLELISTVIEW_H__27B0F285_1672_4572_A52E_7CEFCDB39721__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintableListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintableListView view

class CPrintableListView : public CListView
{
protected:
	CPrintableListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPrintableListView)

// Attributes
public:

// Operations
public:
	void PrintList( CString _sHeader, CString _sFoot );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintableListView)
//	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
//	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPrintableListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrintableListView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTABLELISTVIEW_H__27B0F285_1672_4572_A52E_7CEFCDB39721__INCLUDED_)
