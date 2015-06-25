#pragma once

class CDlgSelectSideStepToRunway : public CDialog
{
public:
	CDlgSelectSideStepToRunway(int nProjID, CString strApproachRunway, int nSelRunwayID = -1, CWnd* pParent = NULL);
	~CDlgSelectSideStepToRunway();

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
	CString m_strApproachRunway;
	CTreeCtrl m_wndRunwayTree;

	afx_msg void OnSelChangedTreeRunways(NMHDR *pNMHDR, LRESULT *pResult);
};
