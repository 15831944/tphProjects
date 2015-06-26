#include "stdafx.h"
#include "3DLandsideNode.h"

#include <common/IEnvModeEditContext.h>
#include "3DFloorNode.h"
#include "Common/RenderFloor.h"
#include "Landside/ILandsideEditContext.h"
#include "common/IARCportLayoutEditContext.h"

void C3DLandsideNode::UpdateAll( IARCportLayoutEditContext* pEditContext )
{
	ILandsideEditContext* landside = pEditContext->GetLandsideContext();
	//update floors
	CRenderFloorList flist = landside->GetFloorList();
	for(size_t i=0;i<flist.size();++i){
		CRenderFloor* floor = flist.at(i);
		C3DFloorNode floorNode  = GetCreateChildNode(floor->GetUID());
		floorNode.UpdateAll(floor, pEditContext->GetCurContext() == landside);
	}
}

void C3DLandsideNode::UpdateFloorAltitude( const CString& floorId, double d )
{
	C3DFloorNode floorNode  = GetCreateChildNode(floorId);
	floorNode.UpdateAltitude(d);
}
