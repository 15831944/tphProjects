#pragma once


class CNumRange
{
public:
	CNumRange(){}
	CNumRange(double dmin, double dmax):m_minor(dmin),m_max(dmax){}
	void MinorMerge(const CNumRange& other);

	bool IsValid()const{ return m_minor<=m_max; }

	double m_minor;
	double m_max;
};

template<class T>
class CRange
{
public:
	typedef CRange<T>  _this_type;
	CRange(const T& tmin, const T& tmax):m_min(tmin), m_max(tmax) { makeValid(); }

	bool isValid()const{ return m_min<= m_max; }
	
	CRange& makeValid(){  
		if(!isValid()){ 
			std::swap(m_min,m_max);
		}
		return *this; 
	}

	CRange validCopy()const{  _this_type ret  = *this;  return ret.makeValid(); }
	
	bool isOverlap(const _this_type& t )const
	{
		ASSERT(isValid());
		ASSERT(t.isValid());
		return !( t.m_max < m_min || m_max < t.m_min );
	}

protected:
	double m_min;
	double m_max;
};