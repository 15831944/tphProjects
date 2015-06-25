#if !defined(AFX_PASSENGERTYPEDIALOG_H__7F404B7E_5F66_4884_9129_5577027CEC2A__INCLUDED_)
#define AFX_PASSENGERTYPEDIALOG_H__7F404B7E_5F66_4884_9129_5577027CEC2A__INCLUDED_

#include "..\Inputs\pax_cnst.h"
#include "..\Inputs\MobileElemConstraint.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassengerTypeDialog.h : header file
//
#include "UnselectableListBox.h"
#include "UnselectableTreeCtrl.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "DlgSelMultiPaxType.h"

//#define LEVELS              4
#define SELECTED_TEXT_LEN   1024

/////////////////////////////////////////////////////////////////////////////
// CPassengerTypeDialog dialog

class CPassengerTypeDialog : public CDialog
{
// Construction
public:
	CPassengerTypeDialog(CWnd* pParent,BOOL _bOnlyPax = FALSE, BOOL bNoDefault = FALSE);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPassengerTypeDialog)
	enum { IDD = IDD_DIALOG_PASSENGERTYPE };
	CButton	m_CheckAllType;
	CListCtrlEx	m_listPassenger;
	CTreeCtrl	m_treePaxType;
	CUnselectableListBox	m_listAirlineGroup;
	CUnselectableTreeCtrl	m_treeAirlineGroup;
	CButton	m_butEdit;
	CComboBox	m_comboMobileElem;
	CUnselectableTreeCtrl	m_AirlineTree;
	CTreeCtrl	m_PaxTypeTree;
	CUnselectableListBox	m_Sector;
	CUnselectableListBox	m_Category;
	CUnselectableListBox	m_Airport;
	CUnselectableListBox	m_ACType;
	CString	m_strPaxType;
	CButton		m_butOtherFlight;
	BOOL	m_bAllBranchesInType;
	BOOL m_bDisableDefault;
	//}}AFX_DATA

public:
	CToolBar m_wndToolBar;
	void Setup(const CMobileElemConstraint &MobileSelection);
	CMobileElemConstraint GetMobileSelection()			{ return m_MobileElem; }


	void SetShowFlightIDFlag( BOOL _bShow) { m_bShowFlightID = _bShow;	}
	
	BOOL	m_bNewVersion;
	void SetTerminal(Terminal* pTerminal){ m_pTerminal = pTerminal;}

protected:
	
	void InsertTreeItem(HTREEITEM hItem, int nLevel);
	virtual void ResetSeletion();

	virtual void LoadPassengerTypeTree();

	void ResetPaxType();

	int m_nConstraintType;		//
	bool bOnlyPax;

	std::map<HTREEITEM,int> m_mapLevelItem;
private:
	Terminal *m_pTerminal;
	bool m_bIsNewItem;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassengerTypeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	void InitRadio();
	void LoadCombo();
	void EnableToolbarButtonByRules();
	InputTerminal* GetInputTerminal();
	//PassengerConstraint m_PaxSelection;
	CMobileElemConstraint m_MobileElem;
	BOOL	m_bShowFlightID;		
	// Generated message map functions
	//{{AFX_MSG(CPassengerTypeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListActypePxtd();
	afx_msg void OnSelchangeListAirportPxtd();
	afx_msg void OnSelchangeListCategoryPxtd();
	afx_msg void OnSelchangeListSectorPxtd();
	afx_msg void OnSelchangedTreeAirline(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreePassengertype(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioAll();
	afx_msg void OnRadioArriving();
	afx_msg void OnRadioDeparting();
	afx_msg void OnCcancel();
	afx_msg LRESULT OnUnselectableListBoxSelchange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUnselectableTreeSelchange( WPARAM wParam, LPARAM lParam );
	afx_msg void OnButEditNonpax();
	afx_msg void OnRadioPax();
	afx_msg void OnRadioNonpax();
	afx_msg void OnSelchangeComboNopax();
	afx_msg void OnSelchangeListAirlineGroup();
	afx_msg void OnButtonOtherflight();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemchangedListPassenger(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioAlltype();
	afx_msg void OnDblclkListPassenger(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckAlltype();
	//}}AFX_MSG
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
private:
	int ReloadNonpaxCount();
	void ModifyNopaxCountItem( int _row, int _col, const CString& _strItem );
	void AddNopaxCountItem(const CString& _strNonPax);
	void ControlFlightRadio();
	void SelchangedTreePassengertype(HTREEITEM& hSelItem);
	void DeleteAllChildren(HTREEITEM hItem);
	
public:
	afx_msg void OnClickStopOverAirport();
private:
    DlgSelMultiPaxType  m_dMultiSel;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSENGERTYPEDIALOG_H__7F404B7E_5F66_4884_9129_5577027CEC2A__INCLUDED_)
