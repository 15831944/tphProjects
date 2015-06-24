#pragma once
#include "../Common/ALTObjectID.h"

// CDlgAircraftModelItemDefine dialog
class CComponentMeshModel;
class CAircraftComponentModelDatabase;
class CDlgAircraftModelItemDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftModelItemDefine)

public:
	CDlgAircraftModelItemDefine(CAircraftComponentModelDatabase* pAircraftComponentModelDB,CComponentMeshModel* pModel,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftModelItemDefine();

	bool m_bNewComponent;
	void SetNewComponent(bool b){ m_bNewComponent = b; }

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCRAFTMODELITEMDEFINE };

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

	ALTObjectIDList m_lstExistObjectName;
	ALTObjectID m_objName;
	CComponentMeshModel* m_pModel;
	CAircraftComponentModelDatabase* m_pAircraftComponentModelDB;
	CString m_strCreator;
	CString m_sImportFile;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonImport();
};
