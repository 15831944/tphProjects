// IntervalStat.cpp: implementation of the CIntervalStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "IntervalStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntervalStat::CIntervalStat()
{

}

CIntervalStat::~CIntervalStat()
{

}

void CIntervalStat::update (int value)
{
    count++;
    total += value;

    if (value < minimum)
        minimum = value;

    if (value > maximum)
        maximum = value;
}


