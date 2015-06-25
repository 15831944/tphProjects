#include "StdAfx.h"
#include ".\iarcportlayouteditcontext.h"

IARCportLayoutEditContext::IARCportLayoutEditContext(void)
{
}

IARCportLayoutEditContext::~IARCportLayoutEditContext(void)
{
}

ITerminalEditContext* IARCportLayoutEditContext::GetTerminalContext()
{
	if(IEnvModeEditContext* pCtx = GetContext(EnvMode_Terminal) )
	{
		return pCtx->toTerminal();
	}
	return NULL;
}

ILandsideEditContext* IARCportLayoutEditContext::GetLandsideContext()
{
	if(IEnvModeEditContext* pCtx = GetContext(EnvMode_LandSide) )
	{
		return pCtx->toLandside();
	}
	return NULL;
}

IAirsideEditContext* IARCportLayoutEditContext::GetAirsideContext()
{
	if(IEnvModeEditContext* pCtx = GetContext(EnvMode_AirSide) )
	{
		return pCtx->toAirside();
	}
	return NULL;
}


