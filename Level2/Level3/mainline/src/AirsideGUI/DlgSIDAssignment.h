#pragma once
#include "NodeViewDbClickHandler.h"
#include "MFCExControl/ListCtrlEx.h"
#include <iostream>
#include "../MFCExControl/XTResizeDialog.h"


// CDlgSIDAssignment dialog
//class CLandingRunwaySIDAssignmentList;
//class CLandingRunwaySIDAssignment;
//class CFlightUseSIDList;
//class CFlightUseSID;
class FlightSIDAssignmentList;
class SIDPercentage;
class FlightSIDAssignment;

using namespace std;

enum TREEITEMCATE;

typedef pair <int, int> Int_Pair;

class CDlgSIDAssignment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSIDAssignment)

public:
	CDlgSIDAssignment(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSIDAssignment();

// Dialog Data
	enum { IDD = IDD_DIALOG_SIDASSIGNMENT };

private:
	void InitToolBar(void);
	void UpdateToolBar(void);
	void InitFlightTypeTree(void);
	void InitListCtrl(void);
	void SetListContent(void);
	SIDPercentage* GetCurFlightUseSID(void);
	TREEITEMCATE GetCurSeleltTreeItemState(HTREEITEM hTreeItem);
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

	afx_msg void OnCmdNewSIDAssignment();
	afx_msg void OnCmdDeleteSIDAssignment();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonSave();

	afx_msg void OnTvnSelchangedTreeFlightType(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedListSIDAssignment(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListSIDAssignment(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListSIDAssignment(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
private:
	CStatic     m_StaticToolBarTime;
	CStatic     m_StaticToolBarSIDAssignment;
	CToolBar    m_ToolBarTime;
	CToolBar    m_ToolBarSIDAssignment;
	CTreeCtrl   m_wndTreeFltTime;
	CListCtrlEx	m_SIDAssigmentCtrl;

	PSelectFlightType                m_pSelectFlightType;
	//CLandingRunwaySIDAssignmentList *m_pLandingRunwaySIDAssignmentList;
	//CLandingRunwaySIDAssignment     *m_pCurLandingRunwaySIDAssignment;
	//CFlightUseSIDList               *m_pCurFlightUseSIDList;
	//CFlightUseSID                   *m_pCurFlightUseSID;
	FlightSIDAssignmentList*  m_pSIDAssignmentList;
	int                              m_nProjID;
	TREEITEMCATE                     m_CurrentTreeItemCategory;
	CStringList                      m_SIDNameList;
	map<int,int>                     m_AirRouteID;
	std::vector<FlightSIDAssignment*>	m_vFlightTypeDelList;
	CAirportDatabase*	m_pAirportDB;
};
