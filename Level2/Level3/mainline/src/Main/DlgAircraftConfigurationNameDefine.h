#pragma once
#include "afxwin.h"


// CDlgAircraftConfigurationNameDefine dialog
class CAircaftLayOut ;
class InputTerminal;
class CDlgAircraftConfigurationNameDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftConfigurationNameDefine)

public:
	CDlgAircraftConfigurationNameDefine(CAircaftLayOut* _layout ,InputTerminal* _terminal ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftConfigurationNameDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCONFIG_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	CAircaftLayOut* m_layout ;
	InputTerminal* m_Terminal ;
protected:
	virtual BOOL OnInitDialog() ;
	void OnOK() ;
    void OnCancel() ;
protected:
	CComboBox m_Combo_level2;
protected:
	CString GetCurSelLevel1Name() ;
	CString GetCurSelLevel2Name() ;
	CString GetCurSelLevel3Name() ;
	CString GetCurSelLevel4Name() ;
	CString GetCreator() ;
	virtual void SetTimeString() ;
	CComboBox m_Combo_level1;
	CComboBox m_Combo_level4;
	CComboBox m_Combo_level3;
	CEdit m_edit_Creator;
};
class CDlgAircraftConfigurationNameEdit : public CDlgAircraftConfigurationNameDefine
{
public:
     CDlgAircraftConfigurationNameEdit(CAircaftLayOut* _layout ,InputTerminal* _terminal ,CWnd* pParent = NULL);
	 virtual ~CDlgAircraftConfigurationNameEdit() ;
protected:
	void SetTimeString() ;
	BOOL OnInitDialog() ;
};