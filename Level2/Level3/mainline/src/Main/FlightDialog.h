#pragma once
#include "..\common\flt_cnst.h"

class InputTerminal;

class CFlightDialog : public CDialog
{
public:
	CFlightDialog(CWnd* pParent, bool bShowThroughout = false);
	void Setup(const FlightConstraint& FlightSelection, int Intrinsic = 0);
	FlightConstraint GetFlightSelection() { return m_FlightSelection; }	
	
protected:
	enum { IDD = IDD_DIALOG_FLIGHT };

	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL	OnInitDialog();
	InputTerminal*	GetInputTerminal();
	virtual void	ResetSeletion();
	void			ResetFlightID();

	FlightConstraint    m_FlightSelection;
	int                 m_Intrinsic;
	CTreeCtrl			m_AirlineTree;
	CListBox			m_listAirlineGroup;
	CListBox			m_Sector;
	CListBox			m_Category;
	CListBox			m_ACType;
	CListBox			m_Airport;
	CString				m_strFlight;
	bool				m_bShowThroughoutMode;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnRadioAllflightsFlt();
	afx_msg void OnRadioArrivingFlt();
	afx_msg void OnRadioDepartingFlt();
	afx_msg void OnRadioThroughoutFlt();
	afx_msg void OnSelchangedTreeAirlineFlt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeListAirlineGroup();
	afx_msg void OnSelchangeListAirportFlt();
	afx_msg void OnSelchangeListActypeFlt();
	afx_msg void OnSelchangeListSectorFlt();
	afx_msg void OnSelchangeListCategoryFlt();
	afx_msg void OnClickStopOverAirport();
};

#define DECLARE_FLIGHTTYPE_SELECT_CALLBACK()									\
	static FlightConstraint SelectFlightType(CWnd* pParent)						\
	{																			\
		FlightConstraint fltType;												\
		CFlightDialog flightDlg(pParent);										\
		if(IDOK == flightDlg.DoModal())											\
			fltType = flightDlg.GetFlightSelection();							\
		return fltType;															\
	}																			\
	static BOOL FuncSelectFlightType(CWnd* pParent,FlightConstraint& fltCons)	\
	{																			\
		CFlightDialog flightDlg(pParent);										\
		if(IDOK == flightDlg.DoModal())											\
		{																		\
			fltCons = flightDlg.GetFlightSelection();							\
			return TRUE;														\
		}																		\
		return FALSE;															\
	}
