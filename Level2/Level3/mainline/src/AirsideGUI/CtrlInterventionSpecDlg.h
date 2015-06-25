#pragma once
#include "..\MFCExControl\SimpleToolTipListBox.h"
class CCtrlInterventionSpecDataList;
class CCtrlInterventionSpecData;
enum INTERVENTIONMETHOD;

// CCtrlInterventionSpecDlg dialog

class CCtrlInterventionSpecDlg : public CDialog
{
	DECLARE_DYNAMIC(CCtrlInterventionSpecDlg)

public:
	CCtrlInterventionSpecDlg(int nProjID, CWnd* pParent =NULL);   // standard constructor
	virtual ~CCtrlInterventionSpecDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CTRLINTERVENTIONSPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCmdNewRunway();
	afx_msg void OnCmdDeleteRunway();

private:
	void DeleteDataWhichRunwayIsNotExist();
	void InitToolBar(void);
	void InitListBox(void);
	void InitComboBox(void);
	void SetComboBoxContent(CCtrlInterventionSpecData *pCtrlInterventionSpecData);
	void SaveData(void);
	void SetDiffMethod(CComboBox &cbxPriority, INTERVENTIONMETHOD iMethodOne, INTERVENTIONMETHOD iMethodTwo);
	void UpdateInterventionMethod();

private:
	CStatic     m_StaticToolBar;
	CToolBar    m_RunwayToolBar;
	CSimpleToolTipListBox    m_RunwayListBox;
	CComboBox   m_PriorityOneComboBox;
	CComboBox   m_PriorityTwoComboBox;
	CComboBox   m_PriorityThreeComboBox;

	CCtrlInterventionSpecData          *m_pCurCtrlInterventionSpecData;
	CCtrlInterventionSpecDataList      *m_pCtrlInterventionSpecDataList;
	int                                m_nProjID;
public:
	afx_msg void OnLbnSelchangeListRunway();
	afx_msg void OnCbnSelchangeComboPriorityone();
	afx_msg void OnCbnSelchangeComboPrioritytwo();
	afx_msg void OnCbnSelchangeCombo();

};
