#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../InputAirside/TrainingFlightContainer.h"
#include "NodeViewDbClickHandler.h"
#include "UnitPiece.h"
#include "../MFCExControl/XTResizeDialog.h"


class InputTerminal;
class CProbDistEntry;

// CDlgTrainFlightSpec dialog

class CDlgTrainFlightSpec : public CXTResizeDialog, public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgTrainFlightSpec)

	enum UnitType
	{
		Unit_Length,
		Unit_Speed,
		Unit_Time
	};
public:
	CDlgTrainFlightSpec(int nProjID,InputTerminal* pInTerm,InputAirside* pInputAirside,PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTrainFlightSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRAINFLIGHTSPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

	void OnInitToolbar();
	void DisplayTrainFlight(CTrainingFlightData* pTrainFlight);
	void DisplayListBoxContent();

	afx_msg void AddTrainFlight();
	afx_msg void DeleteTrainFlight();
	afx_msg void EditTrainFlight();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	afx_msg void OnSelchangeListBox();
	afx_msg void OnSave();

	void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	CProbDistEntry* ChangeProbDist( HTREEITEM hItem,const CString& strDist);

private:
	CString GetUintString(UnitType emType);
	double ConvertValueToDefaultUnit(UnitType emType,double dValue);
	double ConvertValueToCurrentUnit(UnitType emType,double dValue);

private:
	CTrainingFlightData* GetTrainFlightData();
private:
	CListBox  m_wndListBox;
	CCoolTree m_wndTreeCtrl;
	CToolBar  m_wndToolBar;
	HTREEITEM m_hCarrierItem;
	HTREEITEM m_hActypItem;
	HTREEITEM m_hTimeRange;
	HTREEITEM m_hNumFlightItem;
	HTREEITEM m_hStartPointItem;
	HTREEITEM m_hDistItem;
	HTREEITEM m_hLowNumItem;
	HTREEITEM m_hLowValueItem;
	HTREEITEM m_hTouchNumItem;
	HTREEITEM m_hTouchValueItem;
	HTREEITEM m_hStopNumItem;
	HTREEITEM m_hStopValueItem;
	HTREEITEM m_hForceNumItem;
	HTREEITEM m_hEndPointItem;

	CTrainingFlightContainer m_trainFlightContainer;
	InputTerminal* m_pInTerm;
	InputAirside* m_pAirsideInput;
	int m_nProjID;
	PSelectProbDistEntry m_pSelectProbDistEntry;

	std::vector<CString> sortAcTypeList;
	std::vector<CString> sortAirlineList;
};
