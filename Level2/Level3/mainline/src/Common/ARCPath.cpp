#include "stdafx.h"
#include "Path.h"
#include "ARCPath.h"
#include <limits>
#include "./BizierCurve.h"
#include "ARCRay.h"

ARCPath::ARCPath(const Path* _pPath) :
	std::vector<ARCPoint2>(_pPath->getCount())
{
	size_type N = size();
	for(size_type i=0; i<N; i++) {
		at(i) = ARCPoint2(_pPath->getPoint(i).getX(), _pPath->getPoint(i).getY());
	}
}

ARCPath& ARCPath::operator=(const ARCPath& _rhs)
{
	using std::vector;
	if(this == &_rhs)
		return *this;
	this->vector<ARCPoint2>::operator=(_rhs);
	return *this;
}

ARCPath& ARCPath::operator=(const std::vector<ARCPoint2>& _rhs)
{
	using std::vector;
	vector<ARCPoint2>* thisBase = static_cast<vector<ARCPoint2>*>(this);
	if(thisBase == &_rhs)
		return *this;
	this->vector<ARCPoint2>::operator=(_rhs);
	return *this;
}

void ARCPath::Rotate(DistanceUnit _degrees)
{
	for(iterator it=begin(); it!=end(); it++) {
		(*it).Rotate(_degrees);
	}
}

const ARCPoint2& ARCPath::GetClosestPoint(const ARCPoint2& _p) const
{
	ASSERT(size() > 0);
	DistanceUnit d;
	DistanceUnit dSmallest = at(0).DistanceTo(_p);
	const_iterator itSmallest = begin();
	for(const_iterator it=begin()+1; it!=end(); it++) {
		if((d=(*it).DistanceTo(_p)) < dSmallest) {
			itSmallest = it;
			dSmallest = d;
		}
	}
	return (*itSmallest);
}

void ARCPath::Distribute(const std::vector<ARCPoint2>& _vList, DistanceUnit _clearance)
{
	clear();
	size_type nPtCount = _vList.size();
    if(!nPtCount)
        return;

	if(nPtCount == 1 ) {
        *this = _vList;
        return;
    }

    if(nPtCount >= 1023)
		return;
        //throw new StringError("Maximum point count exceeded in ARCPath::Distribute(...)");

	ARCVector2 ptvector;
	for(size_type i=0; i<nPtCount-1; i++) {
		push_back( _vList[i] );
        ptvector = _vList[i+1]-_vList[i];
        ptvector.SetLength(_clearance);
        while(at(size()-1).DistanceTo(_vList[i+1]) > _clearance) {
			push_back( at(size()-1) + ptvector );
        }
    }
}

void ARCPath::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset)
{
	for(iterator it=begin(); it!=end(); it++) {
		(*it)[VX]+=_xOffset;
		(*it)[VY]+=_yOffset;
	}
}

void ARCPath::DoOffset(const ARCVector2& _offset)
{
	for(iterator it=begin(); it!=end(); it++) {
		(*it)+=_offset;
	}
}

ARCPoint2 ARCPath::GetRandPoint(DistanceUnit _area) const
{
	const ARCPoint2& pt = at(random(size()));
	return pt.GetRandPoint(_area);
}

DistanceUnit ARCPath::CalculateArea() const
{
	size_type i,j;
	DistanceUnit ret = 0.0;
	unsigned N = size();

	for(i=0;i<N;i++) {
		j = (i + 1) % N;
		ret += at(i)[VX] * at(j)[VY];
		ret -= at(i)[VY] * at(j)[VX];
	}

	ret = ret/2;
	return(ret < 0 ? -ret : ret);
}

ARCPoint2 ARCPath::Centroid() const
{
	size_type N = size();
	unsigned i,j;
	ARCPoint2 ret(0.0,0.0);
	DistanceUnit area = CalculateArea();

	if (area < 0.000000001)	{
		for(i=0;i<N;i++) {
			const ARCPoint2& pi = at(i);
			ret[VX] += pi[VX];
			ret[VY] += pi[VY];
		}

		if (N > 0) {
			ret[VX] = ret[VX] / N;
			ret[VY] = ret[VY] / N;
		}
	}
	else {
		for(i=0;i<N;i++) {
			j = (i+1) % N;
			const ARCPoint2& pi = at(i);
			const ARCPoint2& pj = at(j);
			ret[VX] = ret[VX] + ( (pi[VX]+pj[VX]) * (pi[VX]*pj[VY] - pj[VX]*pi[VY]) );
			ret[VY] = ret[VX] + ( (pi[VY]+pj[VY]) * (pi[VX]*pj[VY] - pj[VX]*pi[VY]) );
		}
		ret = ret*(1/(6*area));
	}

	return ret;
}

void ARCPath::TransfromToRayCoordX( const ARCRay2& ray )
{
	ARCVector2 dirY = ray.m_dir.PerpendicularLCopy();
	for(size_t i=0;i<size();i++)
	{
		ARCVector2 offset = at(i) - ray.m_orign;
		double x = offset.dot(ray.m_dir);
		double y = offset.dot(dirY);
		(*this)[i] = ARCVector2(x,y);
	}
}


