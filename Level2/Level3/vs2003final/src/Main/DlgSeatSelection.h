#pragma once

#include "../MFCExControl/CoolTree.h"
#include "../Common/ALTObjectID.h"
class CSeatDataSet;
class CSeat;
// CDlgSeatSelection dialog

class CDlgSeatSelection : public CDialog
{
	DECLARE_DYNAMIC(CDlgSeatSelection)

public:
	CDlgSeatSelection(CSeatDataSet* pSeatDataSet,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatSelection();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEATSELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
public:
	void InitTreeCtrl();
	CSeat* getSeat();
	void InitIDNameList();
protected:
	CSeatDataSet* m_pSeatDataSet;
	CSeat* m_pSeat;
	ALTObjectIDList m_ObjIDList;

	CTreeCtrl m_wndTreeCtrl;
};
