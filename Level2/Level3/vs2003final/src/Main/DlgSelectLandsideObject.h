#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "..\Common\ALTObjectID.h"

class LandsideFacilityLayoutObjectList;

// CDlgSelectLandsideObject dialog

class CDlgSelectLandsideObject : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectLandsideObject)

public:
	CDlgSelectLandsideObject(LandsideFacilityLayoutObjectList *tmpList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectLandsideObject();

// Dialog Data
	//enum { IDD = IDD_DIALOG_SELECTLANDSIDEOBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtSelect;
	CTreeCtrl m_TreeCtrl;


	LandsideFacilityLayoutObjectList *objectList;
	ALTObjectID m_altID;
	ALTObjectID getSelectObject();

	void AddObjType(int nType);
	void SetObjectTypeVector(std::vector<int> objectTypeList);

protected:
	void InitTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObjectID objName);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	bool FindRootItem(HTREEITEM hObjRoot);

	virtual BOOL OnInitDialog();

	bool TypeNeedToShow(int nType) const;
	std::map<int,HTREEITEM> ObjectMap;
	HTREEITEM m_hItemALL;
	HTREEITEM objectItem;
	//if size is ZERO, show all object
	std::vector<int> m_vObjType;
protected:
	virtual void OnOK();

public:
	afx_msg void OnTvnSelchangedTreeObjects(NMHDR *pNMHDR, LRESULT *pResult);
};
