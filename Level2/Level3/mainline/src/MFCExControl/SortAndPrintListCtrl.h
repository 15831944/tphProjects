#pragma once
#include "../MFCExControl/SortableHeaderCtrl.h"
#include <map>

#define   SORT_INDEX_COMPLETE_MSG	WM_USER + 180

struct SortItemData 
{
	COLORREF crTextColor;
	COLORREF crBkColor;
	DWORD_PTR  m_dwData;
};
class MFCEXCONTROL_API CMultiSortHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNCREATE(CMultiSortHeaderCtrl)
public:
	CMultiSortHeaderCtrl();
	virtual ~CMultiSortHeaderCtrl();
protected:
	DECLARE_MESSAGE_MAP()

public:
	int	SetSortImage( int nCol, BOOL bAsc );
	void RemoveAllSortImages();
	void RemoveSortImage( int iItem );
public:
	int     m_nSortCol;
	BOOL    m_bSortAsc;
	CBitmap m_bmpArrowUp;
	CBitmap m_bmpArrowDown;
};

class MFCEXCONTROL_API CSortAndPrintListCtrl: public CListCtrl
{
	DECLARE_DYNAMIC(CSortAndPrintListCtrl)
public:
	enum ItemDataType{ITEM_NULL,ITEM_INT,ITEM_DOUBLE,ITEM_STRING,ITEM_TIME,ITEM_DATATIME};

	CSortAndPrintListCtrl();
	virtual ~CSortAndPrintListCtrl();
protected:
	DECLARE_MESSAGE_MAP()
public:
	void SubClassHeadCtrl();
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);	
public:
	//single sort, multiple sort
	void SetDataType(int iItem, ItemDataType Type) { m_arrColType[iItem] = Type; }
	int GridCtrlMultiItemCompare(int nRow,int nMid);
	int GridCtrlItemCompare(int nSortedCol,const CString& csText1, const CString& csText2);
	int	FindItemInCombinedSortedList( int p_nItem );
	int	GetNumCombinedSortedColumns();
	void MoveItemInCombinedSortedListToEnd( int p_nItem );
	bool NotInCombinedSortedColumnList( int p_nItem );
	void SortColumn( int nSubItem, int nSortingMultipleColumns/* = 0 */);
	BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);

	//print
	void PrintList( CString _sHeader, CString _sFoot );
	int GetRowHeight(int nRow);
	int GetHeadRowHeight(int nRow);
	int GetHeadColumnWidth(int nCol);
	int GetColumnWidth(int nCol);
	int GetVirtualWidth();
	int GetVirtualHeight();
	void DrawHeadItem(CDC* pDC, int nRow, int nCol, CRect rect);
	void DrawItem(CDC* pDC, int nRow, int nCol, CRect rect);
	void AutoSize();
	CSize GetTextExtent(LPCTSTR str);

	// Sets the data (lParam) associated with a particular item.
	BOOL SetItemData(int nItem, DWORD_PTR dwData);

	// Retrieves the data (lParam) associated with a particular item.
	DWORD_PTR GetItemData(int nItem) const;

	BOOL SetItemColor(int nItem,COLORREF crTextColor,COLORREF crBkColor);

	BOOL DeleteItem(int nItem);
	void DrawText(int nItem, int nSubItem, CDC *pDC, COLORREF crText, COLORREF crBkgnd, CRect &rect);
public:
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

	virtual void PrintColumnHeadings(CDC *pDC, CPrintInfo *pInfo);
	virtual void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
	virtual void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
private:
	CMultiSortHeaderCtrl	m_wndSortableHeaderCtrl;
	int						m_aCombinedSortedColumns[32];
	ItemDataType			m_arrColType[32];
	bool					m_bAscending;

	// Printing information
	CFont					m_PrinterFont; 
	CSize					m_CharSize;
	int						m_nPageHeight;
	CSize					m_LogicalPageSize; 
	CSize					m_PaperSize; 
    int						m_nGridLines;
	std::map<int,int>       m_arRowHeights;//Row id, row heigth
	std::map<int,int>		m_arColWidths;//column id,column width
};