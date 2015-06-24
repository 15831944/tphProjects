#pragma once


// CAffinityGroupSpec dialog

class CAffinityGroupSpec : public CDialog
{
	DECLARE_DYNAMIC(CAffinityGroupSpec)

public:
	CAffinityGroupSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAffinityGroupSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_AFFINITY_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
