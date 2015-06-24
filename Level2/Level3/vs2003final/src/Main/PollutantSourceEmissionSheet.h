#pragma once
#include "ResizePS\ResizableSheet.h"
#include "..\inputs\in_term.h"
#include ".\TermPlanDoc.h"
#include "..\InputEnviroment\SourceEmissionManager.h"
#include "..\Common\AirportDatabase.h"
#include "..\InputEnviroment\FuelProperties.h"
#include "..\InputEnviroment\AirPollutantList.h"

// typedef std::map<int,CString> PollutantIDSymbolMap;
class CPollutantSourceEmissionSheet :
	public CResizableSheet
{
	DECLARE_DYNCREATE(CPollutantSourceEmissionSheet)
public:
	CPollutantSourceEmissionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage = 0);
	virtual ~CPollutantSourceEmissionSheet(void);
protected:
	virtual BOOL OnInitDialog();
// 	CString GetProjPath();
// 	InputTerminal* GetInputTerminal();
	CAirportDatabase *GetAirportDatabase();
// 	int GetProjectID();
// 	void InitPollutantMap();

	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParent;
	CSourceEmissionManager *m_pSourceEmissionManager;
	FuelProperties *m_pFuelProperties;
	CAirPollutantList *m_pAirPollutantList;
// 	PollutantIDSymbolMap m_mapPollutantIDSymbol;

};
