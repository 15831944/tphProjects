#include "StdAfx.h"
#include ".\dlgselectmulti_altobjectlist.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"

DlgSelectMulti_ALTObjectList::DlgSelectMulti_ALTObjectList(int nProjID, std::vector<ALTOBJECT_TYPE> objectTypes, CWnd* pParent /*= NULL*/)
:CDlgSelectALTObject(nProjID,ALT_UNKNOWN, pParent)
{
	m_vObjectTypes.clear();

	if (!objectTypes.empty())
		m_vObjectTypes.assign(objectTypes.begin(),objectTypes.end());
}

DlgSelectMulti_ALTObjectList::~DlgSelectMulti_ALTObjectList(void)
{
}

BOOL DlgSelectMulti_ALTObjectList::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjectID, vAirportIds);
	ALTAirport airport;
	airport.ReadAirport(vAirportIds.at(0));
	airport.getName();
	HTREEITEM hAirport = m_wndALTObjectTree.InsertItem(airport.getName());
	m_hItemAll = m_wndALTObjectTree.InsertItem(_T("All"),hAirport);
	int nCount = m_vObjectTypes.size();
	for (int i =0; i < nCount; i++)
	{
		m_objectType = m_vObjectTypes.at(i);
		InitTreeContentsForAirport(hAirport, vAirportIds.at(0));
	}

	return TRUE;
}