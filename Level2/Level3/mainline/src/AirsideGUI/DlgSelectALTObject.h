#pragma once
#include "../InputAirside/ALTObject.h"

class CDlgSelectALTObject : public CDialog
{
public:
	CDlgSelectALTObject(int nProjID, ALTOBJECT_TYPE objectType, CWnd* pParent = NULL);
	virtual ~CDlgSelectALTObject();
	enum { IDD = IDD_DIALOG_ALTOBJECT };

	const ALTObjectID& GetALTObject() const { return m_selectedALTObjectID; }
	ALTOBJECT_TYPE GetALTObjectType(void)const{return (m_objectType);} 
	int GetProjectID(void){return (m_nProjectID);}

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	void InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID);
	HTREEITEM FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem);
	int GetTreeItemDepthIndex(HTREEITEM hTreeItem);

	CTreeCtrl m_wndALTObjectTree;
	ALTOBJECT_TYPE m_objectType;
	int m_nProjectID;
	ALTObjectID m_selectedALTObjectID;
};
