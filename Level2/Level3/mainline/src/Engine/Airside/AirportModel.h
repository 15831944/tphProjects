#ifndef __AIRSIDE_SIMMODEL_DEF
#define  __AIRSIDE_SIMMODEL_DEF

#include "../EngineDll.h"
#include "../EngineConfig.h"


class CAirsideInput;
class AirsideInput;


NAMESPACE_AIRSIDEENGINE_BEGINE



class AirspaceModel;
class RunwayModel;
class TaxiwayModel;
class GateModel;

//Abstraction of the airport 
class ENGINE_TRANSFER AirportModel{
public:

	AirportModel();
	~AirportModel();
	
	AirspaceModel * getAirspaceModel(){return m_pAirspace;}
	RunwayModel * getRunwayModel(){ return m_pRunwayModel; }
	TaxiwayModel * getTaxiwayModel(){ return m_pTaxiwayModel; }
	GateModel * getGateModel(){ return m_pGateModel; }

	//build the airport from the Input
	bool Build( CAirsideInput* _inputC, AirsideInput* _inputD);
	
	void Clear();
private:
	AirspaceModel * m_pAirspace;
	RunwayModel * m_pRunwayModel;
	TaxiwayModel * m_pTaxiwayModel ;
	GateModel * m_pGateModel ;

};



NAMESPACE_AIRSIDEENGINE_END














#endif