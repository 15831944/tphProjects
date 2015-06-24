#pragma once

#include "MFCExControl/ListCtrlEx.h"

class CalloutDispSpecDataList;
#include "AirsideGUI/NodeViewDbClickHandler.h"

class CalloutDispSpecDataItem;
class CalloutDispSpecItem;

/////////////////////////////////////////////////////////////////////////////
// CCalloutDispSpecsListCtrl

class CTermPlanDoc;

class CCalloutDispSpecsListCtrl : public CListCtrlEx
{
public:
	CCalloutDispSpecsListCtrl(PFuncSelectFlightType pSelFltType, int nProjID, CTermPlanDoc* pTermPlanDoc);
	virtual ~CCalloutDispSpecsListCtrl();

	void InitListHeader();
	void ResetListContent(CalloutDispSpecItem* pSpecItem);

protected:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);

	void DrawTypeValuePairText( CString strText, CDC* pDC, CRect &rcClient, BOOL bSelected );
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	void GetEnabledRect(CRect& r);
	void EditTimeRange( CalloutDispSpecDataItem* pDataItem );
	void MoveDialogToCursor(CDialog& dlg);

	DECLARE_MESSAGE_MAP()

private:
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;

	PFuncSelectFlightType m_pSelFltType;
	int m_nProjID;
	CTermPlanDoc* m_pTermPlanDoc;
	InputTerminal* m_pInTerminal;


	enum ColumnIndex
	{
		colEnabled = 0,
		colEnvMode,
		colFacility,
		colClient,
		colStartTime,
		colEndTime,
		colIntervalTime,
		colMeasure,
		colConnection,

		col_TotalCount,
	};
};

