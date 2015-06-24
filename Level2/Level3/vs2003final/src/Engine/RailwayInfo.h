// RailwayInfo.h: interface for the RailwayInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAILWAYINFO_H__A08C226E_D0A7_4D80_8CE0_73AB1E3CD459__INCLUDED_)
#define AFX_RAILWAYINFO_H__A08C226E_D0A7_4D80_8CE0_73AB1E3CD459__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\common\path.h"
#include "RailwayInfoKey.h"

// Railway information between two stations
class RailwayInfo  
{
private:
	//railway path
	Path m_railWay;

	//railway key
	RailwayInfoKey m_key;

	//linked type from second station
	int m_iTypeFromSecondViewPoint;

	//owner station's index
	int m_iOwnerIndex;

public:
	RailwayInfo();
	RailwayInfo(const RailwayInfo& _anotherRail);
	RailwayInfo& operator=(const RailwayInfo& _anotherRail);
	virtual ~RailwayInfo();

	void SetRailWay(int _pointCount, const Point* _pointList) { m_railWay.init(_pointCount,_pointList);	}
	void SetFirstStationIndex(int _index){ m_key.SetFirstStationIndex( _index ); }
	void SetSecondStationIndex(int _index){ m_key.SetSecondStationIndex( _index ); }
	void SetTypeFromFirstViewPoint( int _type){ m_key.SetTypeFromFirstViewPoint( _type ) ;}
	void SetTypeFromSecondViewPoint( int _type){ m_iTypeFromSecondViewPoint= _type;}
	void SetOwnerIndex( int _index){ m_iOwnerIndex= _index;}


	int GetFirstStationIndex() const {return m_key.GetFirstStationIndex();};
	int GetSecondStationIndex() const {return m_key.GetSecondStationIndex();};
	int GetTypeFromFirstViewPoint( ) const { return m_key.GetTypeFromFirstViewPoint();}
	int GetTypeFromSecondViewPoint( ) const { return m_iTypeFromSecondViewPoint;}
	int GetOwnerIndex()const { return m_iOwnerIndex;	}
	const Path& GetRailWayPath() const { return m_railWay;	}

	//read my info from file
	void ReadDataFromFile(ArctermFile& p_file);
	// save my data to file
	void SaveDataToFile(ArctermFile& p_file);
	//check if my path is defined correctly
	bool IsPathDefined() const { return m_railWay.getCount() >0;}
	const RailwayInfoKey& GetRailWayKey() const { return m_key ;};
	bool operator == ( const RailwayInfo& _anotherRail ) const ;

	void InsertPoint( int _iAfterIdx, Point& _pt );
	void GetPoint ( int _iIdx, Point & _pt ) const;
	void SetPoint ( int _iIdx, Point & _pt );
	void DeletePoint( int _iIdx );
	
}; 

#endif // !defined(AFX_RAILWAYINFO_H__A08C226E_D0A7_4D80_8CE0_73AB1E3CD459__INCLUDED_)
