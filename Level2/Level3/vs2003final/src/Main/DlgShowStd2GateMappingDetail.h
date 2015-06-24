#pragma once
#include "afxcmn.h"
#include "TermPlanDoc.h"
#include "../InputAirside/ALTObject.h"

// CDlgNewStd2GateMapping dialog

class CDlgShowStd2GateMappingDetail : public CDialog
{
	DECLARE_DYNAMIC(CDlgShowStd2GateMappingDetail)

public:
	CDlgShowStd2GateMappingDetail(InputTerminal* _pInTerm,int nAirportID,int nFlag,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgShowStd2GateMappingDetail();

	// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_STD2GATE_MAP };
protected:
	int m_nFlag;//flag of arrival gate(0x1) or departure gate(0x2)
	CTreeCtrl m_wndStand;
	CTreeCtrl m_wndGate;

	InputTerminal* m_pInTerm;
	int m_nAirportID;
public:
	ALTObjectID m_StandID; 
	ProcessorID m_GateID;
	int m_iMappingFlag;
protected:
	void InitializeStand(void);
	void InitializeGate(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