//
//
//
//
//get the point distance from the begin point,the start point must in the Path
ARCVector3 ARCPath3::getDistancePoint(ARCVector3 startPt, double dist)const
{
	return getDistancePoint( getPointDistance(startPt)+dist );
}

//get Point distance from the path's first point
ARCVector3 ARCPath3::getDistancePoint(double dist)const
{
	ASSERT(size() > 1);
	double accDist =0;
	int i=0;
	if(size()<1)
		return ARCVector3();
	if(size()<2)
		return at(0);

	for(i=0;i<(int)size()-1;i++ )
	{
		ARCVector3 v = (*this)[i+1] - (*this)[i];
		double mag = v.Magnitude();
		accDist += mag;
		if(accDist > dist )
		{
			double t = ( accDist - dist ) / mag;
			return (*this)[i] * t + (*this)[i+1] * (1-t);
		}
	}
	return (*this)[i];
}

#include "./DistanceLineLine.h"
//get the distance from the path's first pt to the endpt;
double ARCPath3::getPointDistance(ARCVector3 endPt)const
{
	CPath2008 path = getPath2008();
	CPoint2008 pt(endPt.x,endPt.y,endPt.z);
	DistancePointPath3D distPt3D;	
	distPt3D.GetSquared(pt,path);
	return GetIndexDist(distPt3D.m_fPathParameter);
}
//get 
ARCVector3 ARCPath3::getRelativePoint(double relateDist) const
{
	return getDistancePoint(relateDist*GetLength());
}
//get all path length
DistanceUnit ARCPath3::GetLength()const
{
	double len = 0;
	for(int i=0;i<(int)size() -1 ;i++)
	{
		len += ( (*this)[i+1] - (*this)[i] ).Magnitude();
	}
	return len;
}

ARCPath3::ARCPath3( const ARCPath3& path )
{
	resize(path.size());
	copy(path.begin(),path.end(),begin());
}

ARCPath3::ARCPath3( const CPath2008& path )
{
	int nCount = path.getCount();
	reserve(nCount);
	for(int i=0;i<nCount;i++)
	{
		push_back(path.getPoint(i));
	}

}

int ARCPath3::getClosestPointId( const ARCPoint3& pt ) const
{
	int retid = 0;
	double closelen = (std::numeric_limits<double>::max)();
	for(size_t i = 0 ; i<size();++i)
	{
		double len = ( at(i) - pt ).Length();
		if( closelen > len ){
			closelen = len ;
			retid = i;
		}
	}

	return retid;
}

ARCPoint3 ARCPath3::GetIndexDir( double fIndex ) const
{
	ASSERT(size()>1);
	int idist =(int)fIndex;
	if(size()<2)
		return ARCVector3(0,0,0);

	if(idist < 0)
	{
		return at(0) - at(1);
	}

	if(idist > (int)size() -2)
	{
		return at(size() -1) - at( size() -2 );
	}

	return at(idist+1) - at(idist);
}

CPath2008 ARCPath3::getPath2008() const
{
	CPath2008 reslt;
	reslt.init( (int)size() );
	int i=0;
	for(const_iterator itr = begin(); itr!=end();itr++)
	{
		reslt[i] = CPoint2008( (*itr)[VX],(*itr)[VY],(*itr)[VZ]);
		i++;
	}
	return reslt;
}

Path ARCPath3::getPath()const
{
	Path reslt;
	reslt.init( (int)size() );
	int i=0;
	for(const_iterator itr = begin(); itr!=end();itr++)
	{
		reslt[i] = Point( (*itr)[VX],(*itr)[VY],(*itr)[VZ]);
		i++;
	}
	return reslt;
}

ARCPath3& ARCPath3::DoOffset( const ARCVector3 & doffset )
{
	for(iterator itr = begin(); itr!=end();itr++)
	{
		*itr += doffset;
	}
	return *this;
}

double ARCPath3::GetDistIndex( double realDist ) const
{
	if(size()<2) return 0;

	double accdist = 0;
	double thisdist = 0;
	int i=0;
	for( i=0 ;i< (int)size()-1;i++){
		if(accdist >= realDist ){
			break;
		}		
		thisdist  = at(i).DistanceTo(at(i+1));
		accdist += thisdist;
	}

	double exceed = 0;
	if(thisdist == 0)
	{
		exceed = 0;
	}
	else
	{
		exceed = (accdist - realDist) / thisdist;
	}
	if(exceed > 1)
	{
		//ASSERT(false);
		exceed = 1;
	}
	if(exceed <0)
	{
		//ASSERT(false);
		exceed = 0;
	}

	return i - (float)exceed;
}

ARCPoint3 ARCPath3::GetIndexPos( double fIndex ) const
{
	ASSERT(size());
	int npt = (int)fIndex;
	double offset = fIndex - npt;

	if(npt < 0) at(0);

	if( npt >= (int)size()-1 )
	{
		return at(npt);		
	}
	return at(npt) * (1-offset) + at(npt+1)*offset;
}

