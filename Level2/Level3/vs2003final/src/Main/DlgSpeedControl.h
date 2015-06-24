#pragma once
#include "MFCExControl/ListCtrlEx.h"
#include "../AirsideGUI/UnitPiece.h"
#include "..\inputs\in_term.h"

class StretchSpeedControlData;
class StretchSpeedControlDataList;

class InputLandside;
// CDlgSpeedControl dialog
class CDlgSpeedControl : public CXTResizeDialog, public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgSpeedControl)

public:
	CDlgSpeedControl(InputLandside* pLandsideInput,/*int nProjectID,*/CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSpeedControl();

// Dialog Data
	enum { IDD = IDD_DIALOG_SPEEDCONTROL };	
protected:
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
public:
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnAddStretch();
	afx_msg void OnDelStretch();
	afx_msg void OnEditStretch();
	afx_msg void OnSave();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDbClickListCtrl(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedListSpeedControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemEditdListOperateDistribution(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void InitListCtrl();
	void SetListContent();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void InitToolBar();

	void UpdateToolBarState();

	StretchSpeedControlDataList& getData(){  return *m_pSpeedCtrlData; }
protected:	
	InputLandside* m_pLandsideInput;
	StretchSpeedControlDataList* m_pSpeedCtrlData;
	CToolBar m_wndSpeedToolBar;
	CListCtrlEx m_wndListCtrl;
	int m_nProjectID;
	CStringList m_strListNode;
	DECLARE_MESSAGE_MAP()		
};


