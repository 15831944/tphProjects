#pragma once

class CUnselectableListBox : public CListBox
{
public:
	CUnselectableListBox();
	virtual ~CUnselectableListBox();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
