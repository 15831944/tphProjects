#pragma once
#include "Inputs/MultiMobConstraint.h"
#include "../MFCExControl/XTResizeDialog.h"

class CTermPlanDoc;

// CDlgMEDefine dialog

class CDlgMEDefine : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgMEDefine)

public:
	CDlgMEDefine(CTermPlanDoc* pTermDoc,bool bGenerate,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMEDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_MEDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnOK();

	afx_msg void OnAddMoblieType();
	afx_msg void OnDeleteMobileType();
	afx_msg void OnLbnSelchangeListData();
	DECLARE_MESSAGE_MAP()
	
	void OnInitToolbar();
	void LoadMeContent();
	void UpdateToolbarStatus();

public:
	int GetSelectConstraint()const {return m_nIdxCon;}
private:
	CToolBar m_wndToolBar;
	CListBox m_wndListBox;
	CTermPlanDoc* m_pTermDoc;
	CMultiMobConstraint  m_createMeList;
	int m_nIdxCon;
	bool m_bGenerate;
};
