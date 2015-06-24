#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../Inputs/procdist.h"
#include "../MFCExControl/ListCtrlReSizeColum.h"
#include "../Common/elaptime.h"
#include "ProcAssignDlg.h"
// CRosterList dialog
// CProcAssignDlg dialog

class CRosterList : public CTabPageSSL
{
	DECLARE_DYNAMIC(CRosterList)

public:
	CRosterList(CProcAssignDlg* Prodlg ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CRosterList();

// Dialog Data
	enum { IDD = IDD_DIALOG_ROSTERLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
    void InitRosterToolBar() ;
	DECLARE_MESSAGE_MAP()
	CSortListCtrlEx m_rosterList;
protected:
		std::vector<CString> m_RosterListColNames ;
protected:

		CProcAssignDlg*  m_mainDlg ;
private:
	CToolBar m_RosterToolBar ;
public:
	afx_msg void OnHdnItemclickList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	/*afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnPeoplemoverChange();*/
	afx_msg void OnSelListItem(NMHDR *pNMHDR, LRESULT *pResult) ;
	//afx_msg void ImportFileForRosterList() ;
	afx_msg void ExportFileFromRosterList() ;
	afx_msg void OnSize(UINT nType, int cx, int cy) ;
//    afx_msg BOOL OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult) ;
	BOOL OnInitDialog() ;
	afx_msg void OnPrintRosterList() ;
public:
	void SaveData() ;
public:
	//the function Init roster list 
	void InitRosterListData() ;
	void InitRosterList() ;
	void InitALLProcessorData() ;
protected:
	void InitRosterListView() ;
	void InitRosterListData(HTREEITEM ProcessNode ) ;
	void InsertRosterOfProcessor(HTREEITEM ProcessNode ) ;
	void AddLineToRosterList(ItemRosterList& _data,ProcessorRoster* _PAss) ;
	//void DeleteSelLineFromRosterList() ;
	//void EditSelRosterFromRosterList(ItemRosterList& _data) ;
	InputTerminal* GetInputTerminal() ;
    ProcessorRosterSchedule* GetCurSelProcSchdFromRosterList() ;
	CToolBar* GetTooBar () {return &m_RosterToolBar ;} ;
	 CSortListCtrlEx* GetRosterListCtrl() { return &m_rosterList ;} ;
protected:
	CStatic mfg;

protected:


	BOOL WriteRosterToFile(const CString& filename) ;
	void GetStringArrayFromRosterList(CStringArray& strArray);

	//bool ReadTextFromFile(const CString &_szFileName) ;
	//void AddRosterFromStrList(const CStringArray& strItem) ;
	//int  getColNumFromStr(CString strText, CString strSep, CStringArray &strArray) ;
};
