#pragma once
#include "SelectStandFamilyAndTakeoffPositionDlg.h"
// CRunwayExitAndStandFamilySelectDlg dialog
class ALTObject;
enum RUNWAY_MARK;
class Stand;
class RunwayExit;
typedef std::vector<RunwayExit> RunwayExitList;


class CRunwayExitAndStandFamilySelectDlg : public CSelectStandFamilyAndTakeoffPositionDlg
{
	DECLARE_DYNAMIC(CRunwayExitAndStandFamilySelectDlg)

public:
	CRunwayExitAndStandFamilySelectDlg(const UINT nID,int nProjID,const CString& strSelExits, const CString& strSelStands, CWnd* pParent = NULL);   // standard constructor
	 ~CRunwayExitAndStandFamilySelectDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_RUNWAYEXITANDSTANDFAMILYSELECT };

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//    virtual BOOL OnInitDialog(); 
//	DECLARE_MESSAGE_MAP()
//
//private:
//	void SetRunwayExitTreeContent(void);
//	void SetStandFamilyTreeContent(void);
//	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
//	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
//	RUNWAYEXITTYPE GetRunwayExitType(HTREEITEM hRunwayTreeItem);
//	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
//	void GetRunwayExitName(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName);
//
//public:
//	void SetSelRunwayID(int nRunwayID);
//	int GetSelRunwayID();
//	void SetSelRunwayMarking(RUNWAY_MARK runwayMarking);
//	RUNWAY_MARK GetSelRunwayMarking();
//	void SetRunwayExitID(int nRunwayExitID);
//	int GetSelRunwayExitID();
//	void SetSelStandFamilyID(int nStandFamilyID);
//	int GetSelStandFamilyID();
//	void SetRunwayAllExitFlag(BOOL bRunwayAllExitFlag);
//	BOOL GetRunwayAllExitFlag();
//	void SetRunwayExitName(CString& strRunwayExitName);
//	CString GetRunwayExitName();
//	void SetStandFamilyName(CString& strStandFamilyName);
//	CString GetStandFamilyName();
//	void SetStand(int nStandGroupID);
//
//public:
//	afx_msg void OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnTvnSelchangedTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedOk();
//
//private:
//	CTreeCtrl m_TreeCtrlRunwayExit;
//	CTreeCtrl m_TreeCtrlStandFamily;
//
//	CString   m_strRunwayExit;
//	CString   m_strStandFamily;
//	int         m_nProjID;
//	int         m_nSelRunwayID;
//	RUNWAY_MARK m_nSelRunwayMarking;
//	int         m_nSelRunwayExitID;
//    int         m_nSelStandFamilyID;
//	BOOL        m_bRunwayAll;
//	Stand       *m_pSelStand;
};
