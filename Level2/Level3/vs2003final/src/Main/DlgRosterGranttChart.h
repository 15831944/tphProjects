#pragma once
#include "..\MFCExControl\SortableHeaderCtrl.h"
#include "gtchart.h"
#include "CommonData\Procid.h"
#include "Inputs\AutoRosterRule.h"
#include "MFCExControl\SortAndPrintListCtrl.h"



class InputTerminal;
class CAutoRosterRule;
class CProcRosterRules;
class ProcAssignEntry;
class ProcessorRoster;
class ProcessorID;
class ProcessorRosterSchedule;

// CDlgRosterGranttChart dialog

class CDlgRosterGranttChart : public CDialog
{


	DECLARE_DYNAMIC(CDlgRosterGranttChart)
public:
	CDlgRosterGranttChart(CProcRosterRules* m_pProcRosterRules,
							std::vector<ProcessorID> &vProcID,
							InputTerminal* pInTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRosterGranttChart();

// Dialog Data
	enum { IDD = IDD_DIALOG_ROSTERASSIGNMENT };

	CButton	m_btnUnassignAll;
	CButton	m_btnSummary;
	CButton	m_btnAutoAssign;
	CButton	m_btnSave;
	CButton	m_btnOk;
	CButton	m_btnCancel;

	CSortAndPrintListCtrl 	m_listctrlFlightList;
	CGTChart	m_gtcharCtrl;
	CStatic m_stProgAssignStatus;
	CProgressCtrl m_progAssignStatus;

	CImageList m_imageList;

	CSortableHeaderCtrl	m_ctlHeaderCtrl;	
	CImageList* m_pDragImage;
	std::vector<ProcAssignEntry*> m_vProAssignEntries;

	InputTerminal* m_pInTerm;
	CProcRosterRules* m_pProcRosterRules;

	int m_nMaxScheduleDay;

	std::vector<ProcessorID> m_vProcID;

	RosterAssignFlightList m_lstAssignFlights;
	
	int m_nDragIndex;
	bool m_bDragging;
	RosterAssignFlight *m_pDragFlight;


protected:
	void LoadFlightList();
	void LoadGranttChart();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDlgRosterGranttChart)
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()

	//}}AFX_MSG
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBegindragListFlight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnClickBtnAddItemGtchartctrl(BOOL bSuccess, long nLineIndex);
	afx_msg void OnClickBtnDelItemGtchartctrl(BOOL bSuccess, long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData);
	afx_msg void OnDblClickItemGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData);
	afx_msg void OnItemResizedGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration);
	afx_msg void OnItemMovedGtchartctrl(long nLineIndexFrom, long nLineIndexTo, long nItemIndex, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration);
	afx_msg void OnCheckFlightOperationMoveGtchartctral(long nLineIndexFrom, long nLineIndexTo,long nItemIndex,long FAR* pnLineData);
	afx_msg void OnRClickGtchartctrl(long nItemIndex, long nLineIndex);
	afx_msg void OnShowItemToolTipGtchartctrl(long nLineIndex, long nItemIndex, long timeCurrent);


	DECLARE_MESSAGE_MAP()
	void InitFlightListCtrl();
	void InitGTCtrlLayout();
	void AddLineToGT(ProcessorRoster* _Ass,ProcessorID& id);
	void AddItemToGT(int lineId , ProcessorRoster* _Ass);
	void AddProcessor(ProcessorRosterSchedule* _schedule, const ProcessorID& id);

	void FreshProcessorRoster(int iLine);
	void AddFlightIntoList(RosterAssignFlight* pUnassignFlight);

	afx_msg void OnBnClickedButtonSummary();
	afx_msg void OnBnClickedButtonAutoAssign();
	afx_msg void OnBnClickedButtonUnassignAll();
	afx_msg void OnBtnSave();

private:
	int GetListCtrlIndex(RosterAssignFlight* pUnassignFlight);
};
