#include "stdafx.h"
#include "Resource.h"
#include "MSVNodeData.h"
#include <algorithm>



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

MSV::CNodeData::~CNodeData()
{
	if(mpCreator)
		mpCreator->remove(this);
}

MSV::CNodeData* MSV::CNodeDataAllocator::allocateNodeData( enumNodeType t )
{
	CNodeData* pnewData =new CNodeData(t,this);
	vDatas.push_back(pnewData);
	return pnewData;
}

MSV::CNodeDataAllocator::~CNodeDataAllocator()
{
	std::vector<CNodeData*> vDataCopys = vDatas;
	vDatas.clear();

	for(size_t i=0;i<vDataCopys.size();i++){
		delete vDataCopys[i];
	}
	
}

void MSV::CNodeDataAllocator::remove( CNodeData* d )
{
	std::vector<CNodeData*>::iterator itr = std::find(vDatas.begin(),vDatas.end(),d);
	if(itr!=vDatas.end())
		vDatas.erase(itr);
}

