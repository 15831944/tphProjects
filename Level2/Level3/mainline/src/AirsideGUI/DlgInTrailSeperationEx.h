#pragma once


// CDlgInTrailSeperationEx dialog
//#include "../Engine/terminal.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\InputAirside\AircraftClassifications.h"
#include "..\InputAirside\InTrailSeperationEx.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/XTResizeDialog.h"

class CDlgInTrailSeperationEx : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgInTrailSeperationEx)

public:
	CDlgInTrailSeperationEx(int nProjID, Terminal* pTerminal,CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInTrailSeperationEx();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnToolAdd();
	afx_msg void OnBtnToolEdit();
	afx_msg void OnBtnToolDel();
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

// Dialog Data
	enum { IDD = IDD_DIALOG_IN_TRAIL_SEPARATIONEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void InitListCtrl();
	void InitCategoryType();
	void InitToolBar();
	void InitTreeCtrl();
	void LoadData();
	void SaveData();
	void AddAllNewData(int nSectorID,PhaseType emPaseType);
	void AddDataByType(PhaseType emPhaseType, FlightClassificationBasisType emCategoryType,int nSectorID);
	void DisplayCurData(int nSectorID,PhaseType emPhaseType);
	CString getCStringByPaseType(PhaseType emPaseType);
	CString GetClassificationsName(int nClsItemID, FlightClassificationBasisType emCategoryType);
	int GetComboxIndexByCategoryType(FlightClassificationBasisType emCategoryType);
	HTREEITEM FindItem(HTREEITEM item,int nSectorID);
	BOOL FindItemPhase(HTREEITEM item,PhaseType emPhaseType);

public:
	CTreeCtrl m_wndTreeCtrl;
	CComboBox m_cbCategoryType;
	CListCtrlEx m_wndListCtrl;
	CStatic   m_toolBarRect;
	CToolBar  m_wndToolBar;
	int m_nSector;
	HTREEITEM m_hRightClick;

	CAirportDatabase* m_pAirportDB;
	InputAirside* m_pInputAirside;
	PSelectProbDistEntry m_pSelectProbDistEntry;
	long m_nCurDivRadius;
protected:
	int m_nProjID;	
	Terminal *m_pTerminal;
	PhaseType m_emCurPhaseType;
	FlightClassificationBasisType m_emCurCategoryType;

	AircraftClassifications* m_pCurClassifications;
	AircraftClassifications* m_pClsNone;
	AircraftClassifications* m_pClsWakeVortexWightBased;
	AircraftClassifications* m_pClsWingspanBased;
	AircraftClassifications* m_pClsSurfaceBearingWeightBased;
	AircraftClassifications* m_pClsApproachSpeedBased;

	AircraftClassificationItem* m_pCurClsTrailItem;
	AircraftClassificationItem* m_pCurClsLeadItem;
	
	CInTrailSeparationDataEx* m_pCurInTrailSepData;
	CInTrailSeparationEx* m_pCurInTrailSep;
	CInTrailSeparationListEx m_inTrailSepList;
public:
	afx_msg void OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboData();
	afx_msg void OnSelListCtrlComboBox( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnEnKillfocusEditRadius();
	afx_msg void OnAddTerminalPhase();
	afx_msg void OnAddCruisePhase();
	afx_msg void OnAddApproachPhase();
	afx_msg void OnAddAllPhase();
};
