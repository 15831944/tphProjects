#ifndef DEPENDENT_SOURCE_PROCESSOR_H
#define DEPENDENT_SOURCE_PROCESSOR_H

#include "common\template.h"
#include "common\containr.h"
#include "engine\process.h"
#include "inputs\procdist.h"
#include "common\fsstream.h"

//Tied up with dependent sink.
class ENGINE_TRANSFER DependentSource : public Processor
{
protected:
	// 0 has the highest priority.
	//int m_nPriority;//改动		

    //person as dependent source.
    Person* m_pOccupant;

    //Index of dependent sink.
    GroupIndex* m_pSinks;

    //number of dependent sinks assigned to a processor.
    int m_nSinkCount;

	// reserved sink for used by leave server.
	int m_idxReservedSink;		

public:
//    DependentSource ():flow( false ) { occupant = NULL; sinks = NULL; sinkCount = 0; m_nPriority = 0; }
	DependentSource (){ m_pOccupant = NULL; m_pSinks = NULL; m_nSinkCount = 0;  m_idxReservedSink=-1; }//改动
	
    virtual ~DependentSource ();

	//Returns processor type.
    virtual int getProcessorType (void) const { return DepSourceProc; }

    //Returns processor name.
    virtual const char *getProcessorName (void) const { return "DPND_SOURCE"; }

    //It initializes a Processor as a DependentSource and assigns it a list of DependentSinks.
    void initDependent (int listCount, const GroupIndex *sinkList);

    void initSpecificMisc (const MiscData *miscData);


    // DependentSource method called by sinks when another client can be served.
    void release (ElapsedTime availTime);

	// Returns true if the processor can serve the passed pax type.
	// if a dependent source can serve a pax, and one of its sinks does too, return true
	virtual int canServe (const CMobileElemConstraint& paxType,   CanServeProcList* _canServeProcList = NULL) const;

    void beginService (Person *person, ElapsedTime curTime);

    //It checks if the processor is available. Returns true if available otherwise false.
    int isAvailable (void) const { return available; }

	// Clean all sim engine related data, for the new sim process
	virtual void CleanData();
	
	Person* GetOccupant(){ return m_pOccupant;}	
	
	// copy dependent sink's roster to its source
	// discarded by Benny, 20091214
	void CopySinkAssignToSource();

	int GetReservedSink(){ return m_idxReservedSink; }

	void SetReservedSink( int _idxSink ){ m_idxReservedSink = _idxSink; }

	int GetSinkCount(void) { return m_nSinkCount;}

	GroupIndex *GetSink(void){return m_pSinks;}

	virtual Processor* CreateNewProc();

	//int GetPriority() const { return m_nPriority; }//改动

	//void SetPriority( int _nPriority ){ m_nPriority = _nPriority; }//改动

protected:
    
	//It tests each DependentSink to see if it is available (isNextAvail)
    int nextAvailable (Person *_pPerson);

	//copy constructor.
    DependentSource (const DependentSource& aProc) ;

	const DependentSource& operator = (const DependentSource& aProc) ;

};

#endif
