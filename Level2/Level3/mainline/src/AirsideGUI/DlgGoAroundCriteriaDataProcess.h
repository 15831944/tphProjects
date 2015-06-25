#pragma once
#include "DialogResize.h"
#include "Resource.h"
#include "../InputAirside/GoAroundCriteriaDataProcess.h"
#include "../common/UnitsManager.h"
// CDlgGoAroundCriteriaDataProcess dialog

class CDlgGoAroundCriteriaDataProcess : public CDialogResize
{
	DECLARE_DYNAMIC(CDlgGoAroundCriteriaDataProcess)

public:
	CDlgGoAroundCriteriaDataProcess(int nProjID,CUnitsManager* pGlobalUnits,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGoAroundCriteriaDataProcess();

// Dialog Data
	enum { IDD = IDD_DIALOG_GOAROUNDCRITERIA };
protected: 
	int m_nProjID;
	CGoAroundCriteriaDataProcess m_gacData;
	CUnitsManager * m_pGlobalUnits;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
