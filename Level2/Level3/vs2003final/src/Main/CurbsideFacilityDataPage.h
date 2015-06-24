#pragma once
#include "facilitydatapage.h"
#include "Resource.h"


class CCurbsideFacilityDataPage :
	public CFacilityDataPage
{
public:
	CCurbsideFacilityDataPage(InputLandside *pInputLandside,
		InputTerminal* _pInTerm,
		CFacilityBehaviorList *pBehaviorDB,
		enum ALTOBJECT_TYPE enumFacType,
		UINT nIDCaption,
		int nProjectID);
	~CCurbsideFacilityDataPage(void);

protected:

	virtual void LoadTreeProperties();

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void UpdateStoppingTimeItem();


protected:
	HTREEITEM m_hStoppingTime;

};
