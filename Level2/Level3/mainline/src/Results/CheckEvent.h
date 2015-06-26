#pragma once
#include "Common\TERM_BIN.H"


class CheckEvent
{
public:
	static void check(int mobid, const MobEventStruct& track);
};
#ifdef _DEBUG
#define CHECKEVENT(paxid, e) CheckEvent::check(paxid, e)
#else
#define CHECKEVENT(paxid, e)
#endif
