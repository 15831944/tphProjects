#pragma once
#include "afxcmn.h"

#include "../MFCExControl/ListCtrlReSizeColum.h"
#include "afxwin.h"
// CDlgCrewAssignmentSpec dialog
class CabinCrewPhysicalCharacteristics ;
class CabinCrewGeneration ;
//-------------------------------------------------------------------------------------------------------
//CFltRelativeIconListCtrl
#define  INITPOSITION_ICON 0x01
#define  FINALPOSITION_ICON 0x02
#include <map>
class CICONPair
{
public:
     COLORREF m_InitIcon ;
	 COLORREF m_FinalIcon ;
public:
	CICONPair():m_FinalIcon(NULL),m_InitIcon(NULL) {} ;
	CICONPair(const CICONPair& _pair) { m_InitIcon = _pair.m_InitIcon ; m_FinalIcon = _pair.m_FinalIcon ;} ;
};
class CCRewPositionIconListCtrl : public CReSizeColumnListCtrl
{
	DECLARE_DYNAMIC(CCRewPositionIconListCtrl)

public:
	CCRewPositionIconListCtrl();
	virtual ~CCRewPositionIconListCtrl();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	COLORREF	GetImage(CabinCrewPhysicalCharacteristics* _Crew,int pos);

	std::map<CabinCrewPhysicalCharacteristics*, CICONPair>		m_mapElementIcon;
	CBitmap							m_hDefaultIcon;
public:
    void AddImage(CabinCrewPhysicalCharacteristics* _crew) ;
	DECLARE_MESSAGE_MAP()
	COLORREF GetRandomColor() ;
   void CreateInitImage(CabinCrewPhysicalCharacteristics* _Crew,CDC& PDC ,CRect rect) ;
   void CreateFinalImage(CabinCrewPhysicalCharacteristics* _Crew,CDC& PDC ,CRect rect) ;
};


//-------------------------------------------------------------------------------------------------------
class CDlgCrewAssignmentSpec : public CDialog
{
	DECLARE_DYNAMIC(CDlgCrewAssignmentSpec)

public:
	CDlgCrewAssignmentSpec(CabinCrewGeneration* _crewlist ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCrewAssignmentSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_CREWASSIGNMENT };
protected:
	CabinCrewGeneration* m_CrewPaxList ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	BOOL OnInitDialog() ;

	afx_msg void OnOK() ;
	afx_msg void OnCancel() ;
    void OnSize(UINT nType, int cx, int cy) ;
protected:
	void InitListView() ;
	void InitListData() ;

	void AddItemToList(CabinCrewPhysicalCharacteristics* _Crew) ;

	void InitCheckBoxState(CabinCrewPhysicalCharacteristics* _Crew) ;
	CCRewPositionIconListCtrl m_ListCrew;
public:
	afx_msg void OnHdnItemclickListCrew(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginrdragListCrew(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEnddragListCrew(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton m_CheckDoc;
	CButton m_CheckDirect;
	CButton m_CheckHelp;
	CButton m_Distribute;
	CButton m_checkBlock;
public:
	afx_msg void OnBnClickedCheckCheckDoc();
	CabinCrewPhysicalCharacteristics* GetCurselCrew() ;
	afx_msg void OnBnClickedCheckdirect();
	afx_msg void OnBnClickedCheckHelp();
	afx_msg void OnBnClickedCheckDistribute();
	afx_msg void OnBnClickedCheck3();
};
