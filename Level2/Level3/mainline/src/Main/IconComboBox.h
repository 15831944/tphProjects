// IconComboBox.h: interface for the CIconComboBox class.
// Show Icon in the combobox

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONCOMBOBOX_H__54642913_3308_47F0_AA3B_D949F27A3C09__INCLUDED_)
#define AFX_ICONCOMBOBOX_H__54642913_3308_47F0_AA3B_D949F27A3C09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListWndOld.h"

#define BITMAP_NUM 256

class CIconComboBox : public CComboBox  
{
public:
	CIconComboBox();
	virtual ~CIconComboBox();

// 	static const char* s_szShapeLabel[];
// 	static const char* s_szShapePath[];

public:
	void DisplayListWnd();
	void HideCtrl();
	void CalculateDroppedRect(LPRECT lpDroppedRect);

	BOOL CreateListWnd();

	void SetSelectedIndex(const char* pszName);
	void SetSelectedIndex(int index);

protected:
	CListWndOld	m_listWnd;
	int			m_nIndex;
	CBitmap		m_bitmaps[BITMAP_NUM];
	//CImageList	m_imgList;
	
protected:
	BOOL		LoadAllBitmaps();

protected:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpmis);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg LRESULT OnChangeIcon(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CIconComboBox)
public:
	int GetSelectedIndex(void);
};

#endif // !defined(AFX_ICONCOMBOBOX_H__54642913_3308_47F0_AA3B_D949F27A3C09__INCLUDED_)
