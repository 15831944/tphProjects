#pragma once
#include "../MFCExControl/TabPageSSL.h"
#include "../MFCExControl/ListCtrlExExEx.h"
#include "afxcmn.h"
#include "../InputAirside/TakeoffClearanceCriteria.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../common/ProbDistManager.h"
#include "../common/AirportDatabase.h"
#include "../common/ProbabilityDistribution.h"
#include "../InputAirside/InputAirside.h"
#include "NodeViewDbClickHandler.h"
#include "./InputAirside/ARCUnitManager.h"

// CDlgTakeoffClearanceItems dialog

class CDlgTakeoffClearanceItems : public CTabPageSSL
{
	DECLARE_DYNAMIC(CDlgTakeoffClearanceItems)

public:
	CDlgTakeoffClearanceItems(int nClearanceTypeListIdx,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTakeoffClearanceItems();

// Dialog Data
	enum { IDD = IDD_DIALOG_TAKEOFFCLEARANCECRITERIA };

public:
	void InitApproachListCtrl();
	void InitBasisListCtrl();

	void SetTakeoffClearanceCriteria(CTakeoffClearanceCriteria* pData);

	CListCtrlExExEx m_wndListCtrlApproach;
	CListCtrlExExEx m_wndListCtrlBasis;
	CToolBar  m_wndToolbar;
protected:
	void SetApproachListCtrlContent(void);
	void SetBasisListCtrlContent(FlightClassificationBasisType emClassType);
protected:
	CTakeoffClearanceCriteria* m_pTakeoffClearanceCriteria;
	CTakeoffClearanceTypeList * m_pTakeoffClearanceData;

	CTakeoffClaaranceRunwayNode* m_pCurNode;
	CTakeoffClaaranceSeparationItem * m_pCurItem;

	InputAirside* m_pInputAirside;
	PSelectProbDistEntry m_pSelectProbDistEntry;
	int m_nClearanceTypeListIdx;
	CARCUnitManager& m_unitManager;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnNewItem(void);
	afx_msg void OnDelItem(void);
	afx_msg void OnEditItem(void);
	afx_msg void OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnNMClickListApproach(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListBasis(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_strLeadDistance;
	CString m_strAfterTime;
	CString m_strAppDistance;
	CString m_strAppTime;
	CString m_strDepartureTime;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEdit6();
};
