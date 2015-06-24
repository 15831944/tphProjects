#pragma once
#include "mfcexcontrol\listctrlex.h"
#include "vehicledispproplistctrlEX.h"
#include "afxwin.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgVechiclProbDisp dialog

class CDlgVechiclProbDisp : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVechiclProbDisp)

public:
	CDlgVechiclProbDisp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVechiclProbDisp();

// Dialog Data
   enum { IDD = IDD_DIALOG_VEHICLEPROBDISP };
   void InitToolbar();
   virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnSave();
	 afx_msg void OnProbDispsAdd();
	 afx_msg void OnProbDispsDel();
	
	 afx_msg void onProbDispDetailAdd();
	 afx_msg void OnProbDispDetailDel();
	
	virtual void OnOK();
	virtual void OnCancel();
	void UpdateDispProbBar();
	void UpdateDispProbDetailBar();
	DECLARE_MESSAGE_MAP()
	void UpdateVehicleprobDetail();
	void UpdateVehicleprob();
public:
	CListCtrlEx m_listNamedSets;
	CVehicleDispPropListCtrlEX m_listctrlProp;
	CToolBar m_DispProbsBar;
	CToolBar m_DispProbDetailBar;
	CButton m_btnOk;
	CButton m_btnSave;
	CButton m_btnCancel;
	CStatic m_btnBarFrame;
	CStatic m_btnBarFrame2;
	CLandSideVehicleProbDispList m_landsideVehicleDispProps;
	afx_msg void OnLvnEndlabeleditListVehicleprobdetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListVehicleprobdetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListVehicleprobs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListVehicleprobs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListVehicleprobs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawRecQueryList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
