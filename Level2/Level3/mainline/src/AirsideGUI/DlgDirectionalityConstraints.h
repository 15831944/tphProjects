#pragma once
#include "../MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/ComboBoxListBox.h"
#include "../InputAirside/DirectionalityConstraints.h"
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/ALTAirport.h"
#include "../MFCExControl/XTResizeDialog.h"
#include <map>
#include <algorithm>

class CDirConsListCtrlEx : public CListCtrlEx
{
public:
	CDirConsListCtrlEx();
	~CDirConsListCtrlEx();

public:
	void SetAirportID(int nAirportID){ m_nAirportID = nAirportID; }
	void SetProjectID(int nProjID){ m_nProjID = nProjID; }

	LRESULT OnComboBoxChange(WPARAM wParam, LPARAM lParam);

	void LoadTaxiwayInfo();
protected:
	void ResetDropDownListContext(DirectionalityConstraintItem* pItem);

	void ResetTaxiwayRelativeColnum(int nItem, DirectionalityConstraintItem* pItem);
	void ResetSegIDRelativeColnum(int nItem, DirectionalityConstraintItem* pItem);
	void ResetItemSegContext(int nItem, DirectionalityConstraintItem* pItem);
	std::map<DirectionalityConstraintItem*, BOOL> m_mapModifyHistory;


	int m_nProjID;
	int m_nAirportID;

	CStringList				m_taxiwayStrList;
	CStringList				m_nodeStrList;
	CStringList				m_bearingStrList;

	CStringArray			m_strArrayType;

	std::map<CString, int>	m_mapTaxiway;
	std::map<CString, int>	m_mapNodeList;

	ALTAirport				m_airport;
	std::vector<int>		m_vectTaxiwayID;

	IntersectionNodeList	m_intersecNodeList;
	void GetNodeListByTaxiwayID( int nTaxiwayID);
	BOOL IfExist(int nTaxiwayID);
	void CalculateBearing(DirectionalityConstraintItem* pItem);

public:
DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

};

class CDlgDirectionalityConstraints : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDirectionalityConstraints)

public:
	CDlgDirectionalityConstraints(int nProjID, int m_nAirportID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDirectionalityConstraints();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIRECTIONALITYCONSTRAINTS };
protected:
	void UpdateUIState();
	void InitListControl();
	void ResetListContent();


protected:
	int						m_nProjID;
	int						m_nAirportID;

	CToolBar				m_wndToolBar;
	CDirConsListCtrlEx		m_wndListCtrl;
	CStringArray			m_strArrayType;

	ALTAirport				m_airport;
	std::vector<int>		m_vectTaxiwayID;
	DirectionalityConstraints* m_pDirectionalityConstraints;
	CString GetNodeNameString(Taxiway& taxiway, int nNodeID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void InitToolbar();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();

	afx_msg void OnNewItem();
	afx_msg void OnDelItem();

	afx_msg void OnLvnEndLabelEditList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemChangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	//afx_msg void OnNMRdblclkListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnComboBoxChange(WPARAM wParam, LPARAM lParam);
};
