#if !defined(AFX_GATEASSIGNSUMMARYDLG_H__A925AC7E_0AA8_4AF9_A7BA_C653539947D9__INCLUDED_)
#define AFX_GATEASSIGNSUMMARYDLG_H__A925AC7E_0AA8_4AF9_A7BA_C653539947D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GateAssignSummaryDlg.h : header file
//
#include <CommonData/procid.h>
#include "common\elaptime.h"
class CGateAssignmentMgr;
#include <vector>
typedef struct 
{
	CString m_strGateID;
	CString m_strAirline;
	ElapsedTime m_timeOccupied;
	int m_iNumberOfFlight;
}tagGateAssignSummary;

typedef struct 
{
	CString m_strKey;
	ElapsedTime m_timeUsage;
	int m_iFrequency;
	float m_fOccupancy;
	CString m_strComparativeIndex;
}tagGateAssignDataItem;
class CGateAssignUtility
{
	CGateAssignmentMgr* m_pGateAssManager;
	std::vector<tagGateAssignSummary>m_vGateAssignData;
	std::vector<tagGateAssignDataItem>m_vFromGateViewPoint;
	std::vector<tagGateAssignDataItem>m_vFromAirlineViewPoint;

	ElapsedTime m_timeTotalOccupied;
	int m_iTotalFrequence;
public:
	CGateAssignUtility( CGateAssignmentMgr* _pGateAssignMgr );
	~CGateAssignUtility(){};
public:
	void InitData();
	ElapsedTime GetTotalOccupiedTime()const { return m_timeTotalOccupied;	}
	int GetTotalFrequence()const { return m_iTotalFrequence;	}
	const std::vector<tagGateAssignDataItem>& GetDataFromGateViewPoint()const { return m_vFromGateViewPoint;	}
	const std::vector<tagGateAssignDataItem>& GetDataFromAirlineViewPoint()const { return m_vFromAirlineViewPoint;	}
	const std::vector<tagGateAssignSummary>& GetDataOfGateAirline()const { return m_vGateAssignData;	}
	
private:
	void AddNewData( const CString& standStr/*const ProcessorID& _gateID*/, CString _strAirline, const ElapsedTime& _occupiedTime );
	void AddNewDataToGateViewPoint(tagGateAssignSummary* _pInfo);
	void AddNewDataToAirlineViewPoint(tagGateAssignSummary* _pInfo);
};
/////////////////////////////////////////////////////////////////////////////
// CGateAssignSummaryDlg dialog

class CGateAssignSummaryDlg : public CDialog
{
// Construction
public:
	CGateAssignSummaryDlg(CGateAssignmentMgr* _pGateAssignMgr,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGateAssignSummaryDlg)
	enum { IDD = IDD_DIALOG_GATE_ASSING_SUMMARY };
	CListCtrl	m_listSummary;
	CButton	m_bExport;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGateAssignSummaryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGateAssignSummaryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnGateassSummaryExport();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	//CGateAssignmentMgr* m_pGateAssManager;
	//std::vector<tagGateAssignSummary>m_vGateAssignSummaryData;

	//ElapsedTime m_allGateTotalOccupiedTime;
	//float m_fAllGateUtilizationPercent;
	CGateAssignUtility m_utility;
	CStringArray m_strTitleList;
private:
	
public:
	afx_msg void OnLvnItemchangedListGataAssignSummary(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATEASSIGNSUMMARYDLG_H__A925AC7E_0AA8_4AF9_A7BA_C653539947D9__INCLUDED_)
