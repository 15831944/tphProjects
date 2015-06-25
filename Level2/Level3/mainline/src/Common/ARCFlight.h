#ifndef ARCFLIGHT_H
#define ARCFLIGHT_H

#pragma once

#include "elaptime.h"
#include "ALTObjectID.h"
#include "IDGather.h"
#include "Flt_cnst.h"

class FlightConstraint;
class CArrFlightOperation;
class CDepFlightOperation;
enum enumFlightType
{
	BaseFlight = 0,
	ItinerantFlightType,
	TerminalFlight,
};
class  COMMON_TRANSFER ARCFlight
{
public:
	ARCFlight();
	ARCFlight(const ARCFlight& _flight);
	virtual ~ARCFlight();

	const ARCFlight& operator = (const ARCFlight& aFlight);
	bool operator == (const ARCFlight& aFlight) const;

	bool IsEqual(const ARCFlight* _rhs)const;

public:
	void getFlightIDString( char *p_str ) const;
	void setACType(const char *p_str) ;
	void setAirline(const char *p_str) ;
	void setAirport(const char *p_str, char p_mode = 0);
	void setFlightID(const char *p_str, char p_mode = 0);

	void setArrID(const char *p_str) ;
	void setDepID(const char *p_str) ;	

	void setOrigin(const char *p_str);
	void setDestination(const char *p_str);

	void getAirline (char *p_str) const;
	void getFlightID (char *p_str, char p_mode = 0) const;
	void getFullID (char *p_str, char p_mode = 0) const;
	void getArrID (char *p_str) const;
	void getDepID (char *p_str) const;
	void getOrigin(char *p_str) const;

	void getDestination(char *p_str) const;
	void getACType (char *p_str) const;

	const CFlightID getDepID (void) const;
	const CFlightID getArrID (void) const;

	void setArrParkingTime(ElapsedTime p_time);
	void setDepParkingTime(ElapsedTime p_time);
	void setIntParkingTime(ElapsedTime p_time);

	ElapsedTime getArrParkingTime() const;
	ElapsedTime getDepParkingTime() const;
	ElapsedTime getIntParkingTime() const;

	ElapsedTime getEnplaneTime() const;
	ElapsedTime getDeplaneTime() const;
	ElapsedTime getMaxParkingTime();

	void setStandID(int p_ndx);
	void setDepStandID(int p_ndx);
	void setArrStandID(int p_ndx);

	int getStandID(void)const;
	int getArrStandID()const;
	int getDepStandID()const;

	void setStand(const ALTObjectID& p_id);
	void setArrStand(const ALTObjectID& p_id);
	void setDepStand(const ALTObjectID& p_id);
	void setIntermediateStand(const ALTObjectID& p_id);

	const ALTObjectID getStand(void)const ;
	const ALTObjectID getArrStand()const;
	const ALTObjectID getDepStand()const;
	const ALTObjectID getIntermediateStand() const;

	BOOL isArriving (void) const ;
	BOOL isDeparting (void) const ;
	BOOL isTurnaround (void) const ;
	int isFlightMode (char p_mode) const ;

	void setDelay(ElapsedTime p_time);
	void setArrDelay(ElapsedTime p_time);
	void setDepDelay(ElapsedTime p_time);

	void setGateTime(ElapsedTime p_time, char p_mode = 0);
	void setArrTime(ElapsedTime p_time) ;
	void setDepTime(ElapsedTime p_time) ;
	void setServiceTime(ElapsedTime p_time);

	ElapsedTime getArrDelay(void) const;
	ElapsedTime getDepDelay(void) const;

	ElapsedTime getGateTime (void) const;
	ElapsedTime getArrTime (void) const;
	ElapsedTime getDepTime (void) const;
	ElapsedTime getServiceTime (void) const { return serviceTime; }

	const CAirportCode getOrigin(void) const;
	const CAirportCode getDestination(void) const;

	virtual bool isSameFlightID();
	char getFlightMode(void) const ;

	virtual FlightConstraint getType (char p_mode) const;
	virtual FlightDescStruct getLogEntry (void) ;
	virtual FlightDescStruct getOpLogEntry (char mode);
	virtual enumFlightType getFlightType() { return BaseFlight;}

	void SetTowoffTime(long tTime);
	long GetTowoffTime()const;

	void SetExIntStandTime(long tTime);
	long GetExIntStandTime()const;

	void setArrBagCount(int nCount);
	int getArrBagCount() const;


	void setDepBagCount(int nCount);
	int getDepBagCount() const;

	void ResetBaggageCount();
protected:
	CAircraftType	m_AcType;
	CAirlineID m_Airline;

	CArrFlightOperation* m_pArrFlightOp;
	CDepFlightOperation* m_pDepFlightOp;

	ElapsedTime serviceTime;

	ElapsedTime delay;
	ElapsedTime m_arrDelay;		
	ElapsedTime m_depDelay;

	ElapsedTime m_tArrParking;
	ElapsedTime m_tDepParking;
	ElapsedTime m_tIntParking;

	long m_towoffTime;//Tow off -1 do not have 
	long m_exIntStandTime;//Ex Int Stand -1 do not have 

	ALTObjectID m_IntermediateStand;

	int m_nArrBagCount;
	int m_nDepBagCount;
};

#endif
