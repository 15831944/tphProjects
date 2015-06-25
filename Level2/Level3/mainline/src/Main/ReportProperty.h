#if !defined(AFX_REPORTPROPERTY_H__E503829B_3B64_49E0_918D_0E7726117A82__INCLUDED_)
#define AFX_REPORTPROPERTY_H__E503829B_3B64_49E0_918D_0E7726117A82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportProperty.h : header file
//

#include "../Inputs/MultiMobConstraint.h"
#include "../compare/ReportsManager.h"
#include "../compare/ReportToCompare.h"	// Added by ClassView
#include "../compare/ModelsManager.h"
//#include "../Engine/terminal.h"
#include "../Reports/ReportType.h"
#include "../Engine/proclist.h"

/////////////////////////////////////////////////////////////////////////////
// CReportProperty dialog
//

class CDlgReportProperty : public CDialog
{
// Construction
public:
	enum ItemType
	{
		IT_MODEL,
		IT_PAXTYPE,
		IT_PROCTYPE,
		IT_FROM,
		IT_TO,
		IT_FROMPROC,
		IT_TOPROC,
		IT_AREA
	};

	class ItemData
	{
		
	public:
		ItemData(const ItemType& itemType,const long nIndex)
		{
			m_itemType = itemType;
			m_nIndex = nIndex;
		}

		ItemType m_itemType;
		long m_nIndex;
	};
public:
	CDlgReportProperty(CWnd* pParent = NULL);   // standard constructor
	//~CReportProperty();

	void DeleteTreeData(CTreeCtrl& tree);
	void DeleteTreeSubItemData(CTreeCtrl& tree,HTREEITEM hItem);
	void SetManager(CModelsManager* pModelManager,CSingleReportsManager* pReportManager);
	void SetProjName(const CString& strName){ m_strProjName = strName; }
	CReportToCompare& GetReport(){return m_reportToCompare;}
	void SetReport(const CReportToCompare& value){ m_reportToCompare = value; }
	static COleDateTime GetOleDateTime(long _nTime);

	enum MODELTYPE
	{
		MT_NOMODEL, 
		MT_NOLOCATION, 
		MT_HASLOCATION
	};

	MODELTYPE HasModelInLocation();

// Dialog Data
	//{{AFX_DATA(CReportProperty)
	enum { IDD = IDD_REPORTPROPERTY };
//	CComboBox	m_cmbArea;
	CComboBox	m_cmbEndDateIndex;
	CComboBox	m_cmbStartDateIndex;
	CDateTimeCtrl	m_dtCtrlEndDate;
	CDateTimeCtrl	m_dtCtrlStartDate;
	CComboBox		m_nReportType;
	CDateTimeCtrl	m_tInterval;
	CStatic			m_toolbarProcessorType;
	CStatic			m_toolbarPessengerType;
/*	CListBox		m_listPessengerType;
	CListBox		m_listProcessorType*/;
	CString			m_strName;
	COleDateTime	m_tEnd;
	COleDateTime	m_tStart;
	COleDateTime	m_dStart;
	COleDateTime	m_dEnd;
	CString	m_strArea;
	CTreeCtrl m_treeProcFromTo;
	CTreeCtrl m_treePaxType;
	CTreeCtrl m_treeProcType;
	CTreeCtrl m_treeArea;
	CEdit    m_editInterval;
	//}}AFX_DATA

	CString m_strProjName;

	static CRect m_rcWindow[4];

	std::vector<CModelParameter> m_vModelParam;

public:
	void LoadPaxList();
	void LoadPaxListByModel(int nModelIndex,HTREEITEM hItemModel);
	BOOL CopyModelToLocal();
	BOOL CopyDirectory(const CString& strSrcDir, const CString& strDistDir, BOOL bDelSrc = FALSE);
//	CMultiMobConstraint	m_paxConst;
//	std::vector<ProcessorID> m_vProcList;
//	CReportParameter::FROM_TO_PROCS m_fromToProcs;
//	Terminal*			m_pTerminal;
	ENUM_REPORT_TYPE	m_enumReportType;
	ENUM_REPORT_TYPE    m_enumOldType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ArrangeControls();
	void Init();
	BOOL CheckParameter();
	// load processor into the list base on the m_vProcList;
	void LoadProcList();
	void LoadProcListByModel(int nModelIndex,HTREEITEM hItemModel);

	void LoadProcFromToList();
	void LoadProcFromToListByModel(int nModelIndex,HTREEITEM hItemModel);

	void LoadArea();
	BOOL CheckUserHasAssignedArea();
	void LoadAreaByModel(int nModelIndex,HTREEITEM hItemModel);
	void OnUpdateToolBarUI();

	CReportToCompare m_reportToCompare;
	CModelsManager* m_pModelsManager;
	void InitToolBar();
	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;
	CToolBar m_ToolBar3;
	CToolBar m_ToolBar4;
	CSingleReportsManager* m_reportsManager;

	CString m_strOldName;

	BOOL	m_bShowProcessor;

	CStartDate	m_sdStart;
	//HTREEITEM m_hItemTo;
	//HTREEITEM m_hItemFrom;

	void OnUpdateUIProcTypeAdd();
	void OnUpdateUIFromToAdd();
	void OnUpdateUIAreaAdd();
	void OnUpdateUIAreaDel();
	void OnUpdateUIPaxTypeDel();
	void OnUpdateUIProcTypeDel();
	void OnUpdateUIFromToDel();
	void OnUpdateUIPaxTypeAdd();
	// Generated message map functions
	//{{AFX_MSG(CReportProperty)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPessengerTypeAdd();
	afx_msg void OnProcessorTypeAdd();
	afx_msg void OnPessengerTypeDelete();
	afx_msg void OnProcessorTypeDelete();
	afx_msg void OnProcessorFromToAdd();
	afx_msg void OnProcessorFromToDelete();
	afx_msg void OnAddArea();
	afx_msg void OnDelArea();
	virtual void OnOK();
	afx_msg void OnSelchangeComboReporttype();
//	afx_msg void OnUpdateUIPaxTypeAdd(CCmdUI *pCmdUI);

	//afx_msg void OnUpdateUIProcTypeAdd(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateUIFromToAdd(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateUIAreaAdd(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateUIAreaDel(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateUIPaxTypeDel(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateUIProcTypeDel(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateUIFromToDel(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();

	afx_msg void OnTvnSelchangedTreePaxtype(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeProcs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeProcfromto(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeArea(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPROPERTY_H__E503829B_3B64_49E0_918D_0E7726117A82__INCLUDED_)
