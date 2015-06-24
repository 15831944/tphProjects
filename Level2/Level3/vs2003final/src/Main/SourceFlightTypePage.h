#pragma once
#include "PollutantSourcePage.h"
#include "..\Common\AirportDatabase.h"
#include ".\TermPlanDoc.h"


class CSourceFlightTypePage :
	public CPollutantSourcePage
{
	DECLARE_DYNCREATE(CSourceFlightTypePage )

	enum EditItemType
	{
		ArrAPUTime=1,
		DepAPUTime,
		GroundAcc,
		GroundTaxi,
		GroundDec,
		APUGenerated,
		TakeOff,
		Landing
	};

public:
	CSourceFlightTypePage(CSourceEmissionManager *pSourceEmissionManager,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList,CWnd* pParentWnd);
	CSourceFlightTypePage();
	~CSourceFlightTypePage(void);
	DECLARE_MESSAGE_MAP()
// 	BOOL OnInitDialog();
	void OnNewFlightType();
	void OnDelFlightType(); 
	void ReloadTree();
	void OnButtonAdd();
	void OnButtonDel();

	HTREEITEM AddFltTypeItem(CSourceFlightTypeProperty *pSourceFltTypePro);
	void AddPollutantChildrenItem(CSourceFlightTypeProperty *pSourceFltTypePro,HTREEITEM hPollutantItem,FuelProperty *pFuelPro=NULL);
	HTREEITEM AddPollutantSettingItem(CString strPollutantSymbol,CFlightTypePollutantSetting *pSetting,HTREEITEM parentItem);

	virtual BOOL OnApply();

	void OnTvnSelchangedTreePollutantsource(NMHDR *pNMHDR, LRESULT *pResult);

	CAirportDatabase *GetAirportDatabase();

	
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CAirportDatabase *m_pAirportDatabase;
	CSourceFlightTypeComposite* m_pSourceFlightTypeComposite;
};
