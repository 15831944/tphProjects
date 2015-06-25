#include "StdAfx.h"
#include ".\airsideflighttype.h"
#include "../Common/AirportDatabase.h"

const CString strTO = _T(" --> ");
AirsideFlightType::AirsideFlightType(void)
:m_bArrToDep(false)
{
	m_fltConst1.initDefault();
	m_fltConst2.initDefault();
}

AirsideFlightType::~AirsideFlightType(void)
{
}

bool AirsideFlightType::IsFit( ARCFlight* pflight, char mode)
{
	if (!m_bArrToDep)
	{
		return m_fltConst1.fits(pflight->getOpLogEntry(mode)) >0? true:false;
	}
	else
	{
		if(mode == 'A')
		{
			return m_fltConst1.fits(pflight->getOpLogEntry(mode)) >0? true:false;
		}
		else if(mode == 'D')
		{
			return m_fltConst2.fits(pflight->getOpLogEntry('D'))>0? true:false;
		}

	}

	return false;
}

bool AirsideFlightType::IsFit( ARCFlight* pflight )
{
	if (!m_bArrToDep)
	{
		return m_fltConst1.fits(pflight->getLogEntry()) >0? true:false;
	}

	return m_fltConst1.fits(pflight->getOpLogEntry('A')) && m_fltConst2.fits(pflight->getOpLogEntry('D'));
}

bool AirsideFlightType::IsFit( const AirsideFlightType& flttype )
{
	if(!m_bArrToDep && !flttype.m_bArrToDep)		//both not arr --> dep
		return m_fltConst1.fits(flttype.m_fltConst1) >0? true:false;

	if (m_bArrToDep && flttype.m_bArrToDep)		//both arr --> dep
		return m_fltConst1.fits(flttype.m_fltConst1) && m_fltConst2.fits(m_fltConst2);	

	if (m_bArrToDep && !flttype.m_bArrToDep)		//arr-->dep and other is not arr --> dep
		return false;

	return true;
}

CString AirsideFlightType::getPrintString()
{
	CString strFltCnst1;
	m_fltConst1.screenPrint(strFltCnst1);

	if (!m_bArrToDep)
		return strFltCnst1;

	CString strFltCnst2;
	m_fltConst2.screenPrint(strFltCnst2);

	return strFltCnst1+strTO+strFltCnst2;
}

CString AirsideFlightType::getDBString()
{
	char sFltCnst1[1024];
	m_fltConst1.WriteSyntaxStringWithVersion(sFltCnst1);

	if (!m_bArrToDep)
		return sFltCnst1;

	char sFltCnst2[1024];
	m_fltConst2.WriteSyntaxStringWithVersion(sFltCnst2);

	CString strType;
	strType.Format("%s%s%s",sFltCnst1,strTO,sFltCnst2);

	return strType;
}

void AirsideFlightType::FormatDBStringToFlightType( const CString& strFlightType )
{
	int nIdx = strFlightType.Find(strTO);
	if (nIdx < 0)
	{
		m_bArrToDep = false;
		if (m_pAirportDB)
		{
			m_fltConst1.SetAirportDB(m_pAirportDB);
			m_fltConst1.setConstraintWithVersion(strFlightType);
		}
		return;
	}

	m_bArrToDep = true;

	CString strFltCnst1 = strFlightType.Left(nIdx);

	int nChar = strFlightType.GetLength() - strFltCnst1.GetLength() - strTO.GetLength();
	CString strFltCnst2 = strFlightType.Right(nChar);

	if (m_pAirportDB)
	{
		m_fltConst1.SetAirportDB(m_pAirportDB);
		m_fltConst1.setConstraintWithVersion(strFltCnst1);

		m_fltConst2.SetAirportDB(m_pAirportDB);
		m_fltConst2.setConstraintWithVersion(strFltCnst2);		
	}

}

bool AirsideFlightType::IsArrToDep()
{
	return m_bArrToDep;
}

void AirsideFlightType::SetArrToDep( bool bArrToDep )
{
	m_bArrToDep = bArrToDep;
}

void AirsideFlightType::SetFltConst1( const FlightConstraint& flt_cnst )
{
	m_fltConst1 = flt_cnst;
}

void AirsideFlightType::SetFltConst2( const FlightConstraint& flt_cnst )
{
	m_fltConst2 = flt_cnst;
}

const FlightConstraint& AirsideFlightType::GetFltConst1()
{
	return m_fltConst1;
}

const FlightConstraint& AirsideFlightType::GetFltConst2()
{
	return m_fltConst2;
}

void AirsideFlightType::SetAirportDB( CAirportDatabase* pAirportDB )
{
	m_pAirportDB = pAirportDB;
}

bool AirsideFlightType::IsDefaultFlightType()
{
	if (m_fltConst1.isDefault()&&m_fltConst2.isDefault())
	{
		return true;
	}
	return false;	
}
