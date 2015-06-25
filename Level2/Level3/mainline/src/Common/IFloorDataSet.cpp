#include "StdAfx.h"
#include ".\ifloordataset.h"
#include "./RenderFloor.h"

double IFloorDataSet::GetVisualHeight( double dfloorIndex ) const
{
	static double DefaultVisualSpan = 500;  //5 meter
	int nCount = GetCount();
	double dBaseHeight = 0;
	double dTopHeight = 0;
	if(nCount>0){
		const CRenderFloor* pFloor = GetFloor(0);
		dBaseHeight = pFloor->Altitude();

		pFloor = GetFloor(nCount-1);
		dTopHeight = pFloor->Altitude();
	}
		

	if( dfloorIndex <= 0 )
	{
		return dfloorIndex * DefaultVisualSpan + dBaseHeight;
	}
	if(dfloorIndex>=nCount-1){
		return (dfloorIndex-nCount+1)*DefaultVisualSpan + dTopHeight;
	}

	const CRenderFloor* pFloor1 = GetFloor((int)dfloorIndex);
	const CRenderFloor* pFloor2 = GetFloor((int)dfloorIndex+1);
	double dOffset = dfloorIndex-(int)dfloorIndex;
	return pFloor1->Altitude()*(1-dOffset) + pFloor2->Altitude()*dOffset;

}

double IFloorDataSet::GetRealHeight( double dfloorIndex ) const
{
	static double DefaultVisualSpan = 500;  //5 meter
	int nCount = GetCount();
	double dBaseHeight = 0;
	double dTopHeight = 0;
	if(nCount>0){
		const CRenderFloor* pFloor = GetFloor(0);
		dBaseHeight = pFloor->RealAltitude();

		pFloor = GetFloor(nCount-1);
		dTopHeight = pFloor->RealAltitude();
	}


	if( dfloorIndex <= 0 )
	{
		return dfloorIndex * DefaultVisualSpan + dBaseHeight;
	}
	if(dfloorIndex>=nCount-1){
		return (dfloorIndex-nCount+1)*DefaultVisualSpan + dTopHeight;
	}

	const CRenderFloor* pFloor1 = GetFloor((int)dfloorIndex);
	const CRenderFloor* pFloor2 = GetFloor((int)dfloorIndex+1);
	double dOffset = dfloorIndex-(int)dfloorIndex;
	return pFloor1->RealAltitude()*(1-dOffset) + pFloor2->RealAltitude()*dOffset;
}
