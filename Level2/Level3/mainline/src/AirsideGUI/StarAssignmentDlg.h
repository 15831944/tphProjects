#pragma once
#include "NodeViewDbClickHandler.h"
#include "MFCExControl/ListCtrlEx.h"
#include <iostream>
#include <vector>
#include "MFCExControl/XTResizeDialog.h"

// CStarAssignmentDlg dialog
//class CLandingRunwayStarAssignmentList;
//class CLandingRunwayStarAssignment;
//class CFlightUseStarList;
//class CFlightUseStar;
class FlightStarAssignmentList;
class StarPercentage;
class FlightStarAssignment;

using namespace std;

enum TREEITEMCATE
{
	TREEITEMCATE_NONE           = 0,
	TREEITEMCATE_FLIGHTTYPE,
	TREEITEMCATE_TIME
};

typedef pair <int, int> Int_Pair;

class CStarAssignmentDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CStarAssignmentDlg)

public:
	CStarAssignmentDlg(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent = NULL);   // standard constructor
	virtual ~CStarAssignmentDlg();


private:
	void InitToolBar(void);
	void UpdateToolBar(void);
	void InitFlightTypeTree(void);
	void InitListCtrl(void);
	void SetListContent(void);
	StarPercentage* GetCurFlightUseStar(void);
	TREEITEMCATE GetCurSeleltTreeItemState(HTREEITEM hTreeItem);
	//void DeleteDataWhichStarIsNotExist();
	//void DeleteDataWhichRunwayIsNotExist();
	void SaveData();
	bool IsvalidTimeRange(ElapsedTime tStart, ElapsedTime tEnd); 

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog(); 
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCmdNew();
	afx_msg void OnCmdDelete();
	afx_msg void OnCmdEdit();

	afx_msg void OnAddTimeRange();
	afx_msg void OnEditTimeRange();
	afx_msg void OnDelTimeRange();

	afx_msg void OnCmdNewStarAssignment();
	afx_msg void OnCmdDeleteStarAssignment();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnTvnSelchangedTreeFlightType(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedListStarassignment(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListStarassignment(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListStarassignment(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

private:
	CStatic     m_StaticToolBarTime;
	CStatic     m_StaticToolBarStarAssignment;
	CToolBar    m_ToolBarTime;
	CToolBar    m_ToolBarStarAssignment;
	CTreeCtrl   m_wndTreeFltTime;
	CListCtrlEx	m_StarAssigmentCtrl;

	PSelectFlightType                m_pSelectFlightType;
	//CLandingRunwayStarAssignmentList *m_pLandingRunwayStarAssignmentList;
	//CLandingRunwayStarAssignment     *m_pCurLandingRunwayStarAssignment;
	//CFlightUseStarList               *m_pCurFlightUseStartList;
	//CFlightUseStar                   *m_pCurFlightUseStar;
	FlightStarAssignmentList* m_pStarAssignmentList;
	int                              m_nProjID;
	TREEITEMCATE                     m_CurrentTreeItemCategory;
	CStringList                      m_StarNameList;
	map<int,int>                     m_AirRouteID;
	std::vector<FlightStarAssignment*>	m_vFlightTypeDelList;
	CAirportDatabase*	m_pAirportDB;
};
