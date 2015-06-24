#pragma once
#include "PollutantSourcePage.h"
#include "..\InputEnviroment\SourceEmissionManager.h"
#include "..\InputAirside\VehicleSpecifications.h"
#include ".\TermPlanDoc.h"
#include "Landside\InputLandside.h"

class CSourceVehiclePage :
	public CPollutantSourcePage
{
	enum EditItemType
	{
		Type_Acceleration=1,
		Type_SteadySpeed,
		Type_Deceleration,
		Type_Idle
	};

	DECLARE_DYNCREATE(CSourceVehiclePage )
	DECLARE_MESSAGE_MAP()

public:
	CSourceVehiclePage (CSourceEmissionManager *pSourceEmissionManager,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList,CWnd* pParentWnd,UINT nIDCaption);
	CSourceVehiclePage();
	~CSourceVehiclePage (void);
	BOOL OnApply();
	void ReloadTree();
	void ReloadGSETree();
	void ReloadAMETree();
	void ReloadLandsideTree();

	HTREEITEM AddGSEVehicleItem(CVehicleProperty *pVehicleProperty,CString strVehicleName);
	void AddPollutantChildrenItem(CVehicleProperty *pVehicleProperty,HTREEITEM hPollutantItem,FuelProperty *pFuelPro=NULL);
	HTREEITEM AddPollutantSettingItem(CString strPollutantSymbol,CVehiclePollutantSetting *pSetting,HTREEITEM parentItem);

	int GetProjectID();
	InputLandside *GetInputLanside();

protected:
	BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);


private:
	Source_VehicleType m_VehicleType;
	CVehicleComposite* m_pVehicleComposite;
	int m_nPrjID;

};
