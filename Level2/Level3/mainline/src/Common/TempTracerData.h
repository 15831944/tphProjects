// TempTracerData.h: interface for the CTempTracerData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPTRACERDATA_H__84C866A4_FD40_43A4_BFF0_93C71D80BCAF__INCLUDED_)
#define AFX_TEMPTRACERDATA_H__84C866A4_FD40_43A4_BFF0_93C71D80BCAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\ARCVector.h"

class CTrackerVector3 
{
public:
	union{
		DistanceUnit n[3];
		struct{
			DistanceUnit x,y,z;
		};
	};
public:
	CTrackerVector3():m_IsRealZ(0) {} ;

	CTrackerVector3(const DistanceUnit _x, const DistanceUnit _y, const DistanceUnit _z) {
			 n[VX]=_x; n[VY]=_y; n[VZ]=_z; 
	} ;
	virtual ~CTrackerVector3() {} ;
protected:
	int m_IsRealZ ;
public:
	int IsRealZ() { return m_IsRealZ ;} ;
	void IsRealZ(int _is) { m_IsRealZ = _is ;} ;
	CTrackerVector3& operator	+=	( const CTrackerVector3& _v )
	{ n[VX]+=_v.n[VX]; n[VY]+=_v.n[VY]; n[VZ]+=_v.n[VZ]; return *this; }
	CTrackerVector3& operator	-=	( const CTrackerVector3& _v )
	{ n[VX]-=_v.n[VX]; n[VY]-=_v.n[VY]; n[VZ]-=_v.n[VZ]; return *this; }
	CTrackerVector3& operator	*=	( const DistanceUnit _d )
	{ n[VX]*=_d; n[VY]*=_d; n[VZ]*=_d; return *this; }
	CTrackerVector3& operator	/=	( const DistanceUnit _d )
	{ n[VX]/=_d; n[VY]/=_d; n[VZ]/=_d; return *this; }

	CTrackerVector3& operator	*=	( const CTrackerVector3& _v )
	{ n[VX]*=_v[VX]; n[VY]*=_v[VY]; n[VZ]*=_v[VZ]; return *this; }
	CTrackerVector3& operator	/=	( const CTrackerVector3& _v )
	{ n[VX]/=_v[VX]; n[VY]/=_v[VX]; n[VZ]/=_v[VZ]; return *this; }

	CTrackerVector3 operator	*	( const CTrackerVector3& _v )const
	{ return CTrackerVector3(*this)*=_v; }
	CTrackerVector3 operator	/	( const CTrackerVector3& _v )const
	{ return CTrackerVector3(*this)/=_v; }
	DistanceUnit dot(const CTrackerVector3& _v)const{
		return n[VX]*_v[VX]+n[VY]*_v[VY]+n[VZ]*_v[VZ];
	}

	DistanceUnit& operator [] ( int i ) { return n[i]; }
	const DistanceUnit& operator [] ( int i ) const { return n[i]; }
	operator DistanceUnit*() { return n; }				//pointer cast
	operator const DistanceUnit*() const { return n; }	//const pointer cast

	friend CTrackerVector3 operator - ( const CTrackerVector3& _v )								// -v
	{ return CTrackerVector3(-_v.n[VX],-_v.n[VY],-_v.n[VZ]); }
	friend CTrackerVector3 operator + ( const CTrackerVector3& _v, const CTrackerVector3& _u )			// v + u
	{ return CTrackerVector3(_v.n[VX] + _u.n[VX], _v.n[VY] + _u.n[VY], _v.n[VZ] + _u.n[VZ]); }
	friend CTrackerVector3 operator - ( const CTrackerVector3& _v, const CTrackerVector3& _u )			// v - u
	{ return CTrackerVector3(_v.n[VX] - _u.n[VX], _v.n[VY] - _u.n[VY], _v.n[VZ] - _u.n[VZ]); }
	friend CTrackerVector3 operator / ( const CTrackerVector3& _v, const DistanceUnit _d )		// v / scalar
	{ return CTrackerVector3(_v.n[VX]/_d,_v.n[VY]/_d,_v.n[VZ]/_d); }
	friend CTrackerVector3 operator * ( const CTrackerVector3& _v, const DistanceUnit _d )		// v * scalar
	{ return CTrackerVector3(_v.n[VX]*_d,_v.n[VY]*_d,_v.n[VZ]*_d); }
	friend CTrackerVector3 operator * ( DistanceUnit _d, const CTrackerVector3& _v )				// scalar * v
	{ return CTrackerVector3(_v.n[VX]*_d,_v.n[VY]*_d,_v.n[VZ]*_d); }
	//friend CTrackerVector3 operator * ( const CTrackerVector3& _v, const CTrackerVector3& _u )		// v * u (not dot product)
	//	{ return CTrackerVector3(_v.n[VX]*_u.n[VX],_v.n[VY]*_u.n[VY],_v.n[VZ]*_u.n[VZ]); }
	friend CTrackerVector3 operator ^ ( const CTrackerVector3& _v, const CTrackerVector3& _u );		// v x u (cross product)
	friend int operator == ( const CTrackerVector3& _v, const CTrackerVector3& _u )				// v == u ?
	{ return _v.n[VX]==_u.n[VX] && _v.n[VY]==_u.n[VY] && _v.n[VZ]==_u.n[VZ]; }
	friend int operator != ( const CTrackerVector3& _v, const CTrackerVector3& _u )				// v != u ?
	{ return _v.n[VX]!=_u.n[VX] || _v.n[VY]!=_u.n[VY] || _v.n[VZ]!=_u.n[VZ]; }
	int Compare(const CTrackerVector3& _v, const DistanceUnit _epsilon=ARCMath::EPSILON) const;						// this == v ? with epsilon
	//friend std::ostream& operator << ( std::ostream& _s, CTrackerVector3& _v );				// output to stream
	//friend std::istream& operator >> ( std::istream& _s, CTrackerVector3& _v );				// input from stream

	DistanceUnit DistanceTo(const CTrackerVector3& _rhs) const;
	DistanceUnit Magnitude2() const
	{ return n[VX]*n[VX]+n[VY]*n[VY]+n[VZ]*n[VZ]; }
};
class CTempTracerDataItem
{
public:
	std::vector<CTrackerVector3> vTracks;
	int iDispPropIdx;
	int nPaxId;
	COLORREF mColor;
	int mLineType;
};

class CTempTracerData  
{
public:
	CTempTracerData();
	virtual ~CTempTracerData();

	void clear(); //clears all track data

	int GetTrackCount(); //return number of tracks
	int GetDispPropIdx(int nTrackIdx); //returns the associate display properties index for a track
	int GetPaxID(int nTrackIdx);

	std::vector<CTrackerVector3>& GetTrack(int nTrackIdx); //returns the track points for a track
	const std::vector<CTrackerVector3>& GetTrack(int nTrackIdx)const;
	const COLORREF GetColor(int nTrackIdx)const;
	const int GetLineType(int nTrackIdx)const;

	int AddTrack(int nHits, int nDPIdx, int nPaxId);	//adds a track and reserves space for nHits, and sets display prop idx
											//returns the index of the new track

	int AddTrack(int nHits, int nDPIdx, int nPaxId, COLORREF nColor, int nLineType );
private:
	std::vector<CTempTracerDataItem> m_vItems;
	
	//std::vector< std::vector<ARCVector3> > m_vTracks;
	//std::vector<int> m_vDispPropIdx;

};

#endif // !defined(AFX_TEMPTRACERDATA_H__84C866A4_FD40_43A4_BFF0_93C71D80BCAF__INCLUDED_)
