#ifndef SERVICE_TIME_DATABASE_H
#define SERVICE_TIME_DATABASE_H

#include "inputs\prochrch.h"

class INPUTS_TRANSFER ServiceTimeDatabase : public ProcessorHierarchy
{
public:
    ServiceTimeDatabase () : ProcessorHierarchy (ServiceTimeFile,(float)2.5)
        { units = "Seconds"; }
    virtual ~ServiceTimeDatabase () {};

    // delete all references to the passed index at p_level
    void deletePaxType (int p_level, int p_index);

    virtual const char *getTitle (void) const
        { return "Service Times database"; }
    virtual const char *getHeaders (void) const
        { return "Processor,Passenger Type,Service Time"; }

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData ( ArctermFile& p_file );
	virtual void writeData(ArctermFile& p_file) const;
	virtual bool getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
};

#endif
