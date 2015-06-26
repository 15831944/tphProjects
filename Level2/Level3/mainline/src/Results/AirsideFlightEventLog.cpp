#include "StdAfx.h"
#include ".\airsideflighteventlog.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <common\Types.hpp>

#include <Engine/Airside/AirsideFlightInSim.h>


//////////////////////////////////////////////////////////////////////////

DynamicClassImpl(AirsideFlightEventLog)
DynamicClassImpl(AirsideConflictionDelayLog)
DynamicClassImpl(AirsideRunwayCrossigsLog)
DynamicClassImpl(AirsideFlightStandOperationLog)
DynamicClassImpl(AirsideTakeoffProcessLog)

AirsideFlightEventLog::AirsideFlightEventLog(void)
{
	x=y=z=0;
	mMode= OnBirth;
	state = NONE;
	distInRes = 0.0;
	dSpd = 0.0;
	m_ExitId = -1 ;
}

AirsideFlightEventLog::~AirsideFlightEventLog(void)
{

}


ResourceDesc::ResourceDesc()
{
	fromnode = tonode = -1;
	resid = -1;
	resdir = POSITIVE;
	resType =AirsideResource::ResourceType( -1);
}

AirsideConflictionDelayLog::AirsideConflictionDelayLog()
{
	mAction = FlightConflict::NOACTION;
	mConflictLocation = FlightConflict::UNKNOWNLOC;
//	m_DelayReason = SlowerTrafficAhead;
	m_emFlightDelayReason = FltDelayReason_Unknown;
	mMode = OnBirth;
	distInRes = 0.0;
	mAreaID = -1;
	sAreaName = "All";
}

bool AirsideConflictionDelayLog::bTheSameDelay( const ARCMobileDesc& conflictMobilDesc,const FlightConflict::ActionType& action , const FlightConflict::LocationType& conflictLocation) const
{
	if(motherMobileDesc == conflictMobilDesc && action == mAction && conflictLocation == mConflictLocation)
		return true;
	return false;
}

CString AirsideConflictionDelayLog::GetDelayReasonByCode( )
{
	CString val ;
	val.Format(_T("Slower traffic ahead")) ;
	return val;
}
ARCMobileDesc::ARCMobileDesc()
{
	mType = FLIGHT;
	nMobileID = -1;
}

bool ARCMobileDesc::operator==( const ARCMobileDesc& other ) const
{
	return mType == other.mType && nMobileID==other.nMobileID;
}

AirsideRunwayCrossigsLog::AirsideRunwayCrossigsLog()
{

}

AirsideRunwayCrossigsLog::~AirsideRunwayCrossigsLog()
{

}

#include <InputAirside/Stand.h>

AirsideFlightStandOperationLog::AirsideFlightStandOperationLog()
: m_nStandID(-1)
, m_eParkingOpType(ARR_PARKING)
, m_eDelayReason(NoDelay)
{
	time = 0;
}

#ifdef _DEBUG // preserve for test, place a prob in AirsideFlightStandOperationLog::AirsideFlightStandOperationLog

bool gs_bToFetch = true;
CStringList gs_strStandList;
CStringList gs_strFlightList;

static void InitStandFlight()
{
	if (!gs_bToFetch)
		return;

	gs_strStandList.RemoveAll();
	gs_strFlightList.RemoveAll();

	CStdioFile f;
	CString strItem;
	if (f.Open(_T("C:\\StandDelay\\stand_prob.txt"), CFile::modeRead))
	{
		while (f.ReadString(strItem))
		{
			gs_strStandList.AddTail(strItem);
		}
		f.Close();
	}
	if (f.Open(_T("C:\\StandDelay\\flight_prob.txt"), CFile::modeRead))
	{
		while (f.ReadString(strItem))
		{
			gs_strFlightList.AddTail(strItem);
		}
	}
	gs_bToFetch = false;
}

static bool CheckStand(const ALTObjectID& altStandID)
{
	return NULL != gs_strStandList.Find(altStandID.GetIDString());
}
static bool CheckFlight(AirsideFlightInSim* pFlight)
{
	if (pFlight)
	{
		char cTmp[1024];
		char* p = cTmp;
		pFlight->GetFlightInput()->getAirline(p);
		p += strlen(p);
		BOOL bArr = pFlight->GetFlightInput()->isArriving();
		if (bArr)
		{
			pFlight->GetFlightInput()->getArrID(p);
			p += strlen(p);
		}
		if (pFlight->GetFlightInput()->isDeparting())
		{
			if (bArr)
			{
				*p++ = '/';
			}
			pFlight->GetFlightInput()->getDepID(p);
		}

		return NULL != gs_strFlightList.Find(cTmp);
	}
	return false;
}

#endif

AirsideFlightStandOperationLog::AirsideFlightStandOperationLog(long lTime, AirsideFlightInSim* pFlight,
															   int nStandID, ALTObjectID altStandID, StandOperationType eOpType)
	: m_nStandID(nStandID)
	, m_altStandID(altStandID)
	, m_eOpType(eOpType)
	, m_eDelayReason(NoDelay)
	, m_eParkingOpType(ARR_PARKING)
{
	time = lTime;

#ifdef _DEBUG // debug prob & assertion
	InitStandFlight();
	ASSERT(!CheckStand(altStandID));
	ASSERT(!CheckFlight(pFlight));
#endif
}

AirsideFlightStandOperationLog::~AirsideFlightStandOperationLog(void)
{

}

void AirsideFlightStandOperationLog::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	ar & boost::serialization::base_object<ARCEventLog>(*this);
	ar & m_nStandID;
	std::string strAltStand;
	ar & strAltStand;
	m_altStandID.FromString(strAltStand.c_str());
	ar & m_eOpType;
	ar & m_eDelayReason;
	ar & m_eParkingOpType;
}

void AirsideFlightStandOperationLog::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ar & boost::serialization::base_object<ARCEventLog>(*this);
	ar & m_nStandID;
	std::string strAltStand = m_altStandID.GetIDString();
	ar & strAltStand;
	ar & m_eOpType;
	ar & m_eDelayReason;
	ar & m_eParkingOpType;
}

AirsideTakeoffProcessLog::AirsideTakeoffProcessLog()
:m_state(-1)
,m_lCount(0)
,m_lTotalCount(0)
,m_dDist(0.0)
,m_speed(0.0)
,x(0.0)
,y(0.0)
,z(0.0)
{

}

AirsideTakeoffProcessLog::~AirsideTakeoffProcessLog()
{

}