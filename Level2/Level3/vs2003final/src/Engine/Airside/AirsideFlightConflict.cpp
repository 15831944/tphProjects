#include "StdAfx.h"
#include ".\airsideflightconflict.h"
#include "../../Common/termfile.h"

AirsideFlightConflict::AirsideFlightConflict(void)
{
}

AirsideFlightConflict::~AirsideFlightConflict(void)
{
}

void AirsideFlightConflict::WriteLog(ArctermFile& outFile)
{
	//int id
	outFile.writeInt(m_nUniqueID);
	//time
	outFile.writeInt((long)m_tTime);
	//reporting area
	outFile.writeInt(m_nReportAreaID);
	//who
	outFile.writeInt(m_nFltUID);	
	//conflict object
	outFile.writeInt(m_nConflictMobileID);
	//conflict type
	outFile.writeInt((int)m_eConflictType);
	//location type
	outFile.writeInt((int)m_eLocationType);
	//operation type
	outFile.writeInt((int)m_eOperationType);
	//action type
	outFile.writeInt((int)m_eActionType);

	outFile.writeLine();
}

const static int MAX_STRBUF_SIZE = 1024;

AirsideFlightConflict* AirsideFlightConflict::ReadLog( ArctermFile& inFile )
{
	long varInt;
	//char strbuf[MAX_STRBUF_SIZE];
	//get id
	int uUId;
	inFile.getInteger( uUId );

	////delay type
	//inFile.getInteger(varInt);
	AirsideFlightConflict* theConflict = new AirsideFlightConflict();
	if(theConflict) // read data
	{		
		theConflict->m_nUniqueID = uUId;

		inFile.getInteger(varInt);
		ElapsedTime tTime; tTime.setPrecisely(varInt);
		theConflict->m_tTime = tTime;

		inFile.getInteger(varInt);
		theConflict->m_nReportAreaID =varInt;

		inFile.getInteger(varInt);
		theConflict->m_nFltUID = varInt;

		inFile.getInteger(varInt);
		theConflict->m_nConflictMobileID = varInt;

		inFile.getInteger(varInt);
		theConflict->m_eConflictType =(FlightConflict::ConflictType)varInt;

		inFile.getInteger(varInt);
		theConflict->m_eLocationType = (FlightConflict::LocationType)varInt;

		inFile.getInteger(varInt);
		theConflict->m_eOperationType = (FlightConflict::OperationType)varInt;

		inFile.getInteger(varInt);
		theConflict->m_eActionType = (FlightConflict::ActionType)varInt;

	}
	inFile.getLine();
	return theConflict;
}

bool AirsideFlightConflict::IsIdentical(const AirsideFlightConflict* otherConflict)const
{
	bool bSame = (m_tTime == otherConflict->m_tTime) 
		&&(m_nFltUID == otherConflict->m_nFltUID)
		&&(m_nConflictMobileID == otherConflict->m_nConflictMobileID)
		&&(m_eLocationType == otherConflict->m_eLocationType);

	return bSame;
}