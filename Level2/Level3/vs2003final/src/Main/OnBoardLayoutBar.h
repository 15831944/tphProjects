#pragma once

#include "ToolBar24X24.h"

class CAircraftComponentCategory;
class CComponentMeshModel;
class InputOnboard;
class CACComponentModelBar :
	public CToolBar24X24
{
public:
	CACComponentModelBar(void);
	virtual ~CACComponentModelBar(void);
	///CComboBoxXP m_combActiveCategory;
	//CComboBoxXP m_combActiveModel;
	bool bIsCreated()const{ return m_bCreated; }
 
	void SetInputOnboard(InputOnboard* pInputOnboard);
public:
	void OnDeleteComponetOrCategory();
	void OnUpdateUIDelCompOrCat(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeActiveCategoryComboBox();
	afx_msg void OnSelChangeActiveModelComboBox();




	//BOOL UpdateComboBox();
	CAircraftComponentCategory* m_pActiveCat;
	CComponentMeshModel* m_pActiveModel;
protected:
	InputOnboard * m_pInputOnboard;
	bool m_bCreated;
};
