#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "commondata/procid.h"
#include "../MFCExControl/XTResizeDialog.h"


// CAutoRosterRules dialog
class CProcRosterRules;
class CAutoRosterRule;
class CRosterRulesByAbsoluteTime;
class CPriorityRule;
class ProcAssignEntry;
class ProcessorID;
class InputTerminal;
class CDlgAutoRosterRules : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAutoRosterRules)

public:
	CDlgAutoRosterRules(const ProcessorID& selProcID ,std::vector<ProcessorID> vSelectedProcID, InputTerminal* pInTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAutoRosterRules();

// Dialog Data
	enum { IDD = IDD_DLGAUTOROSTERRULES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual BOOL OnInitDialog();

	afx_msg void OnRulesNew() ;
	afx_msg void OnRulesEdit() ;
	afx_msg void OnRulesDelete() ;

	afx_msg void OnPriorityNew() ;
	afx_msg void OnPriorityDelete() ;

	afx_msg void OnLbnSelchangeListPaxType();

	afx_msg void OnBnClickedShowGranttChart();
private:
	void initProcessorName(const ProcessorID& groupID);
    void InitToolBar() ;
	void InitPriorityTree();
	void SetListContent();
	void InitRuleTimeSetting();
	void InitDailySetting(CRosterRulesByAbsoluteTime* pRule);

	void GenerateProcRoster();

	void NewPriority();
	void NewProcessor(CPriorityRule* pPriority);

	void SetPriorityItem(CPriorityRule* pPriority);

	void GetProcAssignEntries(std::vector<ProcAssignEntry*>& vProAssignEntries);


private:
	CListBox m_listPaxType;
	CButton m_btnRelativeToSched;
	CButton m_btnDaily;
	//CDateTimeCtrl m_ctlOpenTimeBySched;
	//CDateTimeCtrl m_ctlCloseTimeBySched;
	//CDateTimeCtrl m_ctlOpenTimeByAbsolute;
	//CDateTimeCtrl m_ctlCloseTimeByAbsolute;
	CComboBox m_cmbOpenType;
	CComboBox m_cmbCloseType;
	CComboBox m_cmbOpenDay;
	CComboBox m_cmbCloseDay;
	CButton m_btnForEachFlight;
	CButton m_btnNumProc;
	CSpinButtonCtrl m_spinNumProc;
	CARCTreeCtrl m_ctlPriorityTree;

	CToolBar m_PaxTypeToolBar;
	CToolBar m_PriorityToolBar;
	CStatic m_StaticPaxType;
	CStatic m_StaticPriority;

	const ProcessorID m_selProcID ;
	std::vector<ProcessorID> m_vProcList;
	InputTerminal* m_pInTerm ;
	CProcRosterRules* m_pProcRosterRules;
	CAutoRosterRule* m_pCurRosterRule;

	CDateTimeCtrl m_ctlOpenTime;
	CDateTimeCtrl m_ctlCloseTime;

	CRect m_rcOpenTime;
	CRect m_rcCloseTime;


public:
	afx_msg void OnBnClickedCheckByschedtime();
	afx_msg void OnBnClickedCheckDailytime();
	afx_msg void OnBnClickedCheckNumproc();
	afx_msg void OnNMSetfocusTreePriority(NMHDR *pNMHDR, LRESULT *pResult);

	
public:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void DisableSetting();
	void SaveTimeSettingToCurrentRule();
	void SaveRosterRules();
public:
	afx_msg void OnEnKillfocusEditNumproc();
	afx_msg void OnTvnSelchangedTreePriority(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckForeachflight();

	afx_msg void OnSize(UINT nType, int cx, int cy);
};
