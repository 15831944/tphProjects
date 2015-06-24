// AircraftDispPropItem.h: interface for the CAircraftDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AIRCRAFTDISPPROPITEM_H__
#define __AIRCRAFTDISPPROPITEM_H__

#pragma once


#include "FlightType.h"


class CAircraftDispPropItem  
{
public:
	enum ENUM_LINE_TYPE { 
		ENUM_LINE_1PTSOLID=0, ENUM_LINE_2PTSOLID, ENUM_LINE_3PTSOLID,
		ENUM_LINE_1PTDASH, ENUM_LINE_2PTDASH, ENUM_LINE_3PTDASH,
		ENUM_LINE_1PTDOT, ENUM_LINE_2PTDOT, ENUM_LINE_3PTDOT
	};

	CAircraftDispPropItem( const CString& _csName );
	CAircraftDispPropItem( const CAircraftDispPropItem& _rhs );
	virtual ~CAircraftDispPropItem();

	CFlightType* GetFlightType() { return &m_flightType; }
	BOOL IsVisible() const { return m_isVisible; }
	const std::pair<int, int>& GetShape() const { return m_shape; }
	COLORREF GetColor() const { return m_colorRef; }
	BOOL IsLeaveTrail() const { return m_isLeaveTrail; }
	ENUM_LINE_TYPE GetLineType() const { return m_linetype; }
	float GetDensity()const { return m_fdensity;}    //for test 	
	BOOL IsLogoOn(){ return m_isLogoOn; }
	float GetScaleSize(){ return m_fscalesize; }
	void SetDensity(float density_){ m_fdensity = density_;}
	void SetLogoOn(BOOL blogoOn = TRUE){ m_isLogoOn = blogoOn; }
	void SetScaleSize(float scalesize_){ m_fscalesize = scalesize_;	}
	void SetFlightType( const CFlightType& _flightType ){ m_flightType = _flightType; }
	void SetVisible( BOOL _isVisible ){ m_isVisible = _isVisible; }
	void SetShape( const std::pair<int, int>& _shape ){ m_shape = _shape; }
	void SetColor( COLORREF _colorRef ){ m_colorRef = _colorRef; }
	void SetLeaveTrail( BOOL _isLeaveTrail ){ m_isLeaveTrail = _isLeaveTrail; }
	void SetLineType( enum ENUM_LINE_TYPE _linetype ){ m_linetype = _linetype; }
    int operator < (CAircraftDispPropItem& _rhs)  { if(this!=NULL) return m_flightType.GetFlightConstraint() < _rhs.m_flightType.GetFlightConstraint(); else return 0;}

protected:
	float m_fscalesize;
	float m_fdensity ;
	CFlightType m_flightType;
	BOOL m_isVisible;
	BOOL m_isLogoOn;
	std::pair<int, int> m_shape;
	COLORREF m_colorRef;
	BOOL m_isLeaveTrail;
	enum ENUM_LINE_TYPE m_linetype;
};

#endif //__AIRCRAFTDISPPROPITEM_H__
