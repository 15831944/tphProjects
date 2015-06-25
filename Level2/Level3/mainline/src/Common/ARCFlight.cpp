#include "StdAfx.h"
#include "ARCFlight.h"
#include "exeption.h"
#include "ArrFlightOperation.h"
#include "DepFlightOperation.h"
#include "InputAirside/Stand.h"
/////////////////////////////////////////////////////////////
//////ARCFlight
ARCFlight::ARCFlight( )
:m_arrDelay(0l)
,m_depDelay(0l)
,serviceTime(0l)
,delay(0l)
{
	m_tArrParking = m_tDepParking = 0L;
	m_tIntParking = 0L;

	m_towoffTime = -1;
	m_exIntStandTime = -1;

	m_pArrFlightOp = new CArrFlightOperation;
	m_pDepFlightOp = new CDepFlightOperation;

	m_nArrBagCount = 0;
	m_nDepBagCount = 0;
}

ARCFlight::~ARCFlight()
{
	if (m_pArrFlightOp)
	{
		delete m_pArrFlightOp;
		m_pArrFlightOp = NULL;
	}
	if (m_pDepFlightOp)
	{
		delete m_pDepFlightOp;
		m_pDepFlightOp = NULL;
	}
}

ARCFlight::ARCFlight(const ARCFlight& aFlight)
{
	serviceTime = aFlight.serviceTime;
	delay = aFlight.delay;

	m_AcType = aFlight.m_AcType;
	m_Airline = aFlight.m_Airline;

	m_arrDelay = aFlight.m_arrDelay;
	m_depDelay = aFlight.m_depDelay;

	m_tArrParking = aFlight.m_tArrParking;
	m_tDepParking = aFlight.m_tDepParking;
	m_tIntParking = aFlight.m_tIntParking;

	*m_pArrFlightOp = *(aFlight.m_pArrFlightOp);
	*m_pDepFlightOp = *(aFlight.m_pDepFlightOp);

	m_nArrBagCount = aFlight.m_nArrBagCount;
	m_nDepBagCount = aFlight.m_nDepBagCount;

	m_towoffTime = aFlight.m_towoffTime;//Tow off
	m_exIntStandTime = aFlight.m_exIntStandTime;//Ex Int Stand
 	
	m_IntermediateStand = aFlight.m_IntermediateStand;

}

const ARCFlight& ARCFlight::operator = (const ARCFlight& aFlight)
{
	serviceTime = aFlight.serviceTime;
	delay = aFlight.delay;

	m_AcType = aFlight.m_AcType;
	m_Airline = aFlight.m_Airline;

	m_arrDelay = aFlight.m_arrDelay;
	m_depDelay = aFlight.m_depDelay;

	m_tArrParking = aFlight.m_tArrParking;
	m_tDepParking = aFlight.m_tDepParking;
	m_tIntParking = aFlight.m_tIntParking;

	*m_pArrFlightOp = *(aFlight.m_pArrFlightOp);
	*m_pDepFlightOp = *(aFlight.m_pDepFlightOp);

	m_nArrBagCount = aFlight.m_nArrBagCount;
	m_nDepBagCount = aFlight.m_nDepBagCount;

 	m_IntermediateStand = aFlight.m_IntermediateStand;

	return *this;
}
bool ARCFlight::operator == (const ARCFlight& aFlight) const
{
	return IsEqual(&aFlight);
}   

void ARCFlight::getAirline(char *p_str)const
{
	strcpy (p_str, m_Airline); 
}

void ARCFlight::getFlightID(char *p_str, char p_mode /* = 0 */)const
{
	CFlightID arrID = m_pArrFlightOp->GetArrFlightID();
	CFlightID depID = m_pDepFlightOp->GetDepFlightID();

	if (p_mode == 'D')
		strcpy(p_str, depID);
	else if (p_mode == 'A')
		strcpy(p_str, arrID);
	else if (isDeparting())
		strcpy(p_str, depID);
	else
		strcpy(p_str, arrID);
}

void ARCFlight::getFullID(char *p_str, char p_mode /* = 0 */)const
{
	getAirline (p_str);
	getFlightID(p_str + strlen(p_str),p_mode);
}

void ARCFlight::getArrID(char *p_str)const
{
	strcpy(p_str,getArrID()); 
}

void ARCFlight::getDepID(char *p_str)const	
{
	strcpy(p_str, getDepID()); 
}

void ARCFlight::getOrigin(char *p_str)const
{
	strcpy (p_str, getOrigin()); 
}

void ARCFlight::getDestination(char *p_str)const
{
	strcpy (p_str, getDestination()); 
}

