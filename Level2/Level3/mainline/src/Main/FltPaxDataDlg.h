#if !defined(AFX_FLTPAXDATADLG_H__C5D507CC_7784_4449_AFAA_713CDFC9D38A__INCLUDED_)
#define AFX_FLTPAXDATADLG_H__C5D507CC_7784_4449_AFAA_713CDFC9D38A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FltPaxDataDlg.h : header file
//
#include "ConDBListCtrl.h"
#include "..\inputs\in_term.h"
#include "..\inputs\con_db.h"
#include "MoblieElemTips.h"
#include "afxwin.h"
#include "CoolBtn.h"

class CMobileElemConstraintDatabase;
/////////////////////////////////////////////////////////////////////////////
// CFltPaxDataDlg dialog

enum FLTPAXDATATTYPE 
{ 
  FLIGHT_DELAYS, 
  FLIGHT_LOAD_FACTORS, 
  FLIGHT_AC_CAPACITIES, 
  FLIGHT_BOARDING_CALLS,
  PAX_BAG_COUNT, 
  PAX_CART_COUNT, 
  PAX_GROUP_SIZE, 
  PAX_LEAD_LAG, 
  PAX_IN_STEP, 
  PAX_SIDE_STEP, 
  PAX_SPEED, 
  PAX_VISITORS, 
  PAX_VISIT_TIME,
  PAX_RESPONSE_TIME,
  PAX_SPEED_IMPACT,
  PAX_PUSH_PROPENSITY,
  VISITOR_STA_TRIGGER,
  ENTRY_FLIGHT_TIME_DISTRIBUTION
};

static const char* sFltPaxString[] = 
{
	"DELAY",
	"LOAD",
	"CAPACITY",
	"",
	"",
	"",
	"GROUP_SIZE",
	"LEAD_LAG_TIME",
	"IN_STEP",
	"SIDE_STEP",
	"SPEED",
	"",
	"VISIT_TIME",
	"RESPONSE_TIME",
	"SPEED_IMPACT",
	"PUSH_PROPENSITY",
	"VISITOR_STA_TRIGGER",
	"ENTRY_FLIGHT_TIME_DESTRIBUTION"
};
class CFltPaxDataDlg : public CDialog
{
// Construction
public:
	CFltPaxDataDlg( enum FLTPAXDATATTYPE _enumType, CWnd* pParent );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFltPaxDataDlg)
	enum { IDD = IDD_DIALOG_FLTPAX_DATA };
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CConDBListCtrl	m_listctrlData;
	CCoolBtn m_btnImport;
	CButton m_btnOk;
	CButton m_btnCancel;
	CStatic m_Framebarcontenter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFltPaxDataDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowTips( int iItemData );
	void InitTooltips();
	//set styles & show the toolbar
	void InitToolbar();
	

	CToolBar m_ToolBar;

	//CToolTipCtrl m_toolTips;
	CMoblieElemTips m_toolTips;
	int m_nListPreIndex;
	//Get the current selected item index.
	int GetSelectedItem();

	//Get constraint databases for various flight type pointed out in m_enumType.
	ConstraintDatabase* GetConstraintDatabase();

	//Get the project path 
	CString GetProjPath();

	//Get input terminal
	InputTerminal* GetInputTerminal();
	
	//Reload data from database and select the item as parameter.
	void ReloadData( Constraint* _pSelCon );
	
	//Set styles and columns of the list control
	void SetListCtrl();

	enum FLTPAXDATATTYPE m_enumType;

	// Generated message map functions
	//{{AFX_MSG(CFltPaxDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnEndlabeleditListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnToolbarbuttonedit();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDelayImport();
	afx_msg void OnExportData();
	afx_msg void OnLocalProjectOperation();
	afx_msg void OnExportedProjectOperation();
	afx_msg void OnCSVFileOperation();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	void ExportFlightData(ConstraintDatabase* pFlightDatabase,const CString& strFileName,const CString& strFlightName);
	void ExportPaxData(CMobileElemConstraintDatabase* pPaxDatabasae,const CString& strFileName,const CString& strPaxName);
	void ExportBridgeData(AircraftEntryProcessorData* pBridgeDatabase,const CString& strFileName);
public:
	CButton m_btnNeglectSchedData;
	afx_msg void OnBnClickedCheckNeglectscheddata();
	void OnClickMultiBtn();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLTPAXDATADLG_H__C5D507CC_7784_4449_AFAA_713CDFC9D38A__INCLUDED_)
