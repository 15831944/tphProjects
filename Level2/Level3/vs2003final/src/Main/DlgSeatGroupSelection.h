#pragma once

#include "../MFCExControl/CoolTree.h"
#include "../Common/ALTObjectID.h"
class CSeatDataSet;
class CSeat;
// CDlgSeatGroupSelection dialog

class CDlgSeatGroupSelection : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSeatGroupSelection)

public:
	CDlgSeatGroupSelection(CSeatDataSet* pSeatDataSet,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatGroupSelection();

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
	ALTObjectID getSeatGroup();
protected:
	CSeatDataSet* m_pSeatDataSet;
	CSeat* m_pSeat;
	ALTObjectIDList m_ObjIDList;

	CTreeCtrl m_wndTreeCtrl;
	ALTObjectID m_objID;
};
