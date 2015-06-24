// PaxDispPropItem.h: interface for the CPaxDispPropItem class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include "PaxType.h"

#include <utility>

class CPaxDispPropItem  
{
public:
	enum ENUM_LINE_TYPE { 
		ENUM_LINE_1PTSOLID=0, ENUM_LINE_2PTSOLID, ENUM_LINE_3PTSOLID,
		ENUM_LINE_1PTDASH, ENUM_LINE_2PTDASH, ENUM_LINE_3PTDASH,
		ENUM_LINE_1PTDOT, ENUM_LINE_2PTDOT, ENUM_LINE_3PTDOT
	};

	CPaxDispPropItem( CString _csName );
	CPaxDispPropItem( const CPaxDispPropItem& _rhs );
	virtual ~CPaxDispPropItem();

	CPaxType* GetPaxType(){ return &m_paxType; }
	BOOL IsVisible() const { return m_isVisible; }
	const std::pair<int, int>& GetShape() const { return m_shape; }
	COLORREF GetColor() const { return m_colorRef; }
	BOOL IsLeaveTrail() const { return m_isLeaveTrail; }
	ENUM_LINE_TYPE GetLineType() const { return m_linetype; }
	double GetDensity() const { return m_dDensity; }

	void SetPaxType( const CPaxType& _paxType ){ m_paxType = _paxType; }
	void SetVisible( BOOL _isVisible ){ m_isVisible = _isVisible; }
	void SetShape( const std::pair<int, int>& _shape ){ m_shape = _shape; }
	void SetColor( COLORREF _colorRef ){ m_colorRef = _colorRef; }
	void SetLeaveTrail( BOOL _isLeaveTrail ){ m_isLeaveTrail = _isLeaveTrail; }
	void SetLineType( enum ENUM_LINE_TYPE _linetype ){ m_linetype = _linetype; }
	void SetDensity(double _d) { m_dDensity=_d; }
    int operator < (CPaxDispPropItem& Item)  { if(this!=NULL) return m_paxType.GetPaxCon() < Item.GetPaxType()->GetPaxCon(); else return 0;}
protected:
	CPaxType m_paxType;
	double m_dDensity;
	BOOL m_isVisible;
	std::pair<int, int> m_shape;
	COLORREF m_colorRef;
	BOOL m_isLeaveTrail;
	enum ENUM_LINE_TYPE m_linetype;
};


