#pragma once
#include "afxwin.h"
#include "../MFCExControl/ListCtrlEx.h"
// CDlgWingspanAdjacancyConstraints dialog
#include "InputAirside\WingspanAdjacencyConstraints.h"
class Taxiway;

class CDlgWingspanAdjacancyConstraints : public CDialog
{
	DECLARE_DYNAMIC(CDlgWingspanAdjacancyConstraints)

public:
	CDlgWingspanAdjacancyConstraints(int nProjectID ,CAirportDatabase* pAirportDB,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWingspanAdjacancyConstraints();

// Dialog Data
	//enum { IDD = IDD_DIALOG_WINGSPAN_ADJCONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);


	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnCmdEditItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSave();
	afx_msg LRESULT OnCollumnIndex(WPARAM wParam,  LPARAM lParam);
	//comes from list control
	//notify that the combox lost its focus
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);


protected:
	//resize the control
	void RefreshLayout(int cx = -1, int cy = -1);

	//update toolbar state, enable or gray
	void UpdateToolBarState();
	//initialize the list control, column
	void InitListControl();
	
	//insert a item to list
	void AddConstraintItem(CWingspanAdjacencyConstraints* conItem);

	//refresh the item's content
	void SetListItemContent(int nIndex, CWingspanAdjacencyConstraints& conItem);

	//load data to fill listctrl 
	void FillListControl();

protected:
	CToolBar		m_wndToolBar;
	CListCtrlEx		m_wndListCtrl;

	int m_nProjID;

	//taxiway, it should come from InputAirside,
	std::vector<Taxiway *> m_vTaxiway;
	//the data
	CWingspanAdjacencyConstraintsList m_vData;

	CAirportDatabase* m_pAirportDB;
protected:
	virtual void OnOK();
	virtual void OnCancel();
	void GetAllTaxiway();
	int GetSelectedListItem();
	//return the taxiway's node
	void GetNodeListByTaxiway(Taxiway& pTaxiway, CStringList& lstNode);
};
