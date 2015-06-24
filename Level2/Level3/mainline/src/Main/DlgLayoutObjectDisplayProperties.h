#pragma once



#include "ColorBox.h"
#include "XPStyle/CJColorPicker.h"

class LandsideFacilityLayoutObjectList;

// CDlgAirsideObjectDisplayProperties dialog

class ILayoutObjectDisplay;
class CDlgLayoutObjectDisplayProperties : public CDialog
{
	DECLARE_DYNAMIC(CDlgLayoutObjectDisplayProperties)

public:
	CDlgLayoutObjectDisplayProperties(LandsideFacilityLayoutObjectList* pLayoutObjectList, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLayoutObjectDisplayProperties();
	// Dialog Data
	enum { IDD = IDD_ALTOBJECT_DISPLAYPROP };

public:	
	
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

//	ILayoutObjectDisplay* p3DProp;
	LandsideFacilityLayoutObjectList* m_pLayoutObjectList;
};

