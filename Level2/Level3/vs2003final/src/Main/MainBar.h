// MainBar.h: interface for the CMainBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINBAR_H__40A8FFB9_7D74_413A_8946_3B5D93D91627__INCLUDED_)
#define AFX_MAINBAR_H__40A8FFB9_7D74_413A_8946_3B5D93D91627__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBar24X24.h"
#include "ProjectViewSetting.h"
#include "TermPlanDoc.h"
class CMainBar : public CToolBar24X24  
{
public:
	CMainBar();
	virtual ~CMainBar();
void setCombox(CDocument* doc);
	void EnableComboBox(bool bEnable = true, bool bSetContentDefault = true);
   void SaveComBoxToDB() ;
   void getCurrentEnviro(EnvironmentMode& enviro , SimulationType& sim_type) ;
protected:
	// Generated message map functions
	//{{AFX_MSG(CMainBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeEnvModeComboBox();
	afx_msg void OnSelChangeSimuTypeComboBox();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CComboBoxXP m_cmbEnvMode;
	CComboBoxXP m_cmbSimulationType;
	CProjectViewSetting setting ;
protected:
	void UpdateEnvironmentView(CTermPlanDoc* pDoc) ;
	int  getEnvComboxIndexFromEnvironMode(EnvironmentMode env) { return (int)env ;} 
	EnvironmentMode getEnvironModeFromComboxIndex(int index) {return (EnvironmentMode)index ;}
	int  getSimComboxIndexFromSimMode(SimulationType env) { return (int)env ;} 
	SimulationType getSimModeFromComboxIndex(int index) {return (SimulationType)index ;}
};

#endif // !defined(AFX_MAINBAR_H__40A8FFB9_7D74_413A_8946_3B5D93D91627__INCLUDED_)
