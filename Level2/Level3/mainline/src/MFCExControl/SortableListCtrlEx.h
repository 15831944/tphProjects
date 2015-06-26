#pragma once
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "listctrlex.h"

class MFCEXCONTROL_API CSortableListCtrlEx :
	public CListCtrlEx
{
	DECLARE_DYNAMIC(CSortableListCtrlEx)

public:
	CSortableListCtrlEx(void);
	virtual ~CSortableListCtrlEx(void);

	CSortableHeaderCtrl m_wndSortableHeaderCtrl;

public:
	int InsertColumn(int nCol, const LV_COLUMNEX* pColumn, EDataType dataType);
protected:
	DECLARE_MESSAGE_MAP()
public:
	void SubClassHeadCtrl();
	//virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};