#pragma once



// CAircraftFurningCommentView form view

class CAircraftFurningCommentView : public CFormView
{
	DECLARE_DYNCREATE(CAircraftFurningCommentView)

protected:
	CAircraftFurningCommentView();           // protected constructor used by dynamic creation
	virtual ~CAircraftFurningCommentView();

public:
	enum { IDD = IDD_AIRCRAFTFURNINGCOMMENTVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	afx_msg void OnBnClickedOk();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_editComment;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonsave();
	CStatic m_staticTitile;
	CButton m_btnOK;
	CButton m_btnSave;
	CButton m_btnSaveAs;
	CButton m_btnCancel;

	DECLARE_MESSAGE_MAP()
};



