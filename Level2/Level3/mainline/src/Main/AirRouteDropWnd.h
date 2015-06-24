#pragma once
#include "XPStyle/ToolBarXP.h"
#include "PipeShowListBox.h"

class CTermPlanDoc;

class CAirRouteDropWnd : public CWnd
{
public:
	CAirRouteDropWnd();
	~CAirRouteDropWnd();

	void InitFont(int nPointSize, LPCTSTR lpszFaceName);
	void InitListBox();
	void ShowAirRouteWnd(CPoint& point);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnButPipeshowall();
	afx_msg void OnButPipeshowno();
	afx_msg void OnListSelChanged();

	CTermPlanDoc* GetTermPlanDoc();

	CFont m_Font;
	CToolBarXP m_toolBar;
	CPipeShowListBox m_listBox;
};
