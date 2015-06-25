//{{AFX_INCLUDES()
#include "mschart.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLGPROBDIST_H__D8EDD7E0_1B5E_4B06_BAE7_43A6E111A4F1__INCLUDED_)
#define AFX_DLGPROBDIST_H__D8EDD7E0_1B5E_4B06_BAE7_43A6E111A4F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProbDist.h : header file
//
#include <MFCExControl/SplitterControl.h>
#include "ListCtrlEx2.h"
#include "InPlaceComboBoxEx.h"
#include "CoolBtn.h"
#include <vector>
#include "afxwin.h"

class CProbDistEntry;
class Terminal; 
class CProbDistManager;
class CAirportDatabase;

/////////////////////////////////////////////////////////////////////////////
// CDlgProbDist dialog

class CDlgProbDist : public CDialog
{
// Construction
public:
	CDlgProbDist(CAirportDatabase* _pAirportDB, BOOL bNeedRet, CWnd* pParent = NULL);   // standard constructor
	~CDlgProbDist();
	CProbDistEntry* GetSelectedPD();
	int GetSelectedPDIdx() { return m_nSelectedPD; }
	
	void OpenAndReadFile();
	void OnClickMultiBtn();
//	void setProbDistMan( CProbDistManager* _pProbDistMan );
//	void setAirportDatabase(CAirportDatabase* _pAirportDB );
// Dialog Data
	//{{AFX_DATA(CDlgProbDist)
	enum { IDD = IDD_DIALOG_PROBDIST2 };
	CComboBox	m_cmbGraphType;
	CButton	m_staticProbDensityFunc;
	CButton	m_staticParamGroup2;
	CButton	m_staticParamGroup;
	CButton	m_staticDistGroup2;
	CButton	m_staticDistGroup;
	CStatic	m_staticToolbarParams;
	CStatic	m_staticToolbarDist;
	CListCtrlEx2 m_lstDistributions;
	CListCtrlEx2 m_lstParameters;
	CStatic	m_staticExtraParamName;
	CEdit	m_editExtraParam;
	CMSChart1	m_chart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProbDist)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nSelectedPD;
	BOOL m_bNeedReturn;
	CToolBar m_ToolBarDist;
	CToolBar m_ToolBarParams;
	CToolTipCtrl m_toolTipCtrl;
	int m_nCurToolTipsIndex;

	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;
	CInPlaceComboBoxEx m_distTypeInPlaceComboBox;
	CSize m_sizeLast;
	BOOL m_bShowExtraParam;
	int m_nPDTypeSelItem;
	int m_nSortColumn;
	CProbDistManager* m_pProbDistMan;
	CAirportDatabase* m_pAirportDB;
	//CProbDistEntry* m_pSelectedPD;
	void InitToolBars();
	void LoadDistributionList();
	void LoadParamList(CProbDistEntry* _pd);
	void DoSplitterResize(int delta, UINT nIDSplitter);
	void DoResize(int cx, int cy);
	void UpdateDistributionData();
	void UpdateExtraParam(CProbDistEntry* _pd);
	void UpdateChart(CProbDistEntry* _pd);
	CWnd* EditSubItem(int nItem, int nSubItem);
	void InitializeChart();
	void SortByColumn(int nSortCol);

	static int CALLBACK CompareFuncAscending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareFuncDescending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	std::vector<double> m_vdbValue;
	typedef std::pair<double, double> pair_range;
	std::vector<pair_range> m_vPairRange;

	// Generated message map functions
	//{{AFX_MSG(CDlgProbDist)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetDispInfoListDistributions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfoListParameters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListDistributions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditListDistributions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClickListDistributions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedListDistributions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangingListDistributions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedListParameters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClickListParameters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditListParameters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeGraphType();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnDistToolbarbuttonADD();
	afx_msg void OnParamToolbarbuttonADD();
	afx_msg void OnDistToolbarbuttonDEL();
	afx_msg void OnParamToolbarbuttonDEL();
	afx_msg void OnDistToolbarbuttonCopy();
	afx_msg void OnDistToolbarbuttonPaste();
	afx_msg void OnParamToolbarbuttonFF();
	afx_msg void OnParamToolbarbuttonRAWFF();

	afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg virtual void OnBnClickedOk();
	afx_msg virtual void OnCancel();
	afx_msg void OnBnClickedButtonLoaddefault();
protected:
		BOOL m_commitDefault ;
		CCoolBtn m_button_loadDefault;
protected:
			BOOL m_Commit_Oper ;
			BOOL m_Load_Oper ;
			BOOL m_IsEdit ;
protected:
	afx_msg void OnLoadFromTemplate();
	afx_msg void OnSaveAsTemplate();
};
class CDlgProbDistForDefaultDB : public CDlgProbDist
{
public:
	CDlgProbDistForDefaultDB(CAirportDatabase* _pAirportDB, BOOL bNeedRet, CAirportDatabase* _globalDB ,CWnd* pParent = NULL) ;
	virtual ~CDlgProbDistForDefaultDB() ;
public:
	BOOL OnInitDialog() ;
	void OnBnClickedOk();
	void OnCancel();
protected:
	CAirportDatabase* m_GlobalDB ;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROBDIST_H__D8EDD7E0_1B5E_4B06_BAE7_43A6E111A4F1__INCLUDED_)