//
ARCPath3 ARCPath3::GetRoundCornerPath( double dCornerRad ) const
{
	if(size()<3)
		return *this;

	ARCPath3 _out;


	const static int outPtsCnt = 6;
	
	_out.reserve( (size()-2)*outPtsCnt + 2);

	_out.push_back(at(0));


	ARCPoint3 ctrPts[3];
	for(int i=1;i<(int)size()-1;i++)
	{
		ctrPts[0] = at(i-1);
		ctrPts[1] = at(i);
		ctrPts[2] = at(i+1);

		//get the two side point
		ARCVector3 v1 = ctrPts[0] - ctrPts[1];
		ARCVector3 v2 = ctrPts[2] - ctrPts[1];
		double minLen1,minLen2;
		minLen1 = min(dCornerRad,v1.Length());
		minLen2	= min(dCornerRad,v2.Length());

		v1.SetLength(minLen1);
		v2.SetLength(minLen2);
		ctrPts[0] = ctrPts[1] + v1; 
		ctrPts[2] = ctrPts[1] + v2;

		std::vector<ARCVector3> ctrlPoints(ctrPts,ctrPts+3);
		std::vector<ARCVector3> output;
		BizierCurve::GenCurvePoints(ctrlPoints,output,outPtsCnt);
		for(int k = 0;k<outPtsCnt;k++)
		{
			_out.push_back(output[k]);
		}
	}

	_out.push_back(	*rbegin() );	
	return _out;
}

ARCPath3 ARCPath3::GetSubPathToEndIndex( double beginIndex ) const
{
	ARCPath3 ret;
	int iIndex = (int)beginIndex;
	if(iIndex >=0 && iIndex < (int)size()-1 )
	{
		const ARCPoint3& p1 = at(iIndex);
		const ARCPoint3& p2 = at(iIndex+1);
		double frat =  beginIndex - iIndex;
		ret.push_back( p1*(1-frat)+p2*frat );
	}

	for(int i = iIndex+1;i<(int)size();i++)
	{
		ret.push_back(at(i));
	}

	return ret;	
}

ARCPath3& ARCPath3::RotateXY( double degree, const ARCVector3& center )
{
	DoOffset(-center);
	RotateXY(degree);
	DoOffset(center);
	return *this;
}

ARCPath3& ARCPath3::RotateXY( double degree )
{
	for(size_t i=0; i<size(); ++i )
	{
		ARCVector3& pt = (*this)[i];
		pt.RotateXY(degree);
	}
	return *this;
}

ARCPath3 ARCPath3::GetSubPathIndex( double beginIndex, double endIndex )const
{
	bool bswaped = false;
	if(beginIndex > endIndex){
		std::swap(beginIndex, endIndex);
		bswaped = true;
	}

	ARCPath3 ret;
	//ASSERT(relatPos1>-1 && relatPos1 <= getCount());
	//ASSERT(relatPos2>-1 && relatPos2 <= getCount());

	int piFrom =(int) beginIndex; int piTo =(int)endIndex;

	ASSERT(piFrom <= piTo);
	ret.reserve(piTo -piFrom + 2);

	if(piFrom >=0 && piFrom < (int)size()-1 )
	{
		const ARCPoint3& p1 = at(piFrom);
		const ARCPoint3& p2 = at(piFrom+1);
		double frat =  beginIndex - piFrom;
		ret.push_back( p1*(1-frat)+p2*frat );
	}

	for(int i = piFrom+1;i<=piTo;i++)
	{
		ret.push_back(at(i));
	}

	if(piTo >=0 && piTo < (int)size()-1 && endIndex > piTo )
	{
		const ARCPoint3& p1 = at(piTo);
		const ARCPoint3& p2 = at(piTo+1);
		double frat =  endIndex - piTo;
		ret.push_back( p1*(1-frat)+p2*frat );
	}

	return bswaped?ret.Reverse():ret;

	
}

ARCPath3 ARCPath3::GetSubPathFromBeginIndex( double endIndex ) const
{
	ARCPath3 ret;
	int iIndex = (int)endIndex;
	for(int i = 0;i<=iIndex;i++)
	{
		ret.push_back(at(i));
	}
	if(endIndex > iIndex && iIndex < (int)size()-1 && iIndex>=0)
	{
		const ARCPoint3& p1 = at(iIndex);
		const ARCPoint3& p2 = at(iIndex+1);
		double frat =  endIndex - iIndex;
		ret.push_back( p1*(1-frat)+p2*frat );
	}

	return ret;	
}

double ARCPath3::GetIndexDist( double fIndex ) const
{
	if(fIndex < 0) return -1;
	if(size() < 2) return 0;

	if(fIndex >= size() -1 ) return GetLength();

	int npt = (int)fIndex;
	double offset = fIndex - npt;

	double len = 0;
	for(int i=0;i< npt;i++){
		len += at(i).DistanceTo( at(i+1) );
	}	
	len += at(npt).DistanceTo(at(npt+1)) * offset;
	return len;
}