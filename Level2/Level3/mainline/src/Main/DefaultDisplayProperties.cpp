// DefaultDisplayProperties.cpp: implementation of the CDefaultDisplayProperties class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "DefaultDisplayProperties.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaultDisplayProperties::CDefaultDisplayProperties() : DataSet( DefaultDisplayPropertiesFile )
{
	m_colorRef[AreasColor] = ARCColor3( 170, 25, 25 );
	m_colorRef[PortalsColor] = ARCColor3( 235, 185, 30 );
	m_colorRef[ProcShapeColor] = ARCColor3( 55, 145, 170 );
	m_colorRef[ProcServLocColor] = ARCColor3( 0, 100, 200 );
	m_colorRef[ProcQueueColor] = ARCColor3( 64, 210, 64 );
	m_colorRef[ProcInConstraintColor] = ARCColor3( 255, 100, 0 );
	m_colorRef[ProcOutConstraintColor] = ARCColor3( 230, 210, 25 );
	m_colorRef[ProcNameColor] = ARCColor3( 0, 0, 0 );
	

	m_boolVal[ProcShapeOn] = TRUE;
	m_boolVal[ProcServLocOn] = TRUE;
	m_boolVal[ProcQueueOn] = TRUE;
	m_boolVal[ProcInConstraintOn] = TRUE;
	m_boolVal[ProcOutConstraintOn] = TRUE;
	m_boolVal[ProcNameOn] = FALSE;
}

CDefaultDisplayProperties::~CDefaultDisplayProperties()
{
}

BOOL CDefaultDisplayProperties::GetBoolValue(enum ENUM_BOOLVAL _enumBoolVal) const
{
	return m_boolVal[_enumBoolVal];
}

const ARCColor3& CDefaultDisplayProperties::GetColorValue(ENUM_COLORREF _enumClrRef) const
{
	return m_colorRef[_enumClrRef];
}

void CDefaultDisplayProperties::SetBoolValue(enum ENUM_BOOLVAL _enumBoolVal, BOOL _bVal )
{
	m_boolVal[_enumBoolVal] = _bVal;
}

void CDefaultDisplayProperties::SetColorValue(ENUM_COLORREF _enumClrRef, const ARCColor3& _clrRef)
{
	m_colorRef[_enumClrRef] = _clrRef;
}


void CDefaultDisplayProperties::readData(ArctermFile& p_file)
{
	for( int i=0; i<DEFAULT_COLORREF_COUNT; i++ )
	{
		p_file.getLine();
		p_file.skipField(1);
		long lVal;
		p_file.getInteger( lVal );
		m_colorRef[i] = ARCColor3(static_cast<COLORREF>(lVal));
	}

	p_file.skipLines( RESERVED_LINE );

	for( int i=0; i<DEFAULT_BOOLVAL_COUNT; i++ )
	{
		p_file.getLine();
		p_file.skipField(1);
		int nVal;
		p_file.getInteger( nVal );
		m_boolVal[i] = nVal;
	}	
}

void CDefaultDisplayProperties::writeData(ArctermFile& p_file) const
{
	static char csClrRefLabel[][128] = { "AreasColor", 
										"PortalsColor",
										"ProcShapeColor",
										"ProcServLocColor",
										"ProcQueueColor",
										"ProcInConstraintColor",
										"ProcOutConstraintColor",
										"ProcNameColor" };

	static char csBoolValLabel[][128] = { "ProcShapeOn",
											"ProcServLocOn",
											"ProcQueueOn",
											"ProcInConstraintOn",
											"ProcOutConstraintOn",
											"ProcNameOn" };
	
	for( int i=0; i<DEFAULT_COLORREF_COUNT; i++ )
	{
		p_file.writeField( csClrRefLabel[i] );
		COLORREF col = static_cast<COLORREF>(m_colorRef[i]);
		p_file.writeInt( (long) col );
		p_file.writeLine();		
	}

	for(int i=0; i<RESERVED_LINE; i++ )
		p_file.writeLine();		

	for(int i=0; i<DEFAULT_BOOLVAL_COUNT; i++ )
	{
		p_file.writeField( csBoolValLabel[i] );
		p_file.writeInt( (int)m_boolVal[i] );
		p_file.writeLine();		
	}	
}