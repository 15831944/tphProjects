#ifndef __LAND_CONTROL_DEVICE
#define __LAND_CONTROL_DEVICE
#pragma once
#include "./LandfieldProcessor.h"

class LandControlDevice :
	public LandfieldProcessor
{
public:
	LandControlDevice(void);
	virtual ~LandControlDevice(void);

	

	//virtual void Rotate( DistanceUnit _degree );

	virtual bool HasProperty(ProcessorProperty propType)const;

	//processor i/o	
	/*virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;*/

	
private:
	
	//double m_rotateAngle;    //angle of the device in degree	 
};

#endif