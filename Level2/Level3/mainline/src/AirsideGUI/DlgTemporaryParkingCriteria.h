#pragma once
#include "afxwin.h"
#include "../MFCExControl/ComboBoxListBox.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/HoldPosition.h"
#include "../InputAirside/TemporaryParkingCriteria.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgTemporaryParkingCriteria dialog

class CDlgTemporaryParkingCriteria : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTemporaryParkingCriteria)

public:
	CDlgTemporaryParkingCriteria(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTemporaryParkingCriteria();

// Dialog Data
	enum { IDD = IDD_DIALOG_TEMPORARY_PARKING_CRITERIA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	int m_nProjID;
	int m_nTaxiwayCount;
	int m_nCurNodeCount;
	std::vector<int> m_vTaxiwaysIDs;
	std::vector<IntersectionNode> m_vNodeListAll;
	std::vector<IntersectionNode> m_vNodeList;
	CStringList m_strListTaxiway;
	CStringList m_strListNode;
	CStringList m_strListOrientation;
	CStringList m_strListEntryNode;

	CTemporaryParkingCriteriaList* m_pTempParkingCriteriaList;
	CTemporaryParkingCriteria* pCurTaxiwayItem;
	int m_nCurFirtNodeID;

	BOOL m_bSameTaxiway;


	CToolBar m_taxiwayToolBar;
	CToolBar m_dataToolBar;
	CListCtrlEx m_listData;
	CComboBoxListBox m_listTaxiway;	

public:
	BOOL CreateToolBar();
	void InitToolBar();
	void InitListCtrlEx();
	void SetListCtrlEx();
	void LoadData();
	void GetAllTaxiway();
	void UpdateToolBar();
	void GetNodeListByTaxiwayID(int nTaxiwayID);
	void GetEntryNodeListByTaxiwayID(int nTaxiwayID);
	void SetCurNodList();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnNewTaxiway();
	afx_msg void OnDelTaxiway();
	afx_msg void OnNewData();
	afx_msg void OnDelData();
	afx_msg void OnLbnSelchangeListTaxiway();
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMsgComboKillfocus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDbClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
