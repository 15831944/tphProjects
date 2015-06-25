#pragma once
#include "AirsideGUIAPI.h"
class ALTObject;
class Stand;
// CDlgStandFamily dialog


// use this interface to decide whether stand family is allowed
// if not, strError will be displayed in a message box
class IStandFamilyFilter
{
public:
	virtual BOOL IsAllowed(int nStandFamilyID, CString& strError) = 0;
};

class AIRSIDEGUI_API CDlgStandFamily : public CDialog
{
	DECLARE_DYNAMIC(CDlgStandFamily)

public:
	CDlgStandFamily(int nProjID, IStandFamilyFilter* pStandFamilyFilter = NULL, CWnd* pParent = NULL);   
	virtual ~CDlgStandFamily();

	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreestandfamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	int GetSelStandFamilyID(){return m_nSelStandFamilyID;}
	CString GetSelStandFamilyName(){return m_strStandFamily;}

// Dialog Data
	//enum { IDD = IDD_DIALOG_STANDFAMILY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	bool StandFamilyInTree();

	DECLARE_MESSAGE_MAP()

private:
	std::vector<ALTObject> m_vStandFamily;
	int m_nProjID;
	CTreeCtrl m_TreeStandFamily;
	CString m_strStandFamily;
	int m_nSelStandFamilyID;

	IStandFamilyFilter* m_pStandFamilyFilter;
};
