#pragma once

#include "ARCVector.h"
//****************************************************************
enum LineType
{
	Cubic_Spline,
	Straight_Line,
};
class CubicFunc
{
public:
	CubicFunc():_a(0),_b(0),_c(0),_d(0) { mLineType = Cubic_Spline; }
	CubicFunc(double a, double b, double c, double d) :_a(a),_b(b),_c(c),_d(d) { }
	

	double _a,_b,_c,_d;  

	bool getRoot(double dval, double& dRoot)const;

	LineType mLineType;

	double eval(double u)const{
		return (mLineType==Cubic_Spline)?evalCubic(u):evalLine(u);
	}
	double evalD(double u)const;

private:
	double evalCubic(double u) const{ return (((_d*u) + _c)*u + _b)*u + _a; }
	double evalCubicD(double u)const{  return (3*_d*u + 2*_c)*u + _b;  }

	double evalLine(double u)const;
	double evalLineD(double u)const;

};

enum CubicSplineType
{
	Free,
	Closed,
};


struct CubicSpline3
{
	
	std::vector<CubicFunc> m_vect_cubicX, m_vect_cubicY, m_vect_cubicZ;
	CubicSpline3(CubicSplineType ctype = Free):m_type(ctype){	}

	CubicSplineType m_type;

	void setType(CubicSplineType ctype){ m_type = ctype; }
	CubicSplineType getType()const{ return m_type; }	
	
	void clear(){ m_vect_cubicX.clear(); m_vect_cubicY.clear(); m_vect_cubicZ.clear(); }
	ARCVector3 getLinePoint(double  p)const;
	ARCVector3 getLinePoint(int i, double p)const;
	virtual void build(const std::vector<ARCVector3>& vPts); 

	bool GetXPosRoot(double dXpos, double& p)const;
	
	ARCVector3 getLineDir(double p)const;
	ARCVector3 getLineDir(int i, double p)const;

	void setLineType(int idx, LineType t);
	LineType getLineType(int idx)const;
	
};

struct CubicSpline2
{
	std::vector<CubicFunc> m_vect_cubicX, m_vect_cubicY;

	CubicSplineType m_type;

	void setType(CubicSplineType ctype){ m_type = ctype; }
	CubicSplineType getType()const{ return m_type; }

	void clear(){   m_vect_cubicX.clear(); m_vect_cubicY.clear(); }
	ARCVector2 getLinePoint(double  p)const;
	ARCVector2 getLinePoint(int i, double p)const;
	virtual void build(const std::vector<ARCVector2>& vPts); 

	ARCVector2 getLineDir(double p)const;
	ARCVector2 getLineDir(int i, double p)const;
	void GetPointList(int nPtCnt, std::vector<ARCVector2>& vOut)const;

	void setLineType(int idx, LineType t);
	LineType getLineType(int idx)const;

};





//****************************************************************

