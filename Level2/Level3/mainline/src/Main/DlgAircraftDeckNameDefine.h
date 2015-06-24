#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgAircraftDeckNameDefine dialog
class CDeck ;
class CDlgAircraftDeckNameDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftDeckNameDefine)

public:
	CDlgAircraftDeckNameDefine(CDeck* _deck , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftDeckNameDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCONFIG_DECK };
protected:
	CDeck* m_deck ;
protected:
	virtual BOOL OnInitDialog() ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_level1;
	CComboBox m_combo_level2;
	CComboBox m_combo_level3;
	CComboBox m_combo_level4;
	CEdit m_Ground;
	CEdit m_thickness;
protected:
	void OnInitComboxName() ;
	void OnInitEdit() ;
	void OnInitCaption() ;
protected:
	void OnOK() ;
	void OnCancel() ;
protected:
	void GetLevel1Name() ;
	void GetLevel2Name() ;
	void GetLevel3Name() ;
	void GetLevel4Name() ;

	void GetGround() ;
	void GetThicknessVal() ;
	CSpinButtonCtrl m_ground_spin;
	CSpinButtonCtrl m_thickness_spin;
public:
	afx_msg void OnNMThemeChangedSpinGround(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedSpinThickness(NMHDR *pNMHDR, LRESULT *pResult);
};
class CDlgAircraftDeckNameEdit :public CDlgAircraftDeckNameDefine
{
public:
	CDlgAircraftDeckNameEdit(CDeck* _deck , CWnd* pParent = NULL):CDlgAircraftDeckNameDefine(_deck,pParent)
	{

	} ;
	~CDlgAircraftDeckNameEdit() {};
protected:
	virtual BOOL OnInitDialog() 
	{
		return CDlgAircraftDeckNameDefine::OnInitDialog() ;
	}
};