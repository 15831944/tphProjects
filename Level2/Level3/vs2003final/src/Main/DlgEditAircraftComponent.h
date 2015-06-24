#pragma once

#include "../Onboard3d/ARCOgreControl.h"
// CDlgEditAircraftComponent dialog

class CDlgEditAircraftComponent : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditAircraftComponent)

public:
	CDlgEditAircraftComponent(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditAircraftComponent();

	ARCOgreControl m_ctrl1;
	ARCOgreControl m_ctrl2;

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_COMPONENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
