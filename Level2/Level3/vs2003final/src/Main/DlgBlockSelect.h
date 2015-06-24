#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgBlockSelect dialog

#include "../MFCExControl/CoolTree.h"
class InputTerminal ;
class COnBoardAnalysisCandidates ;
class CAirportDatabase ;
class COnBoardingCall ;
class COnBoardingCallFlight ;
class COnBoardSeatBlockItem ;
class CDlgFlightSelect : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgFlightSelect)

public:
	CDlgFlightSelect(int nProjID , InputTerminal * pInterm,COnBoardingCall* boardcall,CAirportDatabase* pAirportDB,COnBoardAnalysisCandidates* _pOnBoardList,CWnd* pParent = NULL);   // standard constructor
	CDlgFlightSelect(CWnd* pParent = NULL) :CXTResizeDialog(CDlgFlightSelect::IDD, pParent)	,m_pOnBoardList(NULL)
		,m_boardingCall(NULL)
		,m_nProjID(-1)
		,m_pInterm(NULL)
		,m_selFlight(NULL)
	{

	}
	virtual ~CDlgFlightSelect();

// Dialog Data
	enum { IDD = IDD_DIALOG_BLOCK_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CCoolTree m_wndTreeCtrl;
	COnBoardAnalysisCandidates* m_pOnBoardList;
	COnBoardingCall* m_boardingCall;
	int m_nProjID;
	InputTerminal* m_pInterm;
	std::vector<HTREEITEM>m_vStandItem;
	COnBoardingCallFlight* m_selFlight ;

public:
	afx_msg void OnTvnSelchangedTreeBlock(NMHDR *pNMHDR, LRESULT *pResult);
	COnBoardingCallFlight* GetSelFlight() { return m_selFlight ;  } ;

protected:
	CButton m_ButtonSel;
	virtual BOOL OnInitDialog() ;
    virtual void OnInitTreeCtrl() ;

	virtual void OnOK() ;
	void OnCancel() ;
};
class CDlgBlockSelect : public CDlgFlightSelect
{
public:
	CDlgBlockSelect(COnBoardingCallFlight* _flight,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBlockSelect();
	COnBoardSeatBlockItem* GetSelBlock() { return m_selBlock ;} ;
protected:
	void OnInitTreeCtrl() ;
	BOOL OnInitDialog() ;
	void AddBlockItem(COnBoardSeatBlockItem* _block , HTREEITEM _item) ;
	COnBoardSeatBlockItem* m_selBlock ;
	
	void OnOK() ;
protected:
    COnBoardingCallFlight* m_flight ;
};