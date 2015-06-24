#pragma once

#include "LLCtrl.h"
#include "afxwin.h"
#include "../AirsideGUI/UnitPiece.h"
// CDlgEditARP dialog

class ALTAirport;
class CDlgEditARP : public CDialog,public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgEditARP)

public:
	CDlgEditARP(int nProjID,int nAIrportID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditARP();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDITARP };

	//member
public :
	CLLCtrl m_pLongitude;
	CLLCtrl m_pLatitude;
	CString m_strLongitude;
	CString m_strLatitude;

	double m_dElevation;

	double m_dx;
	double m_dy;

	int m_nAirportID;
	ALTAirport* m_pAirport;
	bool m_bxyModified;
	bool m_bModified;
	int m_nProjID;

	//operation
public :
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	afx_msg void OnBnClickedButtonPick();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);

	CEdit m_editdx;
	CEdit m_editdy;
	CEdit m_editelevation;
	afx_msg void OnBnClickedOk();

	
	CComboBox m_MagVariationEastWestCombo;
	int m_nMagEastWestSelected;
	double m_fMagVariation;
	CEdit m_editMagVar;
	int m_nSelectUnit;
	afx_msg void OnEnKillfocusEditElevation();
	afx_msg void OnEnChangeEditElevation();
};
