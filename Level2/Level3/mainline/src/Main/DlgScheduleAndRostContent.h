#pragma once
#include "../MFCExControl/XTResizeDialog.h"
#include "Inputs/AODBImportManager.h"
#include "ARCportTabCtrl.h"

class InputTerminal;

// CDlgScheduleAndRostContent dialog

class CDlgScheduleAndRostContent : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgScheduleAndRostContent)

protected:
	CARCportTabCtrl m_wndTabCtral;
public:
	CDlgScheduleAndRostContent(InputTerminal* pInputTerm,int nProjectID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgScheduleAndRostContent();

// Dialog Data
	enum { IDD = IDD_DIALOG_SCHEDULEANDROSTCONTENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void CreateTabCtrl();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBtnOpen();
	afx_msg void OnBtnMapFile();

	void OnOK();
protected:
	AODBImportManager m_aodbImprotMgr;
	int m_nProjectID;
	InputTerminal*    m_pInputTerm;
};
