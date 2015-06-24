#if !defined(AFX_COLORBOX_H__8B54C167_3389_11D3_BFC6_00E02C076D33__INCLUDED_)
#define AFX_COLORBOX_H__8B54C167_3389_11D3_BFC6_00E02C076D33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorBox window

class CColorBox : public CStatic
{
// Construction
public:
	CColorBox();

// Attributes
public:

// Operations
public:
	void SetColor(COLORREF NewColor)
	{
		m_Color = NewColor;
		Invalidate();
	}
	COLORREF GetColor() { return m_Color; }
	BOOL IsMultiColor() { return m_bIsMulti; }
	void SetMultiColor(BOOL bVal) { m_bIsMulti = bVal; Invalidate(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorBox)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_Color;
	BOOL m_bIsMulti;

	static HBITMAP m_bmMultiColor;
	static BOOL m_bBMLoaded;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBOX_H__8B54C167_3389_11D3_BFC6_00E02C076D33__INCLUDED_)
