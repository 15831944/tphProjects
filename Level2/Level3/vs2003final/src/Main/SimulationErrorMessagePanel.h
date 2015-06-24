#pragma once

#include "Common\EngineDiagnose.h"

#include <XTToolkitPro.h>
#include <TaskPanel/XTPTaskPanel.h>
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"

class DiagnoseEntry;
class InputTerminal;
class CTermPlanDoc;

class TerminalDiagnose;
class AirsideDiagnose;
class LandsideDiagnose;
class OnBoardDiagnose;
class CTrackWnd;

class SimulationErrorMessagePanel : public CXTTabCtrl
{
public:
	SimulationErrorMessagePanel(void);
	~SimulationErrorMessagePanel(void);

	BOOL Create(CWnd* pWnd, CPoint ptPos);

	void AddSimStatusFormatExMsg(const FORMATMSGEX * pMsg);

	//const
	void AppendTerminalDiagnose(TerminalDiagnose* pDiagnose);
	void AppendAirsideDiagnose(AirsideDiagnose* pDiagnose);
	void AppendLandsideDiagnose(LandsideDiagnose* pDiagnose);
	void AppendOnBoardDiagnose(OnBoardDiagnose* pDiagnose);

	void Move(int cx, int cy);
	void SetTerminal(InputTerminal* pTerm){	m_pTerm = pTerm;}
	void SetTermPlanDoc(CTermPlanDoc* pDoc){m_pTermPlanDoc = pDoc;}

	void reloadLogIfNeed( const DiagnoseEntry* _pEntry );
	CString GetProjPath();

	void ExpandAirside();
	void ExpandTerminal();
	void ExpandLandside();
	void ExpandOnBoard();
	void ExpandCargo();
	void ExpandEnvironment();
	void ExpandFinancial();

	void ClearErrorMessage();
	void SaveErrorMessage(FILE* pFile);

	// 	virtual LRESULT NotifyOwner(WPARAM wParam, LPARAM lParam);

	BOOL GetShowErrorInfo() const { return m_bShowErrorInfo; }
	void SetShowErrorInfo(BOOL val) { m_bShowErrorInfo = val; }

protected:
	BOOL InsertNewTab(CXListCtrl& ctrl, CSortableHeaderCtrl& headerCtrl, int nCtrlID, int nTabIndex, LPCTSTR strTabName,
		const char* const colLabels[], const int nFormats[], const int nDefWidths[], const EDataType colDataType[], int nColNum);

	void SaveTerminalErrorMessage(CXListCtrl& pListControl,FILE* f);
	void SaveAirsideErrorMessage(CXListCtrl& pListControl,FILE* f);
	void SaveLandsideErrorMessage(CXListCtrl& pListControl,FILE* f);
	void SaveOnBoardErrorMessage(CXListCtrl& pListControl,FILE* f);

	void MouseOverListCtrl(CPoint point);
	void MouseOverCtrl(CPoint point, HWND hWnd);

	InputTerminal*			m_pTerm;
	CTermPlanDoc*			m_pTermPlanDoc;

	CXListCtrl				m_wndAirsideListCtrl;
	CXListCtrl				m_wndLandsideListCtrl;
	CXListCtrl				m_wndTerminalListCtrl;
	CXListCtrl				m_wndOnBoardListCtrl;
	CXListCtrl				m_wndCargoListCtrl;
	CXListCtrl				m_wndEnvironmentListCtrl;
	CXListCtrl				m_wndFinancialListCtrl;


	CXTTipWindow			m_tipWindow;

	CSortableHeaderCtrl		m_ctlHeaderCtrlAirside;
	CSortableHeaderCtrl		m_ctlHeaderCtrlLandside;
	CSortableHeaderCtrl		m_ctlHeaderCtrlTerminal;
	CSortableHeaderCtrl		m_ctlHeaderCtrlOnBoard;
	CSortableHeaderCtrl		m_ctlHeaderCtrlCargo;
	CSortableHeaderCtrl		m_ctlHeaderCtrlEnvironment;
	CSortableHeaderCtrl		m_ctlHeaderCtrlFinancial;

	CTrackWnd*				m_pTrackWnd;
	BOOL					m_bShowErrorInfo;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnXListCtrlCheckBoxClicked(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTrackWndDestroy(WPARAM wParam,LPARAM lParam);
	afx_msg void OnColumnclickListErrorMsgTerminal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListErrorMsgAirside(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListErrorMsgLandSide(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListErrorMsgOnBoard(NMHDR* pNMHDR, LRESULT* pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
