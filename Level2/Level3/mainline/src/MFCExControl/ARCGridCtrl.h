#ifndef GRID_H
#define GRID_H

#include "GridCtrl.h"
#define  WM_GRID_CLICK      WM_USER + 1001
#define  WM_GRID_DBCLICK	WM_USER	+ 1002
class MFCEXCONTROL_API ARCGridCtrl: public CGridCtrl
{
public:
	ARCGridCtrl();
	virtual ~ARCGridCtrl();
	
public:
	// Sets the text associated with a particular item.
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);

	// Sets the data (lParam) associated with a particular item.
	BOOL SetItemData(int nItem, DWORD_PTR dwData);

	// Retrieves the data (lParam) associated with a particular item.
	DWORD_PTR GetItemData(int nItem) const;

	// Retreives the number of selected items in the control.
	UINT GetSelectedCount() const;

	// Adds an item to the control.
	int InsertItem(int nItem, LPCTSTR lpszItem);

	// Removes a single item from the control.
	BOOL DeleteItem(int nItem);

	// Removes all items from the control.
	BOOL DeleteAllItems();

	// Retrieves the number of items in the control.
	int GetItemCount();

	// Inserts a column into a report-mode control.
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

};
#endif