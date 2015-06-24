#pragma once

#include "ColorBox.h"
#include "XPStyle/CJColorPicker.h"
#include <common/DispProperties.h>

// CDlgOnBoardObjectDisplayProperty dialog
class OnBoardObjectDisplayProperty;
class CAircraftElement;
class CAircraftElmentShapeDisplay;

class CDlgOnBoardObjectDisplayProperty : public CDialog
{
	DECLARE_DYNAMIC(CDlgOnBoardObjectDisplayProperty)

	
	static const CString DSPPROPNAME[];
public:
	CDlgOnBoardObjectDisplayProperty(CAircraftElmentShapeDisplay* pElement , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOnBoardObjectDisplayProperty();
	CDispProperties mDispPropEdit;
	CAircraftElmentShapeDisplay* m_pElement;

// Dialog Data
	enum { IDD = IDD_ONBOARD_DISPLAYPROP };

protected:
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
