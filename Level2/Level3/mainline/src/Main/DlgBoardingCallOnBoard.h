#pragma once

#include "../MFCExControl/ARCBaseTree.h"
#include "../InputOnboard/OnBoardingCall.h"
#include "../MFCExControl/XTResizeDialog.h"



// CDlgBoardingCallOnBoard dialog
class COnBoardAnalysisCandidates;
class InputTerminal;
class CDlgBoardingCallOnBoard : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgBoardingCallOnBoard)

public:
	CDlgBoardingCallOnBoard(int nProjID,InputTerminal * pInterm,CAirportDatabase* pAirportDB,COnBoardAnalysisCandidates* _pOnBoardList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBoardingCallOnBoard();

// Dialog Data
	enum { IDD = IDD_DIALOG_BOARDINGCALLONBOARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBtnBlock();
	afx_msg void OnAddStage();
	afx_msg void OnRemoveStage();
	afx_msg void OnEditStage();

	afx_msg void OnAddFlightType();
	afx_msg void OnDeleteFlightType();

	afx_msg void OnAddStand();
	afx_msg void OnEditStand();
	afx_msg void OnDeleteStand();

	afx_msg void OnAddPaxType();
	afx_msg void OnEditPaxType();
	afx_msg void OnDeletePaxType();

	afx_msg void OnAddSeatBlock();
	afx_msg void OnDeleteSeatBlock();

	afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg	void OnBnClickedOk();
	afx_msg void OnBtnSave();
public:
	void OnInitToolbar();
	void OnInitTreeCtrl();
	void UpdateToolbarStatus();

	void CreateProdistribution(HTREEITEM hItem);
	void HandleProdistribution(HTREEITEM hItem);

	void CreateSeatBlock(HTREEITEM hItem);
	void HandleSeatBlock(HTREEITEM hItem);

	void UpdateBlockBtnStatus();
private:
	CToolBar m_wndToolBar;
	CARCBaseTree m_wndTreeCtrl;
	COnBoardAnalysisCandidates* m_pOnBoardList;
	COnBoardingCall m_boardingCall;
	int m_nProjID;
	InputTerminal* m_pInterm;

	std::vector<HTREEITEM>m_vPaxTypeItem;
	std::vector<HTREEITEM>m_vStandItem;
};
