// ViehcleMovePath.h: interface for the CViehcleMovePath class.
#pragma once
#include "common\point.h"
#include "common\statetimepoint.h"
#include "common\termfile.h"

class CViehcleMovePath
{
public:
	CViehcleMovePath();
	virtual ~CViehcleMovePath();

	CViehcleMovePath( const CViehcleMovePath& _movePath );


	int GetCount();

	void AddItem( Point _pt, ElapsedTime _time, char _state );

	void AddItem( CStateTimePoint& _item );

	CStateTimePoint GetItem( int _nIdx );

	void SetArrTime( ElapsedTime _time );

	ElapsedTime GetArrTime();

	void SetDepTime( ElapsedTime _time );

	ElapsedTime GetDepTime();

	ElapsedTime GetDestArrTime();

	void DoTimeOffset( ElapsedTime _time );

	void WriteData( ArctermFile& p_file ) const;

	void ReadData( ArctermFile& _file );

	void Clear();

protected:

	ElapsedTime m_arrTime;

	ElapsedTime m_depTime;

	std::vector<CStateTimePoint> m_vectStateTimePoint;
};
