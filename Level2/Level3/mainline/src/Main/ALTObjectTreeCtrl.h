#pragma once
#include "colortreectrl.h"

#include "../InputAirside/ALTObject.h"

class CALTObjectTreeCtrl :
	public CColorTreeCtrl
{
public:
	CALTObjectTreeCtrl(void);
	~CALTObjectTreeCtrl(void);

	CString m_strSelectedID;

	HTREEITEM m_hSelItem;

	virtual void LoadData(int nAptID, ALTOBJECT_TYPE nType);

	void SelectALTObject(const ALTObjectID& objID);

	HTREEITEM AddTreeItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);
	CString GetTreeItemFullString(HTREEITEM hItem);

protected:
	ALTObjectList m_vObjects;

};
