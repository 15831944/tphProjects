#pragma once
#include "MFCExControl/XListCtrl.h"
#include "MFCExControl/SortableHeaderCtrl.h"
#include "../Inputs/AODBFieldMapFile.h"
class AODBImportManager;
// CDlgScheduleAndRosterMapFile dialog

class CDlgScheduleAndRosterMapFile : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgScheduleAndRosterMapFile)

public:
	CDlgScheduleAndRosterMapFile(AODBImportManager* pAodbImportManager,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgScheduleAndRosterMapFile();

// Dialog Data
	enum { IDD = IDD_DIALOG_SCHEDULEANDROSTERMAPFILE };

public:
	const CString& GetMapFilePath()const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnToolBarAdd();
	afx_msg void OnToolBarDel();
	afx_msg void OnToolBarEdit();
	afx_msg void OnBtnOpen();
	afx_msg void OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelListCtrlListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnColumnclickListMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDBClickListCtrl(WPARAM wParam,LPARAM lParam);

	void FillListHeader();
	void OnInitToolbar();
	void FillListCtrl();
	void UpdateToolbar();

protected:
	CXListCtrl			m_wndListCtrl;
	CSortableHeaderCtrl m_wndSortHeaderList;
	CToolBar			m_wndToolbar;
	CString				m_strMapPath;
	AODBImportManager*	m_pAodbImportManager;
	AODB::FieldMapFile	m_mapFile;
};
