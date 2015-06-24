#pragma once
#include "ColorBox.h"
#include "XPStyle/CJColorPicker.h"

// CDlgAirsideObjectDisplayProperties dialog

class ALTObject3D;
class CDlgAirsideObjectDisplayProperties : public CDialog
{
	DECLARE_DYNAMIC(CDlgAirsideObjectDisplayProperties)

public:
	CDlgAirsideObjectDisplayProperties(ALTObject3D *p3DObject, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirsideObjectDisplayProperties();


	enum DSPTYPE
	{
		DSPTYPE_NAME = 0,
		DSPTYPE_SHAPE,
		DSPTYPE_INCONS,
		DSPTYPE_OUTCONS,
		DSPTYPE_DIRECTION,//taxiway
		DSPTYPE_CENTERLINE,
		DSPTYPE_MARKING,
		DSPTYPE_PAD,//deice pad
		DSPTYPE_TRUCK
	};

	static const CString DSPPROPNAME[];

// Dialog Data
	enum { IDD = IDD_ALTOBJECT_DISPLAYPROP };

public:

	ALTObject3D *m_p3DObject;

	CCJColorPicker m_btnColor[6];
	CButton m_chkDisplay[6];
	int		m_nDisplay[6];

	COLORREF	m_cColor[6];
	BOOL m_bApplyDisplay[6];
	BOOL m_bApplyColor[6];

	BOOL m_bSimpleObject;
	int m_nAirGroupID;

	void setZDisplay(int nZDisplay){ m_nDisplay[0] = nZDisplay;}
	int getZDisplay(){ return m_nDisplay[0];}

	void setODisplay(int nODisplay){ m_nDisplay[1] = nODisplay;}
	int getODisplay(){return m_nDisplay[1];}

	void setZColor(COLORREF nZColor) { m_cColor[0] = nZColor;}
	COLORREF getZColor() { return m_cColor[0];}

	void setOColor(COLORREF nOColor) { m_cColor[1] = nOColor;}
	COLORREF getOColor() { return m_cColor[1];}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	
	afx_msg void OnChangeColor(UINT nID);
	afx_msg void OnChkDisplay(UINT nID);
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
	afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
	afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
	afx_msg LONG OnCloseUp(UINT lParam, LONG wParam);
	afx_msg LONG OnDropDown(UINT lParam, LONG wParam);
};


































