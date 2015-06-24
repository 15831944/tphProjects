#include "StdAfx.h"
#include ".\airsideeditcontext.h"
#include "./TermPlanDoc.h"
CAirsideEditContext::CAirsideEditContext(void)
{
}

CAirsideEditContext::~CAirsideEditContext(void)
{
}

InputAirside* CAirsideEditContext::getInput()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	return &pDoc->getARCport()->GetInputAirside();
}

double CAirsideEditContext::GetActiveFloorHeight() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	
	return 0;
}
