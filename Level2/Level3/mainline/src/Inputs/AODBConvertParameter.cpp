#include "StdAfx.h"
#include ".\aodbconvertparameter.h"


AODB::ConvertParameter::ConvertParameter()
{
	oleStartDate = COleDateTime::GetCurrentTime();
	pTerminal = NULL;
	pMapFile = NULL;
}

AODB::ConvertParameter::~ConvertParameter()
{

}