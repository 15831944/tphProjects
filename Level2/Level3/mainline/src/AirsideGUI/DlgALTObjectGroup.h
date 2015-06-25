#pragma once
class ALTObject;
// CDlgALTObjectGroup dialog
#include "InputAirside/ALT_BIN.h"

// use this interface to decide whether stand family is allowed
// if not, strError will be displayed in a message box
class IALTObjectGroupFilter
{
public:
	virtual BOOL IsAllowed(int nALTObjectGroupID, CString& strError) = 0;
};

class AIRSIDEGUI_API CDlgALTObjectGroup : public CDialog
{
	DECLARE_DYNAMIC(CDlgALTObjectGroup)

public:
	CDlgALTObjectGroup(int nProjID, ALTOBJECT_TYPE altobjType, IALTObjectGroupFilter* pALTObjectGroupFilter = NULL, CWnd* pParent = NULL);   
	virtual ~CDlgALTObjectGroup();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	int GetSelALTObjectGroupID(){return m_nSelALTObjectGroupID;}
	CString GetSelALTObjectGroupName(){return m_strSelALTObjectGroupName;}

// Dialog Data
	//enum { IDD = IDD_DIALOG_STANDFAMILY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurALTObjectGroupNamePos(HTREEITEM hTreeItem);
	bool ALTObjectGroupInTree();

	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrl m_treeALTObjectGroup;

	int m_nProjID;
	ALTOBJECT_TYPE m_altobjType;
	CString m_strALTObjectTypeName;

	std::vector<ALTObject> m_vALTObjectGroup;
	CString m_strSelALTObjectGroupName;
	int m_nSelALTObjectGroupID;

	IALTObjectGroupFilter* m_pALTObjectGroupFilter;
};
