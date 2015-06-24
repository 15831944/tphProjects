#pragma once

#include "SelectALTObjectDialog.h"

class CSelectStandDialog :public CSelectALTObjectDialog
{
	DECLARE_DYNAMIC(CSelectStandDialog)

public:
	CSelectStandDialog(int prjID, int airportID, CWnd* pParent = NULL );
	virtual ~CSelectStandDialog(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSelect();

	DECLARE_MESSAGE_MAP()
};
