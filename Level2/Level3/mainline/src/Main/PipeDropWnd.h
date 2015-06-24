#pragma once
#include "XPStyle/ToolBarXP.h"
#include "PipeShowListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPipeDropWnd window

#define PIPEDROPWND_WIDTH 220
#define PIPEDROPWND_HEIGHT 152
#define PIPEDROPWND_TOOLBAR_HEIGHT 32

class CPipeDropWnd : public CWnd
{
public:
	CPipeDropWnd();

	CToolBarXP m_toolBar;
	CPipeShowListBox m_listBox;

	void InitFont(int nPointSize, LPCTSTR lpszFaceName);
	CFont m_Font;
	void InitListBox();
	void ShowPipeWnd(CPoint point);
	virtual ~CPipeDropWnd();

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnButPipeshowall();
	afx_msg void OnButPipeshowno();
	afx_msg void OnListSelChanged();
	DECLARE_MESSAGE_MAP()
};
