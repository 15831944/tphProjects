#pragma once


class ALTObject;
class ALTObjectID;

class CDeicepadAndStandFamilySelectDlg :
	public CDialog
{
	DECLARE_DYNAMIC(CDeicepadAndStandFamilySelectDlg)


public:
	CDeicepadAndStandFamilySelectDlg(int nProjID, CWnd* pParent = NULL);
	~CDeicepadAndStandFamilySelectDlg(void);
	// Dialog Data
	enum { IDD = IDD_DIALOG_SELSTAND_DEICEPAD };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 
	DECLARE_MESSAGE_MAP()


	void SetDeicePadTreeContent(void);
	void SetStandFamilyTreeContent(void);
	void AddObjectToStandTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjStandNode(HTREEITEM hParentItem,const CString& strNodeText);

	void AddObjectToDeiceTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjDeiceNode(HTREEITEM hParentItem,const CString& strNodeText);

	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	void GetStandFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily);

	int GetCurDeiceGroupNamePos(HTREEITEM hTreeItem);
	void GetDeiceAltID(HTREEITEM hItem,ALTObjectID& objName, CString& strStandFamily);


public:
	afx_msg void OnTvnSelchangedTreeDeicePads(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNmKillfocusTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNmKillfocusTreeDeicePads(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();


	CString GetDeiceFamilyName()const{ return m_strDeices; }
	CString GetStandFamilyName()const{ return m_strStandGroups; }

	std::vector<int> GetDeiceGroupList()const{ return m_vDeiceSelID; }
	std::vector<int> GetStandGroupList()const{ return m_vStandSelID; }

	BOOL IsAllDeice()const{ return m_bAllDeice; }
	BOOL IsAllStand()const{ return m_bAllStand; }

public:
	CTreeCtrl m_TreeCtrlDeicepads;
	CTreeCtrl m_TreeCtrlStandFamily;

	std::vector<int> m_vDeiceSelID;
	std::vector<int> m_vStandSelID;

	BOOL m_bAllStand;
	BOOL m_bAllDeice;

	CString m_strDeices;
	CString m_strStandGroups;

	int m_nProjID;

};
