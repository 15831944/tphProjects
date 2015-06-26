#pragma once

#include <vector>
#include "../MFCExControl/CARCTipComboBox.h"
#include "../MFCExControl/CoolTree.h"
// CARCBaseTree
#define ITEMEXPANDING				WM_USER + 1001
#define SELCHANGED					WM_USER + 1002
#define ENDLABELEDIT				WM_USER + 1003
#define BEGINLABELEDIT				WM_USER + 1004
class MFCEXCONTROL_API CARCBaseTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CARCBaseTree)

	class CTreeComboBox : public CARCTipComboBox
	{
	public:
		DWORD_PTR GetItemData(int nIndex );
		int m_nIndexSeled;
	private:
		//{{AFX_MSG(CTreeComboBox)
		afx_msg void OnKillFocus();
		afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

public:
	CARCBaseTree();
	virtual ~CARCBaseTree();

public:
	std::vector<COOLTREE_NODE_INFO*> m_vNOdeInfo;

	void EnableMutiTree(){	m_EnableMutiTree = TRUE; }
private:
	//Enable CoolTree As MutiTree, just has check box
	BOOL m_EnableMutiTree;


	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoolTree)
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	HTREEITEM GetSelectedItem();
	BOOL SelectItem(HTREEITEM hItem);
	int GetCmbBoxCurSel(HTREEITEM hItem);
	DWORD_PTR GetCurSelCmbBoxItemData(HTREEITEM hItem);
	void AutoAdjustCheck2Status(HTREEITEM hItem);
	void SetChect2Status(HTREEITEM hItem);
	BOOL IsCheckBox(HTREEITEM hItem);
	void SetItemBold(HTREEITEM hItem,BOOL bBold=TRUE,BOOL bRedraw=TRUE);
	void SetItemValueRange(HTREEITEM hItem,int nMin,int nMax,BOOL bVerify=TRUE);
	void DoEdit(HTREEITEM hItem);
	static void InitNodeInfo(CWnd* pParent,COOLTREE_NODE_INFO& CNI,BOOL bVerify=TRUE);
	void SelectFromText(CComboBox* pCB,CString strValue);
	BOOL SetItemData( HTREEITEM hItem, DWORD dwData );
	DWORD GetItemData(HTREEITEM hItem);
	void SetItemNodeInfo(HTREEITEM hItem,COOLTREE_NODE_INFO cni);
	BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage);
	CWnd* GetEditWnd(NODE_EDIT_TYPE net);
	CWnd* GetEditWnd(HTREEITEM hItem);
	void ShowComboBoxWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI,BOOL bEditable=FALSE);
	void ShowDateTimeWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI);
	HTREEITEM m_hEditedItem;
	HTREEITEM m_hPreItem;
	void ShowEditWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI,BOOL bCreateOnly=FALSE);
	void ShowEditSpinWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI);
	void AutoAdjustRadioStatus(HTREEITEM hItem);
	void AutoAdjustItemEnableStatus(HTREEITEM hItem);
	BOOL IsRadioItem(HTREEITEM hItem);
	void SetRadioStatus(HTREEITEM hItem,BOOL bRadio=TRUE);
	void DoRadio(HTREEITEM hItem);
	void DoCheck(HTREEITEM hItem);
	TREENODE_IMAGE_INDEX_TABLE GetItemImage(HTREEITEM hItem);
	void SetEnableStatus(HTREEITEM hItem,BOOL bEnable);
	void SetCheckStatus(HTREEITEM hItem,BOOL bCheck,BOOL bRecursive=FALSE);
	CImageList m_imageList;
	BOOL IsEnableItem(HTREEITEM hItem);
	int IsCheckItem(HTREEITEM hItem);
	COOLTREE_NODE_INFO* GetItemNodeInfo(HTREEITEM hItem);
	CWnd* m_pWndComboBox;
	CWnd* m_pWndComboBoxWithEdit;
	CWnd* m_pWndEditSpin;
	CWnd* m_pWndEdit;
	CWnd* m_pWndDateTimeCtrl; 
	CWnd* CreateEditWnd(NODE_EDIT_TYPE net);
	HTREEITEM InsertItem(LPCTSTR lpszItem, COOLTREE_NODE_INFO cni,BOOL bCheckOnOrRadioOn,BOOL bHoldEditWnd=FALSE,HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST );
	BOOL DeleteItem( HTREEITEM hItem );
	int m_nWidth;
	void SetWidth(int nWidth){m_nWidth = nWidth;}
	// Generated message map functions

	void SetItemGray(HTREEITEM hItem,BOOL bGray);
protected:
	//{{AFX_MSG(CCoolTree)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillfocusEdit1();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCombo();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
};


