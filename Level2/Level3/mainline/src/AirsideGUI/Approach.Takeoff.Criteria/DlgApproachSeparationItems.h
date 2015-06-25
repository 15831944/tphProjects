#pragma once

#include "../../MFCExControl/TabPageSSL.h"
#include "../../MFCExControl/ListCtrlExExEx.h"
#include "afxcmn.h"
#include "../../InputAirside/ApproachSeparationCirteria.h"
#include "../../MFCExControl/ListCtrlEx.h"
#include "../../common/ProbDistManager.h"
#include "../../common/AirportDatabase.h"
#include "../../common/ProbabilityDistribution.h"
#include "../../InputAirside/InputAirside.h"
#include "../NodeViewDbClickHandler.h"
#include "../Resource.h"
#include "../../InputAirside/ARCUnitManager.h"
// CDlgApproachSeparationItems dialog

class CDlgApproachSeparationItems : public CTabPageSSL
{
	DECLARE_DYNAMIC(CDlgApproachSeparationItems)

public:
	CDlgApproachSeparationItems(int nApproachTypeListIdx, InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgApproachSeparationItems();

public:
	void InitApproachListCtrl();
	void InitBasisListCtrl();
	void SetApproachSeparationCriteria(CApproachSeparationCriteria* pData);

	CListCtrlExExEx m_wndListCtrlApproach;
	CListCtrlExExEx m_wndListCtrlBasis;
	CToolBar  m_wndToolbar;
protected:
	void SetApproachListCtrlContent(void);
	void SetBasisListCtrlContent(FlightClassificationBasisType emClassType);
protected:
	CApproachSeparationCriteria* m_pAppSepCriteria;
	CApproachTypeList * m_pApproachData;

	CApproachRunwayNode* m_pCurNode;
	CApproachSeparationItem * m_pCurItem;

	InputAirside* m_pInputAirside;
	PSelectProbDistEntry m_pSelectProbDistEntry;

public:
// Dialog Data
	enum { IDD = IDD_DIALOG_APPROACHSEPARATIONITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnNewItem(void);
	afx_msg void OnDelItem(void);
	afx_msg void OnEditItem(void);
	afx_msg void OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnNMClickListApproach(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListBasis(NMHDR *pNMHDR, LRESULT *pResult);

private:
	CString m_strListColumnHeader_LandingThres;
	CString m_strListColumnHeader_LandedThres;

	CARCUnitManager& m_unitManager;
	int m_nApproachTypeListIdx;
};
