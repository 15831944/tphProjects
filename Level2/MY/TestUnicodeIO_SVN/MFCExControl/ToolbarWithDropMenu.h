#pragma once
#include <afxtempl.h>
#include <vector>

typedef struct tagDROPDOWNLIST
{
	UINT nIDButton;
	UINT nIDMenu;
}DROPDOWNLIST;


// CToolbarWithDropMenu

class MFCEXCONTROL_API CToolbarWithDropMenu : public CToolBar
{
	DECLARE_DYNAMIC(CToolbarWithDropMenu)

public:
	CToolbarWithDropMenu();
	virtual ~CToolbarWithDropMenu();

protected:
	afx_msg void OnDropDownBtn( NMHDR * pNotifyStruct, LRESULT * result );
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bDropDownStyle;

	//drop down menu
	CArray<DROPDOWNLIST,DROPDOWNLIST> m_DDBtnArray;
	std::vector<UINT> m_vDisabledMenuItems;
public:
	void AddDropDownBtn(UINT nIDBUTTON, UINT nIDMENU);
	void RemoveDropDownBtn(UINT nIDBUTTON);
	void AddDisabledMenuItem(UINT nIDMeunItem);
	void ClearDisabledMenuItems();


};


