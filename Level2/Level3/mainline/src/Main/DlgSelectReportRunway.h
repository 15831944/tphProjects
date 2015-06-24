#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../AirsideReport/AirsideReportNode.h"

// CDlgSelectReportRunway dialog

class CDlgSelectReportRunway : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectReportRunway)

public:
	CDlgSelectReportRunway(int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectReportRunway();

// Dialog Data
	//enum { IDD = IDD_DIALOG_REPORTSELECTRUNWAYMARK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CListBox m_lbRunway;

	std::vector<CAirsideReportRunwayMark> m_vRunwayMarkSel;
protected:
	int m_nProjID;
	std::vector<CAirsideReportRunwayMark> m_vAllRunwayMark;
};
