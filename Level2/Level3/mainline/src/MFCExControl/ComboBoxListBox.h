#pragma once
#include "vector"

using namespace std;
#include "..\MFCExControl\SimpleToolTipListBox.h"

///////////////////////////////////////////////////////////////////////////
// CComboBoxListBox window
#define WM_COMBO_IN_LISTBOX  1001

class MFCEXCONTROL_API CComboBoxListBox : public CSimpleToolTipListBox
{
public:
	CComboBoxListBox();
	virtual ~CComboBoxListBox();

protected:
	DECLARE_MESSAGE_MAP()
	CComboBox m_curComboBox;
	
public:
	int AddItem(CStringList* pStrList);
	void SetCurText();
	void DelItem(int nIndex);
	void KillCombofocus();

public:
	afx_msg void OnComboKillfocus();
	afx_msg void OnComboSelChange();

};