#include "StdAfx.h"
#include "ILayoutObject.h"



void ILayoutObjectDisplay::InitDefault()
{
	DspProp& dpShape = GetProp(_Shape);
	dpShape.cColor = ARCColor4(55, 145, 170,255);
	dpShape.bOn = true;
	DspProp& dpName = GetProp(_Name);
	dpName.cColor = ARCColor4(0,0,0,255);
	dpName.bOn = false;

}

CString ILayoutObjectDisplay::GetPropTypeString( int nPropType )
{
	if(nPropType==_Name)
		return "Name";
	if(nPropType == _Shape)
		return "Shape";
	return "Unknown";
}

ILayoutObjectDisplay::ILayoutObjectDisplay()
{
	InitDefault();
}

ILayoutObjectDisplay::DspProp::DspProp()
{
	bOn= true;
}

ILayoutObjectDisplay::DspProp::DspProp( const ARCColor4& c, bool b /*= true*/)
{
	cColor = c;
	bOn = b;
}

ILayoutObject::ILayoutObject()
:m_bLocked(false)
{

}
