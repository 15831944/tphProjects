#pragma once

#include "common\SelectedSimulationData.h"

// CDlgSelectedSimulationModels dialog

class CDlgSelectedSimulationModels : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectedSimulationModels)

public:
	CDlgSelectedSimulationModels(SelectedSimulationData &simData,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectedSimulationModels();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTESIMULATIONMODE };

	BOOL SelectedAirside()const{ return m_bAirsideMode; }
	BOOL SelectedLandside()const{return m_bLandsideMode; }
	BOOL SelectedTerminal()const{ return m_bTerminalMode; }
	BOOL SelectedOnBoard()const{ return m_bOnBoardMode; }
private:
	SelectedSimulationData *m_pSimData;
	BOOL m_bTerminalMode;
	BOOL m_bAirsideMode;
	BOOL m_bLandsideMode;
	BOOL m_bOnBoardMode;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckAirside();

};
