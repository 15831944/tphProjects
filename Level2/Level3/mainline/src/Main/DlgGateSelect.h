#pragma once
#include "AllProcessorTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgGateSelect dialog
class InputTerminal ;
class CDlgGateSelect : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgGateSelect)

public:
	CDlgGateSelect(InputTerminal* _pInTerm,int _Type ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGateSelect();

// Dialog Data
	enum { IDD = IDD_DIALOG_GATE_SELECT };

protected:
	InputTerminal* m_pInTerm;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
   CProcessorTreeCtrl m_treeProc;
   int m_Type ;
	CString m_Caption ;
	CString m_SelGateName ;
protected:
	BOOL OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
	void InintListBox() ;
	DECLARE_MESSAGE_MAP()
public:
	CString GetSelGateName() { return m_SelGateName ;} ;
    void OnOK() ;   
};
