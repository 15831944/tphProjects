#ifndef __SHADOWOBJ_
#define __SHADOWOBJ_
#include <iostream>
#include <vector>

#include "../Common/point.h"
#pragma once
/*
This class implements the 3D object with showdow; 
codes are changed from nehe lesson 27
*/
struct sPlaneEq{
	DistanceUnit a, b, c, d;
};
// structure describing an object's face
struct sPlane{
	unsigned int p[3];   //index of each point 
	Point normal;
	unsigned int neigh[3];  //index of neighbour point 
	sPlaneEq PlaneEq;       //plane 
	bool visible;

	sPlane(UINT p1,UINT p2,UINT p3){p[0]=p1;p[1]=p2;p[2]=p3;normal.setPoint(0,0,1.0);neigh[0]=0;neigh[1]=0;neigh[2]=0;}
};
class ShadowObj
{
protected :
	std::vector<Point> m_points;
	std::vector<sPlane> m_faces;
public:
	void addPoints(Point & p){m_points.push_back(p);}
	void addFace(sPlane & face){m_faces.push_back(face);}
	void SetConnectivity();
	
	void CalcPlane();
	void DrawGLObject();
	void CastShadow(double *lp);
	void Clear(){m_points.clear();m_faces.clear();}



};
#endif