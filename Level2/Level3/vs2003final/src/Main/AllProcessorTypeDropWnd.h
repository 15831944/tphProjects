#pragma once
#include "XPStyle/ToolBarXP.h"
#include "AllProcessorTypeShowListBox.h"
#include "engine\Car.h"
/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTypeDropWnd window

#define PIPEDROPWND_WIDTH 220
#define PIPEDROPWND_HEIGHT 152
#define PIPEDROPWND_TOOLBAR_HEIGHT 32

class CAllProcessorTypeDropWnd : public CWnd
{
public:
	CAllProcessorTypeDropWnd();

	CToolBarXP m_toolBar;
	CAllProcessorTypeShowListBox m_listBox;

	void InitFont(int nPointSize, LPCTSTR lpszFaceName);
	CFont m_Font;
	void InitListBox();
	void ShowAllProcessorTypeWnd(CPoint point);
	void SortProTypeString(std::set<CString>& proTypeSet,std::map<CString,int>& ProTypeMap);
	virtual ~CAllProcessorTypeDropWnd();

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnButPipeshowall();
	afx_msg void OnButPipeshowno();
	afx_msg void OnListSelChanged();
	DECLARE_MESSAGE_MAP()
};
