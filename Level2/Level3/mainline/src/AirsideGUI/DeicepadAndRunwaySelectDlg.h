#pragma once
#include "../InputAirside/PostDeice_BoundRoute.h"
class ALTObject;
class ALTObjectID;

class AIRSIDEGUI_API CDeicepadAndRunwaySelectDlg: public CDialog
{
	DECLARE_DYNAMIC(CDeicepadAndRunwaySelectDlg)

public:
	CDeicepadAndRunwaySelectDlg(int nProjID, CWnd* pParent = NULL);
	~CDeicepadAndRunwaySelectDlg(void);

	// Dialog Data
	enum { IDD = IDD_DIALOG_SELSTAND_DEICEPAD };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 
	DECLARE_MESSAGE_MAP()


	void SetDeicePadTreeContent(void);
	void SetRunwayTreeContent(void);

	void AddObjectToDeiceTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjDeiceNode(HTREEITEM hParentItem,const CString& strNodeText);

	int GetCurDeiceGroupNamePos(HTREEITEM hTreeItem);
	void GetDeiceAltID(HTREEITEM hItem,ALTObjectID& objName, CString& strStandFamily);


public:
	afx_msg void OnTvnSelchangedTreeDeicePads(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeRunways(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();


	CString GetDeiceFamilyName()const{ return m_strDeices; }
	CString GetRunwaysName()const{ return m_strRunways; }

	std::vector<int> GetDeiceGroupList()const{ return m_vDeiceSelID; }
	std::vector<CPostDeice_BoundRoute::LogicRwyID> GetRunwayList()const{ return m_vRunwaySelID; }

	BOOL IsAllDeice()const{ return m_bAllDeice; }
	BOOL IsAllRunway()const{ return m_bAllRunway; }

public:
	CTreeCtrl m_TreeCtrlDeicepads;
	CTreeCtrl m_TreeCtrlRunway;

	std::vector<int> m_vDeiceSelID;
	std::vector<CPostDeice_BoundRoute::LogicRwyID> m_vRunwaySelID;

	BOOL m_bAllRunway;
	BOOL m_bAllDeice;

	CString m_strDeices;
	CString m_strRunways;

	int m_nProjID;

};
