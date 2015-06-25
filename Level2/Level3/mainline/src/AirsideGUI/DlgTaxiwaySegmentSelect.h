#pragma once
#include "afxwin.h"
#include "InputAirside/IntersectionNode.h"
class TaxiSegmentData; 
class CDlgTaxiwaySegmentSelect :
	public CDialog
{
public:
	CDlgTaxiwaySegmentSelect(int nProjID,CWnd* pParent = NULL);
	virtual ~CDlgTaxiwaySegmentSelect(void);

	enum { IDD = IDD_DIALOG_SELECTTAXIWAY_SEGMENT };

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnSelchangeComboData();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	void OnOK();


	IntersectionNodeList GetTaxiwayNodeList(int nTaxiwayID);
	void DisplayComboNodeContent(int nTaxiwayID);
	void DisplayComboTaxiContent();

public:
	void GetTaxiwaySegmentData(TaxiSegmentData& segData);
	void SetTaxiwaySegmentData(const TaxiSegmentData& segData);
private:
	CComboBox m_wndComboTaxiway;
	CComboBox m_wndComboStart;
	CComboBox m_wndComboEnd;

	int m_nProjID;
	int m_nTaxiwayID;
	int m_nStartNodeID;
	int m_nEndNodeID;
};

