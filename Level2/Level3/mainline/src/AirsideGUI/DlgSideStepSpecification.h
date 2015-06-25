#pragma once
#include "afxcmn.h"
#include "..\MFCExControl\CoolTree.h"
#include "NodeViewDbClickHandler.h"
#include "../InputAirside/SideStepSpecification.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgSideStepSpecification dialog
class CAirportDatabase;
class CDlgSideStepSpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSideStepSpecification)

public:
	CDlgSideStepSpecification(int nProjID,PSelectFlightType pSelectFlightType,CAirportDatabase* pAirportDB,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSideStepSpecification();

// Dialog Data
	enum { IDD = IDD_SIDESTEPCRITERIA};

private:
	int m_nProjID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_wndTreeFltRunway;
	CCoolTree m_wndTreeRunwayData;
	CToolBar	m_wndFltRunwayToolbar;
	CToolBar	m_wndRunwayDataToolbar;
	CSideStepSpecification m_SideStepSpecification;
	PSelectFlightType	m_pSelectFlightType;
	CAirportDatabase* m_pAirportDB;
	afx_msg void OnNewFlightType();
	afx_msg void OnDelFlightType();
	afx_msg void OnEditFlightType();
	afx_msg void OnNewApprochRunway();
	afx_msg void OnDelApprochRunway();
	afx_msg void OnEditApprochRunway();
	afx_msg void OnNewToRunway();
	afx_msg void OnDelToRunway();
	afx_msg void OnEditToRunway();
	void InitSideStepToolBar();
	void UpdateToolBar();
	void InitFltRunwayTree();
	void InitRunwayDataTree();

	//add by cj
	void UpdateRunwayTree();
public:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void OnContextMenuForRunwayDate(CWnd* pWnd, CPoint& point);
	afx_msg void OnTvnSelchangedTreeFltrunway(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTvnSelchangedTreeRunwaydata(NMHDR *pNMHDR, LRESULT *pResult); 
};
