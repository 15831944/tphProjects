#pragma once
#include "../InputAirside/TakeoffQueues.h"
#include "../InputAirside/TakeoffQueuePosi.h"
#include "../InputAirside/TakeoffQueuesItem.h"
#include "afxcmn.h"

// CDlgSelectTakeoffPosi dialog

class CDlgSelectTakeoffPosi : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectTakeoffPosi)

public:
	CDlgSelectTakeoffPosi(TakeoffQueues* pTakeoffQueues,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectTakeoffPosi();

	typedef std::vector<CString> TakeoffQueuesTaxiVector;
	typedef std::vector<CString>::iterator TakeoffQueuesTaxiIter;

	typedef std::map<CString, TakeoffQueuesTaxiVector> TakeoffQueuesPosiMap;
	typedef std::map<CString, TakeoffQueuesTaxiVector>::iterator TakeoffQueuesPosiMapIter;

// Dialog Data
	enum { IDD = IDD_SELECTTAKEOFFPOSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	TakeoffQueues* m_pTakeoffQueues;
	CString m_strRunTaxiway;
	TakeoffQueuesPosiMap m_TakeoffQueuesPosiMap;
	void AddToMap(TakeoffQueuePosi* pItem);

public:
	virtual BOOL OnInitDialog();
	void SetTreeContents();
	CTreeCtrl m_wndTakeoffPosiTree;
	afx_msg void OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult);
	bool IsTaxiwayItem(HTREEITEM hItem);
	CString GetStrRunTaxiway(){return m_strRunTaxiway;}
};
