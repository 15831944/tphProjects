#pragma once
#include "afxwin.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgAirsideIntersectionSelect dialog

class CDlgAirsideIntersectionSelect : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAirsideIntersectionSelect)

public:
	CDlgAirsideIntersectionSelect(int nAirPortID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirsideIntersectionSelect();

	void SetListContent();
	int GetSelIntersectionID(){return m_nSelectIntersectionID;}
// Dialog Data
	enum { IDD = IDD_DIALOG_INTERSECTION_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CListBox m_listBoxIntersection;
	int m_nSelectIntersectionID;
	int m_nAirPortID;

public:
	afx_msg void OnLbnSelchangeList2();
};
