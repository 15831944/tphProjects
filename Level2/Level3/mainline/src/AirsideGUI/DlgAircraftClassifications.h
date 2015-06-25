#pragma once
#include "..\MFCExControl\listctrlex.h"
#include "..\InputAirside\AircraftClassifications.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../MFCExControl/XTResizeDialog.h"
class CDlgAircraftClassifications : public CXTResizeDialog , public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgAircraftClassifications)

public:
	CDlgAircraftClassifications(int nProjID, FlightClassificationBasisType emCategoryType, CWnd* pParent = NULL);

	virtual ~CDlgAircraftClassifications();
	enum { IDD = IDD_DIALOG_AIRCRAFTCLASSIFICATION };

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnEditItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);

	CListCtrlEx m_wndListCtrl;
	CToolBar m_wndToolBar;

	AircraftClassifications* m_pAircraftClassifications;
	FlightClassificationBasisType m_emCategoryType;
	int m_nProjID;

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitListCtrl();
	void UpdateToolBar();
	void loadData();
public:
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSave();
};
