#pragma once


// CDlgPositionEdit dialog
#include "../Common/Point2008.h"
#include "../Common/CARCUnit.h"
class CDlgPositionEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgPositionEdit)

public:
	CDlgPositionEdit(ARCUnit_Length emType,const CPoint2008& pt,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPositionEdit();

	CPoint2008 GetPoint();

// Dialog Data
	enum { IDD = IDD_DIALOG_POSITIONEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	

	DECLARE_MESSAGE_MAP()

protected:
	double m_dPtX;
	double m_dPtY;
	double m_dPtZ;
	CPoint2008 m_pt;
	ARCUnit_Length m_emType;
};
