#pragma once
#include "..\src\inputs\processordistributionwithpipe.h"
#include "../Inputs/paxflow.h"

//all the class are used in engine, so has no file operation
//the function: untie sub flow and convert data from sub flow to flow


//used in engine
//only used in CProcessorDistributionTransformer, please take care before use
class CFlowItemTransformer
{
public:
	CFlowItemTransformer();
	~CFlowItemTransformer();
protected:

	CFlowItemEx m_flowItemEx;

	//processor id of the flow item
	ProcessorID m_procID;
	//percentage of flow item in the flow
	int m_nPercentage;

public:
	void SetProcID(const ProcessorID& procID);
	void SetProbality(int nProb);

	ProcessorID GetProcID() const;
	int GetProbality() const;

	const CFlowItemEx& GetFlowItem() const {return m_flowItemEx;}
	void SetFlowItemEx(const CFlowItemEx& flowItemEx){m_flowItemEx = flowItemEx;}


};

class PassengerFlowDatabase;
class CSubFlowList;
class CSubFlow;
class CProcessorDestinationList;
class CFlowDestination;
//convert, all the function in the class are only available in the conversion of Flow to Processor transtion
//used in engine
class CProcessorDistributionTransformer : public CProcessorDistributionWithPipe
{
public:

	CProcessorDistributionTransformer(const CProcessorDistributionWithPipe& _otherDistribution);
	CProcessorDistributionTransformer(CProcessorDestinationList* pSubFlowDesinationList);
	CProcessorDistributionTransformer();
	~CProcessorDistributionTransformer();

	std::vector<CFlowItemTransformer> m_vFlowItem;



public:
	//if the destination processor has sub flow, untie the sub flow
	void UntieSubFlows(PassengerFlowDatabase* database,const CSubFlowList& pSubFlowList,const CMobileElemConstraint* pConstraint);

	void InitializeDestinations();

	void MergeFlowItem(std::vector<CFlowItemTransformer>& vPassFlowItem);
protected:
	void UntieThisSubFlow(const CFlowItemTransformer& flowItem,CSubFlow *pSubFlow,PassengerFlowDatabase* database,const CMobileElemConstraint* pConstraint);
	void GetFlowItemEx(const CFlowDestination& flowDest,CFlowItemEx& flowItem);
};

//used in engine
class CPassengerFlowDatabaseTransformer : public PassengerFlowDatabase
{
public:
	CPassengerFlowDatabaseTransformer();
	~CPassengerFlowDatabaseTransformer();

	//the following function are not unavailable, this class has no file operation, has nothing to do with INPUT
	//it is used in engine, to expand the passenger flow, untie sub flow
private:
	virtual void loadDataSet (const CString& _pProjPath);
	// Exception:	DirCreateError;
	virtual void saveDataSet (const CString& _pProjPath, bool _bUndo) const;
	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData ( ArctermFile& p_file );
	virtual void writeData (ArctermFile& p_file) const;
	virtual const char *getTitle (void) const;
	virtual const char *getHeaders (void) const;

	//////////////////////////////////////////////////////////////////////////

class pairMobConsSubFlowExpand
{
public:
	CMobileElemConstraint m_mobConstraint;
	std::vector<CString> m_vSubFlow;

protected:
private:
};

public:
	//remember the process has been expanded, the key is mobile element constraint
	std::vector<pairMobConsSubFlowExpand > m_mapConstraintProcess;

	bool HasBeenExpand(const CMobileElemConstraint&,const CString& strProcessID);
public:
	//expand the pax flow
	void ExpandPaxFlow(const PassengerFlowDatabase *pPaxFlowOriginal,const CSubFlowList& pSubFlowList );
	//initialize the pax flow, 
	void Initialize();

public:
	//add constraint entry into processor entry database
	void AddConstraintEntryToProcDatabase(CMobileElemConstraintDatabase* pMobElemDatabase, ConstraintEntry* pNewConstraintEntry);
};
































