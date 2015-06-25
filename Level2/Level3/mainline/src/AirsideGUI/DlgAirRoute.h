#pragma once

// CDlgAirRoute dialog
#include "../InputAirside/AirRoute.h"
#include "../MFCExControl/EditListBox.h"
#include "../MFCExControl/XTResizeDialog.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
namespace AirsideGUI
{
class CDlgAirRoute : public CXTResizeDialog ,public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgAirRoute)

public:
	CDlgAirRoute(BOOL bEditAirRoute,int nAirspaceID,int nAirRouteID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirRoute();
	CAirRoute* m_pCurAirRoute;
// Dialog Data
	enum { IDD = IDD_DIALOG_AIRROUTEDEFINE };

protected:
	int m_nAirspaceID;
	AirRouteList m_airRoutelst;
	int m_nAirRouteID;
	BOOL m_bEidtAirRoute;

	AirRouteList m_delAirRoutelst;
	std::vector<ARWaypoint *> m_vDelARWaypoint;

	std::vector<AirWayPoint> m_vWayPoint;
	std::vector<LogicRunway_Pair> m_vConnectRunway;


	CToolBar m_toolBaWayPoint;
	CToolBar m_toolBarRunway;

	CListCtrl m_lstWaypoint;
	CListBox m_lstRunway;

	CComboBox m_cmbType;
	//CComboBox m_cmbRunway;
	//CComboBox m_cmbRunwayMark;
	CString   m_strAirRouteName;

protected:
	void InitProperties(CAirRoute *pAirRoute);
	//void SelectRunwayComboboxByRunwayID(int nRunwayID);
	void InitARWaypointList();
	int InsertARWaypointIntoList(ARWaypoint * waypoint, int nRowNum);
	void InitDefaultProperties(); 
	
	void SaveProPerties(CAirRoute *pAirRoute);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void AdjustARWayPointToolBarState();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	void InitNewAirRoute();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnNewARWaypoint();
	afx_msg void OnEditARWaypoint();
	afx_msg void OnDelARWaypoint();
	afx_msg void OnInsertARWaypoint();

	afx_msg void OnCbnSelchangeComboType();

	afx_msg void OnNewRunway();
	afx_msg void OnDelRunway();

	//afx_msg void OnCbnSelchangeComboRunway();
	afx_msg LRESULT OnMessageNewAirRoute( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLvnItemchangedARWayPoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkARWayPointList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillEidtFocus();
	afx_msg void OnLbnSetfocusListRunway();
	afx_msg void OnLbnKillfocusListRunway();
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	void setProjID(int nAirRouteID){ m_nAirRouteID = nAirRouteID;}
	int  getProjID() { return m_nAirRouteID;}
};
}