#ifndef __AIRSIDE_GATEMODEL_DEF
#define __AIRSIDE_GATEMODEL_DEF

#include "../../Common/ARCVector.h"


#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "Entity.h"
#include "../SimClock.h"

#include "../../Inputs/AirsideInput.h"

class AirsideInput;
NAMESPACE_AIRSIDEENGINE_BEGINE


class AirportModel;
class SimFlight;


class ENGINE_TRANSFER Gate : public Entity{
public:

	Point m_pos;      //position

	int m_inNodeId;    //node id in taxiway push in
	int m_outNodeId;   // node id in taxiway push out
	

};

typedef EntContainer<Gate> GatesList;
typedef GatesList::iterator GateIter;
typedef GatesList::const_iterator GateIter_const;

class ENGINE_TRANSFER GateModel{
public:
	GateModel(AirportModel*pAirport){ m_pAirport = pAirport; m_pAirsideInput = NULL; }
	~GateModel();

	AirportModel * getAirport(){ return m_pAirport; }

	GatesList & getGates(){ return m_vgates; } 
	
	void Build(CAirsideInput* _inputC, AirsideInput* _inputD);
	
	//get a available gate at time;
	Gate * getGate(SimFlight * pFlight);
	Gate * getArrGate(SimFlight * pFlight);
	Gate * getDepGate(SimFlight * pFlight);
	void DistributePushback(SimFlight* _flight);
	bool DistributeInGate(SimFlight * _flight);
	int getPushbackTime(SimFlight* _flight);

private:
	AirportModel * m_pAirport;
	AirsideInput * m_pAirsideInput;
	GatesList m_vgates;
};







NAMESPACE_AIRSIDEENGINE_END






#endif