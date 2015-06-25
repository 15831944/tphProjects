#pragma once

class ALTObject;
class DeicePad;
// CDlgSelectDeicePads dialog

class CDlgSelectDeicePads : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectDeicePads)

public:
	CDlgSelectDeicePads(int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectDeicePads();

// Dialog Data
//	enum { IDD = IDD_DIALOG_SELECTDEICEPADS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	std::vector<ALTObject> m_vDeicepadsFamily;
	int m_nProjID;
	CTreeCtrl m_TreeDeicepadsFamily;
	CString m_strDeicepadsFamily;
	int m_nSelDeicepadsFamilyID;

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurDeicepadsGroupNamePos(HTREEITEM hTreeItem);
	bool DeicepadsFamilyInTree();

public:
	afx_msg void OnTvnSelchangedTreeDeicepadsfamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	int GetSelDeicepadsFamilyID(){return m_nSelDeicepadsFamilyID;}
	CString GetSelDeicepadsFamilyName(){return m_strDeicepadsFamily;}
};
