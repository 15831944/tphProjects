#pragma once
#include "PollutantSourcePage.h"

class CSourceOtherSourcePage :
	public CPollutantSourcePage
{
	enum ItemType
	{
		Type_SourceName,
		Type_Pollutant,
		Type_Emission
	};
	DECLARE_DYNCREATE(CSourceOtherSourcePage)
	DECLARE_MESSAGE_MAP()
public:
	CSourceOtherSourcePage(CSourceEmissionManager *pSourceEmissionManager,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList);
	CSourceOtherSourcePage();
	~CSourceOtherSourcePage(void);
	void ReloadTree();
	BOOL OnApply();
	void OnButtonAdd();
	void OnButtonDel();

	void OnNewOtherSource();
	void OnNewPollutant();
	void OnDelOtherSource();
	void OnDelPollutant();

	int GetItemType(HTREEITEM hItem);


	HTREEITEM AddOtherSourceItem(COtherSourceProperty* pOtherSourceProperty);
	HTREEITEM AddPollutantItem(COtherSourcePollutantData *pSetting,HTREEITEM hParentItem);
	
	void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual void OnTvnSelchangedTreePollutantsource(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	COtherSourceComposite	*m_pOtherSourceComposite;
};
