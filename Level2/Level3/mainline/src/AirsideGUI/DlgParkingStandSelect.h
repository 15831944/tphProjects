#pragma once
//#include "afxcmn.h"
#include "Resource.h"
#include "common/ALTObjectID.h"
// #include "../InputAirside/RunwayExit.h"
// #include "../InputAirside/Runway.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgParkingStandSelect
class ALTObject;
class CDlgParkingStandSelect : public CXTResizeDialog
{


	DECLARE_DYNAMIC(CDlgParkingStandSelect)

	enum { IDD=IDD_DIALOG_OCCUPIEDASSIGNEDSTAND_SELECT };

public:
	CDlgParkingStandSelect(const UINT nID,int nProjID,const CString& strSelStands, CWnd* pParent = NULL);
	virtual ~CDlgParkingStandSelect();

//  	CString GetRunwayExitName();
//  	BOOL IsAllRunwayExit();
// 	void SetRunwayExitTreeContent(void);
// 	std::vector<int>& GetRunwayExitList();

	CString GetStandFamilyName();
	std::vector<int>& GetStandGroupList();
	void SetStandFamilyTreeContent(void);
	BOOL IsAllStand();
	afx_msg void OnSize(UINT nType, int cx, int cy);


protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	bool IsStandSelected(const CString& strStandName);
	void GetStandFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);



// 	afx_msg void OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult);
// 	RUNWAYEXITTYPE GetRunwayExitType(HTREEITEM hRunwayTreeItem);


// 	void GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName);
// 	bool IsExitSelected(const CString& strExitName);
	virtual void OnOK();


	CTreeCtrl m_TreeCtrlStandFamily;

	int m_nProjID;
	std::vector<int> m_vStandSelID;

	BOOL m_bAllStand;

	CString m_strStandGroups;
	

};


