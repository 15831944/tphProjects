#pragma once
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"


// CSortableListCtrl
class CAirsideBaseReport;

class MFCEXCONTROL_API CSortableListCtrl : public CXListCtrl
{
	DECLARE_DYNAMIC(CSortableListCtrl)

public:
	CSortableListCtrl();
	virtual ~CSortableListCtrl();

	CSortableHeaderCtrl m_wndSortableHeaderCtrl;

public:
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat/* = LVCFMT_LEFT*/, int nWidth/* = -1*/, int nSubItem/* = -1*/,EDataType dataType/* = dtSTRING*/);
protected:
	DECLARE_MESSAGE_MAP()
public:
	void SubClassHeadCtrl();
	//virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg LRESULT OnXListCtrlCheckBoxClicked(WPARAM wParam,LPARAM lParam);
	//afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


