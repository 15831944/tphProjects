#ifndef PASSENGER_FLOW_DATABASE_H
#define PASSENGER_FLOW_DATABASE_H

#include "inputs\prochrch.h"
#include "inputs\FlowItemEx.h"

typedef std::vector<ProcessorID> VECTPROCID;
typedef std::vector<VECTPROCID> VECTVECTPROCID;
typedef std::vector<CFlowItemEx> VECTFLOWITEMEX;
typedef std::vector<VECTFLOWITEMEX> VECTVECTFLOWITEMEX;

class PassengerFlowDatabase : public ProcessorHierarchy
{
public:
    PassengerFlowDatabase (int p_file) : ProcessorHierarchy( p_file , 2.8f )
        { units = NULL; }
    virtual ~PassengerFlowDatabase () {};

    // removes and deallocates all entries that fit passed procID
    // flows that are directed to passed id will be realigned or removed
    void removeAllEntries (const ProcessorID& id);

	bool getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
    // finds all entries that fit passed oldID and changes them to newID
    void replaceAllEntries (const ProcessorID& oldID, const ProcessorID& newID,InputTerminal*_term);

    // delete all references to the passed index at p_level
    void deletePaxType (int p_level, int p_index);
	void deletePaxType( const CMobileElemConstraint* _paxConstraint );

	void deletePipe( int _iPipeIdx );
	bool IfUseThisPipe( int _iPipeIdx, CMobileElemConstraint*& pConstr) const;

    virtual void initDefaultValues (void);

    virtual const char *getTitle (void) const
        { return "Passenger Flows database"; }
    virtual const char *getHeaders (void) const;

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	// check if the Mobile Type has its onw start;
	bool HasOwnStart( const CMobileElemConstraint& _constr );

	void InitFlowRules();

	// when delete a process, add it's all children to it's parent
	bool AddChildToParent(const ProcessorID& id);
	//get the parents' ids and children's ids of the id be deleted
	void GetParentsAndChildren(const ProcessorID& id, VECTVECTPROCID &vvParentsID, VECTVECTPROCID &vvChildrenID);
	//get the flowitemex vector of the id be deleted
	void GetFlowItemOfProcessID(const ProcessorID& id, VECTVECTFLOWITEMEX &vvFlowItemEx);
};

#endif
