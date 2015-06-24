// PaxType.h: interface for the CPaxType class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FLIGHTTYPE_H__
#define __FLIGHTTYPE_H__

#pragma once


#include "common\AIRSIDE_BIN.h"
#include "../common/flt_cnst.h"


// forward declarations
class Terminal;



class CFlightType
{
public:
	void SetIsFlightConstraintAdd(BOOL _bAdd);
	void SetIsTimeApplied(BOOL _bAdd);
	void SetIsBirthTimeApplied(BOOL _bAdd);
	void SetIsBirthTimeAdd(BOOL _bAdd);
	void SetIsDeathTimeApplied(BOOL _bAdd);
	void SetIsDeathTimeAdd(BOOL _bAdd);
	void SetMaxDeathTime(COleDateTime _dt);
	void SetMinDeathTime(COleDateTime _dt);
	void SetMaxBirthTime(COleDateTime _dt);
	void SetMinBirthTime(COleDateTime _dt);
	void SetEndTime(COleDateTime _dt);
	void SetStartTime(COleDateTime _dt);
	
	BOOL GetIsFlightConstraintAdd() const { return m_isFlightConstraintAdd; }
	BOOL GetIsTimeApplied() const { return m_isTimeApplied; }
	BOOL GetIsBirthTimeApplied( ) const { return m_isBirthTimeApplied; }
	BOOL GetIsBirthTimeAdd( ) const { return m_isBirthTimeAdd; }
	BOOL GetIsDeathTimeApplied() const { return m_isDeathTimeApplied; }
	BOOL GetIsDeathTimeAdd( ) const { return m_isDeathTimeAdd;	}
	COleDateTime GetMaxDeathTime() const { return m_oMaxDeathTime; }
	COleDateTime GetMinDeathTime() const { return m_oMinDeathTime; }
	COleDateTime GetMaxBirthTime() const { return m_oMaxBirthTime; }
	COleDateTime GetMinBirthTime() const { return m_oMinBirthTime; }
	COleDateTime GetEndTime() const { return m_oEndTime; }
	COleDateTime GetStartTime() const { return m_oStartTime; }

	CFlightType();
	CFlightType(const CFlightType& _rhs);
	CFlightType& operator = (const CFlightType& _rhs);
	virtual ~CFlightType();

	
	CString ScreenPrint( BOOL _HasReturn=FALSE ) const;
	const CString& GetName() const { return m_csName; } 
	void SetName( const CString& _csName ){ m_csName = _csName; }
	FlightConstraint& GetFlightConstraint() { return m_flightConstraint; }
	const FlightConstraint& GetFlightConstraint() const { return m_flightConstraint; }
	void SetFlightConstraint( const FlightConstraint& _flightConstraint ){ m_flightConstraint = _flightConstraint; }

	BOOL Matches(const AirsideFlightDescStruct& fds,bool bARR, Terminal* pTerminal) const;

protected:
	CString m_csName;

	FlightConstraint m_flightConstraint;
	BOOL m_isFlightConstraintAdd;

	BOOL m_isTimeApplied;
	COleDateTime m_oStartTime;
	COleDateTime m_oEndTime;
	
	BOOL m_isBirthTimeApplied;
	COleDateTime m_oMinBirthTime;
	COleDateTime m_oMaxBirthTime;
	BOOL m_isBirthTimeAdd;

	BOOL m_isDeathTimeApplied;
	COleDateTime m_oMinDeathTime;
	COleDateTime m_oMaxDeathTime;
	BOOL m_isDeathTimeAdd;

	//min altitude
	//max altitude
	//air route
	//ground route
	//runway
};

#endif // __FLIGHTTYPE_H__
