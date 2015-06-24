#pragma once
/////////////////////////////////////////////////////////////////////////////
// CPipeShowListBox window

class CPipeShowListBox : public CCheckListBox
{
public:
	CPipeShowListBox();
	virtual ~CPipeShowListBox();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
