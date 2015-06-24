#pragma once

//#include "..\Main\CallOutData\CallOutDataManager.h"
#include "..\Main\ARCReportManager.h"
#include "engine\Terminal.h"
#include "results\AirsideSimLogs.h"
#include "InputAirside\InputAirside.h"
#include "Economic\EconomicManager.h"
#include "Main\CalloutDispSpec.h"
#include "engine\ARCPortEngine.h"
#include "Landside/LandSideOperationTypeData.h"



class Terminal;
class InputOnboard;
class CAirsideInput;
class OnBoradInput;
class InputAirside;
class OutputAirside;
class CEconomicManager;
class CTermPlanDoc;
//class CLandSideOperationTypeData;
class InputLandside;
class OutputLandside;
class ProjectCommon;

class CARCPort
{
public:
	CARCPort(CTermPlanDoc *pDoc);
	~CARCPort(void);

public:
	//CalloutDispSpecs m_calloutDispSpecs;
	CCongestionAreaManager m_congManager;

	//terminal
	Terminal m_terminal;
	//airside
	InputAirside m_inputairside;
	CAirsideInput* m_pAirSideInput;
	OutputAirside * m_pAirsideOutput;
	//landside	
	InputLandside* m_pInputLandside;
	OutputLandside* m_OutputlandsiedInst;
	//output
	InputOnboard* m_pInputOnboard;

	//report
	CARCReportManager m_arcReportManager;

	//economic 
	CEconomicManager m_economicManager;

	CARCportEngine m_engine;
public:

	ProjectCommon * getProjectCommon();

	Terminal& getTerminal(){return m_terminal;}

	OutputAirside * GetOutputAirside(){ return m_pAirsideOutput; }

	InputOnboard* GetInputOnboard(){ return  m_pInputOnboard; }

	InputAirside& GetInputAirside(){return m_inputairside;}

	InputLandside* GetInputLandside(){ return m_pInputLandside; }
	OutputLandside* GetOutputLandside(){ return m_OutputlandsiedInst; }
	
	
	CARCReportManager& GetARCReportManager() { return m_arcReportManager; }

	CCongestionAreaManager& getCongestionAreaManager(){ return m_congManager;}

	//economic 
	CEconomicManager& getEconomicManager(){ return m_economicManager;}

	//CLandSideOperationTypeData *landSideOperationList;


public:
	//for engine
	void SetFireEvacuation( const ElapsedTime& _time ) { m_engine.SetFireEvacuation(_time);	}
	ElapsedTime GetFireEvacuation()const { return m_engine.GetFireEvacuation();	}

	void SetFireOccurFlag( bool _bFlag ){ m_engine.SetFireOccurFlag(_bFlag);}
	bool GetFireOccurFlag()const { return m_engine.GetFireOccurFlag();}

private:
	//all the data in the object is reference
	//every time you get the data
	//it will refresh
	//
	ProjectCommon *m_pCommonData;

};















