#pragma once
#include "FloorPropertiesDlg.h"
class CDeck ;
class CDlgDeckProperties :
	public CProperiesDlg
{
public:
	CDlgDeckProperties(CDeck* _deck,CTermPlanDoc* _Doc);
	virtual ~CDlgDeckProperties(void);
	void OnGridOptions();
	void OnOK();
	void OnCancel();
private:
	CDeck* m_deck ;
protected:
	BOOL OnInitDialog() ;
};
