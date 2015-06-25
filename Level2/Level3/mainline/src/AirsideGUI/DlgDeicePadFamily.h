#pragma once

class ALTObject;
// CDlgDeicePadFamily dialog

class CDlgDeicePadFamily : public CDialog
{
	DECLARE_DYNAMIC(CDlgDeicePadFamily)

public:
	CDlgDeicePadFamily(int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeicePadFamily();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEICEPADFAMILY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	std::vector<ALTObject> m_vStandFamily;
	int m_nProjID;
	CTreeCtrl m_TreeStandFamily;
	CString m_strStandFamily;
	int m_nSelStandFamilyID;

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	bool StandFamilyInTree();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreestandfamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	int GetSelStandFamilyID(){return m_nSelStandFamilyID;}
	CString GetSelStandFamilyName(){return m_strStandFamily;}
};
