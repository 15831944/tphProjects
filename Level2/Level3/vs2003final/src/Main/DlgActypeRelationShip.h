#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "TreeCtrlEx.h"
// CDlgActypeRelationShip dialog
class CACTypesManager;
#include "../Common/SuperTypeRelationData.h"
class CDlgActypeRelationShip : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgActypeRelationShip)

public:
	CDlgActypeRelationShip(CACTypesManager* _ActypeManger ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgActypeRelationShip();

// Dialog Data
	enum { IDD = IDD_DIALOG_ACTYPE_RELATION_SHIP };
protected:
	BOOL OnInitDialog() ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
	void OnSize(UINT nType, int cx, int cy) ;
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) ;
protected:
    SuperTypeRelationData* m_RelationData ;
	CACTypesManager* m_ActypeManager ;
protected:
	void OnInitToolBar() ;
	void InitTreeCtrl() ;
	void AddTreeItem( SuperTypeRelationData::P_TREE_NODE _data ,HTREEITEM _AfterItem) ;
	void HandleEditLable(CString& _val ,HTREEITEM _item)  ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CStatic m_StaticTool;
	CCoolTree m_TreeCtrl;
	CToolBar m_ToolBar ;
	HTREEITEM m_RClickItem ;
public:
	afx_msg void OnTvnEndlabeleditTreeShip(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnChangedTreeShip(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CStatic m_Static_Cap;
	CStatic m_Static_SPC;
protected:
	void OnEditAliasName() ;
	void OnAddSubNode() ;
	void OnRemove() ;

	void OnAddRelation() ;
	void OnDelRelation() ;
	void OnEditRelation() ;

	void OnOK() ;
	void OnCancel() ;
#ifdef _DEBUG
	void onfind() ;
#endif
};
