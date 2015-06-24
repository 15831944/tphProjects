#pragma once
#include "ResizePS\ResizableSheet.h"


// CFacilityDataSheet
class CFacilityDataPage;
class InputLandside;
class CFacilityBehaviorList;
class InputTerminal;
class CFacilityDataSheet : public CResizableSheet
{
	DECLARE_DYNAMIC(CFacilityDataSheet)

public:
	CFacilityDataSheet(InputLandside *pInptLandside,InputTerminal* _pInTerm, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFacilityDataSheet(InputLandside *pInptLandside,InputTerminal* _pInTerm, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CFacilityDataSheet();

public:
	CWnd			*m_pParent;
	int				m_nProjectID;
	InputLandside	*m_pInputLandside;
	InputTerminal   *m_pInputTerm;

	CFacilityBehaviorList *m_pBehavior;
protected:
	std::vector<CFacilityDataPage *> m_vPages;
	enum CFacilityBehaviorType
	{
		BehaviorType_CURBSIDE = 0,
		BehaviorType_BUSSTATION,
	//	BehaviorType_WALKWAY,
		BehaviorType_TAXIPOOL,
		BehaviorType_TAXIQUEUE,
		BehaviorType_PARKINGLOT
	};
protected:
	void InitSheet();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);



protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


