#pragma once


#include "coolbtn.h"
#include "../MFCExControl/ARCTreeCtrlEx.h"
#include "../common/FLT_CNST.H"
#include "../common/StartDate.h"

class COnboardReportPerformer;
class OnboardBaseParameter;
class CTermPlanDoc;

// OnboardReportControlView form view

class OnboardReportControlView : public CFormView
{
	DECLARE_DYNCREATE(OnboardReportControlView)

protected:
	OnboardReportControlView();           // protected constructor used by dynamic creation
	virtual ~OnboardReportControlView();

public:
	enum { IDD = IDD_DIALOG_REPORT_SPECS_ONBOARD };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


public:


protected:
	CComboBox	m_comboEndDay;
	CComboBox	m_comboStartDay;
	CARCTreeCtrlEx	m_treePaxType;
	CButton	m_staticTime;
	CStatic	m_staticStartTime;
	CStatic	m_staticEndTime;
	CCoolBtn	m_btnMulti;
	CButton	m_btLoad;
	CDateTimeCtrl	m_dtctrlThreshold;
	CDateTimeCtrl	m_dtctrlInterval;
	CSpinButtonCtrl	m_spinThreshold;
	CSpinButtonCtrl	m_spinInterval;
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
	CEdit	m_edtInterval;//for integer 
	CButton	m_chkThreshold;
	CButton	m_btnApply;
	CButton m_btnLoadDefault;
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



	CToolBar m_PasToolBar;

	HTREEITEM m_hRootFlightType;
	HTREEITEM m_hRootPaxType;
protected:
	OnboardBaseParameter *m_pParameter;
	CStartDate m_startDate;
	CTermPlanDoc* GetDocument(){ return (CTermPlanDoc*) m_pDocument; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ResetAllContent();

protected:
	void InitMuiButtom();
	void ClickMuiButton();
	void OnSaveReport() ;
	void OnLoadReport() ;

	void DoSizeWindow( int cx, int cy );
	void SizeDownCtrl(int cx, int cy );
	void InitToolbar();
	void InitializeControl();
	void ShowOrHideControls();

	afx_msg void OnReportSavePara();
	afx_msg void OnReportLoadPara();
	afx_msg void OnButtonReportDefault();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonCancel();
	afx_msg void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolBarDel(CCmdUI *pCmdUI);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	void EnableControl();
	void InitializeDefaultTreeData();
	void InitializeTree();
	BOOL SaveData();
	void UpdateControlContent();
	void LoadData();

	COnboardReportPerformer* GetTreePerformer();
protected:
	//the tree control's performer, it can be different for different report type	
	COnboardReportPerformer *m_pTreePerformer;
};


