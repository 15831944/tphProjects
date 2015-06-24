#include "stdafx.h"
#include "Resource.h"
#include "MSVNodeData.h"



MSV::CNodeData::CNodeData( enumNodeType nodetype, CNodeDataAllocator* pCreator)
{
	nodeType = nodetype;
	nSubType = 0;
	nIDResource = IDR_CTX_DEFAULT;
	dwData = 0;
	strDesc = "";

	bHasExpand = false;
	nOtherData = -1;

	mpCreator = pCreator;
}

MSV::CNodeData* MSV::CNodeDataAllocator::allocateNodeData( enumNodeType t )
{
	CNodeData* pnewData =new CNodeData(t,this);
	vDatas.push_back(pnewData);
	return pnewData;
}

MSV::CNodeDataAllocator::~CNodeDataAllocator()
{
	for(size_t i=0;i<vDatas.size();i++){
		delete vDatas[i];
	}
}