const CAirportCode ARCFlight::getOrigin(void)const
{
	return m_pArrFlightOp->GetOriginAirport();
}

const CAirportCode ARCFlight::getDestination(void)const
{
	return m_pDepFlightOp->GetDestinationAirport();
}
void ARCFlight::getACType(char *p_str)const
{	
	strcpy (p_str, m_AcType);
}

const CFlightID ARCFlight::getArrID(void)const
{
	return m_pArrFlightOp->GetArrFlightID();
}

const CFlightID ARCFlight::getDepID(void)const
{
	return m_pDepFlightOp->GetDepFlightID();
}

void ARCFlight::setStandID(int p_ndx)
{
	if (!isArriving())
	{
		m_pDepFlightOp->SetDepStandIdx((short)p_ndx);
	}

	m_pArrFlightOp->SetArrStandIdx((short)p_ndx); 
}

void ARCFlight::setDepStandID(int p_ndx)
{
	m_pDepFlightOp->SetDepStandIdx((short)p_ndx);
}

void ARCFlight::setArrStandID(int p_ndx)
{
	m_pArrFlightOp->SetArrStandIdx((short)p_ndx); 
}

void ARCFlight::setArrParkingTime(ElapsedTime p_time)
{
	m_tArrParking = p_time;
}

void ARCFlight::setDepParkingTime(ElapsedTime p_time)
{
	m_tDepParking = p_time;
}

void ARCFlight::setIntParkingTime(ElapsedTime p_time)
{
	m_tIntParking = p_time;
}

ElapsedTime ARCFlight::getEnplaneTime()const
{
	return ElapsedTime(1800L);
}

ElapsedTime ARCFlight::getDeplaneTime()const
{
	return ElapsedTime(1800L);
}

ElapsedTime ARCFlight::getMaxParkingTime()
{
	return ElapsedTime(180*60L);
}	

const ALTObjectID ARCFlight::getStand(void)const
{
	if (!isArriving())
	{
		return getDepStand();
	}

	return getArrStand();
}

const ALTObjectID ARCFlight::getArrStand()const
{
	return m_pArrFlightOp->GetArrStand();
}

const ALTObjectID ARCFlight::getDepStand()const
{
	return m_pDepFlightOp->GetDepStand();
}

const ALTObjectID ARCFlight::getIntermediateStand()const
{
	return m_IntermediateStand;
}


BOOL ARCFlight::isArriving(void)const
{
	return getArrID().HasValue();
}

BOOL ARCFlight::isDeparting(void)const
{
	return getDepID().HasValue(); 
}

BOOL ARCFlight::isTurnaround(void)const
{
	return (getArrID().HasValue() && getDepID().HasValue()); 
}

int ARCFlight::isFlightMode(char p_mode)const
{
	return (p_mode == 'A' && getArrID().HasValue()) 
		|| (p_mode == 'D' && getDepID().HasValue())
		|| (p_mode == 'T' && getArrID().HasValue() && getDepID().HasValue()); 
}

void ARCFlight::setDelay(ElapsedTime p_time)
{
	delay = p_time;
}

void ARCFlight::setArrDelay(ElapsedTime p_time)
{
	m_arrDelay = p_time;
}

void ARCFlight::setDepDelay(ElapsedTime p_time)
{
	m_depDelay = p_time;
}

void ARCFlight::setArrTime(ElapsedTime p_time)
{
	m_pArrFlightOp->SetArrTime((long)p_time);
}

void ARCFlight::setDepTime(ElapsedTime p_time)
{
	m_pDepFlightOp->SetDepTime((long)p_time);
}

void ARCFlight::setServiceTime(ElapsedTime p_time)
{
	serviceTime = p_time;
}

void ARCFlight::getFlightIDString(char *p_str )const
{
	char szAirline[8];
	getAirline (szAirline);

	strcpy( p_str, szAirline );
	if( isTurnaround() )
	{
		strcat(p_str, getArrID());
		strcat(p_str," / ");
		strcat( p_str, szAirline );
		strcat(p_str, getDepID());
	}
	else if( isArriving() )
		strcat(p_str, getArrID());
	else
		strcat(p_str, getDepID());
}

FlightConstraint ARCFlight::getType(char p_mode)const
{
	if (! isFlightMode(p_mode) || p_mode == 'T')
		throw new StringError ("Invalid mode in call to Flight::getType");

	FlightConstraint type;

	type.setAirline (m_Airline);
	type.setFlightID ((p_mode == 'D')? getDepID(): getArrID());
	type.SetFltConstraintMode( (p_mode=='A') ? ENUM_FLTCNSTR_MODE_ARR : ENUM_FLTCNSTR_MODE_DEP );
	type.setAirport ((p_mode == 'D')?getDestination(): getOrigin());
	type.setACType (m_AcType);
	return type;
}

