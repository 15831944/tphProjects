#pragma once
#include "afxcmn.h"
#include "TermPlanDoc.h"
#include "..\MFCExControl\CoolTree.h"
#include "..\Landside\LandsideIntersectionNode.h"
#include "afxwin.h"
// CDlgIntersectionSelect dialog

class CDlgIntersectionSelect : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgIntersectionSelect)

public:
	CDlgIntersectionSelect(CTermPlanDoc *tmpDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIntersectionSelect();

	bool AddObject( LandsideIntersectionNode * pObj );
	bool StrExistUnderItem(HTREEITEM &hItem, const CString& str/*,bool& bInsert*/ );
	void SetTreeContent();
	CString GetIntersectionName(){return m_strIntersection;}
	
// Dialog Data
	enum { IDD = IDD_DIALOG_INTERSECTIONNODE_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CCoolTree m_treeIntersection;

	CTermPlanDoc *m_pDoc;
	CString m_strIntersection;
// 	ALTObjectIDList m_ObjIDList;
public:
	afx_msg void OnTvnSelchangedTreeIntersection(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CButton m_btnOK;
};
