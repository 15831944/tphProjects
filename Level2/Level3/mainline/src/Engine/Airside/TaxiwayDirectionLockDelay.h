#pragma once

#include "./AirsideFlightDelay.h"

class TaxiwaySegDirectionLock;
class ElapsedTime;
class AirsideFlightInSim;

class TaxiwayDirectionLockDelay : public AirsideFlightDelay
{
public:	
	TaxiwayDirectionLockDelay();
	TaxiwayDirectionLockDelay( AirsideFlightInSim* pFlight, TaxiwaySegDirectionLock* pLock ,const ElapsedTime& time);
	//get lock informations
	void SetLockDirection( TaxiwaySegDirectionLock* pLock );

	virtual DelayType GetType()const{ return enumTaxiDirLockDelay; }
	virtual void WriteDelayReason(ArctermFile& outFile );
	virtual void ReadDelayReason(ArctermFile& inFile);	
	virtual bool IsIdentical(const AirsideFlightDelay* otherDelay)const ;

	
protected:
	int m_nNodeFrom;
	int m_nNodeTo;
	std::vector<int> m_vFlightIDs;

};


//////////////////////////////////////////////////////////////////////////

class TaxiLeadFlightDelay : public AirsideFlightDelay
{
public:
	TaxiLeadFlightDelay();
	TaxiLeadFlightDelay(AirsideFlightInSim* pleadflight, const ElapsedTime& tTime);
	virtual DelayType GetType()const{ return enumTaxiLeadFlightDelay; }
	virtual void WriteDelayReason(ArctermFile& outFile );
	virtual void ReadDelayReason(ArctermFile& inFile);
	virtual bool IsIdentical(const AirsideFlightDelay* otherDelay)const ;

	void SetLeadFlight(AirsideFlightInSim* pFlight);

protected:
	int m_nLeadFlightId;
};


//////////////////////////////////////////////////////////////////////////
class IntersectionNodeInSim;
class TaxiIntersectNodeDelay : public AirsideFlightDelay
{
public:
	TaxiIntersectNodeDelay();
	TaxiIntersectNodeDelay(IntersectionNodeInSim* pNode, AirsideFlightInSim* pInNodeFlight, const ElapsedTime& tTime);

	virtual DelayType GetType()const{ return enumTaxiIntersectNodeDelay; }
	virtual void WriteDelayReason(ArctermFile& outFile );
	virtual void ReadDelayReason(ArctermFile& inFile);	
	virtual bool IsIdentical(const AirsideFlightDelay* otherDelay)const ;

	void SetIntersectionNode(IntersectionNodeInSim* pNode);
	void SetInNodeFlight(AirsideFlightInSim* pFlight);

protected:
	int m_nConflictNodeID;
	int m_nInNodeFlightID;
};