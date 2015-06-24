#pragma once

#include "../MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/XTResizeDialog.h"

class InputTerminal;
// CDlgLandsideVehicleAssignment dialog
class LandsideVehicleAssignContainer;
class InputLandside;
class LandsideVehicleAssignEntry;

class CDlgLandsideVehicleAssignment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLandsideVehicleAssignment)

public:
	CDlgLandsideVehicleAssignment(InputLandside* pInputLand,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideVehicleAssignment();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLEASSIGNMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	void OnInitToolbar();
	void UpdateToolBarState();
	void InitListControl();
	void SetListContent();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()

	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDbClickListCtrl(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnSave();

	LandsideVehicleAssignContainer& getAssignData();
	void GetProbDropdownList( CStringList& strList );
	void SetModified( BOOL bModified = TRUE );
	void InsertItemToList( LandsideVehicleAssignEntry* pVehicleEntry );
	void UpdateItem( int nItemIndex, LandsideVehicleAssignEntry* pVehicleEntry );
	void UpdateListStyle();
private:
	CListCtrlEx    m_wndListCtrl;
	CToolBar       m_wndToolBar;

	InputLandside*	m_pInLandside;
	LandsideVehicleAssignContainer* m_vehicleAssignment;

	int m_nEditItemForOleTime;
};
