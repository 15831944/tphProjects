#pragma once
#include <Landside/VehicleGroupProperty.h>
#include <MFCExControl/ListCtrlEx.h>
#include "../AirsideGUI/UnitPiece.h"
class InputLandside;
// CDlgLandsideVehicleGroupProperty dialog

class CDlgLandsideVehicleGroupProperty : public CXTResizeDialog , public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgLandsideVehicleGroupProperty)

public:
	CDlgLandsideVehicleGroupProperty(InputLandside* pInput,int prjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideVehicleGroupProperty();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLEGROUPPROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	void OnInitToolbar();
	void UpdateToolBarState();
	void InitListControl();
	void SetListContent();
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	InputLandside* m_pInputLandside;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDoubleClick(WPARAM wParam,  LPARAM lParam);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBtnSave();
private:
	CToolBar    m_wndToolBar;
	CListCtrlEx m_wndListCtrl;

	LandsideVehicleGroupProperty* m_landsideVehicleGroupProperties;
	int m_nProjID;
	
};
