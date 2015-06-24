#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "3DObjModelEditTree.h"
#include "afxext.h"

#include "3DObjModelEditFrame.h"

class C3DObjModelEditContext;
// C3DObjModelEditTreeView form view

class C3DObjModelEditTreeView : public CFormView
{
	DECLARE_DYNCREATE(C3DObjModelEditTreeView)

protected:
	C3DObjModelEditTreeView();           // protected constructor used by dynamic creation
	virtual ~C3DObjModelEditTreeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	enum { IDD = IDD_FORMVIEW_MODELEDIT_TREE };

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEdit();
	afx_msg void OnUpdateBtnAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnEdit(CCmdUI* pCmdUI);

	afx_msg LRESULT OnUpdateToolbar( WPARAM wParam, LPARAM );
	afx_msg LRESULT OnSelectNodeItem( WPARAM wParam, LPARAM );
	afx_msg LRESULT OnDeleteNodeItem( WPARAM wParam, LPARAM );
	afx_msg LRESULT OnQueryLocked( WPARAM wParam, LPARAM );

	afx_msg void OnSaveAs();
	afx_msg void OnSave();
	afx_msg void OnOK();
	afx_msg void OnCancel();

	C3DObjModelEditFrame* GetParentFrame() const;
	C3DObjModelEditContext* GetEditContext() const;

	void UpdateView(
		C3DObjModelEditFrame::UpdateViewType updateType = C3DObjModelEditFrame::Type_ReloadData,
		DWORD dwData = 0);

	void ReloadData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()


private:
	CStatic m_rectFrame;
	CToolBar m_Toolbar;
	C3DObjModelEditTree m_3DObjTree;
	CButton m_btnSaveAs;
	CButton m_btnSave;
	CButton m_btnOK;
	CButton m_btnCancel;

	CImageList	m_treeImageList;
	C3DObjModelEditTree::SelectedItemType m_selItemType;
};


