#pragma once


// CDlgSectionPointProp dialog

class CDlgSectionPointProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgSectionPointProp)

public:
	CDlgSectionPointProp(double dY, double dZ, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSectionPointProp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SECTIONPTPROP };

	double mdY;
	double mdZ;

	void LoadData();
	void RetriveData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support



	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
