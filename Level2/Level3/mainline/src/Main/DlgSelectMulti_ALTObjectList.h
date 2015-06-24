#pragma once
#include "Resource.h"
#include "DlgSelectALTObject.h"
#include <vector>

class DlgSelectMulti_ALTObjectList : public CDlgSelectALTObject
{
public:
	DlgSelectMulti_ALTObjectList(int nProjID, std::vector<ALTOBJECT_TYPE> objectTypes, CWnd* pParent = NULL);
	~DlgSelectMulti_ALTObjectList(void);

protected:
	virtual BOOL OnInitDialog();

private:
	std::vector<ALTOBJECT_TYPE> m_vObjectTypes;
};
