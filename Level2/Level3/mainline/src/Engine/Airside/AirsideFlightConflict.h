#pragma once
#include "../EngineDll.h"
#include "CommonInSim.h"
#include "../../Common/elaptime.h"

class ArctermFile;
class ENGINE_TRANSFER AirsideFlightConflict
{
public:
	AirsideFlightConflict(void);
	~AirsideFlightConflict(void);

	bool IsIdentical(const AirsideFlightConflict* otherDelay) const;
	void WriteLog(ArctermFile& outFile);
	static AirsideFlightConflict* ReadLog(ArctermFile& inFile);

public:
	int m_nUniqueID;
	ElapsedTime m_tTime;
	int m_nReportAreaID;
	int m_nFltUID;
	int m_nConflictMobileID;
	FlightConflict::ConflictType m_eConflictType;
	FlightConflict::LocationType m_eLocationType;
	FlightConflict::OperationType m_eOperationType;
	FlightConflict::ActionType m_eActionType;

};
