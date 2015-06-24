#pragma once

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "MFCExControl/XListCtrl.h"
#include "../Inputs/AODBImportManager.h"
#include "../Inputs/aodbbasedataitem.h"
#include "../Inputs/AODBRosterFile.h"
#include "../Inputs/AODBScheduleFile.h"

class CTermPlanDoc;

class CTrackWnd;

//just make parent can accept left button double click message
//class CARCportTipWindow : public CXTTipWindow
//{
//public:
//	CARCportTipWindow();
//	~CARCportTipWindow();
//
//	DECLARE_MESSAGE_MAP()
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//
//protected:
//	void ForwardMessage(UINT uiMsg, UINT nFlags, CPoint point);
//};

class CARCportTabCtrl : public CTabCtrl/* public CXTTabCtrl*/
{
public:
	CARCportTabCtrl(AODBImportManager* aodbImportMgr);
	~CARCportTabCtrl(void);

public:
	BOOL Create(CWnd* pWnd,const CRect& rect);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetTitle(const CString& szSchedule, const CString& szRoster);
	void SetRealTitle(const CString& szSchedule, const CString& szRoster);
	void LoadListData(AODBImportManager& aodbImportManager);
protected:
	BOOL InitRosterList();
	void FillRosterList(AODBImportManager& aodbImportManager);
	BOOL InitScheduleList();
	void FillScheduleList(AODBImportManager& aodbImportManager);
	void MouseOverListCtrl(int id,CPoint point);
	void SetScheduleItemColor(int nItem, int nSubItem,const AODB::MapField& pMapField);
	void SetRosterItemColor(int nItem, int nSubItem,const AODB::MapField& pMapField);
	void UpdateTableDisplay(int nSel);
	void UpdateTabLabel(int iTab, const CString& strLable);
protected:
	CXListCtrl				m_wndScheduleListCtrl;
	CXListCtrl				m_wndRosterListCtrl;
	CImageList				m_imgList;


//	CARCportTipWindow			m_tipWindow;

	CSortableHeaderCtrl		m_ctlHeaderCtrlSchedule;
	CSortableHeaderCtrl		m_ctlHeaderCtrlRoster;

	CTrackWnd*				m_pTrackWnd;

	CString					m_strScheduleTitle;
	CString					m_strRosterTitle;

	AODBImportManager*		m_pAodbImportMgr;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLvnEndlabeleditScheduleListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditRosterListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnColumnclickListRoster(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListSchedule(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDBClickListCtrl(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSelChangeTabList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
