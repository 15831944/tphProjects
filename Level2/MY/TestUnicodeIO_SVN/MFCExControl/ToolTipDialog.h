//////////////////////////////////////////////////////////////////////////
// ToolTipDialog.h
// Define a dialog that handle tool tip messages.

#pragma once
#include "MFCExControlAPI.h"

// CToolTipDialog dialog

class MFCEXCONTROL_API CToolTipDialog : public CDialog
{
public:
	CToolTipDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CToolTipDialog();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
};
