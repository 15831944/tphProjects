#pragma once
/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTypeShowListBox window

class CAllProcessorTypeShowListBox : public CCheckListBox
{
public:
	CAllProcessorTypeShowListBox();
	virtual ~CAllProcessorTypeShowListBox();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
