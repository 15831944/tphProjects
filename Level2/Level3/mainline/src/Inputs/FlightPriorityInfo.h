// FlightPriorityInfo.h: interface for the CFlightPriorityInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLIGHTPRIORITYINFO_H__50A4EE06_467F_4F74_A875_9E854EB590C3__INCLUDED_)
#define AFX_FLIGHTPRIORITYINFO_H__50A4EE06_467F_4F74_A875_9E854EB590C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\DataSet.h"
//#include "vector"
//#include "../Common/FLT_CNST.H"
#include "../common/AirsideFlightType.h"
#include <CommonData/procid.h>
//using namespace std;
#include "./../InputAirside/ALTObject.h"
typedef struct _FlightGate
{
	AirsideFlightType flight;
	ALTObjectID procID;
} FlightGate;

class  INPUTS_TRANSFER FlightPriorityInfo : public DataSet  
{
public:

	std::vector<FlightGate> m_vectFlightGate;

	FlightPriorityInfo();
	
	virtual ~FlightPriorityInfo();

    virtual void clear (void) {	m_vectFlightGate.clear();}

    virtual void readData (ArctermFile& p_file);

    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Flight priority"; }
    virtual const char *getHeaders (void) const
        { return "Flight,Gate"; }
};

#endif // !defined(AFX_FLIGHTPRIORITYINFO_H__50A4EE06_467F_4F74_A875_9E854EB590C3__INCLUDED_)
