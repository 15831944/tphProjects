

////
//////+++++++++++++++++++++++++++++++++++++++++
////#pragma once
////// CTreeCtrlRunwayExit
////
////class CTreeCtrlRunwayExit : public CTreeCtrl
////{
////	DECLARE_DYNAMIC(CTreeCtrlRunwayExit)
////
////public:
////	CTreeCtrlRunwayExit();
////	virtual ~CTreeCtrlRunwayExit();
////
////protected:
////	RunwayExitPercentItem *m_pRunwayExitPercentItem;
////	DECLARE_MESSAGE_MAP()
////};
//////+++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------

#pragma once
#include <vector>
#include "..\InputAirside\TimeRangeRunwayExitStrategyItem.h"
#include "..\MFCExControl\CoolTree.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlRunwayExit
//enum UM_COOLTREE_EDIT_WND
//{
//	UM_CEW_EDIT_END=WM_USER+1,
//	UM_CEW_EDIT_BEGIN,
//	UM_CEW_EDITSPIN_END,
//	UM_CEW_EDITWND_END,
//	UM_CEW_EDITSPIN_BEGIN,
//	UM_CEW_COMBOBOX_END,
//	UM_CEW_COMBOBOX_BEGIN,
//	UM_CEW_COMBOBOX_SELCHANGE,
//	UM_CEW_SHOW_DIALOGBOX_END,
//	UM_CEW_SHOW_DIALOGBOX_BEGIN,
//	UM_CEW_SHOWMENU_BEGIN,
//	UM_CEW_SHOWMENU_READY,
//	UM_CEW_SHOWMENU_END,
//	UM_CEW_STATUS_CHANGE,
//};
//enum NODE_TYPE
//{
//	NT_NULL,
//	NT_NORMAL,
//	NT_FREE,
//	NT_CHECKBOX,
//	NT_RADIOBTN
//};
//
//enum NODE_EDIT_TYPE
//{
//	NET_NORMAL,
//	NET_EDIT,
//	NET_EDIT_INT,
//	NET_EDIT_FLOAT,
//	NET_EDITSPIN,
//	NET_COMBOBOX,
//	NET_COMBOBOX_WITH_EDIT,
//	NET_SHOW_DIALOGBOX,
//	NET_EDITSPIN_WITH_VALUE,
//	NET_EDIT_WITH_VALUE
//};
//
//typedef struct COOLTREENODEINFO
//{
//	NODE_TYPE nt;
//	NODE_EDIT_TYPE net;
//	BOOL bEnable;
//	int nImage;
//	int nImageSeled;
//	int nImageDisable;
//	float fMinValue;
//	float fMaxValue;
//	int nMaxCharNum;
//	CWnd* pEditWnd;
//	DWORD dwItemData;
//	BOOL bVerify;
//	COLORREF clrItem;
//	LOGFONT lfontItem;
//	UINT unMenuID;
//	BOOL bAutoSetItemText;
//}COOLTREE_NODE_INFO;
//
//enum TREENODE_IMAGE_INDEX_TABLE
//{
//	TIIT_NULL=0,
//	TIIT_NULL_DISABLE,
//	TIIT_RADIOBTN_TRUE,
//	TIIT_RADIOBTN_TRUE_DISABLE,
//	TIIT_RADIOBTN_FALSE,
//	TIIT_RADIOBTN_FALSE_DISABLE,
//	TIIT_CHECKBOX_TRUE,
//	TIIT_CHECKBOX_TRUE_DISABLE,
//	TIIT_CHECKBOX_2_TRUE,
//	TIIT_CHECKBOX_2_TRUE_DISABLE,
//	TIIT_CHECKBOX_FALSE,
//	TIIT_CHECKBOX_FALSE_DISABLE,	
//	TIIT_NORMAL,
//	TIIT_NORMAL_DISABLE,
//	TIIT_EDIT,
//	TIIT_EDIT_DISABLE,
//	TIIT_EDITSPIN,
//	TIIT_EDITSPIN_DISABLE,
//	TIIT_COMBOBOX,
//	TIIT_COMBOBOX_DISABLE
//};
#define ID_COOLTREE_EDIT 100
#define ID_COOLTREE_COMBO 110
#define ID_IMAGE_COUNT 20
class AIRSIDEGUI_API  CTreeCtrlRunwayExit : public CTreeCtrl
{
	//need access to m_pfnSuper
	friend class CNodeView;

	class CTreeComboBox : public CComboBox
	{
	public:
		int m_nIndexSeled;
	private:
		//{{AFX_MSG(CTreeComboBox)
		afx_msg void OnKillFocus();
		afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};
	// Construction
public:
	 CTreeCtrlRunwayExit();

	// Attributes
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
	//{{AFX_VIRTUAL(CTreeCtrlRunwayExit)
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	HTREEITEM GetSelectedItem();
	int GetCmbBoxCurSel(HTREEITEM hItem);
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
	CWnd* GetEditWnd(NODE_EDIT_TYPE net);
	CWnd* GetEditWnd(HTREEITEM hItem);
	void ShowComboBoxWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI,BOOL bEditable=FALSE);
	HTREEITEM m_hEditedItem;
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
	RunwayExitPercentItem *m_pRunwayExitPercentItem;
	CWnd* CreateEditWnd(NODE_EDIT_TYPE net);
	HTREEITEM InsertItem(LPCTSTR lpszItem, COOLTREE_NODE_INFO cni,BOOL bCheckOnOrRadioOn,BOOL bHoldEditWnd=FALSE,HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST );
	virtual ~CTreeCtrlRunwayExit();
	BOOL DeleteItem( HTREEITEM hItem );
	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlRunwayExit)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillfocusEdit1();
	afx_msg void OnPaint();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//----------------------------
