#pragma once
#include "CommonDll.h"
#include "ALTObjectID.h"

class COMMON_TRANSFER OneToOneProcess
{
public:
	OneToOneProcess(ALTObjectID altObject,ALTObjectIDList altList);
	~OneToOneProcess(void);
	
	bool GetOneToOneObject(ALTObjectID& resultObject,int _nDestIdLength);
private:
	ALTObjectID m_altObject;
	ALTObjectIDList m_altList;
};
