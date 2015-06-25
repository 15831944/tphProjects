#pragma once


#include "ARCPath.h"

class CPath2008;

class ARCPipe
{
public:
	ARCPipe(){}
	ARCPipe(const CPath2008& pts, double width, bool bLoop = false);
	ARCPipe(const ARCPath3&  pts, double width, bool bLoop = false);
	ARCPipe(const CPath2008& pts, double dStartWidth, double dEndWidth);
	ARCPipe(const ARCPath3&  pts, double dStartWidth, double dEndWidth);


// 	void Init(const CPath2008&, double width);
	void Init( const CPath2008& pts, double width, bool bLoop = false );
	void Init( const CPath2008& pts, double dStartWidth, double dEndWidth );
	ARCPath3 m_centerPath;
	double m_dWidth; // width of pipe or start width of the pipe
	double m_dEndWidth; // end width of the pipe
	ARCPath3 m_sidePath1;
	ARCPath3 m_sidePath2;

	void getMidPath(double dDist, CPath2008& path)const;

};