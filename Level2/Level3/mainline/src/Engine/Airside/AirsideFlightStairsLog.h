#pragma once
#include "../../Common/elaptime.h"
#include "../../Common/Point2008.h"
#include "../EngineDll.h"

class ArctermFile;
class ENGINE_TRANSFER AirsideFlightStairsLog
{
public:
	AirsideFlightStairsLog(void);
	~AirsideFlightStairsLog(void);

	void WriteLog(ArctermFile& outFile);
	void ReadLog(ArctermFile& inFile);

public:
	ElapsedTime m_tStartTime;
	ElapsedTime m_tEndTime;
	char m_cFltStatus;
	int m_nStairId;
	int m_nFlightId;
	double m_dStairwidth;


	CPoint2008 m_DoorPos;
	CPoint2008 m_GroundPos;

};
