#if !defined(AFX_REPCONTROLVIEW_H__BBE62F9B_350E_42B4_90C4_0B5355CCB7B2__INCLUDED_)
#define AFX_REPCONTROLVIEW_H__BBE62F9B_350E_42B4_90C4_0B5355CCB7B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepControlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRepControlView form view
#pragma warning( disable : 4786 )

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "inputs\pax_cnst.h"
#include "reports\ReportType.h"
#include "reports\reportspecs.h"
#include "termplandoc.h"
#include "coolbtn.h"
#include "../MFCExControl/CoolTree.h"

enum ENUM_DATE_TYPE
{
	DATETYPE_DATE,
	DATETYPE_INDEX
};


class CReportParameter;
class CRepControlView : public CFormView
{
	enum TreeItemType 
	{
		PAX_TYPE_Root		= 0x1000,
		PAX_TYPE			= 0x1001,
		PROCESSOR_ROOT		= 0x1002,
		PROCESSOR			= 0x1003,
		Flight_Type_Root	= 0x1004,
		Flight_Type			= 0x1005,
		Runs_Root			= 0x1006,
		Runs				= 0x1007,
		PROCESSORTOPROCESSOR= 0x1008,
		PROCESSOR_FROM		= 0x1009,
		PROCESSOR_FROM_PROC	= 0x1010,
		PROCESSOR_TO		= 0x1011,
		PROCESSOR_TO_PROC	= 0x1012
	};


protected:
	CRepControlView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRepControlView)

// Form Data
public:
	//{{AFX_DATA(CRepControlView)
	enum { IDD = IDD_DIALOG_REPORT_SPECS };
	CComboBox	m_comboEndDay;
	CComboBox	m_comboStartDay;
	CCoolTree	m_treePaxType;
	CButton	m_staticTime;
	CStatic	m_staticStartTime;
	CStatic	m_staticEndTime;
	CCoolBtn	m_btnMulti;
	CButton	m_btLoad;
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
	CButton	m_staticProListLabel;
	CListBox	m_lstboProType;
	CButton	m_staticTypeListLabel;
	CButton	m_btnCancel;
	CDateTimeCtrl	m_dtctrlStartTime;
	CDateTimeCtrl	m_dtctrlEndTime;
	CDateTimeCtrl   m_dtctrlStartDate;
	CDateTimeCtrl   m_dtctrlEndDate;
	CButton	m_radioDetailed;
	CButton	m_radioSummary;
	CListBox	m_lstboPaxType;
	CEdit	m_edtThreshold;
	CButton	m_chkThreshold;
	CButton	m_btnApply;
	COleDateTime	m_oleDTEndTime;
	COleDateTime	m_oleDTStartTime;
	COleDateTime	m_oleDTEndDate;
	COleDateTime	m_oleDTStartDate;
	int		m_nRepType;
	UINT	m_nInterval;
	COleDateTime	m_oleDTInterval;
	float	m_fThreshold;
	COleDateTime	m_oleThreshold;
	bool bShowLoadButton;
	//}}AFX_DATA

// Attributes
public:
	//HANDLE m_hProcess;
	CMultiMobConstraint	m_paxConst;
	std::vector<FlightConstraint> m_vFltList;
///	CReportSpecs m_ReportSpecs;
	//std::vector<CString> throughput_pax_data ;
private:
	HTREEITEM m_hPaxTypeRoot;
	HTREEITEM m_hProcRoot;
	HTREEITEM m_hRunsRoot;
	HTREEITEM m_hFlightTypeRoot;
	HTREEITEM m_hProcToProc;
	HTREEITEM m_hItemTo;
	HTREEITEM m_hItemFrom;
	//HTREEITEM m_hItemPaxType;

private:
	CReportParameter* GetReportParameter();
	void ReadDataFromLib();
	void getReportName( CString& strReportType);
	int m_nRepSpecsType;
// Operations
public:
	void ResetAllContent();
public:

	CTermPlanDoc* GetDocument(){ return (CTermPlanDoc*) m_pDocument; }
	void OnClickMultiBtn();
	void Clear();
	//void LoadPaxList();
	//void LoadFltList();
	void ReloadSpecs();
	void RefreshProcessTypeList();

	void LoadFltList(CReportParameter *pReportParam);
	void LoadPaxList(CReportParameter *pReportParam);
	void LoadProTree(CReportParameter *pReportParam);
	void LoadFromToProTree(CReportParameter *pReportParam);


	const ENUM_DATE_TYPE GetDateType() const { return m_emDateType; }
	void SetDateType(const ENUM_DATE_TYPE emDateType) { m_emDateType = emDateType; }
	void ShowDateTypeControl(const ENUM_DATE_TYPE emDateType);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepControlView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadTreeContent(CReportParameter* pReportPara);

	int GetTotalMinutes(COleDateTime _oleDT);
	void InitComboDay(std::vector<std::string>* pvString);
	void InitToolbar();
	void InitCoolBtn();
	CToolBar m_ProToolBar;
	CToolBar m_PasToolBar;
	CString m_strProcessor;
	ENUM_REPORT_TYPE m_enumReportType;
	virtual ~CRepControlView(); 
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CRepControlView)
	afx_msg void InitGUI();//not sure what it comes from
	afx_msg void OnButtonCancel();
	afx_msg void OnButtonApply();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	//afx_msg void OnPeoplemoverNew();
	//afx_msg void OnPeoplemoverDelete();
	afx_msg void OnDeltaposSpinThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonLoadFromFile();
	afx_msg void OnReportSavePara();
	afx_msg void OnReportLoadPara();
	afx_msg void OnReportSaveReport();
	afx_msg void OnReportLoadReport();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRadioDetailed();
	afx_msg void OnRadioSummary();
	//}}AFX_MSG
	afx_msg void OnUpdateBtnAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnDel(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateBtnAddMove(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateBtnDelMove(CCmdUI* pCmdUI);
		
	DECLARE_MESSAGE_MAP()

private:
	CReportParameter* GetReportPara();
	//void LoadProcList();
	void SavePara();
	void LoadPara();
	void SaveReport();
	void LoadReport();
	void GetParaFromGUI(CReportParameter* pReportPara);
	void SetGUIFromPara(CReportParameter* pReportPara, const CStartDate& _startDate);
	void CopyParaData( CReportParameter* _pCopyPara,CReportParameter* _pToPara );
	CString getExtensionString();
	bool CheckReportFileFormat(const CString& _strFileName );
	CString GetReportTitle();
	void DoSizeWindow( int cx, int cy );
	void SizeDownCtrl(int cx, int cy );
	bool MultiRunValid();
	//CReportParameter* m_pReportParam;
	std::vector<ProcessorID> m_vProcList;
	bool m_bNewData;
	bool m_bCanToSave;
	CString m_strCurReportFile;
	std::vector<int> m_vReportRuns;

	HTREEITEM m_hSelectedItem;

	ENUM_DATE_TYPE m_emDateType;
	//std::vector<ProcessorID*> m_TreeDataProID ;
private:// by aivin	
	CStartDate m_startDate;	
	void InitControl();
	void LoadProTree() ;
	void SetPaxTreeToDefaultOrAnothers(BOOL is_or_not);

	int GetTreeItemIndex(HTREEITEM hItem);
	HTREEITEM  TreeInsertItem(const CString& strItemText, HTREEITEM hParentItem, bool bHasCheckBox = false);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTvnSelchangedTreePaxtype(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPCONTROLVIEW_H__BBE62F9B_350E_42B4_90C4_0B5355CCB7B2__INCLUDED_)
