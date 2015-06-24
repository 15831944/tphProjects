//{{AFX_INCLUDES()
#include "gtchart.h"
//}}AFX_INCLUDES
#if !defined(AFX_PROCASSIGNDLG_H__FE6CE9E4_AA55_4521_B1C8_6C3EFBEAFC65__INCLUDED_)
#define AFX_PROCASSIGNDLG_H__FE6CE9E4_AA55_4521_B1C8_6C3EFBEAFC65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcAssignDlg.h : header file
//
#include "GanntChartOneLineWnd.h"
#include "AllProcessorTreeCtrl.h"
#include "..\inputs\assign.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "inputs\ProcessorSwitchTimeDB.h"
#include "ColorTreeCtrl.h"
#include "RosterRules.h"
#include "RosterRulesDlg.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../MFCExControl/TabCtrlSSL.h"
#include "../MFCExControl/ListCtrlReSizeColum.h"
class CRosterList ;
class CGTCtrlDlg ;
typedef struct _tagIDENTICAL
{
	CString strProcName;
	HTREEITEM hEventsItem;
	long lLineID;
} IDENTICAL;
struct ItemRosterList 
{
	CString c_Processor ;
	CString c_PaxTy ;
	CString c_openTime ;
	CString c_closeTime ;
	CString c_relation ;
	CString c_isDaily ;
};
#define DETAIL_MODE 0
#define SIMPLE_MODE 1
class CProcAssignDlg ;
/////////////////////////////////////////////////////////////////////////////

class CTabCtrlSSLEx : public CTabCtrlSSL
{
public:
	CTabCtrlSSLEx ();
	void SetMaimDlg(CProcAssignDlg* _mainDlg) ;
	// Destruction
	virtual ~CTabCtrlSSLEx (void);
protected:
	void OnActivatePage (int nIndex, int nPageID) ;
protected:
    CProcAssignDlg* m_maindlg ; 
};

class CProcAssignDlg : public CXTResizeDialog
{
// Construction
	DECLARE_DYNAMIC(CProcAssignDlg)
public:
	CWnd* m_pParent;
	virtual  ~CProcAssignDlg();
	CProcAssignDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcAssignDlg)
	enum { IDD = IDD_DIALOG_PROCASSIGN };

	CButton	m_butOK;
	CButton	m_butCancel;

	CAllProcessorTreeCtrl	m_treeProc;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcAssignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//ProcAssignment* m_pProc ;
	ProcessorRosterSchedule* m_pProcSchd ;
	CMultiMobConstraint* m_pPaxCon;
	FlightSchedule* m_flightSchedule;
 //   CAutoRosterRulesDB* p_rulesDB ;
	//CAutoRosterByScheduleRulesDB* p_ruleSchedule ;
	// to make evnets tree and gt ctrl ....identical
	std::vector<IDENTICAL> m_vectIdentical;		
		
	void InitToolbar();
	CToolBar m_RukeToolBar;

    UnmergedAssignments m_unMergedAssgn;
	UnmergedAssignments m_unMergedAssgn_readyToPaste;
	bool m_bCopied;
	int m_iGTmode;

	int m_nMaxDayCount;
	// Generated message map functions
	//{{AFX_MSG(CProcAssignDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);

	virtual void OnOK();
	virtual void OnCancel();



	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


//	afx_msg void OnButtonWizard();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    afx_msg void OnClickCloseProcessor() ;
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:


	BOOL m_bDetailProc;

protected:
    void ResetAllProcessorDailyData() ;
	//void OnDailyEvent();
	//void SetProcessorDaily(CString& ProID);
	//void AssigmentData(ProcAssignment* proAss);
	CString GetProjPath() ;

	CButton m_But_AutoAss;
public:
	afx_msg void OnStnClickedStaticListtoolbarcontenter();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListRules(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnButtonWizard() ;
	afx_msg void OnButtonPaste() ;
	afx_msg void OnButtonCopy() ;
	afx_msg void OnButtonSave();
	afx_msg BOOL OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult) ;
	void ReloadDatabase();
	int ndays ;
protected:
	CTabCtrlSSLEx m_TabCtrl;
public:
	CButton m_check_close;
protected:
	CStatic m_StaticPro;

protected:
	void InitRuleToolBar() ;
	void InitStatic(CString& _name) ;
     // the function : Init the rules schedule list 
	CStatic m_staticRule;
	//the function init Tabctrl list 
	void InitTabCtrl() ;
	void InitTabCtrlView() ;
	void InitTabCtrlData() ;
	void LoadProcessorData();


public:
	BOOL GetCurrentSelProcessID(ProcessorID& _Id) ;
	BOOL GetCurrentSelProcessID(HTREEITEM _proNode,ProcessorID& _Id) ;
	CString GetRelationName(int _rel) ;
	CString GetIsDailyName(int _data) ;
	InputTerminal* GetInputTerminal();




public:
		int GetCurrentSelProcessorGroupNumber();
public:
	afx_msg void OnHdnItemclickListRules(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTabRoster(NMHDR *pNMHDR, LRESULT *pResult);
     void OnButtonAutoAss();
	 BOOL getCurrentSelParentProcessID(ProcessorID& _id);
protected:

	void InitProcessorTree();
	void Resizedialog() ;
protected:
	CRosterList*   m_RosterListDlg ; 
	CGTCtrlDlg*   m_GTDlg ;
	CButton m_ClosePro ;
public:
	CRosterList* GetRosterList() { return m_RosterListDlg ;};
	CGTCtrlDlg* GetGTListDlg() { return m_GTDlg ;} ;


	ProcAssRelation GetRelationByName(CString name) ;
private:
	CButton m_btnCopy ;
	CButton m_btnPaste ;
private:
	CSortListCtrlEx m_rosterGroup;
	std::vector<CString> m_RosterListColNames ;
protected:
	void InitRosterForGroupList() ;
	void InitRosterForGroupListView() ;
	void InitRosterForGroupListData();
	void ShowAllProcessorGroupRoster();
	void InsertRosterOfProcessorGroup(ProcessorID& _id);
	void NewRosterForProcessorGroup();
	void EditRosterForProcessorGroup();
	ProcessorRosterSchedule* GetCurSelProcSchdFromRosterList();
	void DelRosterForProcessorGroup();
	void AddLineToRosterForGroupList(ItemRosterList& _data,ProcessorRoster* _PAss);
	void DeleteSelLinRosterForGroupList() ;
	void EditSelRosterForGroupList(ItemRosterList& _data) ;
public:
	BOOL FindDefinedRosterProcessor(HTREEITEM _proNode,ProcessorID& _Id);

protected:

	afx_msg void ImportFileForRosterList() ;
	afx_msg void ExportFileFromRosterList() ;
		BOOL WriteRosterToFile(const CString& filename) ;
		void GetStringArrayFromRosterList(CStringArray& strArray);

		bool ReadTextFromFile(const CString &_szFileName) ;
		void AddRosterFromStrList(const CStringArray& strItem) ;
		int  getColNumFromStr(CString strText, CString strSep, CStringArray &strArray) ;
	protected:
		void InitCheckCloseProcessorState();
		void GetNodeProcessors(HTREEITEM hChildItem, std::vector<ProcessorID> &vProcID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCASSIGNDLG_H__FE6CE9E4_AA55_4521_B1C8_6C3EFBEAFC65__INCLUDED_)
