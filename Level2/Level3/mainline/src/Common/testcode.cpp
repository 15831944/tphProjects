#include "StdAfx.h"
#include <dos.h>
#include <ctype.h>
#include <string.h>

#include "ProbabilityDistribution.h"
#include "inputs\procdist.h"
#include "inputs\fltdist.h"
#include "common\termfile.h"
#include "common\point.h"
#include "exeption.h"
#include "csv_file.h"
#include "elaptime.h"
#include "util.h"
#include "Common/Point2008.h"
#include "testcode.h"

testcode::testcode(void)
{
	_curPos = 0;
	_lineLength = 0;
	a=0;
	b =1;
	c =2;

	e  = 1.0;
	f = 2;
	//curPos=1;
	//lineLength=0;
}


testcode::~testcode(void)
{
}
