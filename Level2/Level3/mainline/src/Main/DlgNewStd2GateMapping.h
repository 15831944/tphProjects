#pragma once
#include "afxcmn.h"
#include "TermPlanDoc.h"
#include "../InputAirside/ALTObject.h"

// CDlgNewStd2GateMapping dialog

class CDlgNewStd2GateMapping : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewStd2GateMapping)

public:
	CDlgNewStd2GateMapping(InputTerminal* _pInTerm,int nAirportID,int nFlag,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewStd2GateMapping();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_STD2GATE_MAP };
protected:
	int m_nFlag;//flag of arrival gate(0x1) or departure gate(0x2)
	CTreeCtrl m_wndStand;
	CTreeCtrl m_wndGate;

	InputTerminal* m_pInTerm;
	int m_nAirportID;
	BOOL m_bRandomMapping;

	bool m_bSel4OfLeft;
	bool m_bSel4OfRight;
public:
	ALTObjectID m_StandID; 
	ProcessorID m_GateID;
	int m_iMappingFlag;//0,normal;1,one to one mapping;2,random mapping;
protected:
	void InitializeStand(void);
	void InitializeGate(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
