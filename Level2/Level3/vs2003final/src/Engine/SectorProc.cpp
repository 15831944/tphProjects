#include "StdAfx.h"
#include ".\sectorproc.h"

SectorProc::SectorProc(void)
{
	 m_dMaxAlt = 0 ;
	 m_dMinAlt = 0 ;
	 m_dBandInterval = 0 ;
}

SectorProc::~SectorProc(void)
{
}


int SectorProc::readSpecialField(ArctermFile& procFile){
	procFile.getFloat(m_dMinAlt);
	procFile.getFloat(m_dMaxAlt);
	procFile.getFloat(m_dBandInterval);

	return TRUE;
}
int SectorProc::writeSpecialField(ArctermFile& procFile)const{
	procFile.writeFloat(float(m_dMinAlt));
	procFile.writeFloat(float(m_dMaxAlt));
	procFile.writeFloat(float(m_dBandInterval));

	return TRUE;
}


double SectorProc::getMinAlt()const {
	return m_dMinAlt;
}
void SectorProc::setMinAlt(double _minalt){
	m_dMinAlt = _minalt;
}
double SectorProc::getMaxAlt()const{
	return m_dMaxAlt;
}

void SectorProc::setMaxAlt(double _maxalt){
	m_dMaxAlt =  _maxalt;
}

double SectorProc::getAltBandInterval()const{
	return m_dBandInterval;
}
void SectorProc::setAltBandInterval(double _dbandinter){
	m_dBandInterval = _dbandinter;
}

void SectorProc::initServiceLocation (int pathCount, const Point *pointList){
	if (pathCount <0)
		throw new StringError ("Processor must have 2 service m_location");

	m_location.init (pathCount, pointList);
}
