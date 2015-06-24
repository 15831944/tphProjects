// FlightScheduleCriteriaDB.h: interface for the FlightScheduleCriteriaDB class.
#pragma once

#include "common\dataset.h"
#include "main\FlightScheduleAddUtil.h"

class FlightScheduleCriteriaDB : public DataSet  
{
	std::vector<CFlightScheduleAddUtil*>m_vAllSchduleUtil;
	bool m_bAutoAssign;
	bool m_bReAssign;
public:
	FlightScheduleCriteriaDB();
	virtual ~FlightScheduleCriteriaDB();
public:
	
	virtual void readData (ArctermFile& p_file);	
	virtual void writeData (ArctermFile& p_file) const;
	virtual void clear (void);

	virtual const char *getTitle (void) const { return "flight schdule criteria File" ;};
	virtual const char *getHeaders (void) const {return "Flight constraint, change distribution, time offset distribution, start time, end time " ;};
	
public:
	//std::vector<CFlightScheduleAddUtil>* GetScheduleUtils(){ return &m_vAllSchduleUtil;	}
	int GetCount()const { return m_vAllSchduleUtil.size();	}
	CFlightScheduleAddUtil* GetItem( int _iIdx );
	void RemoveItem( int _iIdx );
	void RemoveItem( CFlightScheduleAddUtil* _pItem );
	void AddNewItem( CFlightScheduleAddUtil* _pItem ){ m_vAllSchduleUtil.push_back( _pItem );	}
	bool GetAutoAssignFlag()const { return m_bAutoAssign;	}
	bool GetReAssignFlag()const { return m_bReAssign;	}
	void SetAutoAssignFlag( bool _bFlag ){ m_bAutoAssign = _bFlag;	}
	void SetReAssignFlag( bool _bFlag ){ m_bReAssign = _bFlag;	}

	
};
