#ifndef GATE_PROCESSOR_H
#define GATE_PROCESSOR_H

#include "common\template.h"
#include "common\containr.h"
#include "engine\process.h"
#include "inputs\pax_cnst.h"
#include "BridgeConnector.h"

enum GateType
{
	NoGateType,
	ArrGate,
	DepGate
};
//This class attaches all relative waitarea to a gate processor.
class ENGINE_TRANSFER GateProcessor : public Processor
{
protected:

    //The index of a group of waitarea
    GroupIndex *gateWaitArea;

    //Number of wait area of a group.
    int waitAreaCount;
	
	//flag the gate type
	GateType m_emGateType;

	std::vector<GroupIndex > m_vGateGroupIndex;

	//Link bridge connector
	std::vector<GroupIndex>m_vBridgeGroupIndex;

	//latest time of the processor service the pax
	ElapsedTime m_eLastPaxServiceTime;
	//last time of the pax walk out of constraint
	ElapsedTime m_eLastPaxOutConsTime;

private:
	// flag the gate can be ACStandGate or not
	//it is useless, T241
	BOOL fCanBeACStandGate;

public:
    GateProcessor ();;

    //Copy constructor.
    GateProcessor (const GateProcessor& p_proc) { *this = p_proc; };

    virtual ~GateProcessor () 
	{ if (waitAreaCount) delete gateWaitArea;
            gateWaitArea = NULL; };

    void initHoldAreas (int p_count, const GroupIndex *p_holds);

    virtual void setIndex (int p_ndx);

    //Returns processor type.
    virtual int getProcessorType (void) const { return GateProc; };

    //Returns processor name.
    virtual const char *getProcessorName (void) const { return "GATE"; };

    virtual void initMiscData ( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates );

    void initSpecificMisc (const MiscData *p_miscData);

    void releaseWaitPax (float p_percent, const CMobileElemConstraint& p_type, ElapsedTime p_time ,int _iStage);
		
	int GetWaitAreaCount () const { return waitAreaCount; 	};
	GroupIndex* GetWaitAreaProcGroupIdx( ) { return gateWaitArea ;};
	
	// set & get fCanBeACStandGate flag
	void setACStandGateFlag( BOOL _bFlag ) { fCanBeACStandGate = _bFlag; };            
	BOOL getACStandGateFlag( void ) const { return fCanBeACStandGate; };

	// virtual function
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual Processor* CreateNewProc();
	void setGateType(GateType emGateType) { m_emGateType = emGateType;}
	GateType getGateType() { return m_emGateType;}

	virtual void beginService (Person *aPerson, ElapsedTime curTime);

	const ElapsedTime& GetLastServicePaxTime();
	const ElapsedTime& GetLastPaxOutConsTime();

	void SetLastPaxOutConsTime(const ElapsedTime& eTime);

	//init bridge connector
	void appendLinkedBridgeProcs(const MiscProcessorIDList* _pLinkedBridgeIdList);

	BridgeConnector* getBridgeConnector(int nFlightIndex);

	virtual bool CopyOtherData(Processor* _pDestProc);
};

#endif
