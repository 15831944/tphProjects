#pragma once
#include "ServiceObject.h"

/**
*	Class to provide Mobile Elements and Resource Transferring service in ALTO System.
*/

class Terminal;
class Person;
class Passenger;

class CSimEngineTransferService : public ServiceObject < CSimEngineTransferService >
{
public:
	CSimEngineTransferService();
	virtual ~CSimEngineTransferService(void);

public:
	void initialize();

	bool isNeedTransfer(const Person* pPerson){ return false; }
	bool transferPassegenr(const Person* pPerson){};
	bool transferEnplanAgent(){};
	bool transferDeplaneAgent(){};

};


CSimEngineTransferService* simEngineTransferService();
AFX_INLINE CSimEngineTransferService* simEngineTransferService()
{
	return CSimEngineTransferService::GetInstance();
}





