#pragma once
#include "afxcmn.h"
#include "../MFCExControl/CoolTree.h"
// CDlgFlightActypeMatch dialog
class CACType;
class CACTypesManager;
class Flight;
//class CDlgFlightActypeMatch : public CXTResizeDialog
//{
//	DECLARE_DYNAMIC(CDlgFlightActypeMatch)
//public:
//	typedef std::vector<Flight*>  TY_DATA ;
//public:
//	CDlgFlightActypeMatch(std::map<CString ,TY_DATA>* HandleCodeData,CACTypesManager* _ActypeManager , CWnd* pParent = NULL);   // standard constructor
//	virtual ~CDlgFlightActypeMatch();
//
//// Dialog Data
//	enum { IDD = IDD_DIALOG_ACTYPEMATCH };
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//
//	DECLARE_MESSAGE_MAP()
//public:
//	afx_msg void OnTvnSelchangedTreeActype(NMHDR *pNMHDR, LRESULT *pResult);
//protected:
//	CCoolTree m_TreeCtrl;
//	std::map<CString,TY_DATA>* m_HandleCode ;
//	CACTypesManager* m_ActypeManager ;
//protected:
//	BOOL OnInitDialog() ;
//	void InitTreeCtrl() ;
//	void InsertTreeSubItem(CString _code,HTREEITEM _paritem) ;
//	BOOL GetSelectActype(CString& _node ,HTREEITEM _rootItem) ;
//
//	void OnSize(UINT nType, int cx, int cy) ;
//protected:
//	void OnOK() ;
//	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)  ;
//};
