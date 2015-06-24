#pragma once
#include "afxcmn.h"

#include "../Landside/LandsideLayoutObject.h"
// CDlgCurbsideSelect dialog
#include "../Common/ALT_TYPE.h"

class CDlgLandsideObjectSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgLandsideObjectSelect)

public:
	CDlgLandsideObjectSelect(CWnd* pParent = NULL);   // standard constructor
	CDlgLandsideObjectSelect(LandsideFacilityLayoutObjectList *tmpList,ALTOBJECT_TYPE tmpAltType,CWnd *pParent=NULL);
	virtual ~CDlgLandsideObjectSelect();
	afx_msg void btnOk();
	afx_msg void btnCancel();
	virtual BOOL OnInitDialog();
	ALTObjectID getALTObjectID();
	CString getCurbSideName();
	LandsideFacilityLayoutObject *getLayoutObject();
	void initTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObjectID objName);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
    BOOL isGroupNode(CString strNameNode,int level, HTREEITEM &hParentItem);
	int getLevel(HTREEITEM hItem);
// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTCURBSIDE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeCtrl;
	LandsideFacilityLayoutObjectList *objectList;
	CString m_CurbsideName;
	ALTObjectID m_altID;
	ALTOBJECT_TYPE m_AltType;
	LandsideFacilityLayoutObject *m_LayoutObject;
	CString strRootName;

};
