// RailwayInfoKey.h: interface for the RailwayInfoKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAILWAYINFOKEY_H__775886ED_38F3_4BB6_892C_7056FE477AD9__INCLUDED_)
#define AFX_RAILWAYINFOKEY_H__775886ED_38F3_4BB6_892C_7056FE477AD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

// a station pair and PORTTOPORT link infomation.
class RailwayInfoKey  
{ 
private:
	//first station 
	int m_iFirstStationIndex;

	//linked type from first station
	int m_iTypeFromFirstViewPoint;

	//second station
	int m_iSecondStationIndex;
public:
	RailwayInfoKey();
	virtual ~RailwayInfoKey();
public:
	void SetFirstStationIndex(int _index){ m_iFirstStationIndex = _index; }
	void SetSecondStationIndex(int _index){ m_iSecondStationIndex = _index; }
	void SetTypeFromFirstViewPoint( int _type){ m_iTypeFromFirstViewPoint= _type;}
	int GetFirstStationIndex() const {return m_iFirstStationIndex;};
	int GetSecondStationIndex() const {return m_iSecondStationIndex;};
	int GetTypeFromFirstViewPoint( ) const { return m_iTypeFromFirstViewPoint;}
	bool operator == ( const RailwayInfoKey& _another) const ;

};
typedef std::vector<RailwayInfoKey>RAILWAYKEYS;
#endif // !defined(AFX_RAILWAYINFOKEY_H__775886ED_38F3_4BB6_892C_7056FE477AD9__INCLUDED_)
