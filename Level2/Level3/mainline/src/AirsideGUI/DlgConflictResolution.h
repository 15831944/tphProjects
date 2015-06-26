#pragma once
#include "ConflictResolutionCoolTree.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgConflictResolution dialog
class CConflictResolution;
class CAirportDatabase;
class RunwayCrossBuffer;

class CDlgConflictResolution : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgConflictResolution)
public:

	typedef std::vector<std::pair<CString,int> > TaxiwayVector;
	typedef std::vector<std::pair<CString,int> >::iterator TaxiwayVectorIter;

	typedef std::map<CString, TaxiwayVector> TaxiwayVectorMap;
	typedef std::map<CString, TaxiwayVector>::iterator TaxiwayVectorMapIter;

public:
	CDlgConflictResolution(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConflictResolution();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFLICTRESOLUTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();
	afx_msg void OnEditItem();

private:
	CConflictResolutionCoolTree	m_wndConflictResolutionTree;
	CToolBar	m_wndToolbar;
	HTREEITEM   m_hRadiusOfConcern;
	HTREEITEM   m_hAtIntersectionOnRight;
	HTREEITEM   m_hFirstInATaxiway;
	HTREEITEM   m_hOnSpecificTaxiways;
	HTREEITEM	m_hRunwayCrossBuffer;
	HTREEITEM	m_hCaseLanding;
	HTREEITEM	m_hCaseTakeoff;

	HTREEITEM m_hApproachDistance;
	HTREEITEM m_hApproachTime;
	HTREEITEM m_hTakeoffSeconds; 

	std::vector<HTREEITEM> m_hBufferTimes;
	std::vector<HTREEITEM> m_hBufferDists;

	bool isBuffeTime(HTREEITEM hItem){ return std::find(m_hBufferTimes.begin(),m_hBufferTimes.end(),hItem)!=m_hBufferTimes.end(); }
	bool isBuffeDist(HTREEITEM hItem){ return std::find(m_hBufferDists.begin(),m_hBufferDists.end(),hItem)!=m_hBufferDists.end(); }
	CConflictResolution* m_pConflictResolution;
	int m_nProjID;
	CAirportDatabase* m_pAirportDB;

	TaxiwayVectorMap m_TaxiwayVectorMap;
	TaxiwayVectorMap* m_pTaxiwayVectorMap;
	PFuncSelectFlightType	m_pSelectFlightType;
	int m_nDefaultTaxiwayID;
	CString m_strDefaultTaxiwayName;

	void InitToolBar();
	void UpdateToolBar();
	void LoadTree();
public:
	afx_msg void OnBnClickedSave();
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTvnSelchangedTreeConflictresolution(NMHDR *pNMHDR, LRESULT *pResult);


	void addRunwayCrossBufferToTree(HTREEITEM hParent, RunwayCrossBuffer* buffer);
};
