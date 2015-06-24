#if !defined(AFX_CARSCHEDULEDLG_H__914F6D6C_F6CB_424C_85F7_3DB1FF5895BE__INCLUDED_)
#define AFX_CARSCHEDULEDLG_H__914F6D6C_F6CB_424C_85F7_3DB1FF5895BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CarScheduleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCarScheduleDlg dialog
#include "..\inputs\AllCarSchedule.h"
#include "TreeCtrlEx.h"
#include "afxcmn.h"
#include "afxwin.h"

class CCarScheduleDlg : public CPropertyPage
{
// Construction
public:
	CCarScheduleDlg(TrainTrafficGraph* _pTrafficGraph,CAllCarSchedule* _pDocCarSchedule,CWnd* pParent = NULL);   // standard constructor
	virtual ~CCarScheduleDlg();

	CAllCarSchedule* GetAllCarSchedule()  { return &m_allCarSchedule;	}
// Dialog Data
	//{{AFX_DATA(CCarScheduleDlg)
	enum { IDD = IDD_DIALOG_CARSCHEDULE };
	CListCtrl	m_listTimeTalbe;
	CTreeCtrlEx	m_treeCarFlow;
	CButton	m_ctSave;
	CButton	m_ctNew;
	CStatic	m_stTreePos;
	CStatic	m_stCarScheduleTreePos;
	CListBox	m_listCarSchedule;
	CSpinButtonCtrl	m_spinTurnAroundTime;
	CSpinButtonCtrl	m_spinHeadwayTime;
	CSpinButtonCtrl	m_spinTrainNumber;
	CSpinButtonCtrl	m_spinDecSpeed;
	CSpinButtonCtrl	m_spinAccSpeed;
	CSpinButtonCtrl	m_spinNormalSpeed;
	CDateTimeCtrl	m_endTimeControl;
	CDateTimeCtrl	m_startTimeControl;
	COleDateTime 	m_startTime;
	COleDateTime 	m_endTime;
	CString	m_sCarName;
	int		m_iAccSpeed;
	int		m_iDecSpeed;
	int		m_iNorSpeed;
	int		m_iTrainNumber;
	int		m_iTurnAroundTime;
	int		m_iHeadWayTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCarScheduleDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

private:
		void SetControl();
		void LoadExitCarScheduleList();
		void LoadCarFlowTree();
		void ClearTree();
		void InitToolbar();
		void SetDlgRightData();
		void ClearDlgRightData();
		void EnableDlgRightData( bool _state );
		InputTerminal* GetInputTerminal();
		CString GetRailWayName( RailwayInfo* _pRailWay ,int _iFirstStationIndex );
		void InitTimeTableList();
		void SetTimeTableList();
		static	int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
		int m_iCurrentScheduleIndex;
		TrainTrafficGraph* m_pTrafficGraph;
		CAllCarSchedule m_allCarSchedule;
		
		CToolBar m_ToolBar;
		bool m_bIsNew;
		CSingleCarSchedule* m_pNewSingleSchedule;

		HTREEITEM m_hStart;
		HTREEITEM m_hEnd;
		HTREEITEM m_hCurrentSel;
		HTREEITEM m_hLastStationItem;
		HTREEITEM m_hRClickItem;
		int m_iSelectStationIndex;
		
		std::vector<HTREEITEM> m_treeItemVector;
		//std::vector<HTREEITEM> m_treeRailwayVector;
		TimeTableVector m_vTimeTableVctor; 

		enum SheduleClickType
		{
			ClickTypeInvalid = -1,
			ClickTypeNewSchedule = 0,
			ClickTypeSelectSchedule = 1,
		};
		SheduleClickType m_enumClickType;
		CWnd* m_pParent;
protected:

	// Generated message map functions
	//{{AFX_MSG(CCarScheduleDlg)
	afx_msg void OnDeltaposSpinNormalspeed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDecspeed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinAccspeed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDwellTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinHeadwayTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinTurnaroudTime(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListCarschedule();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCarScheduleNew();
	afx_msg void OnCarScheduleSave();
	virtual void OnCancel();
	afx_msg void OnCarScheduleFlowNew();
	afx_msg void OnCarScheduleFlowDelete();
	afx_msg void OnSelchangedTreeCarflow(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnCarScheduleDelete();
	afx_msg void OnColumnclickListTimetable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoListTimetable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCarFlowDwellingtime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditDwellingTime();
	CSpinButtonCtrl m_spinStartDay;
	CSpinButtonCtrl m_spinEndDay;
	CEdit m_editStartDay;
	CEdit m_editEndDay;
	afx_msg void OnBnClickedBtnImportTimeTable();
	afx_msg void OnBnClickedBtnExportTimeTable();
	bool ReadFile(const CString &strFileName);



protected:
	const static CString m_strFileFilterType;
	static CString m_strFileLastImport;
	static CString m_strFileLastExport;

	static CCarScheduleDlg* m_DlgCurrentInstance;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARSCHEDULEDLG_H__914F6D6C_F6CB_424C_85F7_3DB1FF5895BE__INCLUDED_)
