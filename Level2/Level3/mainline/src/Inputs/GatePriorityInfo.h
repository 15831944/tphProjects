// GatePriorityInfo.h: interface for the GatePriorityInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEPRIORITYINFO_H__55FEDA43_12F8_48AF_A873_E408623A0846__INCLUDED_)
#define AFX_GATEPRIORITYINFO_H__55FEDA43_12F8_48AF_A873_E408623A0846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\DataSet.h"
#include <CommonData/procid.h>
#include "../InputAirside/ALTObject.h"

class INPUTS_TRANSFER GatePriorityInfo : public DataSet 
{
public:

	//ProcessorIDList procIDList;
	ALTObjectIDList altObjectIDList;

	GatePriorityInfo();
	
	virtual ~GatePriorityInfo();

    virtual void clear (void);

    virtual void readData (ArctermFile& p_file);

    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Gate priority"; }
    virtual const char *getHeaders (void) const
        { return "Gates"; }
	
};

#endif // !defined(AFX_GATEPRIORITYINFO_H__55FEDA43_12F8_48AF_A873_E408623A0846__INCLUDED_)
