#pragma once
#include "resource.h"
#include "NodeViewDbClickHandler.h"
#include "..\InputAirside\ItinerantFlight.h"
#include "..\InputAirside\InputAirside.h"
#include "afxwin.h"
#include "../MFCExControl/CARCTipEdit.h"
#include "../MFCExControl/CARCTipComboBox.h"
#include "..\InputAirside\ALTObject.h"

class CDlgItinerantTrafficDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgItinerantTrafficDefine)

public:
	CDlgItinerantTrafficDefine(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside,  
		PSelectProbDistEntry pSelectProbDistEntry, ItinerantFlight* pItinerantFlight, 
		BOOL bNewItem = FALSE, CWnd* pParent = NULL);
	virtual ~CDlgItinerantTrafficDefine();

	int m_nProjID;
	ItinerantFlight* m_pItinerantFlight;

	PSelectFlightType	m_pSelectFlightType;
	InputAirside* m_pInputAirside;
// Dialog Data
	enum { IDD = IDD_DIALOG_ITINERANTTRAFFIC_DEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	void UpDateUIState();

private:
	CString m_strCode;
//	CString m_strFltType;
	CString m_strEntry;
	CString m_strExit;
	CString m_strArrTimeWindow;
	BOOL m_bArrTime;
	BOOL m_bDepTime;
	BOOL m_bArrDist;
	BOOL m_bDepDist;
	BOOL m_bEntry;
	BOOL m_bExit;
	ElapsedTime m_strArrStartTime;
	ElapsedTime m_strArrEndTime;
	CString m_strDepTimeWindow;
	ElapsedTime m_strDepStartTime;
	ElapsedTime m_strDepEndTime;
	CString	m_strArrDistName;
	CString m_strDepDistName;
	CString m_strArrDistPrint;
	CString m_strDepDistPrint;
	ProbTypes m_emArrProbTypes;
	ProbTypes m_emDepProbType;
	CString	m_strArrDistScreenPrint;
	CString m_strDepDistScreenPrint;
	CStringList m_strList;
	PSelectProbDistEntry m_pSelectProbDistEntry;
	int m_nFlightCount;
	CButton m_btnEnRoute;
	CButton m_btnArr;
	CButton	m_btnDep;
	CEdit m_EntryEdit;
	CEdit m_ExitEdit;
	CEdit m_ArrTimeEdit;
	CEdit m_DepTimtEdit;
	int m_nEntryWayStandFlag;
	int m_nEntryID;
	int m_nExitWayStandFlag;
	int m_nExitID;
	CARCTipComboBox m_ArrComBoDistribution;
	CARCTipComboBox m_DepComBoDistribution;
	CARCTipComboBox m_comboOrig;
	CARCTipComboBox m_comboAirline;
	CString m_csOrig;
	CARCTipComboBox m_comboDest;
	CString m_csDest;
	CARCTipComboBox m_comboACType;
	CString m_csACType;
	CARCTipEdit m_wndEditName;
	CARCTipEdit m_wndEditFltType;
	CARCTipEdit m_wndEditEntry;
	CARCTipEdit m_wndEditExit;
	CARCTipEdit m_wndEditTime;
	CARCTipEdit m_wndEditNumOfFlts;
	std::vector<ALTObject> m_vWaypoint;

	BOOL m_bInit;
	BOOL m_bNewItem;
public:
	afx_msg void OnBnClickedButtonFlttype();
	afx_msg void OnEnChangeEditNumberofflights();
	afx_msg void OnBnClickedButtonEntry();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonTimewindow();
	afx_msg void OnBnClickedButtonDepTimeWindow();
	afx_msg void OnCbnSelchangeComboDistribution();
	afx_msg void OnCbnSelchangeComboArrDistribution();
	virtual BOOL OnInitDialog();
	void GetAllStand();	
	void InitArrControl(BOOL bEnable);
	void InitDepControl(BOOL bEnable);
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckEnroute();
	afx_msg void OnBnClickedCheckArr();
	afx_msg void OnBnClickedCheckDep();
};
