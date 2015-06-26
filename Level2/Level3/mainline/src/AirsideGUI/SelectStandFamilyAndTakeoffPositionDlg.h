#pragma once
#include "afxcmn.h"
#include "common/ALTObjectID.h"
#include "InputAirside/RunwayExit.h"
#include "MuliSelTreeCtrlEx.h"

class ALTObject;
enum RUNWAY_MARK;
class Stand;


// CSelectStandFamilyAndTakeoffPositionDlg dialog

class CSelectStandFamilyAndTakeoffPositionDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectStandFamilyAndTakeoffPositionDlg)

	enum RUNWAYEXITTYPE
	{
		RUNWAYEXIT_DEFAULT = 0,
		RUNWAYEXIT_ALLRUNWAY,
		RUNWAYEXIT_LOGICRUNWAY,
		RUNWAYEXIT_ITEM
	};

public:
	CSelectStandFamilyAndTakeoffPositionDlg(const UINT nID,int nProjID, const CString& strSelExits, const CString& strSelStands, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectStandFamilyAndTakeoffPositionDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELSTANDANDRUNWAYEXIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 
	DECLARE_MESSAGE_MAP()

public:
	
	CString GetRunwayExitName();
	CString GetStandFamilyName();

	std::vector<int>& GetRunwayExitList();
	std::vector<int>& GetStandGroupList();

	BOOL IsAllRunwayExit();
	BOOL IsAllStand();


protected:
	void SetRunwayExitTreeContent(void);
	void SetStandFamilyTreeContent(void);
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	RUNWAYEXITTYPE GetRunwayExitType(HTREEITEM hRunwayTreeItem);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	void GetStandFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily);
	void GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName);

	bool IsExitSelected(const CString& strExitName);
	bool IsStandSelected(const CString& strStandName);


public:
	afx_msg void OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNmKillfocusTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNmKillfocusTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();

protected:
	CMuliSeltTreeCtrlEx m_TreeCtrlRunwayExit;
	CMuliSeltTreeCtrlEx m_TreeCtrlStandFamily;

	std::vector<int> m_vExitSelID;
	std::vector<int> m_vStandSelID;

	BOOL m_bAllStand;
	BOOL m_bAllRunwayExit;

	CString m_strRunwayExits;
	CString m_strStandGroups;

	int m_nProjID;

};