bool ARCFlight::isSameFlightID()
{
	return (getArrID() == getDepID())?true:false;
}

char ARCFlight::getFlightMode()const
{
	return (isTurnaround())? 'T':(isArriving()? 'A': 'D'); 
}

ElapsedTime ARCFlight::getArrTime()const
{
	ElapsedTime aTime;
	if (!isArriving())
	{
		aTime = 0L;
	}
	else
		aTime.setPrecisely (m_pArrFlightOp->GetArrTime());
	return aTime;
}

ElapsedTime ARCFlight::getDepTime()const
{
	ElapsedTime aTime;
	if (!isDeparting())
	{
		aTime = 0L;
	}
	else
		aTime.setPrecisely (m_pDepFlightOp->GetDepTime());
	return aTime;
}

bool ARCFlight::IsEqual(const ARCFlight* _rhs)const
{
	char szThisAirline[256], szRhsAirline[256];
	this->getAirline(szThisAirline);
	_rhs->getAirline(szRhsAirline);

	if(strcmp(szThisAirline, szRhsAirline) != 0)
		return false;

	char szThisACType[256], szRhsACType[256];
	this->getACType(szThisACType);
	_rhs->getACType(szRhsACType);

	if(strcmp(szThisACType, szRhsACType) != 0)
		return false;

	char szThisIDString[256], szRhsIDString[256];
	this->getFlightIDString(szThisIDString);
	_rhs->getFlightIDString(szRhsIDString);

	if( strcmp(szThisIDString, szRhsIDString) != 0 )
		return false;

	return true;
}

void ARCFlight::setStand(const ALTObjectID& p_id)
{
	if (isArriving())
	{
		m_pArrFlightOp->SetArrStand(p_id);
	}
	if (isDeparting())
	{
		m_pDepFlightOp->SetDepStand(p_id);
	}
}

void ARCFlight::setArrStand(const ALTObjectID& p_id)
{
	m_pArrFlightOp->SetArrStand(p_id);
}

void ARCFlight::setDepStand(const ALTObjectID& p_id)
{
	m_pDepFlightOp->SetDepStand(p_id);
}

void ARCFlight::setIntermediateStand(const ALTObjectID& p_id)
{
	m_IntermediateStand = p_id;
}

ElapsedTime ARCFlight::getArrDelay(void) const
{
	return m_arrDelay;
}

ElapsedTime ARCFlight::getDepDelay(void) const
{
	return m_depDelay;
}

ElapsedTime ARCFlight::getGateTime (void) const
{
	if (isArriving())
		return getArrTime();
	else
		return getDepTime();
}

void ARCFlight::setGateTime (ElapsedTime p_time, char p_mode)
{
	if (p_mode == 'D')
		m_pDepFlightOp->SetDepTime((long)p_time);
	else if (p_mode == 'A')
		m_pArrFlightOp->SetArrTime((long)p_time);
	else if (isDeparting())
		m_pDepFlightOp->SetDepTime((long)p_time);
	else
		m_pArrFlightOp->SetArrTime((long)p_time);
}

void ARCFlight::setFlightID (const char *p_str, char p_mode)
{
	char* szBuffer = new char[AIRLINE_LEN] - 1;
	int ndx=0;
	for ( ndx = 0; isalpha (p_str[ndx]) && ndx < AIRLINE_LEN-1; ndx++)
		szBuffer[ndx] = p_str[ndx];

	if (ndx)
	{
		szBuffer[ndx] = '\0';
		m_Airline = szBuffer;
	}

	if (p_mode == 'D')
		m_pDepFlightOp->SetDepFlightID(p_str + ndx);
	else if (p_mode == 'A')
		m_pArrFlightOp->SetArrFlightID(p_str + ndx);
	else if (isDeparting())
		m_pDepFlightOp->SetDepFlightID(p_str + ndx);
	else
		m_pArrFlightOp->SetArrFlightID(p_str + ndx);

}

int ARCFlight::getStandID(void)const 
{ 
	if (!isArriving())
	{
		return m_pDepFlightOp->GetDepStandIdx();
	}

	return m_pArrFlightOp->GetArrStandIdx(); 
}

void ARCFlight::setAirline (const char *p_str) 
{ 
	m_Airline = p_str; 
}

int ARCFlight::getDepStandID()const
{
	return m_pDepFlightOp->GetDepStandIdx();
}

void ARCFlight::setACType (const char *p_str) 
{ 
	m_AcType = p_str;
}

