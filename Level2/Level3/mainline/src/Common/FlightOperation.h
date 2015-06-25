#pragma once
#include "common\IDGather.h"
#include "common\ALTObjectID.h"

class CFlightID;

class COMMON_TRANSFER CFlightOperation
{
public:
	CFlightOperation(void);
	~CFlightOperation(void);

	CFlightOperation& operator = (const CFlightOperation& flight);
	bool operator == (const CFlightOperation& flight) const;
	bool operator < (const CFlightOperation& flight) const;
	void ClearData();


protected:
	CFlightID	m_ID;
	CAirportCode m_Airport;    
	ALTObjectID m_Stand;
	short m_nStandIdx;
	long m_Time;                       
};
