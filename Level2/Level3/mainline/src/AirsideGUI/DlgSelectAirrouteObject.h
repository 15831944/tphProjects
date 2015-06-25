#pragma once
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/AirRoute.h"

class CDlgSelectAirrouteObject :
	public CDialog
{
public:
	CDlgSelectAirrouteObject(int nProjID, CAirRoute::RouteType emType, CWnd* pParent = NULL);
	virtual ~CDlgSelectAirrouteObject();
	enum { IDD = IDD_DIALOG_ALTOBJECT };

	const ALTObjectID& GetALTObject() const { return m_selectedALTObjectID; }
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
	CAirRoute::RouteType m_emType;
	int m_nProjectID;
	ALTObjectID m_selectedALTObjectID;
};
