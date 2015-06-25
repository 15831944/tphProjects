#pragma once
#include "ALTObjectID.h"

class ALTObjectIDCompareLess
{
public:
	bool operator()(const ALTObjectID& id1,const ALTObjectID& id2);	
};


