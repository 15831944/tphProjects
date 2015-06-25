#pragma once
#include "..\MFCExControl\ToolTipDialog.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\Database\DBElementCollection.h"
#include "NodeViewDbClickHandler.h"
class CAACompCatManager;

class CNECRelatedBaseDlg : public CToolTipDialog
{
public:
	CNECRelatedBaseDlg(PFuncSelectFlightType pSelectFlightType, LPCTSTR lpszCatpion, CWnd* pParent = NULL);
	virtual ~CNECRelatedBaseDlg();

// Dialog Data
	enum { IDD = IDD_NECRELATEDBASEDLG };

private:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	
	void RefreshLayout(int cx = -1, int cy = -1);
	void UpdateToolBarState();
	void InitListControl();

	CToolBar	m_wndToolBar;
	CString		m_strCaption;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnCmdEditItem();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListNec(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListNec(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	CListCtrlEx	m_wndListCtrl;
	PFuncSelectFlightType	m_pSelectFlightType;

	int GetSelectedListItem();
	void OnEditItem();
	
	virtual void ReloadData() = 0;
	virtual void SetListColumn() = 0;
	virtual void SetListContent() = 0;
	virtual void CheckListContent() = 0;	
	virtual void OnNewItem(FlightConstraint& fltType) = 0;
	virtual void OnDelItem() = 0;
	virtual void OnListItemChanged(int nItem, int nSubItem) = 0;
	virtual void OnNECItemChanged(int nItem, int nNewNECTableID) = 0;
};
