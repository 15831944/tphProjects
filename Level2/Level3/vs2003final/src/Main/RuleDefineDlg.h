#pragma once

#include "RosterRules.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "../Inputs/assigndb.h"
#include "../Inputs/IN_TERM.H"
#include "../Inputs/schedule.h"
#include "../Main/PassengerTypeDialog.h"
#include "afxcmn.h"
// CRuleDefineDlg dialog
class CRuleDefineDlg : public CDialog
{
	DECLARE_DYNAMIC(CRuleDefineDlg)

public:
	CRuleDefineDlg(CRosterRule* p_rule,InputTerminal* P_input,int cout_Pro,CWnd* pParent = NULL);   // standard constructor
	virtual ~CRuleDefineDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_RULE_DEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog() ;
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Edit_PaxTY;
protected:
	CComboBox m_ComboStartDay;
	CComboBox m_ComboEndDay;
public:
	CDateTimeCtrl m_start_TimeCtrl;
protected:
	CDateTimeCtrl m_end_TimeCtrl;
	CButton m_Daily_Check;
protected:
	InputTerminal*  InputTer ;
	CRosterRule* P_Rule ;
	COleDateTime m_startDate;
	COleDateTime m_endDate;
	BOOL isDaily ;
	int num_Pro ;
	CMobileElemConstraint m_mobCon  ;
protected:
		void InitComboBeginAndEndDay(int n_day,	int nStartDays ,int nenddays) ;
public:
	afx_msg virtual void OnBnClickedCheckDaily();
	afx_msg void OnBnClickedButtonPaxty();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
protected:
	CSpinButtonCtrl m_spin_num;
protected:
	CRect pos_starttimeCtr ;
	CRect pos_endtimeCtr ;
	void HideComboBoxTime() ;
	void showComboBoxTime() ;
	CStatic m_staTime;
	CStatic m_staEndTime;
	int m_TY ;
	CString m_Caption ;
public:
	void SetDlgCaption(const CString& _str) 
	{
		m_Caption = _str ;
	}
protected:
	CComboBox m_Com_Start;
	CComboBox m_Com_end;

	CButton m_CheckBoxNumberOfPro;
public:
	afx_msg void OnBnClickedCheckDefineNumber();
protected:
	CStatic m_StaticNumber;
public:
	afx_msg void OnCbnSelchangeComboEnd();
protected:
	CEdit m_editNum;
};

class CRuleByScheduleDefineDlg : public CRuleDefineDlg
{
public : 
     CRuleByScheduleDefineDlg(CRosterRule* p_rule,InputTerminal* P_input,int cout_Pro,CWnd* pParent = NULL) ;
	 ~CRuleByScheduleDefineDlg() ;
	// void OnBnClickedCheckDaily();
	 int m_day ;
protected:
     BOOL OnInitDialog() ; 
protected:
	void InitAheadCombox(CComboBox& _comboxCtrl) ;
};
