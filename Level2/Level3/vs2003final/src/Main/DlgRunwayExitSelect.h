#pragma once
#include "afxcmn.h"

#include "../InputAirside/RunwayExit.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Taxiway.h"
// CDlgRunwayExitSelect dialog
class CSelectData
{
public:
	int m_RunwayID ;
	int m_ExitID ;
	CString m_RunwayName ;
	CString m_ExitName ;
	CSelectData():m_RunwayID(-1),m_ExitID(-1) {} ;
};

class CDlgRunwayExitSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgRunwayExitSelect)
public:
	CDlgRunwayExitSelect(std::vector<ALTObject>* _RunwayData , bool bSelectOneExit = false, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRunwayExitSelect();

	int GetSelectRunwayID() { return m_RunwayID ;} ;
	CString GetSelectRunwayName() { return m_RunwayName ;} ;
	std::vector<CSelectData>* GetSelectExitName() { return &m_SelectExit ;}

// Dialog Data
	enum { IDD = IDD_DIALOG_RUNWAYEXIT_SEL };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK() ;
	virtual BOOL OnInitDialog() ;

	void SetRunwayExitTreeContent(void) ;
	void GetRunwayExitName(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName) ;
	void InitTreeCtrlData() ;
	void AddExitItemNode(HTREEITEM _ItemNode) ;

	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrl m_treeCtrl;
	CTreeCtrl m_TreeCtrl ;

	CString m_RunwayName ;
	bool m_bSelectOneExit;
	int m_RunwayID ;
	int m_nProjID ;
	std::vector<ALTObject>* m_RunwayData ;
	std::vector<CSelectData> m_SelectExit ;
};
