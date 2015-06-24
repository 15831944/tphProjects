// ProcessorSwitchTimeDB.h: interface for the CProcessorSwitchTimeDB class.
#pragma once
#include <assert.h>
#include "common\dataset.h"
#include "MobileElemConstraint.h"

class ProSwitchTime
{
private:
	CMobileElemConstraint m_MobType;
	int iStyle;			// 0 : all data is invalidate
						// 1 : dep data is validate
						// 2 : arr data is validate
						// 3 : all data is validate
	int DepOpenTime;
	int DepCloseTime;
	int ArrOpenTime;
	int ArrCloseTime;
    int m_proNum ;
	BOOL m_isDaily ;
public:
	ProSwitchTime()
	{
		m_MobType.initDefault();
		iStyle			= 0;
		DepOpenTime		= 0;
		DepCloseTime	= 0;
		ArrOpenTime		= 0;
		ArrCloseTime	= 0;
		m_proNum = 1 ;
		m_isDaily = TRUE ;
	}
	virtual ~ProSwitchTime(){};

	void SetDefaultValue();
	void SetDepDefValue();
	void SetArrDefValue();
	////////////////////////////////////
	//get
	CMobileElemConstraint* getMobType(void) { return &m_MobType;}
	int getStyle( void ) const { return iStyle;	}
	int GetFltConstraintMode( void ) const { return m_MobType.GetFltConstraintMode(); } 
	int getDepOpenTime(void) const { return DepOpenTime;}
	int getDepCloseTime(void) const { return DepCloseTime;}
	int getArrOpenTime(void) const { return ArrOpenTime;}
	int getArrCloseTime(void) const { return ArrCloseTime; }
	int getProcessNum() { return m_proNum ;} ;
	BOOL getIsDaily() { return m_isDaily ;} ;
	///////////////////////////////////////
	//set
	void setStyle(int _style ) { iStyle = _style; }
	void setMobType( CMobileElemConstraint& _MobType ) { m_MobType = _MobType;}
	void setDepOpenTime( int _time ){ DepOpenTime = _time;}
	void setDepCloseTime( int _time ){ DepCloseTime = _time;}
	void setArrOpenTime( int _time ){ ArrOpenTime = _time;	}
	void setArrCloseTime( int _time){ ArrCloseTime = _time;}
	void setProcessNum(int _pronum) { m_proNum = _pronum ;} 
	void setIsDaily(BOOL _isdaily) { m_isDaily = _isdaily ;}
	/////////////////////////////////////////
	//read&write file
	void readData( ArctermFile& p_file );
	void writeData( ArctermFile& p_file ) const;

	/////////////////////////////////////////////
	//operator
	bool operator == (ProSwitchTime pst) const;
	bool isEqual( ProSwitchTime pst )const;
};

class CProcessorSwitchTimeDB : public DataSet  
{
private:
	std::vector<ProSwitchTime> m_data;
public:
	CProcessorSwitchTimeDB();
	virtual ~CProcessorSwitchTimeDB(); 
	
	//////////////////////////////////////////////////////////////
	// read & write file
	virtual void clear() { m_data.clear(); }
	virtual void readData( ArctermFile& p_file );
	virtual void writeData( ArctermFile& p_file ) const;

	virtual const char *getTitle () const;
	virtual const char *getHeaders () const;

	//////////////////////////////////////////////////////////////
	int getCount(void) const { return m_data.size(); }
	int addItem( ProSwitchTime& pst );
	void setItem( int _indx,ProSwitchTime& pst);
	ProSwitchTime getItem( int _indx ) const
		{
			assert( _indx>=0 && (UINT)_indx < m_data.size() );
			return m_data[_indx];
		}
	void removeAt( int _indx)
		{
			assert( _indx>=0 && (UINT)_indx <m_data.size() );
			m_data.erase( m_data.begin() + _indx );
		}
};
