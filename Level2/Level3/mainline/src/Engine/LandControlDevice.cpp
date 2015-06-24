#include "StdAfx.h"
#include ".\landcontroldevice.h"

LandControlDevice::LandControlDevice(void)
{
	
}

LandControlDevice::~LandControlDevice(void)
{
}

//void LandControlDevice::setRotateAngle(double angle)
//{
//	m_rotateAngle= angle;
//	m_pRenderer->Update();
//}

//void LandControlDevice::Rotate(DistanceUnit _degree )
//{
//	m_pRenderer->Update();
//}

bool LandControlDevice::HasProperty(ProcessorProperty propType)const
{
	if(propType ==LandfieldProcessor::PropControlPoints	)
		return true;
	
	return false;
}

//processor i/o	
 /*int LandControlDevice::readSpecialField(ArctermFile& procFile)
{
	return 1;
}
 int LandControlDevice::writeSpecialField(ArctermFile& procFile) const
{
	return 1;
}*/
