// TrainLogEntry.h: interface for the CTrainLogEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAINLOGENTRY_H__A1F35A3E_61E7_450A_A880_5B399F3C0FE6__INCLUDED_)
#define AFX_TRAINLOGENTRY_H__A1F35A3E_61E7_450A_A880_5B399F3C0FE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include "results\logitem.h"
#include "common\term_bin.h"
#include "common\point.h"

class CTrainLogEntry : public TerminalLogItem<TrainDescStruct,TrainEventStruct>
{
protected:

public:
	
    CTrainLogEntry();

    virtual ~CTrainLogEntry ();

    virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;
	
	void SetTrainID(int nID);
	int GetTrainID();

	void SetId( CString _csId );
	long GetID();

	void SetStartTime( ElapsedTime _time );

	void SetEndTime( ElapsedTime _time );
	
	void SetCarNumber( int _nCars );

	void SetCarLength( DistanceUnit _dLength );

	void SetCarWidth( DistanceUnit _dWidth );

	void SetSpeed( DistanceUnit _Speed );

	void SetAccelerateSpeed( DistanceUnit _accelerateSpeed );

	void SetDecelerateSpeed( DistanceUnit _decelerateSpeed );

	void SetCircleTime( ElapsedTime _time );
	
	void SetTurnAroundTime( ElapsedTime _turnAroundTime );

	void SetHeadWayTime( ElapsedTime _headWayTime);

	void SetIndexNum( int _nIDx );

	int GetIndexNum();
	ElapsedTime GetStartTime() const;
	ElapsedTime GetEndTime() const;
	ElapsedTime GetTurnAroundTime() const;
	ElapsedTime GetHeadWayTime() const;

};



#endif // !defined(AFX_TRAINLOGENTRY_H__A1F35A3E_61E7_450A_A880_5B399F3C0FE6__INCLUDED_)
