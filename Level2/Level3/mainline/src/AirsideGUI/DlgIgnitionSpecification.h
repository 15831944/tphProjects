#pragma once
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"

const CString INITIATEENGINESTART[] = 
{
	_T("Commencing Push back"),
		_T("Ready to power out"),
		_T("Completion of push back"),
		_T("After unhook of push tractor"),
		_T("At release point"),
		_T("After unhook by tow tractor"),
		_T("Within a distance of release point"),

		//add new one according to enumeration TOWOPERATIONTYPE
		_T("Count")
};

class IgnitionSpeicification;
class CTowOperationSpecification;
class DlgIgnitionSpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgIgnitionSpecification)
public:
	DlgIgnitionSpecification(int nPrjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirportDatabase , CWnd* pParent = NULL);
	~DlgIgnitionSpecification(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();	
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	void InitToolbar();
	void SetTreeCtrl();
public:
	afx_msg void OnNewItem(void);
	afx_msg void OnDeleteItem(void);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult); 
private:
	CToolBar m_wndToolBar;
	CARCTreeCtrl m_wndTreeCtrl;
	PSelectFlightType	m_pSelectFlightType;
	CAirportDatabase*  m_pAirportDatabase;
	IgnitionSpeicification* m_pIgnitionSpecification;
	CTowOperationSpecification* m_pTowOperationSpecification;
	int m_nPrjID;
};
