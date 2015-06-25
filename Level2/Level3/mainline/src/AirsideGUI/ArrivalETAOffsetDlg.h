#pragma once
#include "MFCExControl/SortableListCtrlEx.h"
#include "NodeViewDbClickHandler.h"
#include <map>
#include "../MFCExControl/XTResizeDialog.h"


using namespace std;

class CArrivalETAOffsetList;
class CArrivalETAOffset;
// CArrivalETAOffsetDlg dialog

class CArrivalETAOffsetDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CArrivalETAOffsetDlg)

public:
	CArrivalETAOffsetDlg(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CAirportDatabase *pAirPortdb, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent = NULL);   // standard constructor
	virtual ~CArrivalETAOffsetDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ARRIVALETAOFFSET };

private:
	void InitToolbar();
	void InitListCtrl(void);
	void SetListContent();
	void UpdateDisAndTimeFromARPToolBar();
	void InitListBox();
	void InitTimeList(void);

protected:
	virtual BOOL OnInitDialog(); 
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewFlightType();
	afx_msg void OnDelFlightType();
	afx_msg void OnEditFlightType();
	afx_msg void OnNewDistanceAndTimeFromARP();
	afx_msg void OnDelDistanceAndTimeFromARP();
	afx_msg void OnLbnSelchangeListFlightType();
	afx_msg void OnLvnItemchangedListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListDistanceAndTimeFromARP(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);

private:
	CSortableListCtrlEx    	     m_mListCtrl;
	CToolBar                     m_mFlightTypeToolBar;
	CToolBar                     m_mDisAndTimeToolBar;
	CListBox                     m_mFlightTypeListBox;

	CAirportDatabase         *m_pAirportDB;
	CStringList              m_TimeList;
	map<int, CString>        m_TimeMap;
	CArrivalETAOffsetList    *m_pArrivalETAOffsetList;
	CArrivalETAOffset        *m_pCurArrivalETAOffset;
	PSelectFlightType        m_pSelectFlightType;
	int                      m_nProjID;
	PSelectProbDistEntry     m_pSelectProbDistEntry;
	InputAirside             *m_pInputAirside;
};