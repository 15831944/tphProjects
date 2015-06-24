#pragma once
#include "..\mfcexcontrol\listctrlex.h"
#include "..\InputEnviroment\AirPollutantList.h"

class CFuelsPropertiesList :
	public CListCtrlEx
{
public:
	CFuelsPropertiesList(void);
	~CFuelsPropertiesList(void);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	bool GetItemCheck(int nRow,int nColumn);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void SetItemCheck(int nRow,int nColumn,BOOL bCheck);
	void SetAirPollutantList(CAirPollutantList *pAirPollutantList);
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
private:
	CSize m_sizeCheck;
	HBITMAP m_hBitmapCheck;
	CAirPollutantList *m_pAirPollutantList;

};
