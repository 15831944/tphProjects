#pragma once
#include "MFCExControl/ListCtrlEx.h"
#include "../AirsideGUI/UnitPiece.h"

class StretchSpeedConstraintList;
class InputLandside;
// CDlgLandsideSpeedConstraint dialog

class CDlgLandsideSpeedConstraint : public CXTResizeDialog , public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgLandsideSpeedConstraint)

public:
	CDlgLandsideSpeedConstraint(InputLandside* pLandsideInput,int nProjectID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideSpeedConstraint();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDESPEEDCONSTRAINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAddStretch();
	afx_msg void OnDeleteStretch();
	afx_msg void OnEditStretch();
	afx_msg void OnAddConstraint();
	afx_msg void OnDeleteConstraint();
	afx_msg void OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLbnSelchangeListStretch();
	afx_msg void OnLvnItemchangedListConstraint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
private:
	void InitToolBar();
	void InitListBox();
	void InitListCtrl();
	void SetListContent();
	void LoadData();
	void UpdateStretchToolbar();
	void UpdateConstraintToolbar();

	void CreateComboxString();
private:
	CToolBar m_wndStretchToolBar;
	CToolBar m_wndConstraintToolBar;
	CListBox m_wndListBox;
	CListCtrlEx m_wndListCtrl;

	StretchSpeedConstraintList* m_pStretchSpeedConstraintList;
	InputLandside* m_pLandsideInput;
	CStringList m_strListNode;
	int m_nProjectID;
};
