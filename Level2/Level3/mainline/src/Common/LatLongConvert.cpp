#include "StdAfx.h"
#include "latlongconvert.h"

CLatLongConvert::CLatLongConvert(void)
{
	orgCosLatitude = 1.0;
	orgSinLatitude = 0.0 ;
	orgCosLongitude = 1.0; 
	orgSinLongitude = 0.0 ;

	LatLong_Origin=NULL;
	LatLong_Tangent=NULL;
}

CLatLongConvert::~CLatLongConvert(void)
{
	if (LatLong_Origin)
		delete LatLong_Origin;
	
}

void CLatLongConvert::Set_LatLong_Origin(CLatitude *pLatitude , CLongitude *pLongitude,double dx,double dy)
{
	LatLong_Origin=new LatLong_Structure(pLatitude,pLongitude,dx,dy);
	orgCosLatitude = cos( (double)LatLong_Origin->m_fLatitude );
	orgSinLatitude = sin( (double)LatLong_Origin->m_fLatitude );
	orgCosLongitude = cos( (double)LatLong_Origin->m_fLongitude );
	orgSinLongitude = sin( (double)LatLong_Origin->m_fLongitude );
}

void CLatLongConvert::Set_LatLong_Tangent(CLatitude *pLatitude , CLongitude *pLongitude)
{
//	LatLong_Tangent.LatLong_Structure(pLatitude,pLongitude);
}
void CLatLongConvert::Set_LatLong_Tangent(double dx,double dy)
{
//	LatLong_Tangent.LatLong_Structure(dx,dy);
}

void CLatLongConvert::Distance_LatLong(LatLong_Structure *pt2, float &delta_x,float &delta_y, float& distance)
{
	Distance_LatLong_2(LatLong_Origin,pt2,delta_x,delta_y,distance);
}
void CLatLongConvert::Distance_LatLong_2(LatLong_Structure *pt1, LatLong_Structure *pt2, float &delta_x, float &delta_y,float &distance)
{
	double	course_angle ;
	double	v1, v4, theta ;

	if ( fabs ( pt1->m_fLatitude - pt2->m_fLatitude ) < 0.0000001 &&
		fabs ( pt1->m_fLongitude - pt2->m_fLongitude ) < 0.0000001 ) {
			delta_x = delta_y = distance = 0.0 ;
			return;
		}

		/* Distance in radians */
		if ( pt1 == LatLong_Origin )
			theta = ( orgSinLatitude * sin((double)pt2->m_fLatitude) +
			orgCosLatitude * cos((double)pt2->m_fLatitude) *
			cos((double)pt2->m_fLongitude-pt1->m_fLongitude) ) ;
		else
			theta = ( sin((double)pt1->m_fLatitude) * sin((double)pt2->m_fLatitude) +
			cos((double)pt1->m_fLatitude) * cos((double)pt2->m_fLatitude) *
			cos((double)pt2->m_fLongitude-pt1->m_fLongitude) ) ;

		v1 = acos(theta) ;
		/* Distance in nm */
		distance =(float)(60 * v1 * RADTODEG );

		if ( pt1 == LatLong_Origin )
			v4 = ( sin(pt2->m_fLatitude) - orgSinLatitude * cos(v1) ) /
			( sin(v1) * orgCosLatitude ) ;
		else
			v4 = ( sin(pt2->m_fLatitude) - sin(pt1->m_fLatitude) * cos(v1) ) /
			( sin(v1) * cos(pt1->m_fLatitude) ) ;

		/* Fix domain error */
		v4 = ( v4 > 1.0 ? 1.0 : ( v4 < -1.0 ? -1.0 : v4 ) ) ;
		course_angle = M_PI_2 - acos ( v4 ) ;

		delta_x = (float)(distance * cos ( course_angle )) ;
		/* taking care of sign on dx */
		if ( pt2->m_fLongitude < pt1->m_fLongitude )
			delta_x *= -1.0 ;
		delta_y = (float)(distance * sin ( course_angle )) ;
}

void CLatLongConvert::Point_LatLong(float delta_x, float delta_y, LatLong_Structure *pt)
{
	Point_LatLong_2(LatLong_Origin,delta_x,delta_y,pt);
}

void CLatLongConvert::Point_LatLong_2(LatLong_Structure *pt1, float delta_x,float delta_y, LatLong_Structure *pt2)
{
	double	course_angle, distance ;
	double	v1, v3, v4, theta ;

	distance = hypot ( delta_x, delta_y ) ;

	if ( fabs ( distance ) < 0.0000001 )
	{
		pt2->m_fLongitude=pt1->m_fLongitude;
		pt2->m_fLatitude=pt1->m_fLatitude;
		Convent_decimal_Latlong(pt1,pt2);
		
		return;
	}

	v1 = distance * DEGTORAD / 60.0 ;
	theta = cos((double)v1) ;

	course_angle = asin ( (double)(delta_y / distance) ) ;
	v3 = cos ( (double)(M_PI_2 - course_angle) ) ;

	if ( pt1 == LatLong_Origin )
		v3 = v3 * sin((double)v1) * orgCosLatitude +
		orgSinLatitude * cos((double)v1) ;
	else
		v3 = v3 * sin((double)v1) * cos((double)pt1->m_fLatitude) +
		sin((double)pt1->m_fLatitude) * cos((double)v1) ;

	/* Fix domain error */
	v3 = ( v3 > 1.0 ? 1.0 : ( v3 < -1.0 ? -1.0 : v3 ) ) ;
	pt2->m_fLatitude = (float)asin ( v3 ) ;

	if ( pt1 == LatLong_Origin )
		v4 = ( theta - orgSinLatitude * sin((double)pt2->m_fLatitude) ) /
		( orgCosLatitude * cos(pt2->m_fLatitude) ) ;
	else
		v4 = ( theta - sin((double)pt1->m_fLatitude) * sin((double)pt2->m_fLatitude) ) /
		( cos((double)pt1->m_fLatitude) * cos(pt2->m_fLatitude) ) ;

	/* Fix domain error */
	v4 = ( v4 > 1.0 ? 1.0 : ( v4 < -1.0 ? -1.0 : v4 ) ) ;
	pt2->m_fLongitude =(float)(pt1->m_fLongitude + acos((double)v4) * ( delta_x >= 0.0 ? 1.0 : -1.0 ) );
	
	pt2->ConventLatLong(FALSE);
	//Convent_decimal_Latlong(pt1,pt2);
}

