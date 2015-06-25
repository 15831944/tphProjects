#pragma once

//#include "../Engine/terminal.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "..\InputAirside\AircraftClassifications.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\InputAirside\InTrailSeparation.h"
#include "..\MFCExControl\SimpleToolTipListBox.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgInTrailSeparation dialog

class CDlgInTrailSeparation : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgInTrailSeparation)

public:
	CDlgInTrailSeparation(int nProjID, Terminal* pTerminal, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInTrailSeparation();

	// Dialog Data
	enum { IDD = IDD_DIALOG_IN_TRAIL_SEPARATION };

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
	
	CInTrailSeparationData* m_pCurInTrailSepData;
	CInTrailSeparation* m_pCurInTrailSep;
	CInTrailSeparationList m_inTrailSepList;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void InitPhaseType();
	void InitCategoryType();
	void InitListCtrl();
	void LoadData();
	void SaveData();
	void DelInitData();
	void AddAllNewData();
	void AddDataByType(PhaseType emPhaseType, FlightClassificationBasisType emCategoryType);
	void DisplayCurData(PhaseType emPhaseType);
	CString GetClassificationsName(int nClsItemID, FlightClassificationBasisType emCategoryType);
	int GetComboxIndexByCategoryType(FlightClassificationBasisType emCategoryType);

public:
	virtual BOOL OnInitDialog();
	CSimpleToolTipListBox m_lstPhaseType;
	CComboBox m_cbCategoryType;
	CListCtrlEx m_lstData;
	
	afx_msg void OnLbnSelchangeListPhaseType();
	afx_msg void OnCbnSelchangeComboCategoryType();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult);
};
