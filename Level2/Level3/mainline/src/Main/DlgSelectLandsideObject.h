#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "..\Common\ALTObjectID.h"
#include "MuliSelTreeCtrlEx.h"

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
	CMuliSeltTreeCtrlEx m_TreeCtrl;


	LandsideFacilityLayoutObjectList *objectList;
	ALTObjectID m_altID;
	ALTObjectID getSelectObject();

	ALTObjectIDList getSelectObjectList();

	void AddObjType(int nType);
	void SetObjectTypeVector(std::vector<int> objectTypeList);

protected:
	void InitTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObjectID objName);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	bool FindRootItem(HTREEITEM hObjRoot);

	ALTObjectID GetTreeAltObject(HTREEITEM hItem);

	void InitAltObjectList();

	virtual BOOL OnInitDialog();

	bool TypeNeedToShow(int nType) const;
	std::map<int,HTREEITEM> ObjectMap;
	std::vector<HTREEITEM> m_vRootList;
	HTREEITEM m_hItemALL;
	HTREEITEM objectItem;
	ALTObjectIDList m_altIDList;
	//if size is ZERO, show all object
	std::vector<int> m_vObjType;
protected:
	virtual void OnOK();

public:
	afx_msg void OnTvnSelchangedTreeObjects(NMHDR *pNMHDR, LRESULT *pResult);
};
