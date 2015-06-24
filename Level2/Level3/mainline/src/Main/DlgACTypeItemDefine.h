#pragma once
#include "../Common/ALTObjectID.h"
class CACTypesManager;
class CACType;
// CDlgACTypeItemDefine dialog

class CDlgACTypeItemDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgACTypeItemDefine)

public:
	CDlgACTypeItemDefine(CACTypesManager* _pAcMan,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgACTypeItemDefine();
	void SetNewComponent(bool b){ m_bNewComponent = b; }

// Dialog Data
	enum { IDD = IDD_DIALOG_ACTYPEITEMDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnCbnKillfocusComboNameLevel1();
	afx_msg void OnCbnDropdownComboNameLevel1();
	afx_msg void OnCbnDropdownComboNameLevel2();
	afx_msg void OnCbnDropdownComboNameLevel3();
	afx_msg void OnCbnDropdownComboNameLevel4();
protected:
	void InitComboBox();
protected:
	CComboBox m_cmbName1;
	CComboBox m_cmbName2;
	CComboBox m_cmbName3;
	CComboBox m_cmbName4;

	
	CACTypesManager* m_pACMan;
	ALTObjectIDList m_lstExistObjectName;
	ALTObjectID m_objName;
	CString m_strCreator;

	bool m_bNewComponent;
public:
	CString m_sImportFile;
	ALTObjectID getACTypeName(){return m_objName;}
	CString getCreator(){return m_strCreator;}
	afx_msg void OnBnClickedButtonImport();
};
