#pragma once
#include "NodeViewDbClickHandler.h"
#include "common/elaptime.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/XTResizeDialog.h"


class DepartureSlotSepcifications;
class AIRSIDEGUI_API CDlgDepartureSlotSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDepartureSlotSpec)

public:
	CDlgDepartureSlotSpec(int nProjID);
	CDlgDepartureSlotSpec(int nProjID, PSelectFlightType pSelectFlightType,  CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent = NULL);
	virtual ~CDlgDepartureSlotSpec();

protected:
	void UpdateUIState();
	void InitListCtrl();
	void SetListContent();
	int GetSelectedListItem();
protected:
	CToolBar					m_wndFltToolbar;
	CListCtrlEx					m_wndListCtrl;

	int							m_nProjID;
	PSelectFlightType			m_pSelectFlightType;
	CAirportDatabase			*m_pAirportDB;
	DepartureSlotSepcifications *m_pDepSlotSpec;
	std::vector<int>   m_vrSID;
	InputAirside* m_pInputAirside;
	PSelectProbDistEntry m_pSelectProbDistEntry;

	int m_nRowSel;
	int m_nColumnSel;

// Dialog Data
	enum { IDD = IDD_DIALOG_DEPARTURESLOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewDepartureSlotItem();
	afx_msg void OnDelDepartureSlotItem();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnSelComboBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDatetimechangeTimeStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeTimeEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDbClickListItem( WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnEndDbClickNoEditListItem( WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnKillFocusComboBoxOfCtrlList( WPARAM wparam, LPARAM lparam);
public:

};
