#include "StdAfx.h"

//////#include <dos.h>
//////#include <ctype.h>
//////#include <string.h>
//////
//////#include "ProbabilityDistribution.h"
//////#include "inputs\procdist.h"
//////
//////
//////
////////#include "inputs\fltdist.h"
//////#include "common/ProbabilityDistribution.h"
//////#include "common/template.h"
//////#include "common/containr.h"
////////#include "common/flt_cnst.h"
//////#include "constrnt.h"
//////#include "../Common/IDGather.h"
//////#include "../common/term_bin.h"
////////#include "../common/AIRSIDE_BIN.h"
//////#include "../Common/IDGather.h"
//////#include "../Common/termfile.h"

#include "TestFile.h"

////////#include "common\termfile.h"
////////#include "common\point.h"
////////#include "exeption.h"
////////#include "csv_file.h"
////////#include "elaptime.h"
////////#include "util.h"
////////#include "Common/Point2008.h"


#include "common\termfile.h"

#include <dos.h>
#include <ctype.h>
#include <string.h>

#include "ProbabilityDistribution.h"
#include "inputs\procdist.h"
//#include "inputs\fltdist.h"

#include "common\point.h"
#include "exeption.h"
#include "csv_file.h"
#include "elaptime.h"
#include "util.h"
#include "Common/Point2008.h"


//TestFile::TestFile(void)
//{
//}
//
//
//TestFile::~TestFile(void)
//{
//}

testClass::testClass()
{	m_fversion = 0.0f;
trimSpaces = 0;
	nNumber = 111; lNumber = 2222;
}

baseTestClass::baseTestClass()
{
	curPos = 0;
	lineLength = 0;
}
