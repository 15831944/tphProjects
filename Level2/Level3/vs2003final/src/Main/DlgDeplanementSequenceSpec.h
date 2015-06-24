#pragma once
#include "afxcmn.h"
#include "../MFCExControl/ListCtrlReSizeColum.h"
class COnBoardAnalysisCandidates;
// CDlgDeplanementSequenceSpec dialog
class CDeplanementSequenceDataSet ;
class CDeck ;
class CDeplanementSequence ;
class InputTerminal ;
class COnBoardingCall ;
class COnBoardSeatBlockItem ;
class CAirportDatabase ;
class CDlgDeplanementSequenceSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDeplanementSequenceSpec)

public:
	CDlgDeplanementSequenceSpec(COnBoardAnalysisCandidates* _pOnBoardList  , InputTerminal* _input ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeplanementSequenceSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEPLANCEMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CSortListCtrlEx m_listCtrl;
	CToolBar m_ToolBar ;
	CEdit m_ToolEdit ;
    InputTerminal* m_Input ;
	CDeplanementSequenceDataSet* m_Deplanement ;
	COnBoardAnalysisCandidates* m_pOnBoardList ;
protected:
	BOOL OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
	int  OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult) ;
    void OnInitList() ;
	void InitView() ;
	void InitData() ;
	void InitToolBar() ;
	CDeck* GetDeckByID(int _id) ;
	COnBoardSeatBlockItem* GetBlockByID(int _id) ;
protected:
	void AddItemToList(CDeplanementSequence* _deplane ,int ndx = -1) ;
protected:
	void OnAddDeplanementSeq() ;
	void OnDelDeplanementSeq() ;
	void OnEditDeplanementSeq() ;

	void OnMoveListItemUp() ;
	void OnMoveListItemDown() ;

	void OnOK() ;
	void OnCancel() ;

	void EditItemList(CDeplanementSequence* _deplane ,int ndx) ;
};
