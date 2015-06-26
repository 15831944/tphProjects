// ElevatorLogEntry.h: interface for the CElevatorLogEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEVATORLOGENTRY_H__8F646DA0_B103_4849_8B85_21775023815E__INCLUDED_)
#define AFX_ELEVATORLOGENTRY_H__8F646DA0_B103_4849_8B85_21775023815E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "results\logitem.h"
#include "Common\TERM_BIN.H"

class CElevatorLogEntry  : public TerminalLogItem<ElevatorDescStruct,ElevatorEventStruct>
{	
public:
	CElevatorLogEntry();
	virtual ~CElevatorLogEntry();
		
public:
	void SetID( CString _strID );
	
	void SetStartTime( const ElapsedTime& _time );

	void SetEndTime( const ElapsedTime& _time );

	ElapsedTime GetStartTime() const;
	ElapsedTime GetEndTime() const;

	void SetLiftLength( double _dLength );
	double GetLiftLength()const;

	void SetLiftWidth( double _dWidth );
	double GetLiftWidth()const;

	void SetIndex( long _lIdx );
	long GetIndex()const;
	
	void SetOrientation( double _dOrientation );
	double GetOrientation()const;


	virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;

};

#endif // !defined(AFX_ELEVATORLOGENTRY_H__8F646DA0_B103_4849_8B85_21775023815E__INCLUDED_)
