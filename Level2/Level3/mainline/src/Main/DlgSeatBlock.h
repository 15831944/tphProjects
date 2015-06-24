#pragma once

// CDlgSeatBlock dialog
#include "../MFCExControl/ARCBaseTree.h"
#include "../InputOnboard/OnBoardingCall.h"
#include "../MFCExControl/XTResizeDialog.h"


class CAircraftPlacements;
class CDlgSeatBlock : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSeatBlock)

public:
	CDlgSeatBlock(COnBoardSeatBlockList* pSeatBlockList,int nFlightID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatBlock();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEATBLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()	
protected:
	afx_msg void OnAddSeatBlock();
	afx_msg void OnRemoveSeatBlock();
	afx_msg void OnEditSeatBlock();
	afx_msg void OnAddSeats();
	afx_msg void OnDeleteName();
	afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
public:
	void OnInitToolbar();
	void OnInitTreeCtrl();
	CString getStringByIncreaseType(SeatInCreaseType emInCreaseType);
	SeatInCreaseType getSeatInCreaseTypeByString(CString strValue);
	void UpdateToolbarStatus();
private:
	CToolBar m_wndToolBar;
	CARCBaseTree m_wndTreeCtrl;

	int m_nFlightID;
//	CAircraftSeatsManager* m_pSeatManager;
	CAircraftPlacements* m_pPlacement;
	COnBoardSeatBlockList* m_pSeatBlockList;
	COnBoardSeatBlockList* m_pCopySeatBlockList;

	std::vector<HTREEITEM>m_vStartItem;
	std::vector<HTREEITEM>m_vEndItem;
};
