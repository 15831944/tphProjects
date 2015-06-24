#include "StdAfx.h"
#include ".\airsideflightstairslog.h"
#include "../../Common/termfile.h"

AirsideFlightStairsLog::AirsideFlightStairsLog(void)
:m_tStartTime(-1L)
,m_tEndTime(-1L)
,m_nFlightId(-1)
,m_nStairId(-1)
,m_dStairwidth(0.9)
{
}

AirsideFlightStairsLog::~AirsideFlightStairsLog(void)
{
}

void AirsideFlightStairsLog::WriteLog( ArctermFile& outFile )
{
	outFile.writeInt((long)m_tStartTime);
	outFile.writeInt((long)m_tEndTime);

	outFile.writeInt(m_nStairId);
	outFile.writeInt(m_nFlightId);
	outFile.writeChar(m_cFltStatus);

	outFile.writeDouble(m_dStairwidth);

	outFile.writePoint2008(m_DoorPos);
	outFile.writePoint2008(m_GroundPos);		


	outFile.writeLine();
}

void AirsideFlightStairsLog::ReadLog( ArctermFile& inFile )
{
	long varInt;

	inFile.getInteger(varInt);
	m_tStartTime.setPrecisely(varInt);

	inFile.getInteger(varInt);
	m_tEndTime.setPrecisely(varInt);

	inFile.getInteger(varInt);
	m_nStairId = varInt;

	inFile.getInteger(varInt);
	m_nFlightId = varInt;

	inFile.getChar(m_cFltStatus);

	//inFile.getInteger(varInt);

	inFile.getFloat(m_dStairwidth);

	inFile.getPoint2008(m_DoorPos);
	inFile.getPoint2008(m_GroundPos);


	inFile.getLine();

}