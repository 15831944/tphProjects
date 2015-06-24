#if !defined(AFX_INPUTDATAVIEW_H__0A181E16_8E73_4920_906D_D02573838614__INCLUDED_)
#define AFX_INPUTDATAVIEW_H__0A181E16_8E73_4920_906D_D02573838614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDataView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDataView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "TermPlanDoc.h"
#include "resource.h"
#include "PrintableListCtrl.h"

class CInputDataView : public CFormView
{
protected:
	CInputDataView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInputDataView)

// Form Data
public:
	//{{AFX_DATA(CInputDataView)
	enum { IDD = IDD_FORMVIEW_INPUTDATA };
	CListCtrl	m_listVisitTime;
	CListCtrl	m_listTrainSchedule;
	CListCtrl	m_listSpeed;
	CListCtrl	m_listSideStep;
	CListCtrl	m_listServiceTime;
	CListCtrl	m_listRoster;
	CListCtrl	m_listPaxDistribution;
	CListCtrl	m_listMobileElemCount;
	CListCtrl	m_listLoadFactors;
	CListCtrl	m_listLeadLag;
	CListCtrl	m_listInStep;
	CListCtrl	m_listHubbingData;
	CListCtrl	m_listGroupSize;
	CListCtrl	m_listBoardingCall;
	CListCtrl	m_listAirCapacity;
	CListCtrl	m_listProcs;
	CListCtrl	m_listFltSchedule;
	CListCtrl	m_listFltDelay;
	CListCtrl	m_listFloors;
	CListCtrl   m_listBaggageDevice;
	CToolBar	m_wndToolBar;
	std::vector<CListCtrl*> m_vList;
	int m_nIndex;
	int m_nUnique;
	int m_nVHeight;
	//}}AFX_DATA

// Attributes
public:
	void SetActiveDoc(CDocument* pDoc)
	{
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
		m_pDocument=(CTermPlanDoc*)pDoc;
	};
// Operations
public:
	void InitReport();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDataView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadFloors();
	virtual ~CInputDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInputDataView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC ); 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void PrintInputData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void OnInitToolbar();
	void LoadRoster();
	void LoadTrainSchedule();
	void LoadServiceTime();
	void LoadLeadLag();
	void LoadMobElementDistribution();
	void LoadMobElementCount();
	void LoadPaxDistribution();
	void LoadHubbingData();
	void LoadFlightDistribution();
	void LoadBoardingCall();
	void LoadFlightSchedule();
	void LoadBaggageDeviceAssignment();
	void LoadProcs();
	BOOL IsSelectedServiceTime( CString strProcess, const ProcessorDatabase* _pProcDB ,int& nDBIndex);
	void LoadChildServiceTime(const ServiceTimeDatabase* pServiceTimeDatabase,const ProcessorList* pProcessorList,const CString& sParentProcs,int& nPosition);

	BOOL IsSelectedRoster(const CString& sProcID,const ProcAssignDatabase* pProcAssignDB,int& nDBIndex);
	void LoadChildRoster(const ProcAssignDatabase* pProcAssignDatabase,const ProcessorList* pProcessorList,const CString& sParentProcs,int& nPosition);
	void PrintFooter(CDC* pDC, CPrintInfo* pInfo);
	int FormStringLine(CString sData,CDC* pDC,int nxPos,int nyPos,int nCount);
	int ComputerItemCount(CListCtrl* pCtrl,int nIndex);
	int ComputerListItemCount(CListCtrl* pCtrl);
		
	CTermPlanDoc* m_pDocument;
};

class PaxTypePath
{
public:
	PaxTypePath()
	{
		for(int i=0;i<LEVELS;i++)value[i]=-1;
	}
	PaxTypePath(const PaxTypePath& path)
	{
		memcpy(this,&path,sizeof(PaxTypePath));
	}
	virtual ~PaxTypePath(){}
	
	void operator=(const PaxTypePath& path)
	{
		memcpy(this,&path,sizeof(PaxTypePath));
	}
	int& operator[](const int nIndex)
	{
		ASSERT(nIndex<LEVELS && nIndex>=0);
		return value[nIndex];
	}
	int operator[](const int nIndex) const
	{
		ASSERT(nIndex<LEVELS && nIndex>=0);
		return value[nIndex];
	}
	const int* GetPath()
	{
		return value;
	}
private:
	int value[LEVELS];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDATAVIEW_H__0A181E16_8E73_4920_906D_D02573838614__INCLUDED_)
