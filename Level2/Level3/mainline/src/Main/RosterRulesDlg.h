#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "RosterRules.h"
#include "RuleDefineDlg.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "../Inputs/assigndb.h"
// CRosterRulesDlg dialog
#include "../MFCExControl/ListCtrlReSizeColum.h"
#include "../MFCExControl/XTResizeDialog.h"
#define  RULE_APPEND 0
#define  RULE_REPLACE 1 
class CRosterRulesDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CRosterRulesDlg)
public:
	CRosterRulesDlg(ProcessorID& _curSelProID ,InputTerminal* P_inputter ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CRosterRulesDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG24 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog() ;
	DECLARE_MESSAGE_MAP()
public:
	CStatic n_sta_new;
	CStatic n_sta_del;
	CStatic n_sta_edit;
	CToolBar m_ToolBar;
	afx_msg void OnBnClickedCheckBySchedule();
	afx_msg void OnBnClickedButtonAppend();
	afx_msg void OnBnClickedButtonReplace();
	afx_msg void OnBnClickedButtonClose();
	void ONBnClickButtonSave();
	afx_msg void OnLvnItemchangedListRosterRules(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
protected:
	afx_msg void OnRulesNew() ;
	afx_msg void OnRulesEdit() ;
	afx_msg void OnRulesDelete() ;
	afx_msg void OnSize(UINT nType, int cx, int cy);
    void InitToolBar() ;
	void AddRosterToProcessor(int oper) ;
    //get current processor to apply
	int GetCurrentProcessor() {return current ;} ;
	void MoveNextProcessor() { current++ ;} ;
private:
	CAutoRosterRulesDB* p_rulesDB;
	CAutoRosterByScheduleRulesDB* p_ruleSchedule;
    CProcessGroupRules* p_GroupRules ;
	CProcessGroupRules* p_RulesForSchedule ;
	std::vector<CString> columNames ;
	int columNum ;
	InputTerminal* P_input ;
	int cout_proc ;
	const ProcessorID* proGroup_ID ;
	std::vector<ProcessorID> proName_list ;
	int current ;
public:
	afx_msg void OnHdnItemclickListRosterRules(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	void initProcessorName(const ProcessorID& groupID) ;
protected:
	std::vector<CString> m_RuleListColNames ;
    std::vector<CString> m_RuleScheduleListColNames ;
	CSortListCtrlEx m_rulesList ;
	CSortListCtrlEx m_ruleList_Schedule ;
	CButton m_check_schedule;
protected:
	void InitRuleScheduleList() ;
	void InitRuleScheduleListView() ;
	void InitRuleScheduleListData() ;
	void AddLineToRuleScheduleList(CRosterRule* _rule) ;
	void EditSelLineFromRuleScheduleList(CRosterRule* _rule) ;
	void DelSelLineOfRuleScheduleList() ;
	//the function Init rules list and roster list 
	void InitRulesList() ;
	void InitRulesListView() ;
	void InitRulesListData() ;
	void AddLineToRulesList(CRosterRule* _rule);
	void EditSelLineOfRulesList(CRosterRule* _rule) ;
	void DelSelLineOfRulesList() ;

    void SelAllRuleByScheduleList(BOOL sel = TRUE) ;
	void SelAllRuleList(BOOL sel = TRUE) ;
protected:
	void NewRulesForProcessor() ;
	void EditRulesForProcessor() ;
	void DelRulesForProcessor() ;

	//////////////////////////////////////////////////////////////////////////
	void NewRulesByScheduleForProcessor() ;
	void EditRulesByScheduleForProcessor() ;
	void DelRulesByScheduleForProcessor() ;
	CString GetRelationName(int _rel) ;
	CString GetIsDailyName(int _data) ;
	ProcAssRelation GetRelationByName(CString name);
	CButton m_check_Sel;
public:
//	afx_msg void OnBnClickedCheckCheckall();

protected:
protected:
	void ApplyRulesScheduleToRoster(int oper) ;
	void ApplyRulesToRoster(int oper) ;
	void GetProAssignScheduleOfCurrentSelPro(std::vector<ProcessorRosterSchedule*>& _AssSchedule ,int oper);
	void GetCurrentSelRules(std::vector<CRosterRule*>&  _selRules);
	CStatic m_StaRules;

	CString FormatOpenTimeForRuleScheduleList(CRosterRule* _rule) ;
	CString FormatCloseTimeForRuleScheduleList(CRosterRule* _rule) ;
};
