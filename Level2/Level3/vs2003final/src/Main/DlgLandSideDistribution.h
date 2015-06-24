#pragma once
#include "PrintableTreeCtrl.h"
#include "afxwin.h"
#include "../Landside/VehicleDistribtionList.h"
#include "../Landside/VehicleDistribution.h"
#include "../landside/VehicleDistributionDetail.h"
#include "TermPlanDoc.h"
// DlgLandSideDistribution dialog

class DlgLandSideDistribution : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgLandSideDistribution)

public:
	DlgLandSideDistribution(CWnd* pParent = NULL);   // standard constructor
	DlgLandSideDistribution(int tmpProjID,CTermPlanDoc *tmpDoc,CWnd*pParent=NULL);
	virtual ~DlgLandSideDistribution();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEDISTRIBUTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeListPax();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnVehicleDistributionEditSpin();
	afx_msg void OnVehicledistEvenRemainPercent();
	afx_msg void OnVehicledistEvenpercent();
	void OnVehicledistModifypercent() ;
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonSave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAllBranchesPercentEdit(UINT nID);
	afx_msg LRESULT OnEndEditPercent( WPARAM p_wParam, LPARAM p_lParam );//  [5/6/2004]
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
  	afx_msg void OnPeoplemoverChange();//--c
	afx_msg void OnPrintDist();
	afx_msg void OnCancelMode();
	DECLARE_MESSAGE_MAP()
protected:
	CString getItemLabel(const CString& _csName,int _nPercent);
	void ReLoadTree();
	void InitToolBar();
	void SetPercent(int _nPercent);
	void EvenRemainPercentCurrentLevel();
	BOOL CheckPercentFull( HTREEITEM _hItem );
	void EvenPercentCurrentLevel();
	void ModifyPercentAllBranch(int _nNewPercent);
	void ReloadPaxList();
	InputTerminal*GetInputTerminal();
	CString GetProjPath();
	virtual void OnOK();
	virtual void OnCancel();
public:
	CPrintableTreeCtrl m_treeDist;
// 	CConDBListCtrl	m_listctrlData;
	CButton m_btnSave;
	CButton m_bntOk;
	CButton m_btnCancel;
	CListBox m_listboxPax;
	/*CStatic m_toolbarcontenter2;
	CStatic m_toolbarcontenter;*/
	CStatic m_stcFrame2;
	CStatic m_stcFrame;
	CToolBar m_ToolBar;
	CToolBar m_TreeToolBar;
	int m_pProjID;
	int m_nCurPaxListSel;
	HTREEITEM m_hRClickItem;
	int m_nAllBranchesFlag;
	CTermPlanDoc *pDoc;
	bool m_bSized;
	bool m_bOnChangePax;
	
};
