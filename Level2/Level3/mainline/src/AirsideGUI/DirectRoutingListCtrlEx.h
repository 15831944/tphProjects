// ListCtrlEx.h : header file
//
#pragma once
#include "..\MFCExControl\InPlaceList.h"
#include "..\MFCExControl\MySpin.h"
#include "..\MFCExControl\InplaceDatetimeCtrl.h"

#define WM_COLLUM_INDEX                 (WM_USER + 45)

#define LVCFMT_DROPDOWN					0x0040 
#define LVCFMT_SPINEDIT					0x0080 
#define LVCFMT_NOEDIT					0x0100
#define LVCFMT_NUMBER					0x0200
#define	LVCFMT_DATETIME					0x0400
#define LVCFMT_POPUP_GATE_SELECTION		0x0800
#define LVCFMT_EDIT						0x1000
#define LVCFMT_CHECK					0x2000

#define DROP_DOWN			    1
#define SPIN_EDIT			    2
#define NO_EDIT					3
#define NUMBER_ONLY				4
#define DATE_TIME				5
#define POPUP_GATE_SELECTION	6
#define TEXT_EDIT				7
#define CHECKBOX				8

class DRLV_COLUMNEX
{
public:

    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
    int iImage;
    int iOrder;
    CStringList* csList;
	LPCSTR pszDateFormat;
	LPCSTR pszShowDateFormat;

	DRLV_COLUMNEX()
	{
		csList = NULL;
		pszDateFormat = NULL;
		pszShowDateFormat = NULL;
	}
};

struct DRLVCOLDROPLIST
{
   WORD Style;
   CStringList List;
   CString strDateFormat;
   CString strShowDateFormat;
};

class AIRSIDEGUI_API CDirectRoutingListCtrlEx : public CListCtrl
{
public:
	CDirectRoutingListCtrlEx();

	// [EditNew] - call EditSpinLable()/EditSubLable() to edit the label
	void EditNew();

	// [EditSubLabel]	- Start edit of a sub item label
	// Returns			- Temporary pointer to the new edit control
	// nItem			- The row index of the item to edit
	// nCol				- The column of the sub item.
	CEdit* EditSubLabel( int nItem, int nCol );
	
	// [HitTestEx] - Determine the row index and column index for a point
	// Returns   - the row index or -1 if point is not over a row
	// point     - point to be tested.
	// col       - to hold the column index
	int HitTestEx(CPoint &point, int *col);

	//[DeleteItemEx] - Delete the selected item from the list
	void DeleteItemEx( int p_index );

	//[GetCurSel] - Return the row number of the current selected item
	int GetCurSel(void) const { return CurrentSelection; }
	int GetLastSel(void) const { return LastTimeSelection; }
	
	//[InsertColumn] - 
	int InsertColumn(int nCol, const DRLV_COLUMNEX* pColumn);

	void SetSpinDisplayType( int _iType ){ m_iSpinDisplayType = _iType ;};	
	virtual ~CDirectRoutingListCtrlEx();
	void SetHHmmTimeFormat(){ m_bHHmmFormat = TRUE; };
	void SetEditSpinRange(int _low,int _Upp){ m_SpinEdit.m_nSpinEditLower=_low; m_SpinEdit.m_nSpinEditUpper = _Upp; }

	DRLVCOLDROPLIST* GetColumnStyle(int nColumn);

	// Generated message map functions
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	BOOL IsItemEmpty(int nItem);

	//pop up the gate selection dialog.
	void PopUpGateSelectionDlg();

	// [EditSpinLabel]  - 
	CEdit* EditSpinLabel(int nItem, int nCol);

	// ShowInPlaceList - Creates an in-place drop down list for any 
	//                 - cell in the list view control
	// Returns         - A temporary pointer to the combo-box control
	// nItem           - The row index of the cell
	// nCol            - The column index of the cell
	// lstItems        - A list of strings to populate the control with
	// nSel            - Index of the initial selection in the drop down list
	CComboBox* ShowInPlaceList( int nItem, int nCol);

	// [EditDateTimeLabel]
	// Similar with EditSpinLabel
	COleDateTime* EditDateTimeLabel(int nItem, int nCol);

	// Is pre item is empty (not exist or just empty)?
	BOOL IsPreItemEmpty(int nItem);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	void SetProjID(int nProjID){m_nProjID = nProjID;}
protected:
	CPtrArray ddStyleList;
	int m_iSpinDisplayType; // SpinEdit haven't Percent Property if m_iSpinDisplayType == 1
	CMySpin m_SpinEdit;
	CInPlaceDateTimeCtrl m_inPlaceDateTimeCtrl;
	int m_nSpinSelItem;	// remember the selection.
	int m_nSpinSelCol;
	int m_nTimeSelItem;
	int m_nTimeSelCol;
	int CurrentSelection;
	int LastTimeSelection;
	BOOL m_bHHmmFormat; // True: CInPlaceDateTimeCtrl use "HH:mm" format, False : hh:mm:ss
	CStringList m_strListSegFrom;
	CStringList m_strListSegTo;
	CStringList m_strListSegMax;
	void GetStrListSegFrom();
	void GetStrListSegTo(int nSegFromID);
	void GetStrListSegMax(int nSegFromID,int nSegToID);
	bool IsInStrListSegFrom(CString str);
	bool IsInStrListSegTo(CString str);
	bool IsInStrListSegMax(CString str);

	int m_nProjID;
};
