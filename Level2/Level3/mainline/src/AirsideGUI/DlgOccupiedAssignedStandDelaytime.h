#pragma once


// CDlgOccupiedAssignedStandDelaytime dialog

class CDlgOccupiedAssignedStandDelaytime : public CDialog
{
	DECLARE_DYNAMIC(CDlgOccupiedAssignedStandDelaytime)

public:
	CDlgOccupiedAssignedStandDelaytime(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOccupiedAssignedStandDelaytime();

// Dialog Data
	enum { IDD = IDD_DIALOG_OCCUPIEDDELAYTIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int m_nDelaytime;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditDelaytime();
	int GetDelaytime(){return m_nDelaytime;}
};
