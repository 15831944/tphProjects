#pragma once
#include "NodeViewDbClickHandler.h"
#include "../InputAirside/RunwayTakeOffPosition.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgRunwayTakeoffPosition dialog
class InputAirside;
//class CAirportDatabase;
class CDlgRunwayTakeoffPosition : public CXTResizeDialog
{
public:
	enum { IDD = IDD_RUNWAYTAKEOFFPOSITION };
public:
	CDlgRunwayTakeoffPosition(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CWnd* pParent = NULL);
	virtual ~CDlgRunwayTakeoffPosition();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	CTreeCtrl			m_wndTreeRunway;
	CTreeCtrl			m_wndTreeFltTimeRange;
	CARCTreeCtrl		m_wndTreeIntersection;
	CToolBar			m_wndToolbar;
	int					m_nProjID;
	PSelectFlightType	m_pSelectFlightType;
	
	int init;

	CButton	m_bEligible;
	double	m_nRemainFt;
	CEdit m_CtrlRemainFt;
	CSpinButtonCtrl m_spin;

	CRunwayTakeOffPosition m_runwayTakeoffPos;
//	CAirportDatabase* m_pAirportDB;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewFltTimeRange();
	afx_msg void OnDelFltTimeRange();
	afx_msg void OnEditFltTimeRange();
	afx_msg void OnTvnSelchangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNewFltType();
	afx_msg void OnDelFltType();
	afx_msg void OnEditFltType();
	afx_msg void OnMaxBackTrackOp();
	afx_msg void OnMinBackTrackOp();
	afx_msg void OnNoBackTrackOp();
	afx_msg void OnSave();
	void InitAirPortRunwayTreeCtrl();
	void InitRwyTakeoffPositions();
	void InitTimeRangeFltType();

	void AddFlightTypeToTree(CRunwayTakeOffTimeRangeFltType* pFltTypeItem,HTREEITEM hParentItem);
public:
	afx_msg void OnTvnSelchangedTreeRunway(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeTakeoffPosition(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckEligible();
	afx_msg void OnEnChangeEditRemaining();
};
