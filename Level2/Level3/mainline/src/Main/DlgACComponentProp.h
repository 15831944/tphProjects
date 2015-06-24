#pragma once
#include "afxwin.h"
#include <Common/ARCVector.h>

// CDlgACComponentProp dialog

class CDlgACComponentProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgACComponentProp)

public:
	CDlgACComponentProp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgACComponentProp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SHPAE_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_editPosx;
	CEdit m_editPosY;
	CEdit m_editPosZ;
	CEdit m_editRotateX;
	CEdit m_editRotateY;
	CEdit m_editRotateZ;
	CEdit m_editScaleX;
	CEdit m_editScaleY;
	CEdit m_editScaleZ;

	ARCVector3 m_pos;
	ARCVector3 m_rotate;
	ARCVector3 m_scale;

	virtual BOOL OnInitDialog();
	CStatic m_staticUnit;
};
