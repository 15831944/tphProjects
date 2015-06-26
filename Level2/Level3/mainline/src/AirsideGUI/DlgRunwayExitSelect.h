#pragma once
//#include "afxcmn.h"
#include "Resource.h"
#include "../InputAirside/RunwayExit.h"
#include "../InputAirside/Runway.h"
#include "../MFCExControl/XTResizeDialog.h"
#include "MuliSelTreeCtrlEx.h"


// CDlgRunwayExitSelect

class CDlgRunwayExitSelect : public CXTResizeDialog
{

	enum RUNWAYEXITTYPE
	{
		RUNWAYEXIT_DEFAULT = 0,
		RUNWAYEXIT_ALLRUNWAY,
		RUNWAYEXIT_LOGICRUNWAY,
		RUNWAYEXIT_ITEM
	};

	DECLARE_DYNAMIC(CDlgRunwayExitSelect)

	enum { IDD=IDD_DIALOG_OCCUPIEDASSIGNEDSTAND_SELECT };

public:
	CDlgRunwayExitSelect(const UINT nID,int nProjID,const CString& strSelExits, CWnd* pParent = NULL);
	virtual ~CDlgRunwayExitSelect();

 	CString GetRunwayExitName();
 	BOOL IsAllRunwayExit();
	void SetRunwayExitTreeContent(void);
	std::vector<int>& GetRunwayExitList();
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult);
	RUNWAYEXITTYPE GetRunwayExitType(HTREEITEM hRunwayTreeItem);


	void GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName);
	bool IsExitSelected(const CString& strExitName);
	virtual void OnOK();


	CMuliSeltTreeCtrlEx m_TreeCtrlRunwayExit;
	int m_nProjID;
	bool m_bAllRunwayExit;
	CString m_strRunwayExits;
	std::vector<int> m_vExitSelID;
	

};


