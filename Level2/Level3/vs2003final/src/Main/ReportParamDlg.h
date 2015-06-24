#if !defined(AFX_REPORTPARAMDLG_H__32C71608_AB22_45E1_BE3C_90D69DA1EDE0__INCLUDED_)
#define AFX_REPORTPARAMDLG_H__32C71608_AB22_45E1_BE3C_90D69DA1EDE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportParamDlg.h : header file
//
#include "inputs\MultiMobConstraint.h"
#include "assert.h"

class InputTerminal;
class CTermPlanDoc;
class CReportParameter;
/////////////////////////////////////////////////////////////////////////////
// CReportParamDlg dialog
class CReportParamDlg : public CDialog
{
	enum itemItem
	{
		PAX_TYPE = 0,
		THROUGH_PUT_PROCESSOR 
	};
// Construction
public:
	CReportParamDlg(int _type, InputTerminal* _pTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CReportParamDlg();
// Dialog Data
	//{{AFX_DATA(CReportParamDlg)
	enum { IDD = IDD_DIALOG_REPORT_PARAM };
	CButton	m_btnLoad;
	CButton	m_btnSave;
	CButton	m_btnPaste;
	CButton	m_btnCopy;
	CTreeCtrl m_treePaxType;
	CDateTimeCtrl	m_dtctrlThreshold;
	CDateTimeCtrl	m_dtctrlInterval;
	CSpinButtonCtrl	m_spinThreshold;
	CStatic	m_staticArea;
	CComboBox	m_comboAreaPortal;
	CButton	m_staticThreshold;
	CButton	m_staticType;
	CStatic	m_staticInterval;
	CStatic	m_staProc;
	CStatic	m_staPax;
	CStatic	m_toolbarcontenter2;
	CStatic	m_toolbarcontenter1;
	CListBox	m_lstboProType;
	CButton	m_btnCancel;
	CDateTimeCtrl	m_dtctrlStartTime;
	CDateTimeCtrl	m_dtctrlEndTime;
	CButton	m_radioDetailed;
	CButton	m_radioSummary;
	CListBox	m_lstboPaxType;
	CEdit	m_edtThreshold;
	CButton	m_chkThreshold;
	CButton	m_btnApply;
	COleDateTime	m_oleDTEndTime;
	COleDateTime	m_oleDTStartTime;
	int		m_nRepType;
	UINT	m_nInterval;
	COleDateTime	m_oleDTInterval;
	float	m_fThreshold;
	COleDateTime	m_oleThreshold;

	CDateTimeCtrl   m_dtctrlStartDate;
	COleDateTime	m_oleDTStartDate;
	CDateTimeCtrl   m_dtctrlEndDate;
	COleDateTime	m_oleDTEndDate;
	CComboBox	m_comboStartDay;
	CComboBox	m_comboEndDay;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL ContinueModal( ); 
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReportParamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListPaxtype();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonCancel();
	afx_msg void OnSelchangeListProtype();
	afx_msg void OnButtonPaste();
	afx_msg void OnButtonCopy();
	afx_msg void OnButtonLoadfile();
	afx_msg void OnButtonSavefile();
	afx_msg void OnRadioDetailed();
	afx_msg void OnRadioSummary();
	//}}AFX_MSG
	afx_msg void OnUpdateBtnAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnDel(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
private:
	void InitToolbar();
	CToolBar m_ProToolBar;
	CToolBar m_PasToolBar;
	CString m_strProcessor;
	CMultiMobConstraint	m_paxConst;
	std::vector<FlightConstraint> m_vFltList;
	std::vector<ProcessorID> m_vProcList;
	std::vector<ProcessorID*> m_TreeDataProID ;

	bool m_bNewData;

private:
	HTREEITEM m_throughput_paxroot ;
	HTREEITEM m_throughput_proroot ;
	HTREEITEM m_hItemTo;
	HTREEITEM m_hItemFrom;
	HTREEITEM m_hItemPaxType;
private:
	void LoadProcList();
	void LoadProTree();
	CReportParameter* getReportParam( CString strName );
	void getReportString( CString& strReportType );
	void ReadDataFromLib();
	void LoadFltList();
	void Clear();
	void ModifyControl();
	void ReloadSpecs();
	void LoadPaxList();
	void InitAllContent();
	BOOL GetParaFromGUI();
	void SetGUIFromPara();
	void CopyParaData( CReportParameter* _pCopyPara );
	int  GetTotalMinutes(COleDateTime _oleDT);
	int m_iReportType;
	InputTerminal* m_pTerm;
	CTermPlanDoc* m_pDoc;
	CReportParameter* m_pReportParam;
public:
	void DisableTBAllBtn(CToolBar* pToolBar);
	void setDocument( CTermPlanDoc* _pDoc ) { assert( _pDoc ); m_pDoc = _pDoc; }
	int getReportType() const { return m_iReportType;}

private:
	CStartDate m_startDate;	
	void InitControl();
	void InitComboDay(std::vector<std::string>* pvString);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPARAMDLG_H__32C71608_AB22_45E1_BE3C_90D69DA1EDE0__INCLUDED_)
