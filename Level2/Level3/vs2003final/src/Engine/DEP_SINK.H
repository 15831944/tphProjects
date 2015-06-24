#include "common\fsstream.h"
#ifndef DEPENDENT_SINK_PROCESSOR_H
#define DEPENDENT_SINK_PROCESSOR_H

#include "common\template.h"
#include "common\containr.h"
#include "engine\process.h"
#include "inputs\miscproc.h"
#include "inputs\assign.h"
#include "engine\DEP_SRCE.H"

struct PriorityDSource
{
	GroupIndex m_sourceGroup;
	int m_nPriority;
};


class ENGINE_TRANSFER DependentSink : public Processor
{
protected:
    //Index of dependent source.
	std::vector<struct PriorityDSource> m_sourceList;//改动
	//GroupIndex* m_sources;
	//int m_sourceCount;

	bool m_bReserved;		// set when sink is reserved by some source
	
	bool m_bSetInSink;		// used when set in sink page.
    
	CMobileElemConstraint m_closingConstrain;


public:
    DependentSink () {  m_bReserved = false; m_bSetInSink = false; }

	virtual ~DependentSink () {}

    //Returns processor type.
    virtual int getProcessorType (void) const { return DepSinkProc; }

    //Returns processor name.
    virtual const char *getProcessorName (void) const { return "DPND_SINK"; }

    //Initializes a Processor as a DependentSink and assigns it a list of source processors.
	void initDependent (int p_count, const GroupIndex * p_list );

	// 
	void initDependent( const MiscProcessorIDListWithLevel* _pIdList );
		

    virtual void addPerson (Person *aPerson);

    virtual void initSpecificMisc (const MiscData *miscData);

    virtual void makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay);

    //Returns true if both the sink is open and available; otherwise false.
    int isAvailable (void) const { return isOpen && available; }

	int isClosingAvailable(void) const{return isClosing&&available;	}

	void SetReserved( bool _bReserved ){ m_bReserved = _bReserved; }

	bool GetReserved(){ return m_bReserved;	}

	// given an priority, check if any higher priority processor has some one waiting.
	bool AnyOneWaitInHigherPrioritySource( int _nMyPriority );

	virtual bool IsWantSettingClosing(CMobileElemConstraint& _constrain);

	virtual Processor* CreateNewProc();

	//int GetSourceCount() { return m_sourceCount; }

	int GetPriorityofSource( int Id);//根据某个source的Id找出与其对应的priority

	void WakeupPaxWaitingOnDependSource(const ElapsedTime& releaseTime);

protected:
    void releaseNext (ElapsedTime availTime);

};

#endif
