#pragma once
#include "afxwin.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/TaxiwayConstraint.h"
#include "../MFCExControl/ComboBoxListBox.h"
#include "../InputAirside/HoldPosition.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgTaxiwayConstraint dialog
class CAirportDatabase;
class CDlgTaxiwayConstraint : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTaxiwayConstraint)

public:
	CDlgTaxiwayConstraint(int nProjID, TaxiwayConstraintType emType,
		CAirportDatabase* pAirportDB,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaxiwayConstraint();

// Dialog Data
	enum { IDD = IDD_DIALOG_TAXIWAY_CONSTRAINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	int m_nProjID;
	TaxiwayConstraintType m_emType;
	int m_nTaxiwayCount;
	int m_nCurNodeCount;
	std::vector<int> m_vTaxiwaysIDs;
	std::vector<IntersectionNode> m_vNodeList;
	CStringList m_strListTaxiway;
	CStringList m_strListNode;

	CTaxiwayConstraintList* m_pTaxiwayConstraintList;
	CTaxiwayConstraint* pCurTaxiwayItem;
	int m_nCurFirtNodeID;

	BOOL m_bSameTaxiway;


	CToolBar m_taxiwayToolBar;
	CToolBar m_dataToolBar;
	CListCtrlEx m_listData;
	CComboBoxListBox m_listTaxiway;	
	CAirportDatabase* m_pAirportDB;

public:
	BOOL CreateToolBar();
	void InitToolBar();
	void InitListCtrlEx();
	void SetListCtrlEx();
	void LoadData();
	void GetAllTaxiway();
	void UpdateToolBar();
	void GetNodeListByTaxiwayID(int nTaxiwayID);
	void SetCurNodList();

public:	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnNewTaxiway();
	afx_msg void OnDelTaxiway();
	afx_msg void OnNewData();
	afx_msg void OnDelData();
	afx_msg void OnBnClickedButtonSave();
	//only winspan
	afx_msg void OnBnClickedButtonAdjcancyConstraints();
	afx_msg void OnLbnSelchangeListTaxiway();
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMsgComboKillfocus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
