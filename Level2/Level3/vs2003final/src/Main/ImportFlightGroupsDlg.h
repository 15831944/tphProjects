#pragma once
#include <MFCExControl/SplitterControl.h>
#include "HeaderCtrlDrag.h"
// CImportFlightGroupsDlg dialog
class InputTerminal;
class FlightGroup;

class CImportFlightGroupsDlg : public CDialog
{
	DECLARE_DYNAMIC(CImportFlightGroupsDlg)

public:
	CImportFlightGroupsDlg(InputTerminal* _pInTerm,CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportFlightGroupsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMPORTFLIGHTGROUPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnOpenFile();
	afx_msg void OnBtnConvert();
	afx_msg void OnBtnAppend();
	afx_msg void OnBtnReplace();
	afx_msg void OnLbnSelchangeListGroups();
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonTitle();
	afx_msg void OnDeleteRow();
	afx_msg void OnDeleteColumn();
	afx_msg LRESULT OnHeaderCtrlClickColumn(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CToolBar  m_wndToolbar;
	CListCtrl m_wndListData;
	CListCtrl m_wndListGroup;
	CEdit	  m_editResultInfo;
	CListBox  m_wndListBox;
	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;
	CHeaderCtrlDrag  m_headerCtrlDrag;

private:
	void InitListCtrl();
	int AddFlightToListCtrl();
	void DeleteAllColumn();
	void OnConvertCompile();
	void SetItemText(CStringArray &strArray);
	void AddColumn(int nColNum);
	int GetColNumFromStr(CString strText, CString strSep,CStringArray& strArray);
	bool ReadFile(const CString &strFileName);
	bool IfFlightExistInFlightSchedule(const CString& strAirline,const CString& strID,const CString& strDay);
	void PopulateGroupsList();
	void PopulateFlightList(int _nItem);
	void DoResize(int delta,UINT nIDSplitter);

private:
	CStringArray m_vFileData;
	bool m_bFlightGroupFile;
	double m_dFileVersion;
	std::vector<FlightGroup*> m_vFlightGroups;
	InputTerminal* m_pInterm;
	int m_nColIndexClicked;
};