void CLatLongConvert::Convent_decimal_Latlong(LatLong_Structure *pt1,LatLong_Structure *pt2)
{
	pt2->m_fLatitude=pt2->m_fLatitude/DEGTORAD;
	if (pt2->m_fLatitude>0)
	{
		pt2->m_pLatitude->m_dir= pt1->m_pLatitude->m_dir;
	}
	else
	{
		if(pt1->m_pLatitude->m_dir=="N")
			pt2->m_pLatitude->m_dir="S";
		else
			pt2->m_pLatitude->m_dir="N";
	}

	pt2->m_fLongitude= pt2->m_fLongitude/DEGTORAD;

	if (pt2->m_fLongitude>0)
	{
		pt2->m_pLongitude->m_dir=pt1->m_pLongitude->m_dir;
	}else
	{
		if(pt1->m_pLongitude->m_dir=="E")
			pt2->m_pLongitude->m_dir="W";
		else
			pt2->m_pLongitude->m_dir="E";
	}

	int i,s,d;

	//latitude


	double value = fabs (pt2->m_fLatitude ) ;
	double degree = (double) (int)value ;
	double minute = ( value - degree ) * 60 ;
	i = (int)minute ;
	//s=(int)((minute-(float)i)*60) ;
	double second = (minute-(double)i)*60.0;
	s = (int)second;
	double delta = (second-(double)s)*60.0;
	d = (int)delta;

	pt2->m_pLatitude->m_nDeg=(int)degree;
	pt2->m_pLatitude->m_nMinutes=i;
	pt2->m_pLatitude->m_nSeconds=s;
	pt2->m_pLatitude->m_nDelta = d;

	//longitude

	value = fabs ( pt2->m_fLongitude ) ;
	degree = (double) (int)value ;
	minute = ( value - degree ) * 60 ;
	i = (int)minute ;
	//s=(int)((minute-(float)i)*60) ;
	second = (minute-(double)i)*60.0;
	s = (int)second;
	delta = (second-(double)s)*60.0;
	d = (int)delta;
	pt2->m_pLongitude->m_nDeg=(int)degree;
	pt2->m_pLongitude->m_nMinutes=i;
	pt2->m_pLongitude->m_nSeconds=s;
	pt2->m_pLongitude->m_nDelta = d;
}

void LatLong_Structure::ConventLatLong( BOOL bFlag )
{
	if (bFlag)// to latitude longitude structure
	{
		m_fLatitude=m_pLatitude->m_nDeg + (m_pLatitude->m_nMinutes)/60.0 + (m_pLatitude->m_nSeconds)/3600.0 +(m_pLatitude->m_nDelta)/(3600.0*60.0);
		m_fLatitude=m_fLatitude * DEGTORAD;
		if( m_pLatitude->m_dir == "S" ) m_fLatitude *= -1.0f;
		m_fLongitude=(m_pLongitude->m_nDeg) +( m_pLongitude->m_nMinutes)/60.0 + (m_pLongitude->m_nSeconds)/3600.0 + (m_pLongitude->m_nDelta)/(3600.0*60.0);
		m_fLongitude=m_fLongitude* DEGTORAD;
		if( m_pLongitude->m_dir == "W") m_fLongitude *= -1.0f;
	}
	else// to decimal
	{
		int i, k ,s, d;
		m_fLatitude = m_fLatitude * RADTODEG;
		m_fLongitude=m_fLongitude* RADTODEG;

		//latitude
		k = (m_fLatitude < 0.0 ) ;
		double value = fabs ( m_fLatitude ) ;
		double degree = (double) (int)value ;
		double minute = ( value - degree ) * 60.0 ;
		i = (int)minute ;
		//s=(int)((minute-(double)i)*60.0) ;
        double second = (minute-(double)i)*60.0;
		s = (int)second;
        double delta = (second-(double)s)*60.0;
		d = (int)delta;

		if( m_fLatitude< 0.0)
			m_pLatitude->m_dir = "S";
		else 
			m_pLatitude->m_dir = "N";

		m_pLatitude->m_nDeg=(int)degree;
		m_pLatitude->m_nMinutes=i;
		m_pLatitude->m_nSeconds=s;
		m_pLatitude->m_nDelta=d;

		//longitude
		if(m_fLongitude < 0.0)
			m_pLongitude->m_dir = "W";
		else
			m_pLongitude->m_dir = "E";

		value = fabs ( m_fLongitude ) ;
		degree = (double) (int)value ;
		minute = ( value - degree ) * 60.0 ;
		i = (int)minute ;
		//s=(int)((minute-(double)i)*60.0) ;
		second = (minute-(double)i)*60.0;
		s = (int)second;
		delta = (second-(double)s)*60.0;
		d = (int)delta;


		m_pLongitude->m_nDeg=(int)degree;
		m_pLongitude->m_nMinutes=i;
		m_pLongitude->m_nSeconds=s;
		m_pLongitude->m_nDelta=d;
	}
}