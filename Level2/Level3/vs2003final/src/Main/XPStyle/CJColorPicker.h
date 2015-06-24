#ifndef __CJCOLORPICKER_H__
#define __CJCOLORPICKER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CJColorPicker.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include "CJColorPopup.h"

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker window

void AFXAPI DDX_CJColorPicker(CDataExchange *pDX, int nIDC, COLORREF& crColor);

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker window

#define CP_MODE_TEXT 1  // edit text Color
#define CP_MODE_BK   2  // edit background Color (default)

#define CPN_SELCHANGE        WM_USER + 19        // Color Picker Selection change
#define CPN_DROPDOWN			 WM_USER + 20        // Color Picker drop down
#define CPN_CLOSEUP				WM_USER + 21        // Color Picker close up
#define CPN_SELENDOK				 WM_USER + 22        // Color Picker end OK
#define CPN_SELENDCANCEL			 WM_USER + 23        // Color Picker end (cancelled)

class CCJColorPicker : public CButton
{
// Construction
public:
    CCJColorPicker();
    DECLARE_DYNCREATE(CCJColorPicker);

// Attributes
public:
    COLORREF GetColor();
    void     SetColor(COLORREF crColor); 

    void     SetDefaultText(LPCTSTR szDefaultText);
    void     SetCustomText(LPCTSTR szCustomText);

    void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

    void     SetSelectionMode(UINT nMode)              { m_nSelectionMode = nMode; }
    UINT     GetSelectionMode()                        { return m_nSelectionMode; };

    void     SetBkColor(COLORREF crColorBk);
    COLORREF GetBkColor()                             { return m_crColorBk; }
    
    void     SetTextColor(COLORREF crColorText);
    COLORREF GetTextColor()                           { return m_crColorText;}

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCJColorPicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCJColorPicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track Color changes?
    COLORREF m_crColorBk;
    COLORREF m_crColorText;
    UINT     m_nSelectionMode;
    CRect    m_ArrowRect;
    CString  m_strDefaultText;
    CString  m_strCustomText;
	bool	 m_bLBtnDown;

    // Generated message map functions
protected:
	CDC* m_pMemDC;
	void InitMemDC(CDC *pDC);
    //{{AFX_MSG(CCJColorPicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__CJCOLORPICKER_H__)
