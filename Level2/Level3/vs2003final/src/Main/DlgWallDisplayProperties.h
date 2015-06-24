#ifndef __DLG_WALLDISPLAYPROPERTIES_
#define __DLG_WALLDISPLAYPROPERTIES_
#pragma once

#include "../Inputs/WallShape.h"
#include "ColorBox.h"
#include "XPStyle/CJColorPicker.h"

// CDlgWallDisplayProperties dialog

class CDlgWallDisplayProperties : public CDialog
{
	DECLARE_DYNAMIC(CDlgWallDisplayProperties)

public:
	CDlgWallDisplayProperties(WallShape* pWallShape, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWallDisplayProperties();

// Dialog Data
	enum { IDD = IDD_WALLDISPPROPERTIES };
	CButton m_chkDisplay[WSDP_ARRAY_SIZE];
	int		m_nDisplay[WSDP_ARRAY_SIZE];
	CCJColorPicker m_btnColor[WSDP_ARRAY_SIZE];



	COLORREF	m_cColor[WSDP_ARRAY_SIZE];


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();


private:
	WallShape *m_pWallShape;
};

#endif//__DLG_WALLDISPLAYPROPERTIES_