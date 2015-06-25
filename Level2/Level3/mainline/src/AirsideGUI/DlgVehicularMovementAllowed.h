#pragma once
#include "DialogResize.h"
#include "../MFCExControl/ListCtrlEx.h"
#include <vector>
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/VehicularMovementAllowed.h"
#include "../InputAirside/VehicleSpecifications.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgVehicularMovementAllowed dialog

//class CVehicleSpecifications;
class CDlgVehicularMovementAllowed : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehicularMovementAllowed)

public:
	CDlgVehicularMovementAllowed(int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicularMovementAllowed();

// Dialog Data
	//enum { IDD = IDD_DIALOG_VEHICULARMOVEMENTALLOWED };


protected:
	CTreeCtrl m_treeVehicle;
	CListCtrlEx m_lstTaxiway;
	CToolBar m_toolBar;

	int m_nProjID;
	std::vector<int> m_vTaxiwaysIDs;

	CStringList m_strListTaxiway;
	std::vector<IntersectionNode> m_vNodeList;
	CStringList m_strListNode;

	CVehicularMovementAllowed m_VehicularData;
	CVehicleSpecifications m_VehicleSpecifications;


	//std::map<int, std::vector<IntersectionNode> > m_mapTaxiwayNode;
	std::map<int, CString > m_mapTaxiwayName;
	std::map<int, std::vector<IntersectionNode> > m_mapTaxiwayNode;
	std::map<int, CString > m_mapNodeName;

	CVehicularMovementAllowedTypeItem*  m_pCurTypeItemData;
protected:
	void InitTreeCtrl();
	void InitListCtrl();
	void InitToolBar();

	void GetAllTaxiway();
	void GetNodeListByTaxiwayID(int nTaxiwayID);

	void AddTaxiwayIntoList();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	void UpdateToolbar();
	void DisableDialog();
	void EnableDialog();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewTaxiwayItem();
	afx_msg void OnDelTaxiwayItem();
	afx_msg void OnBnClickedCheckDiablesettings();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnTvnSelchangedTreeTypetree(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg LRESULT OnMsgComboKillfocus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLbnSelchangeListTaxiway(NMHDR *pNMHDR, LRESULT *pResult);;
	//afx_msg void OnTvnSelchangingTreeTypetree(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void OnOK();
	virtual void OnCancel();
};
