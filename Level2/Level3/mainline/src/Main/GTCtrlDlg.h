#pragma once
#include "gtchartctrl.h"
#include "../Inputs/procdist.h"
#include "../MFCExControl/TabPageSSL.h"
// CGTCtrlDlg dialog
class CProcAssignDlg ;
class CGTCtrlDlg : public CTabPageSSL
{
	DECLARE_DYNAMIC(CGTCtrlDlg)

public:
	CGTCtrlDlg(CProcAssignDlg* _prodlg,CWnd* pParent = NULL);   // standard constructor
	virtual ~CGTCtrlDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_GT };
    BOOL OnInitDialog() ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CGtchartctrl m_gtCtrl;
	CProcAssignDlg* m_mainDlg ;
protected:
	void InitGTCtrl() ;
	int nDay ;
public:
	//the function Init the m_gCtrl 
    	afx_msg void OnSize(UINT nType, int cx, int cy) ;
	void DeleteSelRosterFromGt(ProcessorRoster* _Ass) ;
	void InitGTCtrlData() ;
	void AddLineToGT(ProcessorRoster* _Ass,ProcessorID& id) ;
	void AddItemToGT(int lineId , ProcessorRoster* _Ass);
	void InitAllProcessorData();

	void InsertRosterOfProcessor(HTREEITEM ProcessNode );
	void InitGTCtrlData(HTREEITEM ProcessNode);

	void AddProcessor(ProcessorRosterSchedule* _schedule,ProcessorID& id) ;
};
