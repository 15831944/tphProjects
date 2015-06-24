#pragma once

#include "Landside/LandsideExternalNode.h"
// #include "Common/ALTObjectID.h"
// #include "Common/ILayoutObject.h"
// CDlgEntryExitNodeSelect dialog
class LandsideFacilityLayoutObjectList;

class CDlgEntryExitNodeSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgEntryExitNodeSelect)

  	typedef std::map<ALTObjectID,LandsideExternalNode *> EntryExitMap;	

public:
	CDlgEntryExitNodeSelect(CWnd* pParent = NULL);   // standard constructor
	CDlgEntryExitNodeSelect(LandsideFacilityLayoutObjectList* objlist,CWnd* pParent = NULL);

	virtual ~CDlgEntryExitNodeSelect();

// Dialog Data
	enum { IDD = IDD_DIALOG_BUSSTATIONSELECT };

	virtual BOOL OnInitDialog();
	void LoadTree();
	LandsideExternalNode *GetEntryExitNode(){return m_pEntryExitNode;}
// 	void InsertEntrExitNode(LandsideExternalNode *pExternNode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	LandsideFacilityLayoutObjectList* m_pObjlist;
	CTreeCtrl m_treeCtrl;
 	EntryExitMap m_mapEntryExit;
	LandsideExternalNode *m_pEntryExitNode;
public:
	afx_msg void OnTvnSelchangedTreeBusstationselect(NMHDR *pNMHDR, LRESULT *pResult);
};
