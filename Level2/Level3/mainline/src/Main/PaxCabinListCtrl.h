#pragma once
#include "..\mfcexcontrol\listctrlex.h"
#include "..\InputOnboard\FlightPaxCabinAssign.h"
#include "..\inputs\IN_TERM.H"
#include "..\InputOnboard\CInputOnboard.h"

// #define WM_ONLBTN_CLICK WM_USER+100
class CPaxCabinListCtrl :
	public CListCtrlEx
{
public:
	CPaxCabinListCtrl(void);
	~CPaxCabinListCtrl(void);
	void resetListContent(CFlightPaxCabinAssign *curAssign);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
// 	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void setTerminal(InputTerminal *terminal);
	void setInputOnboard(InputOnboard* pInputOnboard);
protected:
	DECLARE_MESSAGE_MAP()
private:
	InputTerminal *m_pTerminal;
	InputOnboard* m_pInputOnboard;
	CFlightPaxCabinAssign *m_pCurAssign;
/*
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	void DrawTypeValuePairText( CString strText, CDC* pDC, CRect &rcClient, BOOL bSelected );
	void GetEnabledRect(CRect& r);
	CSize m_sizeCheck;*/

};
