#pragma once

#include "CommonData\Procid.h"
#include "Common\ALTObjectID.h"
#include "Common\containr.h"

class TerminalProcLinkageInSim;
class CFacilityBehavior;
class CFacilityBehaviorList;

class ProjectCommon;
class LandsideFacilityLayoutObjectList;
class LandsideResourceManager;
//this class remembers the linkage between terminal and landisde
//the propose is to easier to get landside object using terminal processor
//Facility is use landisde object as key
//this class uses Terminal Processor as key
//NOTE: the linkage information contains the Lead To 



class LandsideObjectLinkageItemInSim
{
public:
	LandsideObjectLinkageItemInSim();
	LandsideObjectLinkageItemInSim(const ALTObjectID& objID);
	LandsideObjectLinkageItemInSim(const ALTObjectID& objID,const std::vector<int>& vLevels);

	~LandsideObjectLinkageItemInSim(){}

public:
	ALTObjectID& getObject() {return m_altObj;}
	std::vector<int>& getLevels() {return m_vLevels;}

	const ALTObjectID& getObject() const{return m_altObj;}
	const std::vector<int>& getLevels() const {return m_vLevels;}

	bool operator == (const LandsideObjectLinkageItemInSim& pID) const;

	bool CanLeadTo(const ALTObjectID& altDestination) const;
	bool CanLeadTo(const ALTObjectID& altDestination, int nLevel) const;
	bool CanLeadTo(const LandsideObjectLinkageItemInSim& destObject) const;

	CString getOrigin() const;
	void setOrigin(const CString& strOrigin);
protected:
	ALTObjectID m_altObj;
	//if parking lot, there will be may floors
	std::vector<int> m_vLevels;
	//this string shows the linkage where it came from
	//the information is for debugging
	//if there has "lead to problem"
	CString m_strOrigin;

};

class LandsideTerminalLinkageInSimManager
{
public:
	LandsideTerminalLinkageInSimManager(void);
	~LandsideTerminalLinkageInSimManager(void);

public:
	void Initialize(CFacilityBehaviorList *pFacilityBehaviors, ProjectCommon* pCommon, const LandsideFacilityLayoutObjectList& layoutObjectList, LandsideResourceManager* resManagerInst);
	void SetProcessorList(ProcessorList* pProcList);
public:
	int GetCount() const;
	TerminalProcLinkageInSim* GetItem(int nIndex);

	
	//it will return the best fit result
	TerminalProcLinkageInSim *FindItem(const ProcessorID& procID);

	bool GetRandomObject(const std::vector<LandsideObjectLinkageItemInSim>& vAllObject,LandsideObjectLinkageItemInSim& linkObject);

protected:
	//find the processor, if not exists, create a new one
	void AddItem(const ProcessorID& procID,const ALTObjectID& objID );
	void AddItem(const ProcessorID& procID,const ALTObjectID& objID, const std::vector<int>& vLevels );

	
protected:
	SortedContainer<TerminalProcLinkageInSim > m_vLinkage;
	ProcessorList* m_pProcList;

	//passenger type with land side object

private:
	void ProcessFacility(CFacilityBehavior *pFacility, ProjectCommon* pCommon,const LandsideFacilityLayoutObjectList& layoutObjectList, LandsideResourceManager* resManagerInst);
};


class TerminalProcLinkageInSim
{
public:
	TerminalProcLinkageInSim(const ProcessorID& procID);
	~TerminalProcLinkageInSim();

public:
	ProcessorID *getProcID();

	void AddObject(const ALTObjectID& objID);
	void AddObject(const ALTObjectID& objID, const std::vector<int>& vLevels);

	bool CanLeadToAltobject(const ALTObjectID& objID)const;


	const std::vector<LandsideObjectLinkageItemInSim>&  GetLandsideObject() const;


	bool operator <(const TerminalProcLinkageInSim& pItem );
protected:
	ProcessorID m_procID;
	std::vector<LandsideObjectLinkageItemInSim> m_vLandsideObject;
	
};

