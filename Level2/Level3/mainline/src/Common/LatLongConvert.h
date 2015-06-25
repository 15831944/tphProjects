#pragma once

#include "LatLong.h"
#include <math.h>

#define DEGTORAD    0.01745329251994
#define RADTODEG 	57.29577951308
#define TRUE        1
#define FALSE       0
#define RADIUS      3440.0    
#define M_PI_2      1.57079632679489661923

class LatLong_Structure
{

public:
	CLatitude *m_pLatitude;
	CLongitude *m_pLongitude;

	double m_fLatitude;
	double m_fLongitude;
	
	double m_dx;
	double m_dy;

public:
	LatLong_Structure(CLatitude *pLatitude,CLongitude *pLongitude,double dx,double dy)
	{
		m_pLatitude=pLatitude;
		m_pLongitude=pLongitude;
		m_dx=dx;
		m_dy=dy;
		ConventLatLong(TRUE);
	};
	LatLong_Structure(CLatitude *pLatitude,CLongitude *pLongitude)
	{
		m_pLatitude=pLatitude;
		m_pLongitude=pLongitude;
		ConventLatLong(TRUE);
	};
	LatLong_Structure(double dx,double dy)
	{
		m_dx=dx;
		m_dy=dy;
		m_pLatitude=NULL;
		m_pLongitude=NULL;
		m_fLatitude=0.0;
		m_fLongitude=0.0;

	}
	LatLong_Structure()
	{
		m_dx=0.0;
		m_dy=0.0;
		m_pLatitude=NULL;
		m_pLongitude=NULL;
		m_fLatitude=0.0;
		m_fLongitude=0.0;
	}

	~LatLong_Structure() 
	{
	
	};
	void ConventLatLong(BOOL bFlag);;
};

class CLatLongConvert
{
public:
	CLatLongConvert(void);
	~CLatLongConvert(void);

public:
	LatLong_Structure *LatLong_Origin;
	LatLong_Structure *LatLong_Tangent;
	
	//lat long 
	 double orgCosLatitude , orgSinLatitude  ;
	 double orgCosLongitude , orgSinLongitude ;

public:
	
	void Set_LatLong_Origin (CLatitude *pLatitude , CLongitude *pLongitude,double dx,double dy);
	void Set_LatLong_Tangent(CLatitude *pLatitude , CLongitude *pLongitude);
	void Set_LatLong_Tangent(double dx,double dy);

	void Distance_LatLong (LatLong_Structure *pt2, float &delta_x,float &delta_y, float& distance);
	void Distance_LatLong_2 (LatLong_Structure *pt1, LatLong_Structure *pt2, float &delta_x, float &delta_y,float &distance);
	
	void Point_LatLong (float delta_x, float delta_y, LatLong_Structure *pt);
	void Point_LatLong_2 (LatLong_Structure *pt1, float delta_x,float delta_y, LatLong_Structure *pt2); 
	void Convent_decimal_Latlong(LatLong_Structure *pt1,LatLong_Structure *pt2);
};
