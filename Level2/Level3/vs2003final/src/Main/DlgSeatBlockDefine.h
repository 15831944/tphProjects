#pragma once
#include "../MFCExControl/ARCBaseTree.h"

// CDlgSeatBlockDefine dialog
class InputOnboard;
class Terminal;
class CAircraftPlacements;
class OnboardFlightSeatBlock;
class OnboardFlightSeatBlockContainr;

class CDlgSeatBlockDefine : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSeatBlockDefine)

public:
	CDlgSeatBlockDefine(InputOnboard* pInputOnboard,Terminal* pTerm,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatBlockDefine();

	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
// Dialog Data
	enum { IDD = IDD_DIALOG_SEATBLOCKDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAddSeatBlock();
	afx_msg void OnRemoveSeatBlock();
	afx_msg void OnEditSeatBlock();
	afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNewFlight();
	afx_msg void OnDelFlight();
	afx_msg void OnSelChangeFlight();

private:
	void OnInitToolbar();
	void OnInitTreeCtrl();
	void OnInitFlightList();
	void UpdateToolbarStatus();
	CAircraftPlacements* GetPlacement(OnboardFlightSeatBlock* pFlightSeatBlock);

	CToolBar m_wndToolBar;
	CToolBar m_toolBarFlight;
	CListBox m_wndListBox;
	CARCBaseTree m_wndTreeCtrl;
	std::vector<HTREEITEM> m_vSeatItem;
	std::vector<HTREEITEM> m_vBlockItem;
	InputOnboard* m_pOnboard;
	Terminal* m_pTerm;

	OnboardFlightSeatBlockContainr* m_pSeatBlockManager;
};
