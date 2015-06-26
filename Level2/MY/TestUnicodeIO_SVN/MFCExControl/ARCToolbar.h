#pragma once

#include <afxtempl.h>
#include "BtnST.h"

class MFCEXCONTROL_API CARCToolbarButton : public CButtonST
{
public:
	CARCToolbarButton(){
	};
	~CARCToolbarButton(){};

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

};

class MFCEXCONTROL_API CARCToolbar : public CStatic
{
// Construction
public:
	CARCToolbar();

// Attributes
public:

// Operations
public:

	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	void SetStyle(BOOL bBtnRightAlign, BOOL bGradient)
	{
		m_bBtnRightAlign = bBtnRightAlign;
		m_bGradient = bGradient;
	}

	void SetTitle(LPCTSTR lpszTitle){ m_strTitle = (_T("* ")); m_strTitle += lpszTitle; }

	void EnableButton(int nID, bool bEnabled = true);
	void SetImageList(CImageList * pImageList);
	void AddButton(int nID, int nImage, LPCTSTR pszToolTip);
	virtual ~CARCToolbar();

protected:
	struct FLATBUTTON
	{
		~FLATBUTTON()
		{
			if (::IsWindow(btn.m_hWnd))
				btn.DestroyWindow();
		}
		int ID;
		int ImageNumber;
		CString Tooltip;

		CARCToolbarButton btn;
	};

	//CXTPPaintManagerColorGradient	m_clrBackground;
	BOOL m_bBtnRightAlign;
	BOOL m_bGradient;

	CList<FLATBUTTON *, FLATBUTTON *> m_Buttons;
	CImageList *m_pImageList;

	CString m_strTitle;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};