//struct SplineLineSegment
//{
//	Vector3 p0_, p1_, p0_p1_;
//	float min_x, max_x;
//	//Vector2 p0, p1, p0_p1;
//	//float pk, pa, min_y, max_y, min_z, max_z;
//	SplineLineSegment(const Vector3& p0_, const Vector3& p1_)
//	{
//		this->p0_ = p0_;
//		this->p1_ = p1_;
//		min_x = min(p0_.x, p1_.x);
//		max_x = max(p0_.x, p1_.x);
//		p0_p1_ = p1_ - p0_;
//
//		//p0 = Vector2(p0_.y, p0_.z);
//		//p1 = Vector2(p1_.y, p1_.z);
//		//p0_p1 = p1 - p0;
//		//p0_p1.normalise();
//		//pk = p0_p1.x / p0_p1.y;
//		//pa = atan(pk);
//		//min_y = min(p0.x, p1.x);
//		//max_y = max(p0.x, p1.x);
//		//min_z = min(p0.y, p1.y);
//		//max_z = max(p0.y, p1.y);
//	}
//	//float calc_yz(const Vector3& pos_)
//	//{
//	//	Vector2 pos(pos_.y, pos_.z);
//	//	Vector2 p0_pos = pos - p0;
//	//	Vector2 p1_pos = pos - p1;
//	//	float d0 = p0_pos.normalise();
//	//	float d1 = p1_pos.normalise();
//	//	float cosa0 = p0_p1.dotProduct(p0_pos);
//	//	float cosa1 = p0_p1.dotProduct(p1_pos);
//	//	if(cosa0 * cosa1 >= 0)
//	//		return min(d0, d1);
//	//	else
//	//	{
//	//		float sina0 = sqrt(1.0 - cosa0*cosa0);
//	//		float d2 = d0 * sina0;
//	//		return min(d0, min(d1, d2));
//	//	}
//	//	return 10000000;
//	//}
//	//bool insect_yz(Vector2 s, float k, float a, Vector2& r)
//	//{
//	//	if(abs(a-pa) < 0.001) return false;
//	//	float z = ((k*s.y-pk*p0.y)-(s.x - p0.x))/(k - pk);
//	//	if(z < min_z || z > max_z) return false;
//	//	float y = p0.x + pk*(z-p0.y);
//	//	if(y < min_y || y > max_y) return false;
//	//	r.x = y; r.y = z;
//	//	return true;
//	//}
//	bool insect_x(float x, Vector3& p)
//	{
//		if(!(x >= min_x && x < max_x)) return false; 
//		float k = abs((p1_.x-x)/p0_p1_.x);
//		p = p0_*k + p1_*(1.0-k);
//		return true;
//	}
//};
//
//struct CubicSplineDisperse : public CubicSpline
//{
//	std::vector<Vector3> m_point_vec;
//	std::vector<SplineLineSegment> m_line_vec;
//	std::vector<int> m_point_index_vec;
//	int m_level;
//
//	CubicSplineDisperse() { m_level = 15; }
//
//	static void build_line(std::vector<SplineLineSegment>& dest, std::vector<Vector3>& src)
//	{
//		int num = src.size();
//		if(num <= 1) return;
//		dest.reserve(num);
//		for(int i=0; i<num-1; ++i)
//			dest.push_back(SplineLineSegment(src[i], src[i+1]));
//	}
//	Vector3 calc_point_percent(int i, float p)
//	{
//		Vector3 pt;
//		pt.x = ( m_vect_cubicX[i].eval(p) );
//		pt.y = ( m_vect_cubicY[i].eval(p) );
//		pt.z = ( m_vect_cubicZ[i].eval(p) );
//		return pt; 
//	}
//	
//	void clac_point_x(float x, Vector3& p0, Vector3& p1)
//	{
//		p1 = p0 = Vector3(0, 0, 0);
//		int num = m_line_vec.size();
//		for(int i=0; i<num; ++i)
//			if(m_line_vec[i].insect_x(x, p0))
//				break;
//		for(int i=num-1; i>=0; --i)
//			if(m_line_vec[i].insect_x(x, p1))
//				break;
//	}
//	void build()
//	{
//		init();
//		if(m_point_data.size() < 2) return;
//		__super::build();
//
//		int num = m_vect_cubicX.size();
//		int line_num = num * m_level + 1;
//		m_point_vec.reserve(line_num);
//		m_point_index_vec.reserve(line_num);
//		Vector3 pt(m_vect_cubicX[0].eval(0), m_vect_cubicY[0].eval(0), m_vect_cubicZ[0].eval(0));
//		m_point_vec.push_back(pt);
//		for(int i=0; i<num; i++)
//		{
//			for(int j=1; j<=m_level; j++)
//			{
//				float u = (float)j/(float)m_level;
//				pt.x = ( m_vect_cubicX[i].eval(u) );
//				pt.y = ( m_vect_cubicY[i].eval(u) );
//				pt.z = ( m_vect_cubicZ[i].eval(u) );
//				m_point_vec.push_back(pt);
//				m_point_index_vec.push_back(i);
//			}
//		}
//		build_line(m_line_vec, m_point_vec);
//	}
//	virtual void init()
//	{
//		m_line_vec.clear();
//		m_point_vec.clear();
//		m_point_index_vec.clear();
//	}
//	virtual void set_level(int level, bool bulid_now=true)
//	{
//		if(level <= 0) return;
//		if(level == m_level) return;
//		m_level = level;
//		if(bulid_now) build();
//	}
//	
//	virtual void copy(CubicSplineDisperse* src)
//	{
//		if(!src) return;
//		m_point_data = src->m_point_data;
//		m_vect_cubicX = src->m_vect_cubicX;
//		m_vect_cubicY = src->m_vect_cubicY;
//		m_vect_cubicZ = src->m_vect_cubicZ;
//		m_point_vec = src->m_point_vec;
//		m_line_vec = src->m_line_vec;
//		m_point_index_vec = src->m_point_index_vec;
//		m_level = src->m_level;
//	}
//};
//
//****************************************************************

//struct CubicSplineDisperseEx : public CubicSplineDisperse
//{
//	std::vector<ObjBase*> m_obj_vec;
//
//	void clear()
//	{
//		init();
//		m_point_data.clear();
//		m_obj_vec.clear();
//	}
//	void add_point(ObjBase* obj, bool bulid_now=true)
//	{
//		if(!obj) return;
//		m_obj_vec.push_back(obj);
//		if(bulid_now) build();
//	}
//	void add_point_index(ObjBase* obj, int index, bool bulid_now=true)
//	{
//		int num = m_obj_vec.size();
//		std::vector<ObjBase*>::iterator ii(m_obj_vec.begin());
//		if(index == num-1)
//			m_obj_vec.insert(ii, obj);
//		else
//		{
//			ii += index + 1;
//			m_obj_vec.insert(ii, obj);
//		}
//		if(bulid_now) build();
//	}
//	//void add_point_yz(ObjBase* obj, bool bulid_now=true)
//	//{
//	//	if(!obj) return;
//	//	int num = m_point_data.size();
//	//	if(num <= 1)
//	//	{
//	//		add_point(obj, bulid_now);
//	//		return;
//	//	}
//	//	num = m_line_vec.size();
//	//	Vector3 new_pos = obj->get_pos();
//	//	float min_dis = 10000000;
//	//	int index = 0;
//	//	for(int i=0; i<num; ++i)
//	//	{
//	//		float dis = m_line_vec[i].calc_yz(new_pos);
//	//		if(dis <= min_dis) { min_dis = dis; index = i; }
//	//	}
//	//	add_point_index(obj, m_point_index_vec[index], bulid_now);
//	//}
//	void remove_point(ObjBase* obj, bool bulid_now=true)
//	{
//		if(!obj) return;
//		m_obj_vec.erase(std::remove(m_obj_vec.begin(), m_obj_vec.end(), obj));
//		if(bulid_now) build();
//	}
//	void remove_point(int num, Obj** obj, bool bulid_now=true)
//	{
//		if(num <= 0 || !obj) return;
//		for(int i=0; i<num; ++i)
//			remove_point((ObjBase*)obj[i], false);
//		if(bulid_now) build();
//	}
//	void build()
//	{
//		int num = m_obj_vec.size();
//		m_point_data.clear();
//		m_point_data.reserve(num);
//		for(int i=0; i<num; ++i)
//			m_point_data.push_back(m_obj_vec[i]->get_pos());
//		__super::build();
//	}
//};
//
//****************************************************************