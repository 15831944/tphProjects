#pragma once
#include "afxcmn.h"
#include <XTToolkitPro.h>
#include "ACTypeStandConstraintImportListCtrl.h"
// DlgImportACTypeConstraintFromFile dialog
class ACTypeStandConstraintList;
class ALTObjectID;
class DlgImportACTypeConstraintFromFile : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgImportACTypeConstraintFromFile)

public:
	DlgImportACTypeConstraintFromFile(int nAirportID,ACTypeStandConstraintList* pConstraintList, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgImportACTypeConstraintFromFile();

// Dialog Data
	enum { IDD = IDD_DLGIMPORTACTYPECONSTRAINTFROMFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();




protected:
	void MouseOverListCtrl(CPoint point);
	bool ReadFile( const CString& strFileName );
	bool IsValidateStandGroup(const ALTObjectID& objID );
	void SetListCtrl();
	void InitListCtrl();

protected:
	ACTypeStandConstraintImportListCtrl m_wndListCtrl;
    CXTTipWindow			m_tipWindow;
	CProgressCtrl m_wndProgress;
	CStringArray m_ConstraintArray;
	ACTypeStandConstraintList* m_pACTypeStandCons;
	int m_nAirportID;

};
