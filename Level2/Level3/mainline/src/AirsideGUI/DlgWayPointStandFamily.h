#pragma once
#include "..\InputAirside\ALTObject.h"
class Stand;
// CDlgWayPointStandFamily dialog

class CDlgWayPointStandFamily : public CDialog
{
	DECLARE_DYNAMIC(CDlgWayPointStandFamily)

public:
	CDlgWayPointStandFamily(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWayPointStandFamily();

// Dialog Data
	enum { IDD = IDD_DIALOG_WAYPOINTANDSTANDFAMILY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
//	std::vector<std::pair<CString,int> > m_vWaypoint;
//	std::vector<std::pair<CString,int> > m_vStandFamily;
	std::vector<ALTObject> m_vStandFamily;
	int m_nProjID;
	CTreeCtrl m_TreeWayPointStandFamily;
	CString m_strStandFamily;
	int m_nSelStandFamilyID;
	int m_nSelWayPointID;
	CString m_strWayPoint;
	int m_nWayStandFlag;
	std::vector<ALTObject> m_vWaypoint;
	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	bool WayPointStandFamilyInTree();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeWaypointandstandfamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	int GetSelStandFamilyID(){return m_nSelStandFamilyID;}
	CString GetSelStandFamilyName(){return m_strStandFamily;}
	int GetSelWayPointID(){return m_nSelWayPointID;}
	CString GetSelWayPointName(){return m_strWayPoint;}
	int GetWayStandFlag(){return m_nWayStandFlag;}
	void GetAllStand();
};
