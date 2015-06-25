#pragma once
#include "NodeViewDbClickHandler.h"
#include "../InputAirside/CPushBackClearanceProcessor.h"
#include "afxcmn.h"
#include "DialogResize.h"
#include "../common/UnitsManager.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../common/flt_cnst.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgPushBackClearanceCriteria dialog

class CDlgPushBackClearanceCriteria : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPushBackClearanceCriteria)

public:
	CDlgPushBackClearanceCriteria(int nProjID,CUnitsManager* pGlobalUnits,PSelectFlightType	pSelectFlightType,CAirportDatabase *pAirportDatabase , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPushBackClearanceCriteria();

// Dialog Data
	enum { IDD = IDD_DIALOG_PUSHBACKCLEARANCECRITERIA };

protected:
	void InitializeTreeCtrl(void);
	void InitToolBar(void);
	void OnNewItem(void);
	void OnDeleteItem(void);
	//void OnEditItem(void);
	void RebuildList(HTREEITEM hSelItem);
	void ReadData(void);
	void SaveData(void);

	void UpdateSelItem(HTREEITEM hSelItem);
protected:
	CTreeCtrl m_wndConditionTree;
	CListCtrlEx m_wndCriteriaList;
	CToolBar m_wndToolBar;
	CPushBackClearanceProcessor m_criteriaProcessor;
	CUnitsManager * m_pGlobalUnits;
	PSelectFlightType	m_pSelectFlightType;
	CAirportDatabase *  m_pAirportDatabase ;
	int m_nProjID;
	HTREEITEM m_hTreeSelItem;
	double m_dDistancNoPushBackWithin;
	long m_lTimeNoLeadWithin;
	long m_lMaxOutboundFltToAssignedRunway;
	long m_lMaxOutboundFltToAllRunway;
	bool m_bChangeItself;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP() 
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedTreeCondition(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnEndlabeleditListCriteriaValue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
};
