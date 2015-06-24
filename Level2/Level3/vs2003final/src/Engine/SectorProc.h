#pragma once
#include "airfieldprocessor.h"

class SectorProc :
	public AirfieldProcessor
{
public:
	SectorProc(void);
	virtual ~SectorProc(void);

	//processor type 
	virtual int getProcessorType()const{ return SectorProcessor; } 
	//processor name 
	virtual const char * getProcessorName(void)const { return "Sector"; }
	//processor io
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;
	virtual void initServiceLocation (int pathCount, const Point *pointList);


	double getMinAlt()const ;
	void setMinAlt(double _minalt);
	double getMaxAlt()const;
	void setMaxAlt(double _maxalt);

	double getAltBandInterval()const;
	void setAltBandInterval(double _dbandinter);
private:
	double m_dMaxAlt;
	double m_dMinAlt;
	double m_dBandInterval;

};
