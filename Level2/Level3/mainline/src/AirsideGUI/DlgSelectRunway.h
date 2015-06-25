#pragma once
#include "AirsideGUIAPI.h"

class AIRSIDEGUI_API CDlgSelectRunway : public CDialog
{
public:
	CDlgSelectRunway(int nProjID, int nSelRunwayID = -1, CWnd* pParent = NULL);
	~CDlgSelectRunway();

	int GetSelectedRunway() const				{	return m_nSelRunwayID;	}
	int GetSelectedAirport() const				{	return m_nSelAirportID;	}
	int GetSelectedRunwayMarkingIndex() const	{	return m_nSelRunwayMarkingIndex; }
protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	void SetTreeContents();
	bool IsAirportItem(HTREEITEM hItem);
	bool IsRunwayMarkingItem(HTREEITEM hItem);

	int m_nProjID;
	int m_nSelRunwayID;
	int m_nSelRunwayMarkingIndex;
	int m_nSelAirportID;

	CTreeCtrl m_wndRunwayTree;

	afx_msg void OnSelChangedTreeRunways(NMHDR *pNMHDR, LRESULT *pResult);
};
