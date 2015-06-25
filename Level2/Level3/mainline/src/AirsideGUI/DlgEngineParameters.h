#pragma once
#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/EngineParameters.h"
#include "NodeViewDbClickHandler.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../MFCExControl/XTResizeDialog.h"

class InputTerminal;

// CDlgEngineParameters dialog

class CDlgEngineParameters : public CXTResizeDialog,public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgEngineParameters)

public:
	CDlgEngineParameters(InputAirside * pInputAirside,CAirportDatabase* pAirportDB,int nProjID,PFuncSelectFlightType pSelectFlightType,PSelectProbDistEntry m_pSelectProbDistEntry,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEngineParameters();

// Dialog Data
	enum { IDD = IDD_DIALOG_ENGINEPARAMETERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

protected:
	void OnInitToolbar();
	void OnInitListCtrl();
	void DisplayData();
	void OnUpdateToolbar();

	afx_msg void OnAddPaxType();
	afx_msg void OnRemovePaxType();
	afx_msg void OnEditPaxType();
	afx_msg void OnSave();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDBClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
private:
	InputAirside* m_pInputAirside;
	CEngineParametersList m_engineParametersList;
	CToolBar  m_wndToolbar;
	CListCtrlEx m_wndListCtrl;
	int m_nPorjID;
	PFuncSelectFlightType	m_pSelectFlightType;
	PSelectProbDistEntry m_pSelectProbDistEntry;
};