void ARCFlight::SetTowoffTime( long tTime )
{
	m_towoffTime = tTime;
}

long ARCFlight::GetTowoffTime() const
{
	return m_towoffTime;
}

void ARCFlight::SetExIntStandTime( long tTime )
{
	m_exIntStandTime = tTime;
}

long ARCFlight::GetExIntStandTime() const
{
	return m_exIntStandTime;
}
void ARCFlight::setDepID (const char *p_str) 
{ 
	CFlightID depID;
	depID = p_str;
	m_pDepFlightOp->SetDepFlightID(depID);
}

void ARCFlight::setDestination (const char *p_str) 
{ 
	CAirportCode dest;
	dest = p_str;
	m_pDepFlightOp->SetDestinationAirport(dest);
}

void ARCFlight::setOrigin (const char *p_str)
{ 
	CAirportCode origin;
	origin = p_str;
	m_pArrFlightOp->SetOriginAirport(origin);
}

void ARCFlight::setArrID (const char *p_str) 
{ 
	CFlightID arrID;
	arrID =p_str;
	m_pArrFlightOp->SetArrFlightID(arrID) ;
}

int ARCFlight::getArrStandID()const
{
	return m_pArrFlightOp->GetArrStandIdx(); 
}

void ARCFlight::setAirport (const char *p_str, char p_mode)
{
	CAirportCode origin;
	CAirportCode dest;
	if (p_mode == 'D')
		m_pDepFlightOp->SetDestinationAirport(p_str);
	else if (p_mode == 'A')
		m_pArrFlightOp->SetOriginAirport(p_str);
	else if (isDeparting())
		m_pDepFlightOp->SetDestinationAirport(p_str);
	else
		m_pArrFlightOp->SetOriginAirport(p_str);
}


FlightDescStruct ARCFlight::getLogEntry (void)
{
	FlightDescStruct logEntry;

	logEntry.acType = m_AcType ;
	logEntry.airline = m_Airline;

	logEntry.arrDelay = m_arrDelay;
	logEntry.depDelay = m_depDelay;

	if (isArriving())
	{
		logEntry.arrID = m_pArrFlightOp->GetArrFlightID();
		logEntry.arrTime = m_pArrFlightOp->GetArrTime();
		logEntry.origin = m_pArrFlightOp->GetOriginAirport();
		logEntry.gate = m_pArrFlightOp->GetArrStandIdx();
	}
	if (isDeparting())
	{
		logEntry.depID = m_pDepFlightOp->GetDepFlightID();
		logEntry.depTime = m_pDepFlightOp->GetDepTime();
		logEntry.destination = m_pDepFlightOp->GetDestinationAirport();
		logEntry.gate = m_pDepFlightOp->GetDepStandIdx();
	}
	return logEntry; 
}

ElapsedTime ARCFlight::getArrParkingTime() const
{
	return m_tArrParking;
}

ElapsedTime ARCFlight::getDepParkingTime() const
{
	return m_tDepParking;
}

ElapsedTime ARCFlight::getIntParkingTime() const
{
	return m_tIntParking;
}

FlightDescStruct ARCFlight::getOpLogEntry( char mode )
{
	FlightDescStruct logEntry;

	logEntry.acType = m_AcType ;
	logEntry.airline = m_Airline;

	if (mode == 'A')
	{
		logEntry.arrID = m_pArrFlightOp->GetArrFlightID();
		logEntry.arrTime = m_pArrFlightOp->GetArrTime();
		logEntry.origin = m_pArrFlightOp->GetOriginAirport();
		logEntry.gate = m_pArrFlightOp->GetArrStandIdx();
		logEntry.arrDelay = m_arrDelay;
	}
	if (mode == 'D')
	{
		logEntry.depID = m_pDepFlightOp->GetDepFlightID();
		logEntry.depTime = m_pDepFlightOp->GetDepTime();
		logEntry.destination = m_pDepFlightOp->GetDestinationAirport();
		logEntry.gate = m_pDepFlightOp->GetDepStandIdx();
		logEntry.depDelay = m_depDelay;
	}
	return logEntry; 
}

void ARCFlight::setArrBagCount( int nCount )
{
	m_nArrBagCount = nCount;
}

int ARCFlight::getArrBagCount() const
{
	return m_nArrBagCount;
}

void ARCFlight::setDepBagCount( int nCount )
{
	m_nDepBagCount = nCount;
}

int ARCFlight::getDepBagCount() const
{
	return m_nDepBagCount;
}

void ARCFlight::ResetBaggageCount()
{
	m_nArrBagCount = 0;
	m_nDepBagCount = 0;
}
