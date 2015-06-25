#pragma once
#include "afxcmn.h"
#include "NodeBlockListCtrl.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgIntersectionNodeBlock dialog

class IntersectionNodeBlockList;
class CDlgIntersectionNodeBlock : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgIntersectionNodeBlock)

public:
	CDlgIntersectionNodeBlock(int nAirportID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIntersectionNodeBlock();

// Dialog Data
	enum { IDD = IDD_DLGINTERSECTIONNODEBLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void InitNodeOption();


	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

private:
	CSortableHeaderCtrl m_wndSortableHeaderCtrl;
	NodeBlockListCtrl m_lstNodeOption;
	IntersectionNodeBlockList* m_pNodeOptionList;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
};
