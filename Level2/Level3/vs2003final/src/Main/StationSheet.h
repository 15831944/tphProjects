#if !defined(AFX_STATIONSHEET_H__CB82D251_22F0_4A29_B90A_816593C0283F__INCLUDED_)
#define AFX_STATIONSHEET_H__CB82D251_22F0_4A29_B90A_816593C0283F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationSheet dialog
#include "..\inputs\AllCarSchedule.h"
class CStationSheet : public CPropertyPage
{
	DECLARE_DYNCREATE(CStationSheet)

// Construction
public:
	CStationSheet(TrainTrafficGraph* _pTrafficGraph,CAllCarSchedule* _pAllCarSchedule );
	virtual ~CStationSheet();
	CStationSheet(){};
// Dialog Data
	//{{AFX_DATA(CStationSheet)
	enum { IDD = IDD_DIALOG_STATIONINFO };
	CListCtrl	m_listTimeTable;
	CListBox	m_listStation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStationSheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	private:
		TrainTrafficGraph* m_pTrafficGraph;
		CAllCarSchedule* m_pAllCarSchedule ;
		TimeTableVector m_vTimeTable;

		void InitStationListBox();
		void InitTimeTableList();
		void SetListTimeTable( IntegratedStation* _pStation);
			static	int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
protected:
	// Generated message map functions
	//{{AFX_MSG(CStationSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListStation();
	afx_msg void OnGetdispinfoListTimetable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListTimetable(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	const static CString m_strFileFilterType;
	static CString m_strFileLastImport;
	static CString m_strFileLastExport;

	static CStationSheet* m_sheetCurrentInstance;
public:
	// enable related controls, current there are import and export buttons
	void EnableControls(bool bEnabled);
	afx_msg void OnBnClickedBtnImportTimeTable();
	afx_msg void OnBnClickedBtnExportTimeTable();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSHEET_H__CB82D251_22F0_4A29_B90A_816593C0283F__INCLUDED_)
