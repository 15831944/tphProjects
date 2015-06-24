#pragma once

#include "..\inputs\assign.h"
#include "..\CommonData\Procid.h"
#include "..\Inputs\In_term.h"
#include "..\Inputs\Assigndb.h"
#include <vector>
#include "afxcmn.h"
#include "afxwin.h"

class tagRosterDataItem
{
public:
	tagRosterDataItem();
	virtual ~tagRosterDataItem(){}
	CString m_strKey;
	ElapsedTime m_timeUsage;
	float m_fOccupancy;
};

// CRosterAssignSummaryDlg dialog

class CRosterAssignSummaryDlg : public CDialog
{
	DECLARE_DYNAMIC(CRosterAssignSummaryDlg)
public:
	CRosterAssignSummaryDlg(std::vector<ProcessorID>* procList, InputTerminal* pInTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRosterAssignSummaryDlg();
private:
	std::vector<tagRosterDataItem> m_rosterData;
	InputTerminal* m_pInTerm;
	std::vector<ProcessorID> m_vProcID;
	ElapsedTime m_timeTotal;
	void InitRosterData();
	void GetUsageTimeFromSchedule(ProcessorRosterSchedule* pSche, tagRosterDataItem& rosterDataItem);
	void InitRosterAssignSummaryList();
	void InsertRosterData();
public:
// Dialog Data
	enum { IDD = IDD_DIALOG_ROSTER_ASSIGN_SUMMARY };
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_summaryListCtrl;
	afx_msg void OnBnClickedBtnGateassSummaryExport();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_btnExport;
	CButton m_btnOK;
	CButton m_btnCancel;
};
