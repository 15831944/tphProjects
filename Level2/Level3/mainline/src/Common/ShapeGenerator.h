#ifndef __SHAPE_GENERATOR_H
#define __SHAPE_GENERATOR_H

#pragma once

#include <vector>
#include "Point.h"
#include "ARCVector.h"
#include "Path.h"
#include "ARCPath.h"

class ShapeGenerator
{
protected:
	ShapeGenerator(){}
	ShapeGenerator(const ShapeGenerator&){}

public:
	static ShapeGenerator& Instance()
	{
		static ShapeGenerator m_Instance;	
		return m_Instance;
	}
	
	//In: path points input, width
	//out:double points in input order ,left to right
	//return true if success
	bool GenWidthPath(const std::vector<Point>& _vInPathPt,double dwidth,std::vector<Point>& _outPts);
	
	struct WidthPipePath{
		ARCPath3 leftpath;		
		ARCPath3 rightpath;
	};

	static void GenWidthPipePath(const ARCPath3& _in,const double& dwidth, WidthPipePath& _out);
	static void GenWidthPipePath(const ARCPath3& _in,const double& dwidth1, const double& dwidth2, WidthPipePath& _out);

	struct widthPath
	{
		std::vector<ARCVector3> leftpts;
		std::vector<ARCVector3> rightpts;
	};
	static void GenWidthPath(const Path& centerPath,double width,widthPath& output);
	static void GenWidthPath(const CPath2008& centerPath,double width,widthPath& output);

	void GenSmoothPath(const ARCPath3& _input,ARCPath3& _output,double smoothLen = 0.5 * SCALE_FACTOR);	

	void GenBezierPath(const std::vector<ARCVector3>& _input, std::vector<ARCVector3>& _output, int nPtCount );
	

};
#define SHAPEGEN (ShapeGenerator::Instance())
#endif
