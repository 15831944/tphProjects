#pragma once
#include "resource.h"
class CAircraftFurnishingModel ;
class CDlgAircraftFurnishingNameSpec :public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftFurnishingNameSpec)
public:
	CDlgAircraftFurnishingNameSpec(CAircraftFurnishingModel* _furnish , CWnd* pParent = NULL);
	virtual ~CDlgAircraftFurnishingNameSpec(void);
	void SetNew(bool b){ m_bNew = b; }
	enum { IDD = IDD_DIALOG_FURNISHING_NAME_DEF };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual afx_msg BOOL OnInitDialog() ;
protected:
	CComboBox m_Combo_level1;
	CComboBox m_Combo_level2;
	CComboBox m_Combo_level3;
	CComboBox m_Combo_level4;
	CComboBox m_Combo_Type;
	CEdit m_Creator ;
	CString m_sImportFile;
	bool m_bNew;
protected:
	CAircraftFurnishingModel* m_Furnishing ; 
protected:
	void OnOK() ;
	void OnCancel() ;
	void SaveData() ;

	virtual void InitFurnishingType() ;
	CString GetCurSelLevel1Name() ;
	CString GetCurSelLevel2Name() ;
	CString GetCurSelLevel3Name() ;
	CString GetCurSelLevel4Name() ;
	CString GetCurSelLevel5Name() ;
	CString GetCreator() ;
	CString GetType() ;
	virtual void SetTimeString() ;
public:
	afx_msg void OnStnClickedStaticLine3();
	afx_msg void OnBnClickedButtonImport();
};
class CDlgAircraftFurnishingNameSpecEdit : public CDlgAircraftFurnishingNameSpec
{
public : 
	CDlgAircraftFurnishingNameSpecEdit(CAircraftFurnishingModel* _furnish , CWnd* pParent = NULL) ;
	~CDlgAircraftFurnishingNameSpecEdit() ;
protected:
	afx_msg BOOL OnInitDialog() ;
public:
    void SetTimeString() ;
};

